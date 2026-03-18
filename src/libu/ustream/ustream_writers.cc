/* ustream_writers SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extra write methods for the USTREAM object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ustream_writeblanks

	Description:
	Write a specified number of blanks to a USTREAM object.

	Synopsis:
	int ustream_writeblanks(USTREAM *op,int n)

	Arguments:
	op		pointer to object
	n		number of bytes to write

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)


	Name:
	ustream_writefill

	Description:
	Write enough data (bytes) to fill something.

	Synopsis:
	int ustream_writefill(USTREAM *bp,cchar *sp,int sl) noex

	Arguments:
	bp		USTREAM object pointer
	sp		source buffer
	sl		source buffer length

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)


	Name:
	ustream_writealign

	Description:
	Align the file-pointer to the specified alignment (zero-filling
	as needed).

	Synopsis:
	int ustream_writealign(USTREAM *bp,int align) noex

	Arguments:
	bp		USTREAM object pointer
	align		source buffer length

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)


	Name:
	ustream_writezero

	Description:
	We provide some extra small function for special circumstances.

	Synopsis:
	int ustream_writezero(USTREAM *bp,int size) noex

	Arguments:
	bp		USTREAM object pointer
	size		amount of zeros to write

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)


	Name:
	ustream_writefd

	Description:
	Read a file (until a length-limit or an EOF) as given by a
	file-descriptor and write it to the present buffered-file.

	Synosis:
	int ustream_writefd(ustream *op,char *bp,int bl,int mfd,int len) noex

	Arguments:
	op		the present USTREAM object pointer
	bp		buffer pointer to use as staging
	bl		buffer length to use as staging
	mfd		file-descriptor to read from
	len		length of data to read from given file-descriptor

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"ustream.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;

/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using ustream_ns::ustream_writealign ;	/* subroutine */
using ustream_ns::ustream_writezero ;

/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct blanker {
	cint	l = lenstr(sysword.w_blanks) ;
	cchar	*p = sysword.w_blanks ;
    } ; /* end struct (blanker) */
    struct zeroer {
	cint	l = szof(int) ;
	cchar	p[szof(int)] = {} ;
    } ; /* end struct (zeroer) */
} /* end namespace */


/* forward references */


/* local variables */

static blanker		bo ;
constexpr zeroer	zo ;


/* exported variables */


/* exported subroutines */

namespace ustream_ns {
    int ustream_writeblanks(ustream *op,int n) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	    while ((rs >= 0) && (wlen < n)) {
	        cint	ml = min((n - wlen),bo.l) ;
	        rs = op->iwrite(bo.p,ml) ;
	        wlen += rs ;
	    } /* end while */
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (ustream_writeblanks) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_writealign(ustream *op,int asz) noex {
	int		rs ;
	int		wlen = 0 ;
	    if (off_t foff ; (rs = op->tell(&foff)) >= 0) ylikely {
	        cint	r = int(foff & (asz - 1)) ;
	        if (r > 0) {
	            cint	nzero = (asz - r) ;
	            if (nzero > 0) {
	                rs = ustream_writezero(op,nzero) ;
	                wlen += rs ;
	            }
	        }
	    } /* end if (ustream_tell) */
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (ustream_writeallign) */
} /* end namespace (ustream_ns) */

namespace ustream_ns {
    int ustream_writezero(ustream *op,int n) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	    while ((rs >= 0) && (n > 0)) {
	        cint	ml = min(n,zo.l) ;
	        rs = op->iwrite(zo.p,ml) ;
	        n -= rs ;
	        wlen += rs ;
	    } /* end while */
	return (rs >= 0) ? wlen : rs ;
    } /* end subroutine (ustream_writezero) */
} /* end namespace (ustream_ns) */

int ustream::writefill(cchar *sp,int sl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = magic(sp)) >= 0) ylikely {
	    if (sl < 0) sl = (lenstr(sp) + 1) ;
	    if ((rs = iwrite(sp,sl)) >= 0) {
	        cint	asz = szof(int) ;
	        wlen = rs ;
	        rs = ustream_writealign(this,asz) ;
	        wlen += rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (ustream_writefill) */

int ustream::writefd(char *bp,int bl,int mfd,int len) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = magic(bp)) >= 0) ylikely {
	    int		rlen = len ;
	    auto read = [mfd,bp,bl] (int arlen) noex {
		int	rsl = SR_OK ;
		if (arlen > 0) {
	            cint ml = min(arlen,bl) ;
	            rsl = u_read(mfd,bp,ml) ;
		}
		return rsl ;
	    } ; /* end lambda (read) */
	    while ((rs = read(rlen)) > 0) {
	        rs = iwrite(bp,rs) ;
	        wlen += rs ;
	        rlen -= rs ;
		if (rs < 0) break ;
	    } /* end while */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (ustream_writefd) */


