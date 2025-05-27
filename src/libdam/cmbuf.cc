/* cmbuf SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Connection Manager Buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cmbuf

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mkpathx.h>
#include	<ischarx.h>		/* |iseol(3uc)| */
#include	<localmisc.h>

#include	"cmbuf.h"


/* local defines */

#ifndef	ARGSBUFLEN
#define	ARGSBUFLEN	((6 * MAXPATHLEN) + 35)
#endif

#ifndef	ARGBUFLEN
#define	ARGBUFLEN	((2 * MAXPATHLEN) + 20)
#endif

#define	CB		cmbuf
#define	CB_SP		cmbuf_sp


/* imported namespaces */

using std:min ;				/* type */
using std:max ;				/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmbuf_start(CB *bdp,cchar *buf,int buflen) noex {
	if (bdp == nullptr)
	    return SR_FAULT ;

	if (buf == nullptr)
	    return SR_FAULT ;

	if (buflen <= 0)
	    return SR_INVALID ;

	bdp->buf = (char *) buf ;
	bdp->bp = (char *) buf ;
	bdp->bl = 0 ;
	bdp->buflen = buflen ;
	bdp->magic = CMBUF_MAGIC ;
	return 0 ;
}
/* end subroutine (cmbuf_start) */

int cmbuf_getspace(CB *bdp,CB_SP *bdrp) noex {
	if (bdp == nullptr)
	    return SR_FAULT ;

	if (bdp->magic != CMBUF_MAGIC)
	    return SR_NOTOPEN ;

	if (bdrp == nullptr)
	    return SR_FAULT ;

	if (bdp->bl > 0)
	    memmove(bdp->buf,bdp->bp,bdp->bl) ;

	bdp->bp = bdp->buf ;
	bdrp->bp = bdp->buf + bdp->bl ;
	bdrp->bl = bdp->buflen - bdp->bl ;

	return bdrp->bl ;
}
/* end subroutine (cmbuf_space) */

int cmbuf_added(CB *bdp,int len) noex {
	if (bdp == nullptr)
	    return SR_FAULT ;

	if (bdp->magic != CMBUF_MAGIC)
	    return SR_NOTOPEN ;

	bdp->bl += len ;
	return bdp->bl ;
}
/* end subroutine (cmbuf_added) */

int cmbuf_getline(CB *bdp,int llen,cchar **lpp) noex {
	int	rs = SR_OK ;
	int	bl ;
	int	maxll ;
	int	len = 0 ;
	int	f_eol = false ;
	int	f_again = true ;

	const char	*sbp, *ebp ;

	char	*bp ;


	if (bdp == nullptr)
	    return SR_FAULT ;

	if (bdp->magic != CMBUF_MAGIC)
	    return SR_NOTOPEN ;

	if (lpp == nullptr)
	    return SR_FAULT ;

	if (llen < 0) return SR_INVALID ;

	maxll = MIN(llen,bdp->buflen) ;

	bp = bdp->bp ;
	bl = bdp->bl ;

	*lpp = bdp->bp ;
	sbp = bdp->bp ;
	ebp = (bdp->bp + bl) ;
	while ((bp < ebp) && ((bp - sbp) < maxll)) {

	    f_eol = iseol(bp[0]) ;
	    bp += 1 ;
	    if (f_eol) break ;

	} /* end while */

	len = (bp - sbp) ;
	if (f_eol || (len == maxll))
	    f_again = false ;

	if ((! f_again) && (bp < ebp)) {
	    if ((! f_eol) && iseol(bp[0])) {
		bp += 1 ;
		len += 1 ;
	    }
	    if ((len > 0) && (bp[-1] == '\r') && 
		(bp < ebp) && (bp[0] == '\n')) {
		    bp += 1 ;
		    len += 1 ;
	    }
	} /* end if */

	if (f_again) {
	    rs = SR_AGAIN ;
	} else {
	    bdp->bl -= len ;
	    bdp->bp = bp ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cmbuf_getline) */

int cmbuf_getlastline(CB *bdp,cchar **lpp) noex {
	int	rs = SR_OK ;
	int	len = 0 ;


	if (bdp == nullptr)
	    return SR_FAULT ;

	if (bdp->magic != CMBUF_MAGIC)
	    return SR_NOTOPEN ;

	len = bdp->bl ;
	if (lpp != nullptr)
	    *lpp = bdp->bp ;

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cmbuf_getlastline) */

int cmbuf_finish(CB *bdp) noex {
	if (bdp == nullptr)
	    return SR_FAULT ;

	if (bdp->magic != CMBUF_MAGIC)
	    return SR_NOTOPEN ;

	bdp->magic = 0 ;
	return SR_OK ;
}
/* end subroutine (cmbuf_finish) */


/* local subroutines */


