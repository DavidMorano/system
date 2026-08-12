/* pcsngdir SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return a directory name when given a newsgroup name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsngdir

	Description:
	This subroutine will take its argument to be a newsgroup
	name. The subroutine will return the corresponding directory
	name in the spool area.  The 'PCS' variable must be set.

	Synopsis:
	int pcsngdir(cchar *pcs,char *rbuf,cc *bbnewsdir,cc *newsgroup) noex

	Arguments:
	pcs		PCS program root
	rbuf		the directory path to the newsgroup relative
			to the BBNEWS spool directory
	bbnewsdir	the top of the BBNEWS spool directory
	ngname		a name of a newsgroup

	Returns:
	>0		length of returned directory name
	SR_FAULT	NULL argument(s) was given
	SR_INVALID	an invalid argument was given
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkpathx.h>		/* LIBUC */
#include	<pathadd.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"pcsngdir.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsngdir(cc *pcs,char *ngdir,cc *bbnewsdir,cc *newsgroup) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		len = 0 ;
	int		f_first ;
	char		bbnewsdir2[MAXPATHLEN + 1] ;
	char		*bp ;
	char		*cp2 ;
	char		*ndp ;

	if (pcs == NULL) return SR_FAULT ;
	if (newsgroup == NULL) return SR_FAULT ;
	if (bbnewsdir == NULL) return SR_FAULT ;

	if (newsgroup[0] == '\0') return SR_INVALID ;

	ndp = bbnewsdir2 ;
	if (bbnewsdir[0] != '/') {
	    ndp = strwcpy(ndp,pcs,-1) ;
	    ndp = strwcpy(ndp,"/",1) ;
	} /* end if */

	ndp = strwcpy(ndp,bbnewsdir,-1) ;

	ndp = strwcpy(ndp,"/",1) ;

	if ((rs = u_stat(bbnewsdir2,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        if ((rs = mkpath1(ngdir,newsgroup)) >= 0) {

	            cp2 = ngdir ;
	            while ((bp = strchr(cp2,'.')) != NULL) {
	                *bp = '/' ;
	                cp2 = (bp + 1) ;
	            } /* end while */
		    /* looking for closest directory that matches */
	            f_first = true ;
	            rs = SR_ACCESS ;
	            while (f_first || ((bp = strrchr(ngdir,'/')) != NULL)) {
	                if (! f_first) {
	                    *bp = '.' ;
			}
	                f_first = false ;
	                if ((rs = mkpath1(ndp,ngdir)) >= 0) {

	                    if ((u_stat(bbnewsdir2,&sb) >= 0) && 
	                        S_ISDIR(sb.st_mode) &&
	                        (u_access(bbnewsdir2,W_OK) >= 0)) {

	                        rs = SR_OK ;
	                        break ;
	                    }

	                } /* end if */

	                if (rs < 0) break ;
	            } /* end while */
		    /* try the last name we are left with */
	            if (rs < 0) {
	                strcpy(ndp,ngdir) ;
	                if ((u_stat(bbnewsdir2,&sb) >= 0) && 
	                    S_ISDIR(sb.st_mode) &&
	                    (u_access(bbnewsdir2,W_OK) >= 0))
	                    rs = SR_OK ;

	            } /* end if (tried to make it) */

	        } /* end if (mkpath) */
	    } else {
	        rs = SR_NOTDIR ;
	    }
	} /* end if (u_stat) */

	return (rs >= 0) ? len : rs ;
} /* end subroutine (pcsngdir) */


