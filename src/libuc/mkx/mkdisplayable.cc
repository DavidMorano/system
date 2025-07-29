/* mkdisplayable SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* clean up some text string for printing to terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-20, David A­D­ Morano
	I couldn't find something like this already so I popped one.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine will scan the source data looking for
	displayable (like on a terminal) characters. Displayable
	characters are transferred to the result buffer as space
	permits.

	Synopsis:
	int mkdisplayable(char *rbuf,int rlen,cchar *sbuf,int slen) noex

	Arguments:
	rbuf	result buffer
	rlen	result buffer length
	sbuf	source buffer
	slen	source buffer length

	Returns:
	>=0	length of transfered characters
	<0	error code (system-return)

	Notes:
	Implementation note: Remeber that all signed 'char's get
	promoted to stupid signed values on subroutine calls (whether
	the subroutine argument is signed or unsigned)!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sbuf.h>
#include	<ascii.h>		/* |CH_{xx}| */
#include	<mkchar.h>
#include	<ischarx.h>		/* |isprintlatin(3uc)| */
#include	<localmisc.h>

#include	"mkdisplayable.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkdisplayable(char *rbuf,int rlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf && sp) ylikely {
	    if (sbuf s ; (rs = s.start(rbuf,rlen)) >= 0) ylikely {
	        int	pch = 0 ;
	        bool	f_shift = false ;
		if (sl < 0) sl = lenstr(sp) ;
	        for (int i = 0 ; (i < sl) && sp[i] ; i += 1) {
	            cint	ch = mkchar(sp[i]) ;
	            switch (ch) {
	            case CH_SI:
	            case CH_SO:
	            case CH_SS2:
	            case CH_SS3:
		        f_shift = true ;
	                pch = ch ;
		        break ;
	            default:
	                if (isprintlatin(ch)) {
		            if (f_shift) {
			        f_shift = false ;
	    		        rs = s.chr(pch) ;
		            }
			    if (rs >= 0) {
	                        rs = s.chr(ch) ;
			    }
	                } else {
	                    rs = s.blanks ;
		        }
		        break ;
	            } /* end switch */
	        } /* end while */
            	rs1 = s.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkdisplayable) */


