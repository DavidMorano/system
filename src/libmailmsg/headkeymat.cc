/* headkeymat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match on mail-message header keys */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	Module was originally written.  This was written as part of
	the PCS mailer code cleanup!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	headkeymat

	Description:
	Given a mail-message header key name 'key' such as "Subject",
	find if it is present in the user supplied string given as
	'hbuf'.  Return 0 if there is no match, else we return the
	character position of the start of the header value string.
	The match is case independent.

	Synopsis:
	int headkeymat(cc *key,cc *sp,int sl) noex

	Arguments:
	key		key name (c-string) to test for
	sp		c-string to match pointer
	sl		c-string to match length

	Returns:
	>0		the key was found and the position of the 
			value (not the key) in the user supplied
			string under test is returned
	==0		the key was not found
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<strn.h>
#include	<sfx.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"headkeymat.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int headkeymat(cchar *key,cchar *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (key && hbuf) {
	    rs = SR_OK ;
	    if (char *tp ; (tp = strnochr(hbuf,hlen,':')) != nullptr) {
		cint	vi = intconv((tp + 1) - hbuf) ;
		cint	hl = intconv(tp - hbuf) ;
	        cchar	*cp{} ;
		if (int cl ; (cl = sfshrink(hbuf,hl,&cp)) > 0) {
		    if (strwcmp(key,cp,cl) == 0) {
			len = vi ;
		    }
		}
	    } /* end if (had ':') */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (headkeymat) */


