/* deb HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* debugging support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	deb

	Description:
	This header file is associated with the DEBUG module.
	Access of the DEBUG module is gained with the importation
	of the module; like:
	{
		import deb ;
	}
	Enjoy.

*******************************************************************************/

#ifndef	DEB_INCLUDE
#define	DEB_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define DEBOPEN(fn)	debopen(fn)
#define DEBCLOSE	debclose()

#define DEBPRINTF(FMT, ...)	({ 					\
	int rsdebug = 0 ;						\
        if_constexpr (f_debug) {					\
            rsdebug = debprintf(__func__, FMT __VA_OPT__(,) __VA_ARGS__) ; \
        } ; rsdebug ;							\
    }) /* end macro (DEBPRINTF) */

#define DEBPRINTHEX(cols,sbuf,slen) ({					\
	int rsdebug = 0 ;						\
        if_constexpr (f_debug) {					\
            rsdebug = debprinthex(__func__,cols,sbuf,slen) ;		\
        } ; rsdebug ;							\
    }) /* end macro (DEBPRINTHEX) */


#endif /* __cplusplus */
#endif /* DEB_INCLUDE */


