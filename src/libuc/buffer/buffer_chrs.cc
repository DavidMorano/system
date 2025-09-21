/* buffer_chrs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* character insertion options for BUFFER object */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-06-24, David A­D­ Morano
	This was separated out from TERMSTR.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	buffer_chrs
	buffer_blanks
	buffer_backs

	Description:
	Store a given number of characters into the "buffer."

	Synopsis:
	int buffer_blanks(buffer *op,int ch,int n) noex

	Arguments:
	op		object pointer
	ch		character to insert
	n		<n> blanks

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"buffer.h"

import libutil ;
import uconstants ;

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct blanker {
	cint	l = lenstr(sysword.w_blanks) ;
	cchar	*p = sysword.w_blanks ;
    } ; /* end struct (blanker) */
} /* end namespace */


/* forward references */


/* local variables */

static blanker		bo ;		/* "blank" object */


/* exported variables */


/* exported subroutines */

int buffer_chrs(buffer *op,int ch,int n) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	while ((rs >= 0) && (n-- > 0)) {
	    rs = buffer_chr(op,ch) ;
	    len += rs ;
	} /* end while */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_chrs) */

int buffer_blanks(buffer *op,int n) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	while ((rs >= 0) && (n > 0)) {
	    cint	m = min(n,bo.l) ;
	    rs = buffer_strw(op,bo.p,m) ;
	    n -= m ;
	    len += rs ;
	} /* end while */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_blanks) */


