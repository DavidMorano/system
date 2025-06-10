/* fmtopt SUÞPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* options for |fmtstr(3uc)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtopt

	Description:
	This is the support portion pf the FMTOPT object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cwchar>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

#include	"fmtopt.h"


cint	fmtoptms::clean		= (1 << fmtopt_clean) ;
cint	fmtoptms::noover	= (1 << fmtopt_nooverr) ;


