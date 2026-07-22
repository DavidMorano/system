/* atox HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a decimal c-string to an integer type */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-03, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ATOX_INCLUDE
#define	ATOX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern slonglong	atosll(cchar *) noex ;
extern ulonglong	atoull(cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern slonglong	atosll(cchar *,int) noex ;
extern ulonglong	atoull(cchar *,int) noex ;
#endif /* __cplusplus */


#endif /* ATOX_INCLUDE */


