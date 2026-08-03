/* densitryhdr1 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage reading or writing of a density file */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-02-17, David A­D­ Morano
	This code module was inspired from the mail-message-id
	database (which is used to eliminate repeated mail messages).

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	densityx::dbhdr

	Description:
	This module manages the reading and writing of entries in
	a density file.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<serialbuf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"densityhdr.hh"

module densityhdr ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

namespace densityx {
	cint dbhdr::bufsz = (3 * szof(uint)) ;
} /* end namespace (densityx) */


/* exported subroutines */

namespace densityx {
    int dbhdr::rd(char *bufp) noex {
	cint		bufl = bufsz ;
	int		rs ;
	int		rs1 ;
	if (serialbuf mb ; (rs = mb.start(bufp,bufl)) >= 0) ylikely {
	    {
		mb << wcount ;
		mb << wtime ;
		mb << nents ;
	    }
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
    } /* end method (dbhdr::rd) */
    int dbhdr::wr(cchar *bufp) noex {
	cint		bufl = bufsz ;
	int		rs ;
	int		rs1 ;
	char		*buf = cast_const<charp>(bufp) ;
	if (serialbuf mb ; (rs = mb.start(buf,bufl)) >= 0) ylikely {
	    {
		mb >> wcount ;
		mb >> wtime ;
		mb >> nents ;
	    }
	    rs1 = mb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
    } /* end method (dbhdr::wr) */
} /* end namespace (densityx) */


/* local subroutines */

namespace densityx {
    dbhdr::dbhdr() noex {
	wcount	= 0 ;
	wtime	= 0 ;
	nents	= 0 ;
    } /* end ctor */
    dbhdr::dbhdr(const dbhdr &o) noex {
	wcount	= o.wcount ;
	wtime	= o.wtime ;
	nents	= o.nents ;
    } /* end ctor */
    const dbhdr &dbhdr::operator = (const dbhdr &o) noex {
	wcount	= o.wcount ;
	wtime	= o.wtime ;
	nents	= o.nents ;
	return *this ;
    } /* end method (dbhdr::assignment) */
} /* end namespace (densityx) */


