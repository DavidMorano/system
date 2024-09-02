/* babiesfu SUPPORT */
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

	Description:
	This subroutine reads and writes the BABIES shared-memory
	segment header.

	Synopsis:
	int babiesfu(ep,f,hbuf,hlen)
	BABIESFU	*ep ;
	int		f ;
	char		hbuf[] ;
	int		hlen ;

	Arguments:
	- ep		object pointer
	- f		read=1, write=0
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
#include	<mkx.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"babiesfu.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int babiesfu(BABIESFU *ep,int f,char *hbuf,int hlen) noex {
	uint		*header ;
	cint	headsize = babiesfuh_overlast * sizeof(uint) ;
	cint	magicsize = BABIESFU_MAGICSIZE ;
	int		rs = SR_OK ;
	int		bl = hlen ;
	cchar	*magicstr = BABIESFU_MAGICSTR ;
	char		*bp = hbuf ;

	if (ep == NULL) return SR_FAULT ;
	if (hbuf == NULL) return SR_FAULT ;

	if (f) { /* read */
	    if ((bl > magicsize) && hasValidMagic(bp,magicsize,magicstr)) {
	        bp += magicsize ;
	        bl -= magicsize ;

/* read out the VETU information */

	        if (bl >= 4) {

	            memcpy(ep->vetu,bp,4) ;

	            if (ep->vetu[0] != BABIESFU_VERSION) {
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
	                header = (uint *) bp ;
	                ep->shmsize = header[babiesfuh_shmsize] ;
	                ep->dbsize = header[babiesfuh_dbsize] ;
	                ep->dbtime = header[babiesfuh_dbtime] ;
	                ep->wtime = header[babiesfuh_wtime] ;
	                ep->atime = header[babiesfuh_atime] ;
	                ep->acount = header[babiesfuh_acount] ;
	                ep->muoff = header[babiesfuh_muoff] ;
	                ep->musize = header[babiesfuh_musize] ;
	                ep->btoff = header[babiesfuh_btoff] ;
	                ep->btlen = header[babiesfuh_btlen] ;
	                bp += headsize ;
	                bl -= headsize ;
	            } else {
	                rs = SR_ILSEQ ;
		    }
	        } /* end if (ok) */

	    } /* end if (hasValidMagic) */
	} else { /* write */
	    if (bl >= (magicsize + 4)) {
	        if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
	            bp += magicsize ;
	            bl -= magicsize ;
	    	    memcpy(bp,ep->vetu,4) ;
	    	    bp[0] = BABIESFU_VERSION ;
	    	    bp[1] = ENDIAN ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= headsize) {
	        	header = (uint *) bp ;
	        	header[babiesfuh_shmsize] = ep->shmsize ;
	        	header[babiesfuh_dbsize] = ep->dbsize ;
	        	header[babiesfuh_dbtime] = ep->dbtime ;
	        	header[babiesfuh_wtime] = ep->wtime ;
	        	header[babiesfuh_atime] = ep->atime ;
	        	header[babiesfuh_acount] = ep->acount ;
	        	header[babiesfuh_muoff] = ep->muoff ;
	        	header[babiesfuh_musize] = ep->musize ;
	        	header[babiesfuh_btoff] = ep->btoff ;
	        	header[babiesfuh_btlen] = ep->btlen ;
	        	bp += headsize ;
	        	bl -= headsize ;
	            } else {
	                rs = SR_OVERFLOW ;
	            }
	        } /* end if (mkmagic) */
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (read-write) */

	return (rs >= 0) ? (bp - hbuf) : rs ;
}
/* end subroutine (babiesfu) */


