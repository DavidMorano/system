/* logfile_meths SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform logging operations on a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	logfile

	Description:
	This is a module to operate on a logfile.  The subroutines
	in this module are:

		logfile_open
		logfile_write
		logfile_printf
		logfile_vprintf
		logfile_setid
		logfile_check
		logfile_checksize
		logfile_close

	Although masking interrupts during the locking of the logfile
	should NOT be necessary (since a terminated process releases
	all file locks that it may have), it is a good precaution
	against some stupid implementations of the NFS helper lock
	manager daemon.  We want to guard against a program terminating
	abruptly while its lock is retained (for a time) back at
	the server.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"logfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */
import logfile_mag ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int logfile::open(cc *lfn,int of,mode_t om,ccp id) noex {
    	return logfile_open(this,lfn,of,om,id) ;
} /* end method (logfile::open) */

int logfile::printf(cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	{
	    va_begin(ap,fmt) ;
	    rs = logfile_vprintf(this,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
} /* end method (logfile::printf) */

int logfile::vprintf(cchar *fmt,va_list ap) noex {
	return logfile_vprintf(this,fmt,ap) ;
} /* end method (logfile::vprintf) */
 

int logfile::setid(cchar *id) noex {
	return logfile_setid(this,id) ;
} /* end method (logfile::setid) */

int logfile::chmod(mode_t om) noex {
	return logfile_chmod(this,om) ;
} /* end method (logfile::chmod) */

int logfile::control(int cmd,void *ap) noex {
	return logfile_control(this,cmd,ap) ;
} /* end method (logfile::control) */

int logfile::check(time_t dt) noex {
    	return logfile_check(this,dt) ;
} /* end method (logfile::check) */

int logfile::write(cchar *sp,int sl) noex {
	return logfile_write(this,sp,sl) ;
} /* end method (logfile::write) */

int logfile::print(cchar *sp,int sl) noex {
	return logfile_write(this,sp,sl) ;
} /* end method (logfile::print) */

int logfile_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case logfilemem_flush:
	        rs = logfile_flush(op) ;
	        break ;
	    case logfilemem_cksize:
	        rs = logfile_checksize(op,a) ;
	        break ;
	    case logfilemem_close:
	        rs = logfile_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (logfile_co::operator) */


