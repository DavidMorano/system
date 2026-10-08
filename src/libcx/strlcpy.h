/* strlcpy HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* C-language library-extension subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef STRLCPY_INCLUDE
#define	STRLCPY_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin
extern size_t strlcpy(char *,cchar *,size_t) noex ;
EXTERNC_end


#endif	/* STRLCPY_INCLUDE */


