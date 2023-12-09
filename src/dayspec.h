/* dayspec INCLUDE */
/* lang=C20 */

/* load a day-specification */
/* version %I% last-modified %G% */


/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

#ifndef	DAYSPEC_INCLUDE
#define	DAYSPEC_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


#define	DAYSPEC		struct dayspec_head


struct dayspec_head {
	short		y ;
	schar		m, d ;
} ;

typedef struct dayspec_head	dayspec ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int dayspec_default(DAYSPEC *) noex ;
extern int dayspec_load(DAYSPEC *,cchar *,int) noex ;
extern int dayspec_yday(DAYSPEC *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* DAYSPEC_INCLUDE */


