/* dprint HEADER */
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
#include	<cstddef>
#include	<cstdlib>
#include	<cstdio>		/* |stderr| */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin
extern int dprintline(cc *,cc *,int) noex ;
EXTERNC_end

#define DPRINTR(FMT, ...) 						\
    if_constexpr (f_debug) {						\
        fprintf(stderr,FMT __VA_OPT__(,) __VA_ARGS__) ;			\
    }

#define DPRINTF(FMT, ...) 						\
    if_constexpr (f_debug) {						\
        fprintf(stderr,"%s: ",__func__) ;				\
        fprintf(stderr,FMT __VA_OPT__(,) __VA_ARGS__) ;			\
    }

#define DPRINTLINE(DBUF,DLEN)						\
    if_constexpr (f_debug) {						\
        dprintline(__func__,DBUF,DLEN) ;				\
    }


#endif /* __cplusplus (C++ only) */
#endif /* DPRINT_INCLUDE */


