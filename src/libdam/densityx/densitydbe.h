/* densitydbe HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* density DB entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	DENSITYDBE_INCLUDE
#define	DENSITYDBE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DENSITYDBE		struct densitydbe_head

struct densitydbe_head {
	uint		count ;		/* count */
	uint		utime ;		/* update time */
} ;

typedef	DENSITYDBE	densitydbe ;

EXTERNC_begin

extern int densitydbe_rd(densitydbe *,char *,int) noex ;
extern int densitydbe_wr(densitydbe *,cchar *,int) noex ;
extern int densitydbe_entsz(densitydbe *) noex ;

EXTERNC_end


#endif /* DENSITYDBE_INCLUDE */


