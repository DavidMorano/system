/* mailmsgheadkey SUPPORT */
/* lang=C++20 */

/* extract the key from a mail-msg header field (c-string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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
	==0		an empty (zero-length) key was found
	<0		no key was found (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"libmailmsg.h"


/* local defines */

#define	ISSPACETAB(c)	isspacetab(c)
#define	ISKEYCHAR(c)	iskeychar(c)


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
	int		klen = 0 ;
	if (sp && kpp) {
	    int		kl ;
	    bool	f_len = (sl >= 0) ;
	    rs = SR_OK ;
/* skip leading white space (not including NLs) */
	    while (((! f_len) || (sl > 0)) && ISSPACETAB(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    *kpp = charp(sp) ;
/* skip the non-white space */
	    while ((((! f_len) && *sp) || (sl > 0)) && ISKEYCHAR(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    kl = (sp - (*kpp)) ;
/* skip any trailing whitespace */
	    while (((! f_len) || (sl > 0)) && ISSPACETAB(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
/* this character must be a colon (':') or else we did not have a head-key */
	    klen = (*sp == ':') ? kl : -1 ;
	} /* end if (non-null) */
	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mailmsgheadkey) */


