/* pwentry */

/* some miscellaneous PWENTRY subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A-D- Morano
	This program was originally written.

	= 2018-12-21, David A.D. Morano
	Added |getpwentrybufsize(3dam)|.

*/

/* Copyright © 1998,2018 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	We provide some miscelleneous utility subroutines for the PWENTRY
	object.

	Synopsis:
	int getpwentrybufsize(void)

	Arguments:
	-

	Returns:
	>=0		size of necessary buffer to hold the data for 
			a PWENTRY object
	<0		error (yes, due to error from |getbufsize(3uc)|).


*******************************************************************************/


#define	PWENTRY_MASTER	0


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<localmisc.h>

#include	"pwentry.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int getpwentrybufsize() noex {
	int		rs ;
	int		pwlen = 0 ;
	if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	    pwlen += rs ;
	    if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	        pwlen += rs ; /* additional size for some extra stuff */
	    }
	}
	return (rs >= 0) ? pwlen : rs ;
}
/* end subroutine (pwentry_bufsize) */

/* legacy only */
int pwentry_bufsize() noex {
	return getpwentrybufsize() ;
}


