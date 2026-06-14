/* bvshdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* index for Bible-Verse-Structure (BVS) file */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

	= 2017-08-17, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 2009,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bvshdr

	Description:
        This subroutine reads or writes the file header for
        bible-verse-structure (BVS) files.

	Synopsis:
	int bvshdr_rd(bvshdr *op,char *hbuf,int hlen) noex
	int bvshdr_wr(bvshdr *op,cchar *hbuf,int hlen) noex

	Arguments:
	- op		object pointer
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<mkmagic.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bvshdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsz,				/* file size */
	hi_wtime,			/* creation time */
	hi_nverses,			/* total verses */
	hi_nzverses,			/* non-zero verses */
	hi_nzbooks,			/* number of non-zero books */
	hi_btoff,			/* book-table */
	hi_btlen,
	hi_ctoff,			/* chapter-table */
	hi_ctlen,
	hi_overlast
} ; /* end enum */


/* forward references */


/* local variables */

constexpr int		headsize	= hi_overlast * szof(uint) ;
constexpr int		magicsize	= BVSHDR_MAGICSIZE ;
constexpr int		vsz		= szof(uint) ;	/* VETU */
constexpr char		magicstr[]	= BVSHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int bvshdr_rd(bvshdr *op,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) {
	    int		bl = hlen ;
	    char	*bp = hbuf ;
	    rs = SR_INVALID ;
	    if (bl >= (magicsize + vsz)) {
	        if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
	            bp += magicsize ;
	            bl -= magicsize ;
	    	    memcopy(bp,op->vetu,vsz) ;
	    	    bp[0] = BVSHDR_VERSION ;
	    	    bp[1] = uchar(ENDIAN) ;
	    	    bp += vsz ;
	    	    bl -= vsz ;
	    	    if (bl >= headsize) {
	        	uint			*header = uintp(bp) ;
	        	header[hi_fsz]		= op->fsz ;
	        	header[hi_wtime]	= op->wtime ;
	        	header[hi_nverses]	= op->nverses ;
	        	header[hi_nzverses]	= op->nzverses ;
	        	header[hi_nzbooks]	= op->nzbooks ;
	        	header[hi_btoff]	= op->btoff ;
	        	header[hi_btlen]	= op->btlen ;
	        	header[hi_ctoff]	= op->ctoff ;
	        	header[hi_ctlen]	= op->ctlen ;
	        	bp += headsize ;
	        	bl -= headsize ;
			len = intconv(bp - hbuf) ;
	            } else {
	                rs = SR_OVERFLOW ;
	            }
	        } /* end if (mkmagic) */
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (bvshdr_rd) */

int bvshdr_wr(bvshdr *op,cchar *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) {
	    int		bl = hlen ;
	    cchar	*bp = hbuf ;
	    rs = SR_NOTOPEN ;
	    if ((bl > magicsize) && hasValidMagic(bp,magicsize,magicstr)) {
		rs = SR_OK ;
	        bp += magicsize ;
	        bl -= magicsize ;
		/* read out the VETU information */
	        if (bl >= vsz) {
	            memcopy(op->vetu,bp,vsz) ;
	            if (op->vetu[0] != BVSHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (op->vetu[1] != ENDIAN)) {
	                rs = SR_PROTOTYPE ;
		    }
	            bp += vsz ;
	            bl -= vsz ;
	        } else {
	            rs = SR_ILSEQ ;
		}
	        if (rs >= 0) {
	            if (bl >= headsize) {
	            	const uint	*header = uintp(bp) ;
	                op->fsz		= header[hi_fsz] ;
	                op->wtime	= header[hi_wtime] ;
	                op->nverses	= header[hi_nverses] ;
	                op->nzverses	= header[hi_nzverses] ;
	                op->nzbooks	= header[hi_nzbooks] ;
	                op->btoff	= header[hi_btoff] ;
	                op->btlen	= header[hi_btlen] ;
	                op->ctoff	= header[hi_ctoff] ;
	                op->ctlen	= header[hi_ctlen] ;
	                bp += headsize ;
	                bl -= headsize ;
			len = intconv(bp - hbuf) ;
	            } else {
	                rs = SR_ILSEQ ;
		    }
	        } /* end if (ok) */
	    } else {
		rs = SR_ILSEQ ;
	    } /* end if (hasValidMagic) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (bvshdr_wr) */


/* local subroutines */

int bvshdr::rd(char *rbuf,int rlen) noex {
    	return bvshdr_rd(this,rbuf,rlen) ;
} /* end method (bvshdr::rd) */

int bvshdr::wr(cchar *wbuf,int wlen) noex {
    	return bvshdr_wr(this,wbuf,wlen) ;
} /* end method (bvshdr::wr) */


