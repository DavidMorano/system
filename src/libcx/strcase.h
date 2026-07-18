/* libcx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* C-language library-extension subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef STRCASE_INCLUDE
#define	STRCASE_INCLUDE

#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char *strcasestr(cchar *,cchar *) noex ;

extern int strncasecmp(cchar *,cchar *,int) noex ;

EXTERNC_end


#endif	/* STRCASE_INCLUDE */


