/* utimeoutdefs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (MUST be C-lang) */

/* manage UNIX® kernal-call (and std-c library) timeouts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Data:
	utimeout

	Description:
	This module (in part) manages UNIX® system kernel-call timeouts.

	Note:
	1. This module MUST be coded in the C-language!  We need
	C-language features that are NOT in (brain-damaged) C++.
	Also, this module (translaton unit) MUST be loaded (by the
	run-time dynamic linker) into memory before the 'utimeouts'
	(note the 's' on the end) module so that proper run-time
	initialization of the timeout values can occur.  As noted
	elsewhere, neither the C language nor the C++ language offer
	the necessary capabilies to create the type of initialization
	environment I really want.  So the current arrangement using
	both C and C++ is a total hack!  More information (notes)
	on this whole "timeout" facility is located in the source
	language file 'utimeoutdefs.h'.
	2. Use C++ you say? Well, C++ does NOT have the type of
	initialization feature that is used below in this C-language
	source file (the file you are reading right now).  So C++
	can suck cock eggs for all the good it does.  As an aside,
	earlier C++ implementation DID allow for the type of
	initialization used below, but it was a secretly added
	feature that was not standard.  The C++ international ISO
	standards committee eventually cracked down on this fringe
	radical non-standard feature of C++ and the compiler people
	eventually followed suite and are now disallowing this.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"utimeout.h"
#include	"utimeoutdefs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


