/* filebuf_writers SUPPORT */
/* lang=C++20 */

/* extra write methods for the FILEBUF object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	filebuf_writeblanks

	Description:
	Write a specified number of blanks to a FILEBUF object.

	Synopsis:
	int filebuf_writeblanks(FILEBUF *fbp,int n)

	Arguments:
	fbp		pointer to object
	n		number of bytes to write

	Returns:
	<0		error
	>=0		number of bytes written


	Name:
	filebuf_writefill

	Description:
	Write enough data (bytes) to fill something.

	Synopsis:
	int filebuf_writefill(FILEBUF *bp,cchar *sp,int sl) noex

	Arguments:
	bp		FILEBUF object pointer
	sp		source buffer
	sl		source buffer length

	Returns:
	<0		error
	>=0		number of bytes written


	Name:
	filebuf_writealign

	Description:
	Align the file-pointer to the specified alignment (zero-filling
	as needed).

	Synopsis:
	int filebuf_writealign(FILEBUF *bp,int align) noex

	Arguments:
	bp		FILEBUF object pointer
	align		source buffer length

	Returns:
	<0		error
	>=0		number of bytes written


	Name:
	filebuf_writezero

	Description:
	We provide some extra small function for special circumstances.

	Synopsis:
	int filebuf_writezero(FILEBUF *bp,int size) noex

	Arguments:
	bp		FILEBUF object pointer
	size		amount of zeros to write

	Returns:
	<0		error
	>=0		number of bytes written


	Name:
	filebuf_writefd

	Description:
	Read a file (until a length-limit or an EOF) as given by a
	file-descriptor and write it to the present buffered-file.

	Synosis:
	int filebuf_writefd(filebuf *fbp,char *bp,int bl,int mfd,int len) noex

	Arguments:
	fbp		the present FILEBUF object pointer
	bp		buffer pointer to use as staging
	bl		buffer length to use as staging
	mfd		file-descriptor to read from
	len		length of data to read from given file-descriptor

	Returns:
	-		number of bytes written

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstring>
#include	<algorithm>
#include	<usystem.h>
#include	<localmisc.h>

#include	"filebuf.h"


/* local defines */

#undef	NBLANKS
#define	NBLANKS		8		/* number blanks to write at a time */


/* local namespaces */

using std::min ;


/* local typedefs */


/* external subroutines */


/* local structures */

constexpr int	zsize = sizeof(int) ;


/* forward references */

int		filebuf_writealign(filebuf *,int) noex ;
int		filebuf_writezero(filebuf *,int) noex ;


/* local variables */

constexpr int	nblanks = NBLANKS ;

static cchar	blanks[] = "        " ;

static cchar	zerobuf[zsize] = { } ;


/* exported subroutines */

int filebuf_writeblanks(filebuf *fbp,int n) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	while ((rs >= 0) && (wlen < n)) {
	    cint	ml = min((n-wlen),nblanks) ;
	    rs = filebuf_write(fbp,blanks,ml) ;
	    wlen += rs ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filebuf_writeblanks) */

int filebuf_writefill(filebuf *bp,cchar *sp,int sl) noex {
	int		rs ;
	int		wlen = 0 ;
	if (sl < 0) sl = (strlen(sp) + 1) ;
	if ((rs = filebuf_write(bp,sp,sl)) >= 0) {
	    cint	asize = sizeof(int) ;
	    wlen = rs ;
	    rs = filebuf_writealign(bp,asize) ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filebuf_writefill) */

int filebuf_writealign(filebuf *bp,int asize) noex {
	offset_t	foff ;
	int		rs ;
	int		wlen = 0 ;
	if ((rs = filebuf_tell(bp,&foff)) >= 0) {
	    cint	r = int(foff & (asize - 1)) ;
	    if (r > 0) {
	        cint	nzero = (asize - r) ;
	        if (nzero > 0) {
	            rs = filebuf_writezero(bp,nzero) ;
	            wlen += rs ;
	        }
	    }
	} /* end if (filebuf_tell) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filebuf_writeallign) */

int filebuf_writezero(filebuf *fp,int size) noex {
	int		rs = SR_OK ;
	int		rlen = size ;
	int		wlen = 0 ;
	while ((rs >= 0) && (rlen > 0)) {
	    cint	ml = min(rlen,zsize) ;
	    rs = filebuf_write(fp,zerobuf,ml) ;
	    rlen -= rs ;
	    wlen += rs ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filebuf_writezero) */

int filebuf_writefd(filebuf *fbp,char *bp,int bl,int mfd,int len) noex {
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
	        rs = filebuf_write(fbp,bp,rl) ;
	        wlen += rs ;
	    }
	    rlen -= rl ;
	} /* end while */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filebuf_writefd) */


