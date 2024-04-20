/* mailmsgheadkey SUPPORT */
/* lang=C++20 */

/* extract the key from a mail-msg header field (c-string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

	= 2024-05-20, David A-D- Morano
	In review, I could not find any subroutines (anywhere) that
	still use this subroutine.  I do not know what happened,
	but there it is.  There might be stuff in a long (lost)
	source code respository (someplace), but I am (currently)
	unaware of any such thing.

*/

/* Copyright © 1998,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailmsgheadkey

	Description:
	This subroutine extracts the key of a header key, if one
	is present in the supplied buffer.  A pointer to the key and
	the length of the key is returned (if a key was present).

	Synopsis:
	int mailmsgheadkey(cchar *sp,int sl,cchar *kpp) noex

	Arguments:
	sp		pointer to start of user supplied buffer
	sl		length of user supplied buffer
	kpp		pointer to pointer of the found field

	Returns:
	>0		length of found key field
	==0		no key was found
	<0		error code (system-return)

	Usage-note:

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"libmailmsg.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static inline bool isspacetab(int ch) noex {
	return ((ch == ' ') || (ch == '\t')) ;
}

static inline bool iskeychar(int ch) noex {
	return (isalnumlatin(ch) || (ch == '-') || (ch == '_')) ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgheadkey(cchar *sp,int sl,cchar **kpp) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (sp && kpp) {
	    cchar	*kp = nullptr ;
	    if (sl < 0) sl = strlen(sp) ;
	    rs = SR_OK ;
/* skip leading white space (not including NLs) */
	    while (sl && isspacetab(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    kp = sp ;
/* skip the non-white space */
	    while (sl && iskeychar(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    kl = (sp - kp) ;
/* skip any trailing whitespace */
	    while (sl && isspacetab(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
/* this character must be a colon (':') or else we did not have a head-key */
	    if (*sp == ':') {
		*kpp = kp ;
	    } else {
		kl = 0 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (mailmsgheadkey) */


