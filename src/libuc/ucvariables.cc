/* ucvariables SUPPORT */
/* lang=C++20 */

/* define various sytem (global) variables */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David D-A- Morano
	This subroutine was written for Rightcore Network Services.

	= 2014-06-27, David D-A- Morano
	I updated this to use the 'constexpr' capability of C++14.

*/

/* Copyright © 2001,2014 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	We define some system (global) variables in this module.
	This code (below) is some sort of rather lame attempt at
	putting most of the most important global variables used
	by the (rather) low-level facilities of the either the UNIX©
	system or the UNIX© system C-language library into a single
	place.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<climits>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"ucvariables.hh"


/* local defines */


/* typedefs */


/* external variables */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


/* exported (global) variables */

constinit const varnames	varname ;
constinit const valuelimits	valuelimit ;
constinit const digbufsizes	digbufsize ;


