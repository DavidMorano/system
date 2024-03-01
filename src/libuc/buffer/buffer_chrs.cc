/* buffer_chrs SUPPORT */
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
#include	<climits>
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| */
#include	<usystem.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"buffer.h"


/* local defines */


/* local namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    int	buffer_chrs(buffer *,int,int) noex ;
    int	buffer_blanks(buffer *,int) noex ;
    int	buffer_backs(buffer *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static constexpr char		blanks[] = "        " ;


/* exported variables */


/* exported subroutines */

int buffer_chrs(buffer *bp,int ch,int n) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	while ((rs >= 0) && (n-- > 0)) {
	    rs = buffer_char(bp,ch) ;
	    len += rs ;
	} /* end while */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_chrs) */

int buffer_blanks(buffer *bp,int n) noex {
	cint		nblanks = sizeof(blanks) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	while ((rs >= 0) && (n > 0)) {
	    cint	m = min(n,nblanks) ;
	    rs = buffer_strw(bp,blanks,m) ;
	    n -= m ;
	    len += rs ;
	} /* end while */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (buffer_blanks) */

int buffer_backs(buffer *bp,int n) noex {
	return buffer_chrs(bp,CH_BS,n) ;
}
/* end subroutine (buffer_backs) */


