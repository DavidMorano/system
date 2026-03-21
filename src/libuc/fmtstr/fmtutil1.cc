/* fmtutil1 MODEUL (module-implementation-unit) */
/* charset=ISO8859-1 */
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
	fmtutil

	Description:
	This is the support portion of the FMTUTIL module.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

#include	"fmtutil.h"

module fmtutil ;


bool isourbad(int ch) noex {
    	return (! isprintterm(ch)) ;
}


