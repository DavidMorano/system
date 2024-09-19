/* bpihdr SUPPORT */
/* lang=C++20 */

/* index for bible-paragraph file */
/* version %I% last-modified %G% */


/* revision history:

	= 2007-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-17, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 2007,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bpihdr

	Description:
	This subroutine reads and write a bible-verse-index file.

	Synopsis:
	int bpihdr_rd(bpihdr *op,char *hbuf,int hlen) noex
	int bpihdr_wr(bpihdr *op,cchar *hbuf,int hlen) noex

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
#include	<mkx.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"bpihdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsize,			/* file size */
	hi_wtime,			/* creation time */
	hi_vioff,			/* key-string table */
	hi_vilen,
	hi_nverses,
	hi_nzverses,
	hi_maxbook,
	hi_maxchapter,
	hi_overlast
} ;


/* forward references */


/* local variables */

constexpr int		headsize = hi_overlast * sizeof(uint) ;
constexpr int		magicsize = BPIHDR_MAGICSIZE ;
constexpr char		magicstr[] = BPIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int bpihdr_rd(bpihdr *op,char *hbuf,int hlen) noex {
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
	            bp[0] = BPIHDR_VERSION ;
	            bp[1] = ENDIAN ;
	            bp += 4 ;
	            bl -= 4 ;
	            if (bl >= headsize) {
	                uint	*header = (uint *) bp ;
	                header[hi_fsize] = op->fsize ;
	                header[hi_wtime] = op->wtime ;
	                header[hi_vioff] = op->vioff ;
	                header[hi_vilen] = op->vilen ;
	                header[hi_nverses] = op->nverses ;
	                header[hi_nzverses] = op->nzverses ;
	                header[hi_maxbook] = op->maxbook ;
	                header[hi_maxchapter] = op->maxchapter ;
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
/* end subroutine (bpihdr) */

int bpihdr_wr(bpihdr *op,cchar *hbuf,int hlen) noex {
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
	            if (op->vetu[0] != BPIHDR_VERSION) {
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
	                op->vioff = header[hi_vioff] ;
	                op->vilen = header[hi_vilen] ;
	                op->nverses = header[hi_nverses] ;
	                op->nzverses = header[hi_nzverses] ;
	                op->maxbook = header[hi_maxbook] ;
	                op->maxchapter = header[hi_maxchapter] ;
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
/* end subroutine (bpihdr_wr) */
	

