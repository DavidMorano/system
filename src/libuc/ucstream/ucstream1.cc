/* ucstream1 SUPPORT (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* file stream operations for LIBUC */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2011-04-12, David A­D­ Morano
	This code was originally written.  Only the introduction
	of C++11 has allowed this (finally), due to the addition
	(in C++11) of |unordered_map(3c++)|.

*/

/* Copyright © 2011 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	ucstream

	Description:
	This object implements a file stream.

	Symopsis:
	int ucstream_open(cchar *fn,cchar *os,mode_t om) noex

	Arguments:
	fn		file-name
	os		open-string
	om		open-mode

	Returns:
	>=0		ok
	<0		error (system-return)

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>		/* POSIX |dev_t| + |ino_t| */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<getoflags.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

module ucstream ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */


/* local subroutines (methods) */

int ucstream::open(cchar *fn,cchar *os,mode_t om) noex {
	int		rs = SR_FAULT ;
	DPRINTF("ent\n") ;
	if (fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
		DPRINTF("fn=%s\n",fn) ;
		if ((rs = getoflags(os)) >= 0) {
		    cint	µof = rs ;
		    DPRINTF("getoflags() rs=%d\n",rs) ;
		    if ((rs = uc_open(fn,rs,om)) >= 0) {
			fd = rs ;
			DPRINTF("open() rs=%d\n",rs) ;
			rs = start(fd,0z,0,µof) ;
			DPRINTF("start() rs=%d\n",rs) ;
			if (rs < 0) {
			    uc_close(fd) ;
			    fd = -1 ;
			} /* end if (error) */
		    } /* end if (open) */
		} /* end if (getoflags) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (ucstream::open) */

int ucstream::iclose() noex {
	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	DPRINTF("ent\n") ;
	if (fd >= 0) ylikely {
	    rs = SR_OK ;
	    {
		rs1 = finish ;
		if (rs >= 0) rs = rs1 ;
		DPRINTF("finish() rs=%d\n",rs) ;
	    }
	    {
	        rs1 = uc_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
		DPRINTF("uc_close() rs=%d\n",rs) ;
	    }
	    fd = -1 ;
	} /* end if (was open) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (ucstream::iclose) */

void ucstream::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("ucstream",rs,"fini-finish") ;
	}
} /* end method (ucstream::dtor) */

ucstream_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case ucstreammem_close:
	        rs = op->iclose() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (ucstream_co::operator) */


