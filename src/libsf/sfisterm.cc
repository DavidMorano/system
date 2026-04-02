/* sfisterm SUPPORT */
/* charset=ISO8859-1 */
/* lang=++C20 */

/* Safe-Fast Is-a-Terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfisterm

	Description:
	This subroutine determines if the given Safe-Fast stream is
	a terminal or not.

	Synopsis:
	int sfisterm(Sfio_t *fp) noex

	Arguments:
	fp		pointer to Shio_t object

	Returns:
	<0		error
	==0		NO (not a terminal)
	>0		YES (a terminal)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ast.h>			/* configures other stuff also */
#include	<sys/types.h>
#include	<unistd.h>		/* |isatty(3c)| */
#include	<sfio_s.h>
#include	<sfio.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfisterm(Sfio_t *fp) noex {
	int		fisterm = -1 ;
	if (fp) {
	    fisterm = (! (fp->_flags & SF_STRING)) ;
	    fisterm = fisterm && (fp->_file >= 0) ;
	    if (fisterm) {
		fisterm = isatty(fp->_file) ;
	    }
	} /* end if (non-null) */
	return fisterm ;
}
/* end subroutine (sfisterm) */


