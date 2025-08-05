/* ucstream1 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* implement a map container of blocks (of a given structure) */
/* version %I% last-modified %G% */


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
#include	<sys/stat.h>		/* |dev_t| + |ino_t| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ulogerror.h>
#include	<getoflags.h>
#include	<localmisc.h>

module ucstream ;

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


/* local subroutines (methods) */

int ucstream::open(cchar *fn,cchar *os,mode_t om) noex {
	int		rs = SR_FAULT ;
	if (fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
		if ((rs = getoflags(os)) >= 0) {
		    cint	µof = rs ;
		    if ((rs = uc_open(fn,rs,om)) >= 0) {
			fd = rs ;
			rs = start(fd,0z,0,µof) ;
			if (rs < 0) {
			    uc_close(fd) ;
			    fd = -1 ;
			} /* end if (error) */
		    } /* end if (open) */
		} /* end if (getoflags) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (ucstream::open) */

int ucstream::iclose() noex {
	int		rs = SR_NOTOPEN ;
	int		rs1 ;
	if (fd >= 0) ylikely {
	    rs = SR_OK ;
	    {
		rs1 = finish ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uc_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    fd = -1 ;
	} /* end if (was open) */
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


