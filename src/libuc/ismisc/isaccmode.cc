/* isaccmode SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does a file access-mode have a property? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	isaccmode_readable
	isaccmode_writable

	Description:
	These subroutines determine if a file access-mode has
	a property.

  	Synopsis:
	bool isaccmode_readable(int am) noex
	bool isaccmode_writable(int am) noex

	Arguments:
	am		file access-mode

	Returns:
	true		has property
	false		does not have property

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"isaccmode.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

isaccmoder		isaccmode ;


/* exported subroutines */

bool isaccmode_readable(int am) noex {
    	am &= O_ACCMODE ;
	return ((am == O_RDONLY) || (am == O_RDWR)) ;
} /* end subroutine (isaccmode_readable) */

bool isaccmode_writable(int am) noex {
    	am &= O_ACCMODE ;
	return ((am == O_WRONLY) || (am == O_RDWR)) ;
} /* end subroutine (isaccmode_writable) */

bool isaccmoder::rd(int am) const noex {
    	return isaccmode_readable(am) ;
} /* end method */

bool isaccmoder::wr(int am) const noex {
	return isaccmode_writable(am) ;
} /* end method */


