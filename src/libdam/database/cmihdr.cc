/* cmihdr SUPPORT */
/* lang=C++20 */

/* index for Commandment-entry file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-23, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmihdr

	Description:
	This subroutine reads and write a commandment-entry index file.

	Synopsis:
	int cmihdr_rd(cmihdr *ep,char *hbuf,int hlen) noex
	int cmihdr_wr(cmihdr *ep,cchar *hbuf,int hlen) noex

	Arguments:
	- ep		object pointer
	- f		read=1, write=0
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<mkx.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"cmihdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_dbsize,			/* DB file size */
	hi_dbtime,			/* DB modification-time */
	hi_idxsize,			/* IDX file size */
	hi_idxtime,			/* IDX modification-time */
	hi_vioff,			/* key-string table */
	hi_vilen,
	hi_vloff,			/* key-string table */
	hi_vllen,
	hi_nents,			/* number of entries */
	hi_maxent,			/* maximum commandment-number */
	hi_overlast
} ;


/* forward references */


/* local variables */

constexpr int		headsize = hi_overlast * sizeof(uint) ;

constexpr int		magicsize = CMIHDR_MAGICSIZE ;
constexpr char		magicstr[] = CMIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int cmihdr_rd(cmihdr *ep,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && hbuf) {
	    int		bl = hlen ;
	    char	*bp = hbuf ;
	    if (bl >= (magicsize + 4)) {
	        if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
	            bp += magicsize ;
	            bl -= magicsize ;
	    	    memcpy(bp,ep->vetu,4) ;
	    	    bp[0] = CMIHDR_VERSION ;
	    	    bp[1] = ENDIAN ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= headsize) {
	        	uint	*header = (uint *) bp ;
	        	header[hi_dbsize] = ep->dbsize ;
	        	header[hi_dbtime] = ep->dbtime ;
	        	header[hi_idxsize] = ep->idxsize ;
	        	header[hi_idxtime] = ep->idxtime ;
	        	header[hi_vioff] = ep->vioff ;
	        	header[hi_vilen] = ep->vilen ;
	        	header[hi_vloff] = ep->vloff ;
	        	header[hi_vllen] = ep->vllen ;
	        	header[hi_nents] = ep->nents ;
	        	header[hi_maxent] = ep->maxent ;
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
/* end subroutine (cmihdr_rd) */

int cmihdr_wr(cmihdr *ep,cchar *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && hbuf) {
	    int		bl = hlen ;
	    cchar	*bp = hbuf ;
	    if ((bl > magicsize) && hasValidMagic(bp,magicsize,magicstr)) {
		rs = SR_OK ;
	        bp += magicsize ;
	        bl -= magicsize ;
		/* read out the VETU information */
	        if (bl >= 4) {
	            memcpy(ep->vetu,bp,4) ;
	            if (ep->vetu[0] != CMIHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (ep->vetu[1] != ENDIAN)) {
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
	                ep->dbsize = header[hi_dbsize] ;
	                ep->dbtime = header[hi_dbtime] ;
	                ep->idxsize = header[hi_idxsize] ;
	                ep->idxtime = header[hi_idxtime] ;
	                ep->vioff = header[hi_vioff] ;
	                ep->vilen = header[hi_vilen] ;
	                ep->vloff = header[hi_vloff] ;
	                ep->vllen = header[hi_vllen] ;
	                ep->nents = header[hi_nents] ;
	                ep->maxent = header[hi_maxent] ;
	                bp += headsize ;
	                bl -= headsize ;
			len = (bp - hbuf) ;
	            } else {
	                rs = SR_ILSEQ ;
	            }
	        } /* end if (item) */
	    } else {
	        rs = SR_ILSEQ ;
	    } /* end if (hasValidMagic) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cmihdr_wr) */


