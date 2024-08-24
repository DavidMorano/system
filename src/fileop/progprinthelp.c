/* progprinthelp SUPPORT */
/* lang=C++20 */

/* print out a help file if we have one */
/* version %I% last-modified %G% */


/* revision history:

	= 1997-11-01, David A­D­ Morano
	The subroutine was written to get some common code for the
	printing of help files.

*/

/* Copyright © 1997 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	progprinthelp

	Descrption:
	This subroutine will search for a program helpfile and print
	it out (by default to STDOUT). A root filename is supplied
	(usually 'help') but along with a program root. The "standard"
	places within the program root directory tree are scanned
	for the help file.

	Synopsis:
	int progprinthelp(PI *pip,void *fp,cc *helpfname) noex

	Arguments:
	pip		program-information-pointer
	fp		open file pointer (BFILE or SFIO)
	helpfname	program help filename

	Returns:
	>=0		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<bfile.h>
#include	<mkpathx.h>
#include	<xperm.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"proginfo.h"


/* local defines */

#ifndef	HELPSCHEDFNAME
#define	HELPSCHEDFNAME	"etc/progprinthelp.filesched"
#endif

#ifndef	HELPFNAME
#define	HELPFNAME	"help"
#endif

#ifndef	LIBCNAME
#define	LIBCNAME	"lib"
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	findhelpfile(cchar *,cchar *,char *,cchar *) noex ;
static int	loadscheds(vecstr *,cchar *,cchar *) noex ;
static int	printout(void *,char *,int,cchar *) noex ;


/* local variables */

constexpr cpcchar	schedule[] = {
	"%r/%l/%n/%n.%f",
	"%r/%l/%n/%f",
	"%r/share/help/%n.%f",
	"%r/share/help/%n",
	"/usr/extra/share/help/%n.%f",
	"/usr/extra/share/help/%n",
	"%r/%l/help/%n.%f",
	"%r/%l/help/%n",
	"%r/%l/%n.%f",
	"%r/%n.%f",
	"%n.%f",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int progprinthelp(proginfo *pip,void *fp,cchar *helpfname) noex {
	int		rs = SR_OK ;
	if ((helpfname == nullptr) || (helpfname[0] == '\0')) {
	    helpfname = HELPFNAME ;
	}
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    cint	tlen = rs ;
	    cchar	*fname = helpfname ;
	    rs = SR_NOTFOUND ;
	    if ((pip->pr != nullptr) && (helpfname[0] != '/')) {
	        if (strchr(helpfname,'/') != nullptr) {
	            fname = tbuf ;
	            if ((rs = mkpath2(tbuf,pip->pr,helpfname)) >= 0) {
	                rs = u_access(tbuf,R_OK) ;
		    }
	        } /* end if */
	        if ((rs < 0) && isNotPresent(rs)) {
	            fname = tbuf ;
	            rs = findhelpfile(pip->pr,pip->searchname,tbuf,helpfname) ;
	        }
	    } /* end if (searching for file) */
	    if (rs >= 0) {
	        rs = printout(fp,tbuf,tlen,fname) ;
	    }
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (progprinthelp) */


/* local subroutines */

static int findhelpfile(cc *pr,cc *sn,char *tbuf,cc *helpfname) noex {
	vecstr		svars ;
	vecstr		hs ;
	int		rs ;
	int		rs1 ;
	int		f_hs = false ;
	cchar		**spp = schedule ;

/* first see if there is a "help schedule" in the ETC directory */

	if ((rs = mkpath2(tbuf,pr,HELPSCHEDFNAME)) >= 0) {
	    if ((rs = perm(tbuf,-1,-1,nullptr,R_OK)) >= 0) {
		cint	ve = 15 ;
	        cint	vo = VECSTR_OCOMPACT ;
	        if ((rs = vecstr_start(&hs,ve,vo)) >= 0) {
	            f_hs = true ;
	            if ((rs1 = vecstr_loadfile(&hs,false,tbuf)) >= 0) {
	                vecstr_getvec(&hs,&spp) ;
		    }
	        }
	    } /* end if (could access) */
	}

/* create the values for the file schedule searching and find the file */

	if (rs >= 0) {

	    if ((rs = vecstr_start(&svars,6,0)) >= 0) {

	        rs = loadscheds(&svars,pr,sn) ;

/* OK, do the look-up */

	        if (rs >= 0) {
	            rs = permsched(spp,&svars,
	                tbuf,MAXPATHLEN, helpfname,R_OK) ;
		}
	        if (isNotPresent(rs) && (spp != schedule)) {
	            rs1 = permsched(schedule,&svars,
	                tbuf,MAXPATHLEN, helpfname,R_OK) ;
		}

	        vecstr_finish(&svars) ;
	    } /* end if (schedule variables) */

	} /* end if */

	if (f_hs)
	    vecstr_finish(&hs) ;

	return rs ;
}
/* end subroutine (findhelpfile) */

static int loadscheds(vecstr *slp,cchar *pr,cchar *sn) noex {
	int	rs = SR_OK ;
	if (pr != nullptr) {
	    rs = vecstr_envadd(slp,"r",pr,-1) ;
	}
	if (rs >= 0) {
	    rs = vecstr_envadd(slp,"l",LIBCNAME,-1) ;
	}
	if ((rs >= 0) && (sn != nullptr)) {
	    rs = vecstr_envadd(slp,"n",sn,-1) ;
	}
	return rs ;
}
/* end subroutine (loadscheds) */

static int printout(void *fp,char *lbuf,int llen,cchar *fname) noex {
	bfile		outfile ;
	bfile		helpfile, *hfp = &helpfile ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		f_open = false ;

	if (lbuf == nullptr) return SR_FAULT ;

	if (fp == nullptr) {
	    fp = &outfile ;
	    rs = bopen(fp,BFILE_STDOUT,"w",0666) ;
	    f_open = (rs >= 0) ;
	}

	if ((rs >= 0) &&
	    ((rs = bopen(hfp,fname,"r",0666)) >= 0)) {

#if	CF_SFIO
	    {
	        int	len ;

	        while ((rs = bread(hfp,lbuf,llen)) > 0) {
	            len = rs ;

	            rs = sfwrite(fp,lbuf,len) ;

	            if (rs < 0) break ;
		    wlen += rs ;

	        } /* end while */

	    } /* end block */

#else /* CF_SFIO */

	    if (llen) {
	        rs = bcopyblock(hfp,fp,-1) ;
		wlen = rs ;
	    }

#endif /* CF_SFIO */

	    bclose(hfp) ;
	} /* end if (opened helpfile) */

#if	CF_SFIO
	sfsync(fp) ;
#else
	if (f_open)
	    bclose(fp) ;
#endif /* CF_SFIO */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printout) */


