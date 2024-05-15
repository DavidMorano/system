/* filer_writers SUPPORT */
/* lang=C++20 */

/* extra write methods for the FILER object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	filer_writeblanks

	Description:
	Write a specified number of blanks to a FILER object.

	Synopsis:
	int filer_writeblanks(FILER *fbp,int n)

	Arguments:
	fbp		pointer to object
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
	int filer_writefd(filer *fbp,char *bp,int bl,int mfd,int len) noex

	Arguments:
	fbp		the present FILER object pointer
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
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"filer.h"


/* local defines */

#undef	NBLANKS
#define	NBLANKS		8		/* number blanks to write at a time */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    template<int N> struct blanks {
	cint		l = N ;
	char		p[N+1] ;
	constexpr blanks() noex {
	    for (int i = 0 ; i < N ; i += 1) {
		p[i] = ' ' ;
	    }
	    p[N] = '\0' ;
	} ;
    } ;
}


/* forward references */

extern "C" {
    int		filer_writealign(filer *,int) noex ;
    int		filer_writezero(filer *,int) noex ;
}


/* local variables */

constexpr int			zsize = sizeof(int) ;

constexpr blanks<NBLANKS>	bo ;

constexpr cchar			zerobuf[zsize] = { } ;


/* exported variables */


/* exported subroutines */

int filer_writeblanks(filer *fbp,int n) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	while ((rs >= 0) && (wlen < n)) {
	    cint	ml = min((n-wlen),bo.l) ;
	    rs = filer_write(fbp,bo.p,ml) ;
	    wlen += rs ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writeblanks) */

int filer_writefill(filer *op,cchar *sp,int sl) noex {
	int		rs ;
	int		wlen = 0 ;
	if (sl < 0) sl = (strlen(sp) + 1) ;
	if ((rs = filer_write(op,sp,sl)) >= 0) {
	    cint	asize = sizeof(int) ;
	    wlen = rs ;
	    rs = filer_writealign(op,asize) ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writefill) */

int filer_writealign(filer *bp,int asize) noex {
	off_t		foff ;
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filer_tell(bp,&foff)) >= 0) {
	    cint	r = int(foff & (asize - 1)) ;
	    if (r > 0) {
	        cint	nzero = (asize - r) ;
	        if (nzero > 0) {
	            rs = filer_writezero(bp,nzero) ;
	            wlen += rs ;
	        }
	    }
	} /* end if (filer_tell) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writeallign) */

int filer_writezero(filer *fp,int zlen) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	while ((rs >= 0) && (zlen > 0)) {
	    cint	ml = min(zlen,zsize) ;
	    rs = filer_write(fp,zerobuf,ml) ;
	    zlen -= rs ;
	    wlen += rs ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writezero) */

int filer_writefd(filer *fbp,char *bp,int bl,int mfd,int len) noex {
	int		rs = SR_OK ;
	int		rlen = len ;
	int		rl ;
	int		wlen = 0 ;
	while ((rs >= 0) && (rlen > 0)) {
	    cint	ml = min(rlen,bl) ;
	    rs = u_read(mfd,bp,ml) ;
	    rl = rs ;
	    if (rs <= 0) break ;
	    {
	        rs = filer_write(fbp,bp,rl) ;
	        wlen += rs ;
	    }
	    rlen -= rl ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writefd) */


