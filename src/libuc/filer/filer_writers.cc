/* filer_writers SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* extra write methods for the FILER object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	filer_writeblanks

	Description:
	Write a specified number of blanks to a FILER object.

	Synopsis:
	int filer_writeblanks(FILER *op,int n)

	Arguments:
	op		pointer to object
	n		number of bytes to write

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)


	Name:
	filer_writefill

	Description:
	Write enough data (bytes) to fill something.

	Synopsis:
	int filer_writefill(FILER *bp,cchar *sp,int sl) noex

	Arguments:
	bp		FILER object pointer
	sp		source buffer
	sl		source buffer length

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)


	Name:
	filer_writealign

	Description:
	Align the file-pointer to the specified alignment (zero-filling
	as needed).

	Synopsis:
	int filer_writealign(FILER *bp,int align) noex

	Arguments:
	bp		FILER object pointer
	align		source buffer length

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)


	Name:
	filer_writezero

	Description:
	We provide some extra small function for special circumstances.

	Synopsis:
	int filer_writezero(FILER *bp,int size) noex

	Arguments:
	bp		FILER object pointer
	size		amount of zeros to write

	Returns:
	>=0		number of bytes written
	<0		error code (system-return)


	Name:
	filer_writefd

	Description:
	Read a file (until a length-limit or an EOF) as given by a
	file-descriptor and write it to the present buffered-file.

	Synosis:
	int filer_writefd(filer *op,char *bp,int bl,int mfd,int len) noex

	Arguments:
	op		the present FILER object pointer
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
#include	<usystem.h>
#include	<localmisc.h>

#include	"filer.h"

import libutil ;
import uconstants ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct blanker {
	cint	l = xstrlen(sysword.w_blanks) ;
	cchar	*p = sysword.w_blanks ;
    } ; /* end struct (blanker) */
    struct zeroer {
	cint	l = szof(int) ;
	cchar	p[szof(int)] = {} ;
    } ; /* end struct (zeroer) */
}


/* forward references */

extern "C" {
    int		filer_writealign(filer *,int) noex ;
    int		filer_writezero(filer *,int) noex ;
}


/* local variables */

static blanker			bo ;
constexpr zeroer		zo ;


/* exported variables */


/* exported subroutines */

int filer_writeblanks(filer *op,int n) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op)) >= 0) {
	    while ((rs >= 0) && (wlen < n)) {
	        cint	ml = min((n - wlen),bo.l) ;
	        rs = filer_write(op,bo.p,ml) ;
	        wlen += rs ;
	    } /* end while */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writeblanks) */

int filer_writefill(filer *op,cchar *sp,int sl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = (xstrlen(sp) + 1) ;
	    if ((rs = filer_write(op,sp,sl)) >= 0) {
	        cint	asize = szof(int) ;
	        wlen = rs ;
	        rs = filer_writealign(op,asize) ;
	        wlen += rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writefill) */

int filer_writealign(filer *op,int asize) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op)) >= 0) {
	    if (off_t foff ; (rs = filer_tell(op,&foff)) >= 0) {
	        cint	r = int(foff & (asize - 1)) ;
	        if (r > 0) {
	            cint	nzero = (asize - r) ;
	            if (nzero > 0) {
	                rs = filer_writezero(op,nzero) ;
	                wlen += rs ;
	            }
	        }
	    } /* end if (filer_tell) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writeallign) */

int filer_writezero(filer *op,int n) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op)) >= 0) {
	    while ((rs >= 0) && (n > 0)) {
	        cint	ml = min(n,zo.l) ;
	        rs = filer_write(op,zo.p,ml) ;
	        n -= rs ;
	        wlen += rs ;
	    } /* end while */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writezero) */

int filer_writefd(filer *op,char *bp,int bl,int mfd,int len) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filer_magic(op,bp)) >= 0) {
	    int		rlen = len ;
	    auto read = [mfd,bp,bl] (int arlen) noex {
		int	rsl = SR_OK ;
		if (arlen > 0) {
	            cint	ml = min(arlen,bl) ;
	            rsl = u_read(mfd,bp,ml) ;
		}
		return rsl ;
	    } ; /* end lambda (read) */
	    while ((rs = read(rlen)) > 0) {
	        rs = filer_write(op,bp,rs) ;
	        wlen += rs ;
	        rlen -= rs ;
		if (rs < 0) break ;
	    } /* end while */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writefd) */


