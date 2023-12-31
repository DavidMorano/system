/* bpihdr */

/* index for bible-paragraph file */


#define	CF_DEBUGS 	0		/* compile-time debugging */


/* revision history:

	= 2007-03-01, David A�D� Morano
	This subroutine was originally written.

	= 2017-08-17, David A�D� Morano
	I enhanced to use |isValidMagic()|.

*/

/* Copyright � 2007,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine reads and write a bible-verse-index file.

	Synopsis:

	int bpihdr(ep,f,hbuf,hlen)
	BPIHDR		*ep ;
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
	<0		error code


*******************************************************************************/


#include	<envstandards.h>	/* must be before others */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<stdlib.h>
#include	<string.h>

#include	<usystem.h>
#include	<endian.h>
#include	<localmisc.h>

#include	"bpihdr.h"


/* local defines */


/* external subroutines */

extern int	mkmagic(char *,int,cchar *) ;
extern int	isValidMagic(cchar *,int,cchar *) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
#endif


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


/* exported subroutines */


int bpihdr(BPIHDR *ep,int f,char *hbuf,int hlen)
{
	uint		*header ;
	const int	headsize = hi_overlast * sizeof(uint) ;
	const int	magicsize = BPIHDR_MAGICSIZE ;
	int		rs = SR_OK ;
	int		bl = hlen ;
	const char	*magicstr = BPIHDR_MAGICSTR ;
	char		*bp = hbuf ;

	if (ep == NULL) return SR_FAULT ;
	if (hbuf == NULL) return SR_FAULT ;

	if (f) { /* read */
	    if ((bl > magicsize) && isValidMagic(bp,magicsize,magicstr)) {
	        bp += magicsize ;
	        bl -= magicsize ;

/* read out the VETU information */

	        if (bl >= 4) {

	            memcpy(ep->vetu,bp,4) ;

	            if (ep->vetu[0] != BPIHDR_VERSION) {
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

	                ep->fsize = header[hi_fsize] ;
	                ep->wtime = header[hi_wtime] ;
	                ep->vioff = header[hi_vioff] ;
	                ep->vilen = header[hi_vilen] ;
	                ep->nverses = header[hi_nverses] ;
	                ep->nzverses = header[hi_nzverses] ;
	                ep->maxbook = header[hi_maxbook] ;
	                ep->maxchapter = header[hi_maxchapter] ;

	                bp += headsize ;
	                bl -= headsize ;

	            } else {
	                rs = SR_ILSEQ ;
	            }
	        } /* end if (item) */

	    } /* end if (isValidMagic) */
	} else { /* write */
	    if (bl >= (magicsize + 4)) {
	        if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
	            bp += magicsize ;
	            bl -= magicsize ;
	            memcpy(bp,ep->vetu,4) ;
	            bp[0] = BPIHDR_VERSION ;
	            bp[1] = ENDIAN ;
	            bp += 4 ;
	            bl -= 4 ;
	            if (bl >= headsize) {
	                header = (uint *) bp ;
	                header[hi_fsize] = ep->fsize ;
	                header[hi_wtime] = ep->wtime ;
	                header[hi_vioff] = ep->vioff ;
	                header[hi_vilen] = ep->vilen ;
	                header[hi_nverses] = ep->nverses ;
	                header[hi_nzverses] = ep->nzverses ;
	                header[hi_maxbook] = ep->maxbook ;
	                header[hi_maxchapter] = ep->maxchapter ;
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

#if	CF_DEBUGS
	debugprintf("bpidu: f=%d rs=%d\n",f,rs) ;
#endif

	return (rs >= 0) ? (bp - hbuf) : rs ;
}
/* end subroutine (bpihdr) */


