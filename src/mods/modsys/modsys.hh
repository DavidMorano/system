/* modsys HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MODSYS_INCLUDE
#define	MODSYS_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


#define MODPRINTF(FMT, ...) 						\
    if_constexpr (f_debug) {						\
        fprintf(stderr,"%s: ",__func__) ;				\
        fprintf(stderr,FMT __VA_OPT__(,) __VA_ARGS__) ;			\
    }


#endif /* __cplusplus */
#endif /* MODSYS_INCLUDE */


