/* bbspec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* load a bible-book-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-07, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	BBSPEC_INCLUDE
#define	BBSPEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* for the signed special types */


#define	BBSPEC		struct bbspec


struct bbspec {
	cchar		*namp ;
	int		naml ;
	schar		b, c, v ;
} ; /* end struct */

EXTERNC_begin

extern int bbspec_load(bbspec *,cchar *,int) noex ;

EXTERNC_end


#endif /* BBSPEC_INCLUDE */


