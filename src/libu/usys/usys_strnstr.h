/* usys_strnstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* C-language library-extension subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef USYSSTRNSTR_INCLUDE
#define	USYSSTRNSTR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#if	defined(SYSHAS_STRNSTR) && (SYSHAS_STRNSTR > 0)
/******************************************************************************/


/******************************************************************************/
#else /* SYSHAS_STRNSTR */
/******************************************************************************/

EXTERNC_begin
extern charp	strnstr(cchar *,cchar *,size_t) noex ;
EXTERNC_end

/******************************************************************************/
#endif /* defined(SYSHAS_STRNSTR) && (SYSHAS_STRNSTR > 0) */


#endif	/* USTSSTRNSTR_INCLUDE */


