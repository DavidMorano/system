/* mapex INCLUDE */
/* lang=C20 */

/* map status-return (SR) values to program exit-codes (EX) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is basically an independent repeat of the same code located
	in |localmisc.h|.  That code should be dropped and instead just include
	this header file there instead.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAPEX_INCLUDE
#define	MAPEX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>


#ifndef	MAPEX
#define	MAPEX		struct mapex
#endif


#ifndef	STRUCT_MAPEX
#define	STRUCT_MAPEX
struct mapex {
	int	rs, ex ;
} ;
#endif /* STRUCT_MAPEX */


#ifndef	EXTERN_MAXEX
#define	EXTERN_MAXEX

#ifdef	__cplusplus
extern "C" {
#endif

extern int	mapex(const MAXPEX *,int) noex ;

#ifdef	__cplusplus
}
#endif

#endif /* EXTERN_MAXPEX */


#endif /* MAPEX_INCLUDE */


