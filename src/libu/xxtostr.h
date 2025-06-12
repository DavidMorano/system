/* xxtostr HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* subroutines to convert an integer to a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

	= 2024-01-15, David A-D- Morano
	Added a comment below.

*/

/* Copyright (c) 1998,2024 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	{x}tostr

	Description:
	These subroutines convert integers (either signed or unsigned
	of various sized types) into their c-string digit
	representations.

	The following subroutines are provoided for base-20 conversions
	of the various types:

	name		signed	size
	itostr		yes	32-bit
	ltostr		yes	64-bit
	lltostr		yes	128-bit
	uitostr		no	32-bit
	ultostr		no	64-bit
	ulltostr	no	128-bit

	The two subroutine-templates (below) can convert for any
	number base.  The subroutine-templates are:

	name		signed	size	base
	sxxtostr	yes	any	any up to base-64
	uxxtostr	no	any	any up to base-64

	Rnjoy.

	Notes:
	As you may know, aside from the more recent standardization
	of the "convert-to-string" numeric conversion subroutine-templates
	in C++, there has never been a standardized implementation
	of these kinds of conversion functions.
		-- 2024-01-15, David A-D- Morano

*******************************************************************************/

#ifndef	XXTOSTR_INCLUDE
#define	XXTOSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>		/* extended integer types */


EXTERNC_begin

extern char *itostr(int,char *) noex ;
extern char *ltostr(long,char *) noex ;
extern char *lltostr(longlong,char *) noex ;

extern char *uitostr(uint,char *) noex ;
extern char *ultostr(ulong,char *) noex ;
extern char *ulltostr(ulonglong,char *) noex ;

EXTERNC_end


#endif /* XXTOSTR_INCLUDE */


