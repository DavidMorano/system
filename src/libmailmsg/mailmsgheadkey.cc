/* mailmsgheadkey SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* extract the key from a mail-msg header field (c-string) */
/* version %I% last-modified %G% */

#define	CF_PROGRESSIVE		0	/* use progressive access */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

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
	This subroutine extracts the key of a mail-message header
	field, if one is present in the supplied buffer.  A pointer
	to the key and the length of the key is returned (if a key
	was present).

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<strn.h>
#include	<sfx.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"libmailmsg.h"


/* local defines */

#ifndef	CF_PROGRESSIVE
#define	CF_PROGRESSIVE		0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

constexpr bool		f_progressive = CF_PROGRESSIVE ;


/* exported variables */


/* exported subroutines */

int mailmsgheadkey(cchar *sp,int sl,cchar **kpp) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (sp && kpp) {
	    rs = SR_OK ;
	    if_constexpr (f_progressive) {
	        cchar	*kp = nullptr ;
	        while (sl && isspacetab(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        kp = sp ;
	        while (sl && ishdrkey(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        kl = (sp - kp) ;
	        while (sl && isspacetab(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        if (*sp == ':') {
		    *kpp = kp ;
	        } else {
		    kl = 0 ;
	        }
	    } else {
	        if (char *tp ; (tp = strnochr(sp,sl,':')) != nullptr) {
	            cchar	*cp{} ;
		    if (int cl ; (cl = sfshrink(sp,(tp-sp),&cp)) > 0) {
		        if (hasallhdrkey(cp,cl)) {
		            *kpp = cp ;
			    kl = cl ;
		        }
		    }
	        } /* end if (had ':') */
	    } /* end if_constexpr (f_progressive) */
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (mailmsgheadkey) */


