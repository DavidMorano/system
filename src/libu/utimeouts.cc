/* utimeouts */
/* lang=C++20 */

/* some helper management for UNIX® kernal-call timeouts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module manages UNIX® system kernel-call timeouts.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"utimeout.h"
#include	"utimeouts.h"
#include	"utimeoutdefs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* global variables */


/* local strutures */

namespace {
    struct utimeouts {
	constexpr utimeouts() noex ;
    } ; /* end class (utimeouts) */
}


/* forward references */


/* local variables */

static utimeouts	utimeouts_data ;


/* exported subroutines */


/* local subroutines */

constexpr utimeouts::utimeouts() noex {
	for (int i = 0 ; i < uto_overlast ; i += 1) {
	    if (utimeout[i] == 0) utimeout[i] = UTO_DEFAULT ;
	}
} /* end constructor (utimeouts) */


