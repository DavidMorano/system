/* usys_onc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Important note:

	On Darwin (MacOS) the header 'sys/random.h' -- which is
	supposed to contain the declaration for the subroutine
	|getentropy(2)| --  is broken when using GCC (G++) as the
	compiler.  It figures.  So I declared the |getentropy(2)|
	subroutine myself below.  This cr*p gets very old after a
	while.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>		/* |MAXNETNAMELEN| */
#include	<usysrets.h>
#include	<usysflag.h>
#include	<localmisc.h>

#include	"usys_onc.hh"


namespace usys {
    sysret_t getnetnamelen() noex {
	return MAXNETNAMELEN ;
    } /* end subroutine (getnetnamelen) */
} /* end namespace (usys) */


