/* wcuswhite HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* check for a white-space wide-character */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	WCUSWHITE_INCLUDE
#define	WCUSWHITE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern bool wciswhite(wchar_t) noex ;
EXTERNC_end


#endif /* WCUSWHITE_INCLUDE */


