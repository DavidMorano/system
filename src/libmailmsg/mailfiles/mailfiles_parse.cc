/* mailfiles_parse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* subroutine to parse MAILPATH */
/* version %I% last-modified %G% */


/* revision history:

	= 1988-02-01, David A­D­ Morano
	This code was originally written.

	= 1988-02-01, David A­D­ Morano
	This subroutine was modified to not write out anything to
	standard output if the access time of the associated terminal
	has not been changed in 10 minutes.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/************************************************************************

  	Name:
	mailfiles_parse

	Desfcription:
	This subroutine looks at the environment variables MAILPATH
	and parses it into its component mailfiles.  Each mailfile
	is added to the 'mailfiles' object.

	Synposis:
	int mailfiles_parse(mailfiles *mfp,cchar *variable) noex

	Arguments:
	mfp		object pointer
	variable	whatever

	Returns:
	>=0		OK
	<0		error (system-return)

*************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strbrk(3c)| */
#include	<usystem.h>
#include	<cfdec.h>
#include	<strx.h>
#include	<localmisc.h>

#include	"mailfiles.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailfiles_parse(mailfiles *op,cchar *mailpath) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ;
	if ((rs = mailfiles_magic(op,mailpath)) >= 0) {
	    bool	fdone = false ;
	    cchar	*cp = mailpath ;
	    for (cc *tp ; (tp = strbrk(cp,":?")) != np ; ) {
		if ((tp - cp) >= 0) {
		    cint	cl = intconv(tp - cp) ;
	            rs = mailfiles_add(op,cp,cl) ;
		    c += (rs < INT_MAX) ;
	            if (*tp == '?') fdone = true ;
		}
	        cp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (! fdone) && cp[0]) {
	        rs = mailfiles_add(op,cp,-1) ;
		c += (rs < INT_MAX) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailfiles_parse) */


