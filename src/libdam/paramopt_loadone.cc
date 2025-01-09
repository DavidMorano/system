/* paramopt_loadone SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load one parameter (similar to 'paramopt_loadu(3dam)' but special) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	paramopt_loadone

	Description:
	This is very similar to |paramopt_loadu(3dam)| but it allows
	for a special separator character between the key and the
	data values; the special separator character is ':'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<nulstr.h>
#include	<six.h>
#include	<char.h>
#include	<localmisc.h>

#include	"paramopt.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int paramopt_loadone(paramopt *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		i = 0 ;
	if ((rs = paramopt_magic(op,sp)) >= 0) {
	    if (sl <= 0) sl = strlen(sp) ;
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if (int si ; (si = sibreak(sp,sl," \t=,")) >= 0) {
	        cchar	*keyname ;
	        if (nulstr kn ; (rs = kn.start(sp,i,&keyname)) >= 0) {
	            sp += si ;
	            sl -= si ;
	            while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	                sp += 1 ;
	                sl -= 1 ;
	            }
	            if ((sl > 0) && ((*sp == '=') || (*sp == ':'))) {
	                sp += 1 ;
	                sl -= 1 ;
	            }
		    {
	                rs = paramopt_load(op,keyname,sp,sl) ;
		        i = rs ;
		    }
	            rs1 = kn.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (paramopt_loadone) */


