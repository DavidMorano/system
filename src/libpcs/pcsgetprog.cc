/* pcsgetprog SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the path to a program that is used within the PCS system */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsgetprog

	Description:
	This subroutine is used to find a PCS related program and
	to verify that it is executable.

	Important:
	This subroutine differs from |pcsgetprogpath(3pcs)| in that
	a full path is returned *only* when the program path is not
	absolute-rooted and it is found in the PCS distribution
	area.  The |pcsgetprogpath(3pcs)| subroutine, in contrast,
	returns a full path of the found program whenever it is
	different than that supplied.

	Synopsis:
	int pcsgetprog(cc *pcsroot,char *rbuf,cc *program) noex

	Arguments:
	pcsroot		PCS program-root
	rbuf		resulting path to program if it is not absolute
			and it is found in the PCS distribution
	program		program to find

	Returns:
	>0		found the program in the PCS distribution and
			this is the length of the returned path string
	0		found the program in user's PATH
	<0		did not find the program (system-return)

	programpath	resulting path to program if it is not absolute
			and it is found in the PCS distribution

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
#include	<getfiledirs.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DDEBUGPRINTF(3debug)| */

#include	"pcsgetprog.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int pcsgetprog(cchar *pcsroot,char *output,cchar *name) noex {
	ustat	sb ;
	int		rs = SR_NOTFOUND ;
	int		namelen ;
	int		outlen = 0 ;
	int		f_output = FALSE ;
	char		outbuf[MAXPATHLEN + 2] ;

	if (name == nullptr) return SR_FAULT ;

	if (name[0] == '\0') return SR_NOTFOUND ;

	DEBUGPRINTF("pcsroot=%s name=%s\n",pcsroot,name) ;
	/* was output specified? */
	if (output != nullptr) {
	    f_output = TRUE ;
	    output[0] = '\0' ;
	} else {
	    output = outbuf ;
	}

/* check input */

	namelen = lenstr(name) ;

	while ((namelen > 0) && (name[namelen - 1] == '/')) {
	    namelen -= 1 ;
	}

/* start the checks */

	if (strnchr(name,namelen,'/') != nullptr) {
	DEBUGPRINTF("slashed\n") ;
	    if ((rs = mkpath1w(output,name,namelen)) >= 0)
	        rs = u_stat(output,&sb) ;

	    if (rs >= 0) {
	        rs = SR_NOTFOUND ;
	        if (S_ISREG(sb.st_mode))
	            rs = perm(output,-1,-1,nullptr,X_OK) ;
	    }

	    if (rs >= 0)
		rs = 0 ;

	} else {

/* check if the PCS root directory exists */

	if ((rs == SR_NOENT) || (rs == SR_ACCESS) &&
	    (pcsroot != nullptr)) {

	DEBUGPRINTF("rooted\n") ;

	    if ((rs = mkpath3w(output,pcsroot,"bin",name,namelen)) >= 0) {
	    outlen = rs ;
	    if ((rs = u_stat(output,&sb)) >= 0) {
	        rs = SR_NOTFOUND ;
	        if (S_ISREG(sb.st_mode))
	            rs = perm(output,-1,-1,nullptr,X_OK) ;
	    } /* end if */
	    } /* end if */

	    if ((rs == SR_NOENT) || (rs == SR_ACCESS)) {

	        rs = mkpath3w(output,pcsroot,"sbin",name,namelen) ;
	        outlen = rs ;
	        if ((rs = u_stat(output,&sb)) >= 0) {
	            rs = SR_NOTFOUND ;
	            if (S_ISREG(sb.st_mode)) {
	                rs = perm(output,-1,-1,nullptr,X_OK) ;
		    }
	        }

	    } /* end if */

	    if (rs >= 0)
	        rs = outlen ;

	} /* end if (non-null PCS root) */

/* search the execution path */

	if ((rs == SR_NOENT) || (rs == SR_ACCESS)) {
	DEBUGPRINTF("pathed\n") ;

	    if (f_output)
	        output[0] = '\0' ;

	    if (getfiledirs(nullptr,name,"x",nullptr) > 0)
	        rs = 0 ;

	} /* end if */

	} /* end if */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pcsgetprog) */


