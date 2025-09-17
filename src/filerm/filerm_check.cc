/* filerm_check SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* check a directory entry for a match */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/***********************************************************************

  	Name:
	checkname

	Description:
	This subroutine is called by the |process()| subroutine in
	order to check if a filename is marked for some disposition.

	Synopsis:
	int checkname(proginfo *pip,cchar *name,USTAT *sbp) noex

	Arguments:
	pip		user specified argument
	name		directory entry
	sbp		'stat' block pointer

	Returns:
	>0		skip this directory entry in directory walk
	0		continue with directory walk as usual
	<0		error (system-return)

***********************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<bfile.h>
#include	<fsdir.h>
#include	<storebuf.h>
#include	<vecstr.h>
#include	<burn.h>
#include	<localmisc.h>

#include	"removename.h"
#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX((MAXPATHLEN + 10),2048)
#endif


/* external subroutines */

extern int	checklink(char *,ustat *,struct proginfo *) ;


/* external variables */


/* forward references */

static int	checkad(proginfo *,cchar *,int,cchar *) noex ;
static int	mkpathadd(char *,cchar *,int,cchar *,int) noex ;


/* exported variables */


/* exported subroutines */

int checkname(proginfo *,cchar *name,USTAT *sbp) noex {
	USTAT		sb2 ;
	randomvar	*rvp = pip->rvp ;
	int		rs = SR_OK ;
	int	rs1 = 0 ;
	int	rs2 ;
	int	i ;
	int	bnl, dnl ;
	int	bcount = pip->bcount ;
	int	rc = 0 ;
	int	f_link ;
	int	f_follow ;

	cchar	*sp ;
	cchar	*dnp ;
	cchar	*bnp ;
	cchar	*cp ;


	pip->c_processed += 1 ;
	bnl = sfbasename(name,-1,&bnp) ;
	if (bnl <= 0) goto ret0 ;

	f_link = S_ISLNK(sbp->st_mode) ? TRUE : FALSE ;
	f_follow = pip->fl.follow ;

/* do not follow the directory (if there is one) under these conditions */

	rc = f_link && (! f_follow) ;

/* OK, do the appropriate thing */

	if (pip->fl.all) {

	    if (! pip->fl.no) {

	    rs1 = 0 ;
	        if (S_ISREG(sbp->st_mode) || S_ISDIR(sbp->st_mode)) {
		    cint	ro = pip->rn_opts ;
	            rs1 = removename(name,ro,rvp,bcount) ;
	        } else if (! (f_link && f_follow)) {
	            rs1 = u_unlink(name) ;
	            if (rs1 >= 1) rs1 = 1 ;
	        } /* end if */

	    if (rs1 > 0)
	        pip->c_removed += rs1 ;

	    } /* end if */

	    if (pip->fl.print)
	        bprintf(pip->ofp,"%s\n",name) ;

	} /* end if (remove all) */

/* check for a dangling symbolic link */

	if (pip->fl.links && S_ISLNK(sbp->st_mode)) {

	    if (checklink(name,sbp,pip) > 0) {
	        rc = 0 ;
	 	goto ret0 ;
	    }

	} /* end if */

/* should we check for a dangling AppleDouble file? */

	if (pip->fl.appledouble && (strcmp(bnp,APPLEDOUBLE) == 0)) {

	    if (pip->fl.follow && S_ISLNK(sbp->st_mode)) {

	        sbp = &sb2 ;
	        rs2 = u_stat(name,&sb2) ;
	        if (rs2 < 0) {
	            rc = 0 ;
		    goto ret0 ;
		}

	    } /* end if */

	    if (S_ISDIR(sbp->st_mode)) {

/* get the parent directory name */

	        if ((dnl = sfdirname(name,-1,&dnp)) > 0) {

	            checkad(pip,dnp,dnl,name) ;
	            rc = 1 ;
		    goto ret0 ;
	        }

	    } /* end if */

	} /* end if (AppleDouble check) */

/* check for a name match */

	if (! pip->fl.all) {
	    vecstr	*nlp = &pip->names ;
	    for (int i = 0 ; (rs = nlp->get(i,&sp)) >= 0 ; i += 1) {
	        if (sp) {
	            if (sfsub(bnp,-1,sp,&cp) >= 0) break ;
		}
	    } /* end for */

	    if (rs >= 0) {

	        if (! pip->fl.no) {

		rs1 = 0 ;
	            if (S_ISREG(sbp->st_mode) || S_ISDIR(sbp->st_mode)) {

	                rs1 = removename(rvp,bcount,pip->rn_opts,name) ;

	            } else if (! (f_link && f_follow)) {

	                rs1 = u_unlink(name) ;
	                if (rs1 >= 0) rs1 = 1 ;

	            } /* end if */

	        if (rs1 > 0)
	            pip->c_removed += rs1 ;

	        } /* end if */

	        if (pip->fl.print)
	            bprintf(pip->ofp,"%s\n", name) ;

	    } /* end if */

	} /* end if (name match) */

ret0:
	return rc ;
}
/* end subroutine (checkname) */


/* local subroutines */

/* check for the proper APPLEDOUBLE file */
static int checkad(pip,pdirname,pdirlen,dirname)
struct proginfo	*pip ;
cchar	pdirname[] ;
cchar	dirname[] ;
int		pdirlen ;
{
	ustat	sb ;

	FSDIR		dir ;
	FSDIR_ENT	slot ;

	int	rs ;
	int	rs1, rs2 ;
	int	bnl ;
	cchar	*bnp ;
	char	fname[MAXPATHLEN + 1] ;

	if ((rs = fsdir_open(&dir,dirname)) >= 0) {

	    while ((bnl = fsdir_read(&dir,&slot)) > 0) {

	        bnp = slot.name ;
	        if (strcmp(bnp,APPLEPARENT) == 0)
	            continue ;

		if ((rs = mkpathadd(fname,pdirname,pdirlen,bnp,bnl)) >= 0) {

	        rs2 = u_stat(fname,&sb) ;
	        if (rs2 == SR_NOENT) {

	            rs = mkpath2(fname,dirname,bnp) ;

		    if (rs >= 0) {

	            if ((pip->verboselevel > 0) || pip->fl.print)
	                bprintf(pip->ofp,"%s\n",fname) ;

	            rs1 = SR_OK ;
	            if (! pip->fl.no) {

	                if (pip->fl.burn) {
	                    rs = burn(fname,pip->bcount,pip->rvp) ;
			}

	                if ((rs1 = u_unlink(fname)) >= 0) {
	                    pip->c_removed += 1 ;
			}
	            } /* end if */

		    } /* end if (have good path) */

	        } /* end if (stat was bad) */
		}

		if (rs < 0) break ;
	    } /* end while */

	    fsdir_close(&dir) ;
	} /* end if */

	return rs ;
}
/* end subroutine (checkad) */

static int mkpathadd(fbuf,pbuf,plen,np,nl)
char		*fbuf ;
cchar	*pbuf ;
int		plen ;
cchar	*np ;
int		nl ;
{
	const int	flen = MAXPATHLEN ;

	int	rs = SR_OK ;
	int	i = 0 ;


	if (plen < 0) plen = strlen(pbuf) ;

	if (rs >= 0) {
	    rs = storebuf_strw(fbuf,flen,i,pbuf,plen) ;
	    i += rs ;
	}

	if ((rs >= 0) && (plen > 0) && (pbuf[plen-1] != '/')) {
	    rs = storebuf_chr(fbuf,flen,i,'/') ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(fbuf,flen,i,np,nl) ;
	    i += rs ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkpathadd) */


