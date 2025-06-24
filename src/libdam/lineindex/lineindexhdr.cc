/* lineindexhdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Line-Index-Header */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-17, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	lineindexhdr_rd
	lineindexhdr_wr

	Description:
	This subroutine reads from and write to a buffer which
	ropresents a LINEINDEX file header when written out to a
	file.

	Synopsis:
	int lineindexhdr_rd(pwdhdr *op,char *hbuf,int hlen) noex
	int lineindexhdr_wr(pwdhdr *op,cchar *hbuf,int hlen) noex

	Arguments:
	- op		object pointer
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

#include	"lineindexhdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		tabsize = (lineindexhdr_overlast * szof(uint)) ;
constexpr int		magicsize = LINEINDEXHDR_MAGICSIZE ;
constexpr char		magicstr[] = LINEINDEXHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int lineindexhdr_rd(lineindexhdr *op,char *hbuf,int hlen) noex {
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
	    	    bp[0] = LINEINDEXHDR_VERSION ;
	    	    bp[1] = charconv(ENDIAN) ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= tabsize) {
	        	uint	*header = uintp(bp) ;
			header[lineindexhdr_rectab] = op->rectab ;
			header[lineindexhdr_wrtime] = op->wrtime ;
			header[lineindexhdr_lines] = op->lines ;
	        	bp += tabsize ;
	        	bl -= tabsize ;
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
}
/* end subroutine (lineindexhdr_rd) */

int lineindexhdr_wr(lineindexhdr *op,cchar *hbuf,int hlen) noex {
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
	            if (op->vetu[0] != LINEINDEXHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (op->vetu[1] != ENDIAN)) {
	                rs = SR_PROTOTYPE ;
		    }
	            bp += 4 ;
	            bl -= 4 ;
	            if ((rs >= 0) && (bl >= tabsize)) {
			const uint	*header = uintp(bp) ;
	                op->rectab = header[lineindexhdr_rectab] ;
	                op->wrtime = header[lineindexhdr_wrtime] ;
	                op->lines = header[lineindexhdr_lines] ;
	                bp += tabsize ;
	                bl -= tabsize ;
			len = intconv(bp - hbuf) ;
	            } else {
	                rs = SR_ILSEQ ;
		    }
	        } else {
	            rs = SR_ILSEQ ;
	        } /* end if (ok) */
	    } else {
		rs = SR_ILSEQ ;
	    } /* end if (hasValidMagic) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (lineindexhdr_wr) */

int lineindexhdr::rd(char *rbuf,int rlen) noex {
    	return lineindexhdr_rd(this,rbuf,rlen) ;
}

int lineindexhdr::wr(cchar *wbuf,int wlen) noex {
    	return lineindexhdr_wr(this,wbuf,wlen) ;
}


