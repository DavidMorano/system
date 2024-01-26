/* printhelp SUPPORT */
/* lang=C++20 */

/* print out a help file if we have one */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	The subroutine was written to get some common code for the
	printing of help files.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	printhelp

	Description:
	This subroutine will search for a program helpfile and print
	it out (by default to STDOUT).  A root filename is supplied
	(usually 'help') but along with a program root.  The
	"standard" places within the program root directory tree
	are scanned for the help file.

	Synopsis:
	int printhelp(void *fp,cchar *pr,cchar *sn,cchar *hfname) noex

	Arguments:
	fp		open file pointer (BFILE or SFIO)
	pr		program root directory path
	sn		program search name
	hfname		program help filename

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#if	defined(SFIO) && (SFIO > 0)
#define	CF_SFIO	1
#else
#define	CF_SFIO	0
#endif

#if	(defined(KSHBUILTIN) && (KSHBUILTIN > 0))
#include	<shell.h>
#endif

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<bfile.h>
#include	<expcook.h>
#include	<snx.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<mkchar.h>
#include	<isnot.h>
#include	<xperm.h>
#include	<getnodename.h>
#include	<localmisc.h>

#include	"printhelp.h"


/* local defines */

#ifndef	KBUFLEN
#define	KBUFLEN		40
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(2*LINEBUFLEN)
#endif

#ifndef	HELPSCHEDFNAME
#define	HELPSCHEDFNAME	"etc/printhelp.filesched"
#endif

#ifndef	HELPFNAME
#define	HELPFNAME	"help"
#endif

#ifndef	LIBCNAME
#define	LIBCNAME	"lib"
#endif

#ifndef	NDF
#define	NDF		"printhelp.deb"
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	permsched(cchar **,vecstr *,char *,int,cchar *,int) noex ;
    extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) noex ;
    extern int	vecstr_envset(vecstr *,cchar *,cchar *,int) noex ;
    extern int	vecstr_loadfile(vecstr *,int,cchar *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	findhelp(cchar *,cchar *,char *,cchar *) noex ;
static int	loadscheds(vecstr *,cchar *,cchar *) noex ;
static int	procprint(cchar *,cchar *,void *,char *,int,cchar *) noex ;
static int	procexpload(cchar *,expcook *,cchar *) noex ;
static int	procout(expcook *,void *,char *,int,cchar *) noex ;


/* local variables */

static constexpr cchar	*schedule[] = {
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

enum expkeys {
	expkey_sn,
	expkey_ss,
	expkey_pr,
	expkey_rn,
	expkey_overlast
} ;

static constexpr cchar	*expkeys[] = {
	"SN",
	"SS",
	"PR",
	"RN",
	nullptr
} ;


/* exported variables */


/* export subroutines */

int printhelp(void *fp,cchar *pr,cchar *sn,cchar *hfname) noex {
	cint	tlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	cchar	*fname ;
	char		tbuf[MAXPATHLEN + 1] ;

	if ((hfname == nullptr) || (hfname[0] == '\0'))
	    hfname = HELPFNAME ;

#if	CF_SFIO
	if (fp == nullptr) {
	    rs = SR_INVALID ;
	}
#endif /* CF_SFIO */

	if (rs >= 0) {

	    fname = hfname ;
	    if ((pr != nullptr) && (hfname[0] != '/')) {

	        rs = SR_NOTFOUND ;
	        if (strchr(hfname,'/') != nullptr) {
	            fname = tbuf ;
	            if ((rs = mkpath2(tbuf,pr,hfname)) >= 0) {
	                rs = u_access(tbuf,R_OK) ;
		    }
	        } /* end if */

	        if ((rs < 0) && isNotPresent(rs)) {
	            fname = tbuf ;
	            rs = findhelp(pr,sn,tbuf,hfname) ;
	        }

	    } /* end if (searching for file) */

	    if (rs >= 0) {
	        rs = procprint(pr,sn,fp,tbuf,tlen,fname) ;
	    }

	} /* end if (ok) */

	return rs ;
}
/* end subroutine (printhelp) */


/* local subroutines */

static int findhelp(cchar *pr,cchar *sn,char *tbuf,cchar *hfname) noex {
	vecstr		hs ;
	cint	tlen = MAXPATHLEN ;
	int		rs ;
	int		f_hs = FALSE ;
	cchar	**spp = schedule ;

/* first see if there is a "help schedule" in the ETC directory */

	if ((rs = mkpath2(tbuf,pr,HELPSCHEDFNAME)) >= 0) {
	    if ((rs = perm(tbuf,-1,-1,nullptr,R_OK)) >= 0) {
		cint	vo = VECSTR_OCOMPACT ;
	        if ((rs = vecstr_start(&hs,15,vo)) >= 0) {
	            f_hs = TRUE ;
	            if ((rs = vecstr_loadfile(&hs,FALSE,tbuf)) >= 0) {
	                vecstr_getvec(&hs,&spp) ;
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    }
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (mkpath) */

/* create the values for the file schedule searching and find the file */

	if (rs >= 0) {
	    vecstr	svars ;
	    if ((rs = vecstr_start(&svars,6,0)) >= 0) {
	        rs = loadscheds(&svars,pr,sn) ;
	        if (rs >= 0) {
	            rs = permsched(spp,&svars,tbuf,tlen,hfname,R_OK) ;
		}
	        if (isNotPresent(rs) && (spp != schedule)) {
	            rs = permsched(schedule,&svars,tbuf,tlen,hfname,R_OK) ;
		}
	        vecstr_finish(&svars) ;
	    } /* end if (schedule variables) */
	} /* end if (ok) */

	if (f_hs) {
	    vecstr_finish(&hs) ;
	}

	return rs ;
}
/* end subroutine (findhelp) */

static int loadscheds(vecstr *slp,cchar *pr,cchar *sn) noex {
	int		rs = SR_OK ;
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

static int procprint(cc *pr,cc *sn,void *fp,char *rp,int rl,cc *fn) noex {
	expcook		c ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = expcook_start(&c)) >= 0) {
	    if ((rs = procexpload(pr,&c,sn)) >= 0) {
	        rs = procout(&c,fp,rp,rl,fn) ;
	        wlen += rs ;
	    } /* end if (procexpload) */
	    rs1 = expcook_finish(&c) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (expcook) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procprint) */

static int procexpload(cchar *pr,expcook *ecp,cchar *sn) noex {
	int		rs ;
	char		nn[NODENAMELEN+1] ;
	char		dn[MAXHOSTNAMELEN+1] ;

	if ((rs = getnodedomain(nn,dn)) >= 0) {
	    cint	hlen = MAXHOSTNAMELEN ;
	    int		i ;
	    int		vl ;
	    cchar	*vp ;
	    cchar	*ks = "SNDHPR" ;
	    char	hbuf[MAXHOSTNAMELEN + 1] ;
	    char	kbuf[KBUFLEN+1] ;
	    kbuf[1] = '\0' ;
	    for (i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	        cint	kch = mkchar(ks[i]) ;
		vl = -1 ;
		vp = nullptr ;
		switch (kch) {
	        case 'S':
		    vp = sn ;
		    break ;
	        case 'N':
		    vp = nn ;
		    break ;
	        case 'D':
		    vp = dn ;
		    break ;
	        case 'H':
		    if ((rs = snsds(hbuf,hlen,nn,dn)) >= 0) {
			vl = rs ;
		        vp = hbuf ;
		    }
		    break ;
	        case 'P':
		    vp = pr ;
		    break ;
	        case 'R':
		    vl = sfbasename(pr,-1,&vp) ;
		    break ;
	        } /* end switch */
	        if ((rs >= 0) && (vp != nullptr)) {
		    kbuf[0] = kch ;
		    rs = expcook_add(ecp,kbuf,vp,vl) ;
	        }
	    } /* end for */
	    if (rs >= 0) {
		for (i = 0 ; (rs >= 0) && (i < expkey_overlast) ; i += 1) {
		    vl = -1 ;
		    vp = nullptr ;
		    switch (i) {
		    case expkey_sn:
			vp = sn ;
			break ;
		    case expkey_ss:
			vp = hbuf ;
			vl = strwcpyuc(hbuf,sn,-1) - hbuf ;
			break ;
		    case expkey_pr:
			vp = pr ;
			break ;
		    case expkey_rn:
		        vl = sfbasename(pr,-1,&vp) ;
			break ;
		    } /* end switch */
	            if ((rs >= 0) && (vp != nullptr)) {
		        rs = expcook_add(ecp,expkeys[i],vp,vl) ;
	            }
		} /* end for */
	    } /* end if (ok) */
	} /* end if (getnodedomain) */
	return rs ;
}
/* end subroutine (procexpload) */

static int procout(expcook *ecp,void *fp,char *lbuf,int llen,cc *fname) noex {
#if	CF_SFIO
#else
	bfile		outfile ;
#endif /* CF_SFIO */
	bfile		helpfile, *hfp = &helpfile ;
	int		rs = SR_OK ;
	int		wlen = 0 ;

#if	CF_SFIO
#else
	int		f_open = FALSE ;
#endif /* CF_SFIO */

	if (lbuf == nullptr) return SR_FAULT ;

#if	CF_SFIO
	if (fp == nullptr) return SR_FAULT ;
#else /* CF_SFIO */
	if (fp == nullptr) {
	    fp = &outfile ;
	    rs = bopen(fp,BFILE_STDOUT,"w",0666) ;
	    f_open = (rs >= 0) ;
	}
#endif /* CF_SFIO */

	if ((rs >= 0) && ((rs = bopen(hfp,fname,"r",0666)) >= 0)) {
	    cint	elen = EBUFLEN ;
	    char	*ebuf ;
	    if ((rs = uc_malloc((elen+1),&ebuf)) >= 0) {
	        int	len ;

	        while ((rs = breadln(hfp,lbuf,llen)) > 0) {
	            len = rs ;

		    if ((rs = expcook_exp(ecp,0,ebuf,elen,lbuf,len)) > 0) {
#if	CF_SFIO
	                rs = sfwrite(fp,ebuf,rs) ;
	                wlen += rs ;
#else /* CF_SFIO */
	                rs = bwrite(fp,ebuf,rs) ;
	                wlen += rs ;
#endif /* CF_SFIO */
		    } /* end if (expansion) */

	            if (rs < 0) break ;
	        } /* end while */

		uc_free(ebuf) ;
	    } /* end if (memory-allocation) */
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
/* end subroutine (procout) */


