/* combinations HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* n-choose-k function WITHOUT repitition */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	COMBINATIONS_INCLUDE
#define	COMBINATIONS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern long combinations	(int,int) noex ;
extern long multicombinations	(int,int) noex ;

EXTERNC_end


#endif /* COMBINATIONS_INCLUDE */


