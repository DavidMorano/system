/* biblecite HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* bible-citation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLECITE_INCLUDE
#define	BIBLECITE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	BIBLECITE	struct biblecite_head


struct biblecite_head {
	uint	b, c, v ;
} ; /* end struct */

typedef	BIBLECITE	biblecite ;

EXTERNC_begin

extern int	biblecite_ver(biblecite *,cchar *,int,int *) noex ;

EXTERNC_end


#endif /* BIBLECITE_INCLUDE */


