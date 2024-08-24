/* findxfile SUPPORT */
/* lang=C++20 */

/* find an executable file */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	findxfile

	Description:
	This subroutine is used to find an executable file using
	the existing PATH environment variable.

	Synopsis:
	int findxfile(ids *idp,char *buf,cchar *pn) noex

	Arguments:
	idp		pointer to IDS object
	buf		buffer to receive resulting path
	pn		program-name string to search for

	Returns:
	>0		length of found path
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<pathclean.h>
#include	<getprogpath.h>
#include	<xfile.h>
#include	<localmisc.h>

#include	"findxfile.h"


/* local defines */

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#define	NENTS	40		/* initial number path components */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int findxfile(ids *idp,char *rbuf,cchar *pn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	static cchar	*path = getenv(VARPATH) ;
	if (idp && rbuf && pn) {
	rs = SR_NOENT ;
	rbuf[0] = '\0' ;
	if (path != nullptr) {
	    vecstr	plist ;
	    cint	ne = NENTS ;
	    bool	f_pwd = false ;

	    if ((rs = vecstr_start(&plist,ne,0)) >= 0) {
		int	cl ;
		cchar	*tp ;
		cchar	*sp = path ;
	        char	cbuf[MAXPATHLEN + 1] ;

	        while ((tp = strpbrk(sp,":;")) != nullptr) {
	            if ((tp-sp) == 0) {
	                f_pwd = true ;
		    }
	            if ((rs = pathclean(cbuf,sp,(tp-sp))) >= 0) {
	                cl = rs ;
	                rs = vecstr_adduniq(&plist,cbuf,cl) ;
	            }
	            sp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end while */

	        if ((rs >= 0) && (sp[0] != '\0')) {
	            rs = vecstr_adduniq(&plist,sp,-1) ;
		}

	        if (rs >= 0) {
	            rs = getprogpath(idp,&plist,rbuf,pn,-1) ;
		    len = rs ;
		}

	        if ((! f_pwd) && (rs == SR_NOENT)) {
	            if ((rs = xfile(idp,pn)) >= 0) {
	                rs = mkpath1(rbuf,pn) ;
		        len = rs ;
		    }
	        } /* end if */

	        rs1 = vecstr_finish(&plist) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (path-list) */
	} else {
	    if ((rs = xfile(idp,pn)) >= 0) {
		rs = mkpath1(rbuf,pn) ;
		len = rs ;
	    }
	} /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (findxfile) */


