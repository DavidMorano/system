/* dprint HEADER (LIBU - Debug-Print) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* version %I% last-modified %G% */
/* C-language defines */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DPRINT_INCLUDE
#define	DPRINT_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


extern int dprint	(cc *,cc *,int = -1) noex ;
extern int dprintln	(cc *,cc *,int = -1) noex ;
extern int dprintf	(cc *,cc *,...) noex ;
extern int dprintstr	(cc *,cc *,cc *,int = -1) noex ;


#define DPRINT(DBUF,DLEN)						\
    if_constexpr (f_debug) {						\
        dprint(__func__,DBUF,DLEN) ;					\
    }

#define DPRINTLN(DBUF,DLEN)						\
    if_constexpr (f_debug) {						\
        dprintln(__func__,DBUF,DLEN) ;					\
    }

#define DPRINTF(FMT, ...) 						\
    if_constexpr (f_debug) {						\
        dprintf(__func__,FMT __VA_OPT__(,) __VA_ARGS__) ;		\
    }

#define DPRINTSTR(str,DBUF,DLEN)					\
    if_constexpr (f_debug) {						\
        dprintstr(__func__,(str),(DBUF),(DLEN)) ;			\
    }


#endif /* __cplusplus (C++ only) */
#endif /* DPRINT_INCLUDE */


