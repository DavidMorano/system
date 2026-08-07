/* txtindexhdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* text-index hash file */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-17, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 2008,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	txtindexhdr

	Description:
	This subroutine writes out the hash file.

	Synopsis:
	int txtindexhdr_rd(txtindexhdr *ep,char *hbuf,int hlen) noex
	int txtindexhdr_wr(txtindexhdr *ep,cchar *hbuf,int hlen) noex

	Arguments:
	- ep		object pointer
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<mkmagic.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"txtindexhdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_hfsize,
	hi_tfsize,
	hi_ersize,
	hi_eisize,
	hi_wtime,
	hi_sdnoff,
	hi_sfnoff,
	hi_listoff,
	hi_esoff,
	hi_essize,
	hi_eroff,
	hi_erlen,
	hi_eioff,
	hi_eilen,
	hi_eiskip,
	hi_taboff,
	hi_tablen,			/* length (in entries) of hash table */
	hi_taglen,			/* number of tags */
	hi_maxtags,			/* maximum tags in any list */
	hi_minwlen,
	hi_maxwlen,
	hi_overlast
} ; /* end enum (his) */


/* forward references */


/* local variables */

constexpr int		headsz		= hi_overlast * szof(uint) ;
constexpr int		magicsz		= TXTINDEXHDR_MAGICSIZE ;
constexpr char		magicstr[]	= TXTINDEXHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int txtindexhdr_rd(txtindexhdr *ep,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && hbuf) ylikely {
	    int		bl = hlen ;
	    char	*bp = hbuf ;
	    if (bl >= (magicsz + 4)) ylikely {
	        if ((rs = mkmagic(bp,magicsz,magicstr)) >= 0) ylikely {
	            bp += magicsz ;
	            bl -= magicsz ;
	    	    memcpy(bp,ep->vetu,4) ;
	    	    bp[0] = char(TXTINDEXHDR_VERSION) ;
	    	    bp[1] = char(ENDIAN) ;
	    	    bp += 4 ;
	    	    bl -= 4 ;
	    	    if (bl >= headsz) ylikely {
	        	uint	*header = uintp(bp) ;
	        	header[hi_hfsize]	= ep->hfsize ;
	        	header[hi_tfsize]	= ep->tfsize ;
	        	header[hi_ersize]	= ep->ersize ;
	        	header[hi_eisize]	= ep->eisize ;
	        	header[hi_wtime]	= ep->wtime ;
	        	header[hi_sdnoff]	= ep->sdnoff ;
	        	header[hi_sfnoff]	= ep->sfnoff ;
	        	header[hi_listoff]	= ep->listoff ;
	        	header[hi_esoff]	= ep->esoff ;
	        	header[hi_essize]	= ep->essize ;
	        	header[hi_eroff]	= ep->eroff ;
	        	header[hi_erlen]	= ep->erlen ;
	        	header[hi_eioff]	= ep->eioff ;
	        	header[hi_eilen]	= ep->eilen ;
	        	header[hi_eiskip]	= ep->eiskip ;
	        	header[hi_taboff]	= ep->taboff ;
	        	header[hi_tablen]	= ep->tablen ;
	        	header[hi_taglen]	= ep->taglen ;
	        	header[hi_maxtags]	= ep->maxtags ;
	        	header[hi_minwlen]	= ep->minwlen ;
	        	header[hi_maxwlen]	= ep->maxwlen ;
	        	bp += headsz ;
	        	bl -= headsz ;
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
} /* end subroutine (txtindexhdr_rd) */

int txtindexhdr_wr(txtindexhdr *ep,cchar *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && hbuf) ylikely {
	    int		bl = hlen ;
	    cchar	*bp = hbuf ;
	    if ((bl > magicsz) && hasValidMagic(bp,magicsz,magicstr)) ylikely {
		rs = SR_OK ;
	        bp += magicsz ;
	        bl -= magicsz ;
		/* read out the VETU information */
	        if (bl >= 4) ylikely {
	            memcpy(ep->vetu,bp,4) ;
	            if (ep->vetu[0] != TXTINDEXHDR_VERSION) {
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
	        if (rs >= 0) ylikely {
	            if (bl >= headsz) ylikely {
	        	uint	*header = uintp(bp) ;
	        	ep->hfsize	= header[hi_hfsize] ;
	        	ep->tfsize	= header[hi_tfsize] ;
	        	ep->ersize	= header[hi_ersize] ;
	        	ep->eisize	= header[hi_eisize] ;
	        	ep->wtime	= header[hi_wtime] ;
	        	ep->sdnoff	= header[hi_sdnoff] ;
	        	ep->sfnoff	= header[hi_sfnoff] ;
	        	ep->listoff	= header[hi_listoff] ;
	        	ep->esoff	= header[hi_esoff] ;
	        	ep->essize	= header[hi_essize] ;
	        	ep->eroff	= header[hi_eroff] ;
	        	ep->erlen	= header[hi_erlen] ;
	        	ep->eioff	= header[hi_eioff] ;
	        	ep->eilen	= header[hi_eilen] ;
	        	ep->eiskip	= header[hi_eiskip] ;
	        	ep->taboff	= header[hi_taboff] ;
	        	ep->tablen	= header[hi_tablen] ;
	        	ep->taglen	= header[hi_taglen] ;
	        	ep->maxtags	= header[hi_maxtags] ;
	        	ep->minwlen	= header[hi_minwlen] ;
	        	ep->maxwlen	= header[hi_maxwlen] ;
	        	bp += headsz ;
	        	bl -= headsz ;
			len = intconv(bp - hbuf) ;
		    } else {
		        rs = SR_ILSEQ ;
		    }
	        } /* end if (ok) */
	    } else {
		rs = SR_ILSEQ ;
	    } /* end if (hasValidMagic) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (txtindexhdr_wr) */


/* local subroutines */

int txtindexhdr::rd(char *rbuf,int rlen) noex {
    	return txtindexhdr_rd(this,rbuf,rlen) ;
} /* end method (txtindexhdr::rd) */

int txtindexhdr::wr(cchar *wbuf,int wlen) noex {
    	return txtindexhdr_wr(this,wbuf,wlen) ;
} /* end method (txtindexhdr::wr) */


