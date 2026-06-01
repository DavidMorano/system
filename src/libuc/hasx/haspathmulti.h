/* haspathmulti HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* has the given path (counted c-string) have a multi-path variable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASPATHMULTI_INCLUDE
#define	HASPATHMULTI_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#ifndef	DECLARATION_HASPATHMULTI
#define	DECLARATION_HASPATHMULTI
EXTERNC_begin
extern bool	haspathmulti(cchar *,int) noex ;
EXTERNC_end
#endif /* DECLARATION_HASPATHMULTI */


#endif /* HASPATHMULTI_INCLUDE */


