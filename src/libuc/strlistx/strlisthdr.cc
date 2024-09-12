/* strlisthdr SUPPORT */
/* lang=C++20 */

/* string-list database-file header */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This subroutine writes out a STRLIST file.

	Synopsis:
	int strlisthdr(strlisthdr *ep,int f,char *hbuf,int hlen) noex

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
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<strn.h>
#include	<mkmagic.h>
#include	<localmisc.h>

#include	"strlisthdr.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsize,			/* file size */
	hi_wtime,			/* write (creation) time */
	hi_stoff,			/* string table offset */
	hi_stlen,			/* string table length (entries) */
	hi_rtoff,			/* record table offset */
	hi_rtlen,			/* record table length */
	hi_itoff,			/* index (hash) table offset */
	hi_itlen,			/* index (hash) table length */
	hi_nstrs,			/* total number of strings */
	hi_nskip,			/* hash lookup paramter */
	hi_overlast
} ;


/* forward references */


/* local variables */

constexpr nullptr_t	np{} ;
constexpr cint		hdrsz = hi_overlast * sizeof(uint) ;
constexpr cint		magicsize = STRLISTHDR_MAGICSIZE ;
constexpr cchar		magicstr[] = STRLISTHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

/* write to the object from the supplied buffer */
int strlisthdr_wr(strlisthdr *ep,cchar *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && hbuf) {
	    rs = SR_INVALID ;
	    if (hlen > 0) {
	        int	bl = hlen ;
	        cchar	*bp = hbuf ;
	        rs = SR_OK ;
	        if ((rs >= 0) && (bl > 0)) {
	            if (bl >= magicsize) {
	                cchar	*cp = bp ;
	                int	cl = magicsize ;
	                if (cchar *tp ; (tp = strnchr(cp,cl,'\n')) != np) {
	                    cl = (tp - cp) ;
		        }
	                bp += magicsize ;
	                bl -= magicsize ;
    			/* verify the magic string */
	                if (strncmp(magicstr,cp,cl) != 0) {
	                    rs = SR_NXIO ;
		        }
	            } else {
	                rs = SR_ILSEQ ;
		    }
	        } /* end if (ok) */
		/* read out the VETU information */
	        if ((rs >= 0) && (bl > 0)) {
	            if (bl >= 4) {
	                memcpy(ep->vetu,bp,4) ;
	                if (ep->vetu[0] != STRLISTHDR_VERSION) {
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
	        } /* end if (item) */
	        if ((rs >= 0) && (bl > 0)) {
	            if (bl >= hdrsz) {
	                uint	*header = uintp(bp) ;
	                ep->fsize = header[hi_fsize] ;
	                ep->wtime = header[hi_wtime] ;
	                ep->stoff = header[hi_stoff] ;
	                ep->stlen = header[hi_stlen] ;
	                ep->rtoff = header[hi_rtoff] ;
	                ep->rtlen = header[hi_rtlen] ;
	                ep->itoff = header[hi_itoff] ;
	                ep->itlen = header[hi_itlen] ;
	                ep->nstrs = header[hi_nstrs] ;
	                ep->nskip = header[hi_nskip] ;
	                bp += hdrsz ;
	                bl -= hdrsz ;
	            } else {
	                rs = SR_ILSEQ ;
		    }
	        } /* end if (item) */
		if (rs >= 0) {
		    len = (bp - hbuf) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (strlisthdr_wr) */

/* read from object into supplied buffer */
int strlisthdr_rd(strlisthdr *ep,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && hbuf) {
	    rs = SR_INVALID ;
	    if (hlen > 0) {
		char	*bp = hbuf ;
		int	bl = hlen ;
	        if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
	            bp += magicsize ;
	            bl -= magicsize ;
	            memcpy(bp,ep->vetu,4) ;
	            *bp = STRLISTHDR_VERSION ;
	            bp += 4 ;
	            bl -= 4 ;
	            if ((rs >= 0) && (bl >= hdrsz)) {
	                uint	*header = uintp(bp) ;
	                header[hi_fsize] = ep->fsize ;
	                header[hi_wtime] = ep->wtime ;
	                header[hi_stoff] = ep->stoff ;
	                header[hi_stlen] = ep->stlen ;
	                header[hi_rtoff] = ep->rtoff ;
	                header[hi_rtlen] = ep->rtlen ;
	                header[hi_itoff] = ep->itoff ;
	                header[hi_itlen] = ep->itlen ;
	                header[hi_nstrs] = ep->nstrs ;
	                header[hi_nskip] = ep->nskip ;
	                bp += hdrsz ;
	                bl -= hdrsz ;
	            } /* end if */
		    if (rs >= 0) {
		        len = (bp - hbuf) ;
		    }
		} /* end if (mkmagic) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (strlisthdr_rd) */


