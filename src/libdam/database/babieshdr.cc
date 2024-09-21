/* babieshdr SUPPORT */
/* lang=C++20 */

/* header management for BABIES shared-memory segment */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	babieshdr

	Description:
	This subroutine reads and writes the BABIES shared-memory
	segment header.

	Synopsis:
	int babieshdr_rd(babieshdr *op,char *hbuf,int hlen) noex
	int babieshdr_wr(babieshdr *op,cchar *hbuf,int hlen) noex

	Arguments:
	- ep		object pointer
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<mkmagic.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"babieshdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		headsize = babieshdrh_overlast * sizeof(uint) ;
constexpr int		magicsize = BABIESHDR_MAGICSIZE ;
constexpr char		magicstr[] = BABIESHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int babieshdr_rd(babieshdr *ep,char *hbuf,int hlen) noex {
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
	    	    bp[0] = BABIESHDR_VERSION ;
	    	    bp[1] = ENDIAN ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= headsize) {
	        	uint	*header = uintp(bp) ;
	        	header[babieshdrh_shmsize] = ep->shmsize ;
	        	header[babieshdrh_dbsize] = ep->dbsize ;
	        	header[babieshdrh_dbtime] = ep->dbtime ;
	        	header[babieshdrh_wtime] = ep->wtime ;
	        	header[babieshdrh_atime] = ep->atime ;
	        	header[babieshdrh_acount] = ep->acount ;
	        	header[babieshdrh_muoff] = ep->muoff ;
	        	header[babieshdrh_musize] = ep->musize ;
	        	header[babieshdrh_btoff] = ep->btoff ;
	        	header[babieshdrh_btlen] = ep->btlen ;
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
/* end subroutine (babieshdr_rd) */

int babieshdr_wr(babieshdr *ep,cchar *hbuf,int hlen) noex {
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
	            if (ep->vetu[0] != BABIESHDR_VERSION) {
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
	                uint	*header = uintp(bp) ;
	                ep->shmsize = header[babieshdrh_shmsize] ;
	                ep->dbsize = header[babieshdrh_dbsize] ;
	                ep->dbtime = header[babieshdrh_dbtime] ;
	                ep->wtime = header[babieshdrh_wtime] ;
	                ep->atime = header[babieshdrh_atime] ;
	                ep->acount = header[babieshdrh_acount] ;
	                ep->muoff = header[babieshdrh_muoff] ;
	                ep->musize = header[babieshdrh_musize] ;
	                ep->btoff = header[babieshdrh_btoff] ;
	                ep->btlen = header[babieshdrh_btlen] ;
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
/* end subroutine (babieshdr_wr) */


