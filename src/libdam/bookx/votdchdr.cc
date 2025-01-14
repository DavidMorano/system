/* votdchdr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* header for VOTDs shared-memory segment */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A�D� Morano
	This code was originally written.

	= 2017-08-23, David A�D� Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:

	Description:
	This subroutine reads and writes the VOTDs shared-memory
	segment header.

	Synopsis:
	int votdchdr(ep,f,hbuf,hlen)
	VOTDCHDR	*ep ;
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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |memset(3c)| */
#include	<usystem.h>
#include	<endian.h>
#include	<mkmagic.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"votdchdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int votdchdr(VOTDCHDR *ep,int f,char *hbuf,int hlen) noex {
	uint		*header ;
	cint	headsize = votdchdrh_overlast * szof(uint) ;
	cint	magicsize = VOTDCHDR_MAGICSIZE ;
	int		rs = SR_OK ;
	int		bl = hlen ;
	cchar		*magicstr = VOTDCHDR_MAGICSTR ;
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

	            if (ep->vetu[0] != VOTDCHDR_VERSION) {
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
	                ep->shmsize = header[votdchdrh_shmsize] ;
	                ep->wtime = header[votdchdrh_wtime] ;
	                ep->atime = header[votdchdrh_atime] ;
	                ep->wcount = header[votdchdrh_wcount] ;
	                ep->acount = header[votdchdrh_acount] ;
	                ep->muoff = header[votdchdrh_muoff] ;
	                ep->musize = header[votdchdrh_musize] ;
	                ep->bookoff = header[votdchdrh_bookoff] ;
	                ep->booklen = header[votdchdrh_booklen] ;
	                ep->recoff = header[votdchdrh_recoff] ;
	                ep->reclen = header[votdchdrh_reclen] ;
	                ep->balloff = header[votdchdrh_balloff] ;
	                ep->ballsize = header[votdchdrh_ballsize] ;
	                ep->valloff = header[votdchdrh_valloff] ;
	                ep->vallsize = header[votdchdrh_vallsize] ;
	                ep->bstroff = header[votdchdrh_bstroff] ;
	                ep->bstrlen = header[votdchdrh_bstrlen] ;
	                ep->vstroff = header[votdchdrh_vstroff] ;
	                ep->vstrlen = header[votdchdrh_vstrlen] ;
	                bp += headsize ;
	                bl -= headsize ;
	            } else {
	                rs = SR_ILSEQ ;
	            }
	        } /* end if (item) */

	    } else {
	        rs = SR_ILSEQ ;
	    } /* end if (hasValidMagic) */
	} else { /* write */

	    if (bl >= (magicsize + 4)) {
	        if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
	            bp += magicsize ;
	            bl -= magicsize ;
	    	    memcpy(bp,ep->vetu,4) ;
	    	    bp[0] = VOTDCHDR_VERSION ;
	    	    bp[1] = ENDIAN ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= headsize) {
	        	header = (uint *) bp ;
	        	header[votdchdrh_shmsize] = ep->shmsize ;
	        	header[votdchdrh_wtime] = ep->wtime ;
	        	header[votdchdrh_atime] = ep->atime ;
	        	header[votdchdrh_wcount] = ep->wcount ;
	        	header[votdchdrh_acount] = ep->acount ;
	        	header[votdchdrh_muoff] = ep->muoff ;
	        	header[votdchdrh_musize] = ep->musize ;
	        	header[votdchdrh_bookoff] = ep->bookoff ;
	        	header[votdchdrh_booklen] = ep->booklen ;
	        	header[votdchdrh_recoff] = ep->recoff ;
	        	header[votdchdrh_reclen] = ep->reclen ;
	        	header[votdchdrh_balloff] = ep->balloff ;
	        	header[votdchdrh_ballsize] = ep->ballsize ;
	        	header[votdchdrh_valloff] = ep->valloff ;
	        	header[votdchdrh_vallsize] = ep->vallsize ;
	        	header[votdchdrh_bstroff] = ep->bstroff ;
	        	header[votdchdrh_bstrlen] = ep->bstrlen ;
	        	header[votdchdrh_vstroff] = ep->vstroff ;
	        	header[votdchdrh_vstrlen] = ep->vstrlen ;
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
	} /* end if (read-write) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (votdchdr) */


