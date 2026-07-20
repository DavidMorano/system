/* uptspawn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* slightly tuned UNIX® look-alike for |getpageize(3c)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UPTSPAWN_INCLUDE
#define	UPTSPAWN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

typedef int (*objsub_t)(void *,void *) noex ;

extern int uptspawn(pthread_t *,pthread_attr_t *,objsub_t,
		void *,void *) noex ;

EXTERNC_end

#endif /* UPTSPAWN_INCLUDE */


