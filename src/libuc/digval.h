/* digval HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the value of a single numeric digit */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	digval
	digval{x}

	Description:
	We examine a single character, of some number base, and
	return either an error or the integer value of the digit.
	For example, the hexadecimal character-digit 'b' would
	return the decimal value '11'.  The maximum allowed
	number base is 64.

	Returns:
	>=0	valid value
	<0	error (SR_DOM)

	Notes:
	1. The maximum allowed number base is 64.

*******************************************************************************/

#ifndef	DIGVAL_INCLUDE
#define	DIGVAL_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int	digval(int) noex ;	/* <- base 64 */
EXTERNC_end

EXTERNC_begin
extern int	digvalbin(int) noex ;	/* <- base 2 */
extern int	digvaloct(int) noex ;	/* <- base 8 */
extern int	digvaldec(int) noex ;	/* <- base 10 */
extern int	digvalhex(int) noex ;	/* <- base 16 */
EXTERNC_end


#endif /* DIGVAL_INCLUDE */


