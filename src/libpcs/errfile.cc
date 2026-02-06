/* errfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debugging interface to the STDERR output stream */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0

/* revision history:

	= 1994-06-01, David A­D­ Morano
	This module was created so that a file to store debugging
	or error information could be accessed independent from the
	regular I-O of the program.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	errfile

	Description:
	This module implements a sort of debugging or error interface
	so that the regular I-O of the program is not interfered
	with.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdarg>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usysutility.hh>	/* |snvprintf(3u)| */
#include	<uclibmem.h>
#include	<fmtstr.h>
#include	<localmisc.h>

#include	"errfile.h"


/* local defines */

#define	O_FLAGS		(O_WRONLY | O_APPEND | O_CREAT)

#ifndef	BUFLEN
#define	BUFLEN		(2 * 1024)
#endif


/* imported namespaces */

using libu::snprintf ;			/* subroutine */
using libu::snvprintf ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static int	efd = -1 ;
static int	pl = 0 ;

static char	buf[BUFLEN + 1] ;


/* exported variables */


/* exported subroutines */

int erropen(cchar *errfname,cchar *logid) noex {
    	int		rs = SR_FAULT ;
	if (errfname && logid) {
	    cint 	of = O_FLAGS ;
	    cmode	om = 0666 ;
	    if ((rs = u_open(errfname,of,om)) >= 0) {
		efd = rs ;
		if ((rs = fchmod(efd,om)) >= 0) {
		    rs = snprintf(buf,BUFLEN,"%-13s ",logid) ;
		}
		if (rs < 0) {
		    u_close(efd) ;
		    efd = -1 ;
		} /* end if (error) */
	    } /* end if (u_open) */
	} /* end if (non-null) */
	return (rs >= 0) ? efd : rs ;
}
/* end subroutine (erropen) */

int errclose() noex {
    	int		rs = SR_NOTOPEN ;
	if (efd >= 0) {
	    rs = u_close(efd) ;
	    efd = -1 ;
	}
	return rs ;
}
/* end subroutine (errclose) */

int errprintf(cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		sl = 0 ; /* return-value */
	if (fmt) {
	    rs = SR_INVALID ;
	    if (fmt[0]) {
	        rs = SR_NOTOPEN ;
       		if (efd >= 0) {
	            va_begin(ap,fmt) ;
	            if ((rs = fmtstr((buf + pl),(BUFLEN - pl),1,fmt,ap)) >= 0) {
	                if (sl >= (BUFLEN - pl - 1)) {
	                    buf[pl + sl++] = '\n' ;
	                }
	                {
	                    rs = u_write(efd,buf,(pl + sl)) ;
	                    sl = rs ;
	                }
	            } /* end if (fmtstr) */
	            va_end(ap) ;
	        } /* end if (enabled) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (errprintf) */


