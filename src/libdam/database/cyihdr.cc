/* cyihdr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* text-index for CYI-INDEX file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-22, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cyihdr

	Description:
	This subroutine writes out the hash file.

	Synopsis:
	int cyihdr_rd(cyihdr *op,char *hbuf,int hlen) noex
	int cyihdr_wr(cyihdr *op,cchar *hbuf,int hlen) noex

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

#include	"cyihdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsize,			/* file size */
	hi_wtime,			/* creation time */
	hi_diroff,			/* directory-name */
	hi_caloff,			/* calendar-name */
	hi_vioff,			/* key-string table */
	hi_vilen,
	hi_vloff,			/* key-string table */
	hi_vllen,
	hi_nentries,
	hi_nskip,			/* used in hash-collision algorithm */
	hi_year,			/* the year index was created */
	hi_overlast
} ;


/* forward references */


/* local variables */

constexpr int		headsize = hi_overlast * sizeof(uint) ;
constexpr int		magicsize = CYIHDR_MAGICSIZE ;
constexpr char		magicstr[] = CYIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int cyihdr_rd(cyihdr *op,char *hbuf,int hlen) noex {
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
	    	    bp[0] = CYIHDR_VERSION ;
	    	    bp[1] = ENDIAN ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= headsize) {
	        	uint	*header = uintp(bp) ;
	        	header[hi_fsize] = op->fsize ;
	        	header[hi_wtime] = op->wtime ;
	        	header[hi_diroff] = op->diroff ;
	        	header[hi_caloff] = op->caloff ;
	        	header[hi_vioff] = op->vioff ;
	        	header[hi_vilen] = op->vilen ;
	        	header[hi_vloff] = op->vloff ;
	        	header[hi_vllen] = op->vllen ;
	        	header[hi_nentries] = op->nentries ;
	        	header[hi_nskip] = op->nskip ;
	        	header[hi_year] = op->year ;
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
/* end subroutine (cyihdr_rd) */

int cyihdr_wr(cyihdr *op,cchar *hbuf,int hlen) noex {
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
	            if (op->vetu[0] != CYIHDR_VERSION) {
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
	                uint	*header = uintp(bp) ;
	                op->fsize = header[hi_fsize] ;
	                op->wtime = header[hi_wtime] ;
	                op->diroff = header[hi_diroff] ;
	                op->caloff = header[hi_caloff] ;
	                op->vioff = header[hi_vioff] ;
	                op->vilen = header[hi_vilen] ;
	                op->vloff = header[hi_vloff] ;
	                op->vllen = header[hi_vllen] ;
	                op->nentries = header[hi_nentries] ;
	                op->nskip = header[hi_nskip] ;
	                op->year = header[hi_year] ;
	                bp += headsize ;
	        	bl -= headsize ;
			len = (bp - hbuf) ;
	            } else {
	                rs = SR_ILSEQ ;
	            }
	        } /* end if (ok) */
	    } else {
	        rs = SR_ILSEQ ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cyihdr_wr) */


