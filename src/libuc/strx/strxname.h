/* strxname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRXNAME_INCLUDE
#define	STRXNAME_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<string.h>		/* CSTD |strchr(3c)| + |strpbrk(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

/* deprecated (can give erroneous result) */
[[deprecated("can give erroneous result")]] 
extern char *strbasename(cchar *) noex ;

/* deprecated (writes to source string) */
[[deprecated("writes to source string")]] 
extern char *strdirname(char *) noex ; 

EXTERNC_end


#endif /* STRXNAME_INCLUDE */


