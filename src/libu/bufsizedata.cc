/* bufsizedata SUPPORT */
/* lang=C++20 */

/* buffer-size-data */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides the database from which decisions are made
	for determing buffer sizes.  A decision about how to determine
	any particular buffer size is make at the point when a particular
	buffer size is required.  This data base takes the most simplest
	of forms: an array of structures.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<climits>
#include	<usystem.h>
#include	<localmisc.h>

#include	"getbufsizenames.h"
#include	"usysdefs.h"
#include	"bufsizedata.hh"


/* external subroutines */


/* external variables */


/* local subroutines */


/* local sttructures */


/* local subroutines */


/* NOTHING */


