/* mklineclean HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* clean up a line of text w/ options */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mklineclean

	Description:
	This subroutine cleans up a line of text.  A mode paramter
	specifies how clean the resulting line gets.  To wit:

	mode	delete­or­sub	terminate­on­nul
	----------------------------------------
	0	delete		YES
	1	sub		YES
	2	delete		NO
	3	sub		NO

	Synopsis:
	int mklineclean(char *rbuf,int rlen,int m,cchar *lp,int ll) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	m		mode parameter (see above on use of mode)
	lp		source counted c-string pointer
	ll		source counted c-string length

	Returns:
	-		length of resulting line (not NL terminated)

	Important note:
	Resulting lines are never NL (EOL) terminated - even if the
	input line was!

*******************************************************************************/

#ifndef	MKLINECLEAN_INCLUDE
#define	MKLINECLEAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<linecleanopt.h>	/* line-cleaning options */


EXTERNC_begin

extern int	mklineclean(char *,int,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MKLINECLEAN_INCLUDE */


