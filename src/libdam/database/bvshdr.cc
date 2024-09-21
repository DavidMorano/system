/* bvshdr SUPPORT */
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

#include	<envstandards.h>	/* must be before others */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memset(3c)| */
#include	<usystem.h>
#include	<endian.h>
#include	<mkmagic.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"bvshdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsize,			/* file size */
	hi_wtime,			/* creation time */
	hi_nverses,			/* total verses */
	hi_nzverses,			/* non-zero verses */
	hi_nzbooks,			/* number of non-zero books */
	hi_btoff,			/* book-table */
	hi_btlen,
	hi_ctoff,			/* chapter-table */
	hi_ctlen,
	hi_overlast
} ;


/* forward references */


/* local variables */

constexpr int		headsize = hi_overlast * sizeof(uint) ;
constexpr int		magicsize = BVSHDR_MAGICSIZE ;
constexpr char		magicstr[] = BVSHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int bvshdr_rd(bvshdr *op,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) {
	    int		bl = hlen ;
	    char	*bp = hbuf ;
	    if (bl >= (magicsize + 4)) {
	        if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
	            bp += magicsize ;
	            bl -= magicsize ;
	    	    memcpy(bp,op->vetu,4) ;
	    	    bp[0] = BVSHDR_VERSION ;
	    	    bp[1] = ENDIAN ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= headsize) {
	        	uint	*header = (uint *) bp ;
	        	header[hi_fsize] = op->fsize ;
	        	header[hi_wtime] = op->wtime ;
	        	header[hi_nverses] = op->nverses ;
	        	header[hi_nzverses] = op->nzverses ;
	        	header[hi_nzbooks] = op->nzbooks ;
	        	header[hi_btoff] = op->btoff ;
	        	header[hi_btlen] = op->btlen ;
	        	header[hi_ctoff] = op->ctoff ;
	        	header[hi_ctlen] = op->ctlen ;
	        	bp += headsize ;
	        	bl -= headsize ;
			len = (bp - hbuf) ;
	            } else {
	                rs = SR_OVERFLOW ;
	            }
	        } /* end if (mkmagic) */
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bvshdr_rd) */

int bvshdr_wr(bvshdr *op,cchar *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) {
	    int		bl = hlen ;
	    cchar	*bp = hbuf ;
	    if ((bl > magicsize) && hasValidMagic(bp,magicsize,magicstr)) {
		rs = SR_OK ;
	        bp += magicsize ;
	        bl -= magicsize ;
		/* read out the VETU information */
	        if (bl >= 4) {
	            memcpy(op->vetu,bp,4) ;
	            if (op->vetu[0] != BVSHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (op->vetu[1] != ENDIAN)) {
	                rs = SR_PROTOTYPE ;
		    }
	            bp += 4 ;
	            bl -= 4 ;
	        } else {
	            rs = SR_ILSEQ ;
		}
	        if (rs >= 0) {
	            if (bl >= headsize) {
	            	uint	*header = (uint *) bp ;
	                op->fsize = header[hi_fsize] ;
	                op->wtime = header[hi_wtime] ;
	                op->nverses = header[hi_nverses] ;
	                op->nzverses = header[hi_nzverses] ;
	                op->nzbooks = header[hi_nzbooks] ;
	                op->btoff = header[hi_btoff] ;
	                op->btlen = header[hi_btlen] ;
	                op->ctoff = header[hi_ctoff] ;
	                op->ctlen = header[hi_ctlen] ;
	                bp += headsize ;
	                bl -= headsize ;
			len = (bp - hbuf) ;
	            } else {
	                rs = SR_ILSEQ ;
		    }
	        } /* end if (ok) */
	    } else {
		rs = SR_ILSEQ ;
	    } /* end if (hasValidMagic) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bcshdr_wr) */


