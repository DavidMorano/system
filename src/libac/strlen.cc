/* strlen SUPPORT */
/* lang=C++20 */

/* calculate the length of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1982-09-10, David A­D­ Morano
	This subroutine was written because I need this on our own
	embedded (VMS CPU) platform.

*/

/* Copyright © 1992 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is a knock off of the |strlen(3c)| from the
	regular UNIX® system.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* external subroutines */

size_t strlen(const char *s) noex {
	size_t		len = 0 ;
	while (*s++) len += 1 ;
	return len ;
}
/* end subroutine (strlen) */


