/* usupport_snadd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* append a c-string to a given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	snadd

	Description:
	Ths object is used to concatenate a new c-string to
	an existing counted c-string.

	Notes:
	This subroutine should be the same as (the older)
	|storebuf_str(3uc)|.

******************************************************************************/

#ifndef	USUPPORTSNADD_INCLUDE
#define	USUPPORTSNADD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uconcepts.hh>		/* LIBU */


namespace libu {
    extern int snaddx(char *,int,int,int,...) noex ;
    extern int snaddw(char *,int,int,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int snadd(char *dbuf,int dlen,int dl,cchar *sp,int sl = -1) noex {
	return snaddw(dbuf,dlen,dl,sp,sl) ;
    } /* end subroutine (snadd) */
} /* end namespace (libu) */

namespace libu {
    template<conccharp ... Args>
    inline int snadd(char *dbuf,int dlen,int dl,Args ... args) noex {
	cint		na = npack(Args) ;
	return snaddx(dbuf,dlen,dl,na,args ...) ;
    } /* end subroutine (snadd) */
} /* end namespace (libu) */

namespace libu {
    int snaddslash(char *,int,int) noex ;
    int snaddpath(char *,int,int,cchar *,int = -1) noex ;
} /* end namespace (libu) */


#endif /* USUPPORTSNADD_INCLUDE */


