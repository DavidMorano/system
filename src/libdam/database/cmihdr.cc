/* cmihdr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* index for Commandment-entry file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-23, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmihdr

	Description:
	This subroutine reads and write a commandment-entry index file.

	Synopsis:
	int cmihdr_rd(cmihdr *op,char *hbuf,int hlen) noex
	int cmihdr_wr(cmihdr *op,cchar *hbuf,int hlen) noex

	Arguments:
	- op		object pointer
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
#include	<cstring>		/* |memcpy(3c)| */
#include	<usystem.h>
#include	<endian.h>
#include	<mkmagic.h>
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

int cmihdr_rd(cmihdr *op,char *hbuf,int hlen) noex {
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
	    	    bp[0] = CMIHDR_VERSION ;
	    	    bp[1] = ENDIAN ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= headsize) {
	        	uint	*header = uintp(bp) ;
	        	header[hi_dbsize] = op->dbsize ;
	        	header[hi_dbtime] = op->dbtime ;
	        	header[hi_idxsize] = op->idxsize ;
	        	header[hi_idxtime] = op->idxtime ;
	        	header[hi_vioff] = op->vioff ;
	        	header[hi_vilen] = op->vilen ;
	        	header[hi_vloff] = op->vloff ;
	        	header[hi_vllen] = op->vllen ;
	        	header[hi_nents] = op->nents ;
	        	header[hi_maxent] = op->maxent ;
	        	bp += headsize ;
	        	bl -= headsize ;
			len = (bp - hbuf) ;
	            } else {
	                rs = SR_OVERFLOW ;
	            } /* end if */
	        } /* end if (mkmagic) */
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (cmihdr_rd) */

int cmihdr_wr(cmihdr *op,cchar *hbuf,int hlen) noex {
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
	            if (op->vetu[0] != CMIHDR_VERSION) {
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
	                op->dbsize = header[hi_dbsize] ;
	                op->dbtime = header[hi_dbtime] ;
	                op->idxsize = header[hi_idxsize] ;
	                op->idxtime = header[hi_idxtime] ;
	                op->vioff = header[hi_vioff] ;
	                op->vilen = header[hi_vilen] ;
	                op->vloff = header[hi_vloff] ;
	                op->vllen = header[hi_vllen] ;
	                op->nents = header[hi_nents] ;
	                op->maxent = header[hi_maxent] ;
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
/* end subroutine (cmihdr_wr) */


