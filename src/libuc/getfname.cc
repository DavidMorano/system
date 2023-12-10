/* getfname */
/* lang=C++20 */

/* get a file name according to rules */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getfname

	Description:
	This subroutine will form a file name according to some rules.
	This subroutine is used (among others) to find files that
	are specified in configuration files and are supposed to
	be rooted at one place or another in the file system.
	If a type of GETFNAME_TYPELOCAL is given as an argument,
	the file is searched locally before being searched in the
	program root area. If the type is given as GETFNAME_TYPEROOT,
	the file is searched for in the program root area first and
	then locally.

	Synopsis:
	int getfname(cchar *pr,char *rbuf,int type,cchar *fname) noex

	Arguments:
	pr		base directory path to check in
	rbuf		the output file buffer (user supplied)
	type		the type of the check to make
		0	search locally first
		1	search in the program root area first
	fname		the name of the input file to check for

	Returns:
	>0		try file at new path
	0		try file as is
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<mkpathx.h>
#include	<xperm.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getfname.h"


/* local defines */

#ifndef	GETFNAME_TYPELOCAL
#define	GETFNAME_TYPELOCAL	0	/* search locally first */
#define	GETFNAME_TYPEROOT	1	/* search pr area first */
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		rsn = SR_NOTFOUND ;


/* exported subroutines */

int getfname(cchar *pr,char *rbuf,int type,cchar *fname,mode_t fm) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (pr && rbuf && fname) {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (fname[0]) {
		USTAT	sb ;
		bool	f_done = FALSE ;
		bool	f_local = FALSE ;
		rs = SR_OK ;
	        if (fname[0] != '/') {
/* second */
	            if ((rs >= 0) && (! f_done)) {
	                if ((type == GETFNAME_TYPEROOT) && pr) {
	                    if ((rs = mkpath(rbuf,pr,fname)) >= 0) {
	                        len = rs ;
	                        if ((rs = perm(rbuf,-1,-1,nullptr,W_OK)) >= 0) {
		                    f_done = true ;	
	                        } else if (rs == rsn) {
		                    rs = SR_OK ;
	                        }
	                    } /* end if (mkpath) */
	                } else {
	                    f_local = TRUE ;
	                    if ((rs = perm(fname,-1,-1,nullptr,W_OK)) >= 0) {
	                        if ((rs = u_stat(fname,&sb)) >= 0) {
		                    if (! S_ISDIR(sb.st_mode)) {
			                f_done = true ;
		                    }
		                } /* end if (stat) */
	                    } else if (isNotAccess(rs)) {
		                rs = SR_OK ;
	                    } /* end if (perm) */
	                } /* end if */
	            } /* end if (needed) */
/* third */
	            if ((rs >= 0) && (! f_done)) {
	                if ((type == GETFNAME_TYPELOCAL) && (pr != nullptr)) {
	                    if ((rs = mkpath(rbuf,pr,fname)) >= 0) {
	    		        len = rs ;
	    			if ((rs = perm(rbuf,-1,-1,nullptr,W_OK)) >= 0) {
				    f_done = true ;
				}
			    } /* end if (mkpath) */
			} /* end if (type) */
	            } else if (! f_local) {
	                if ((rs = perm(fname,-1,-1,nullptr,W_OK)) >= 0) {
	        	    if ((rs = u_stat(fname,&sb)) >= 0) {
				if (! S_ISDIR(sb.st_mode)) {
				    len = 0 ;
	    			}
			    }
			} /* end if */
		    } /* end if (needed) */
/* forth */
	            if (type != GETFNAME_TYPEROOT) {
	    	        len = 0 ;
		    }
		} /* end if (need search for non-rooted file) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getrbuf) */


