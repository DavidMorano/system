/* varhdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* text-index header for VAR-INDEX file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	varhdr

	Description:
	This subroutine writes out the hash file.

	Synopsis:
	int varhdr_rd(varhdr *ep,char *hbuf,int hlen) noex
	int varhdr_wr(varhdr *ep,cchar *hbuf,int hlen) noex

	Arguments:
	- ep		object pointer
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<mkmagic.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"varhdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsz,				/* file size */
	hi_wtime,			/* creation time */
	hi_ksoff,			/* key-string table */
	hi_kslen,
	hi_vsoff,			/* value-string table */
	hi_vslen,
	hi_rtoff,			/* record table */
	hi_rtlen,			
	hi_itoff,			/* index (hash) table */
	hi_itlen,			
	hi_nvars,			/* number of variables */
	hi_nskip,
	hi_overlast
} ; /* end enum (his) */


/* forward references */


/* local variables */

constexpr int		headsz		= hi_overlast * szof(uint) ;
constexpr int		magicsz		= VARHDR_MAGICSZ ;
constexpr int		vsz		= szof(uint) ;	/* VETU */
constexpr char		magicstr[]	= VARHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int varhdr_rd(varhdr *ep,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && hbuf) ylikely {
	    int		bl = hlen ;
	    char	*bp = hbuf ;
	    if (bl >= (magicsz + vsz)) ylikely {
	        if ((rs = mkmagic(bp,magicsz,magicstr)) >= 0) ylikely {
	            bp += magicsz ;
	            bl -= magicsz ;
	    	    memcopy(bp,ep->vetu,vsz) ;
	    	    bp[0] = uchar(VARHDR_VERSION) ;
	    	    bp[1] = uchar(ENDIAN) ;
	    	    bp += vsz ;
	    	    bl -= vsz ;
	            if (bl >= headsz) ylikely {
	        	uint			*header = uintp(bp) ;
	        	header[hi_fsz]		= ep->fsz ;
	        	header[hi_wtime]	= ep->wtime ;
	        	header[hi_ksoff]	= ep->ksoff ;
	        	header[hi_kslen]	= ep->kslen ;
	        	header[hi_vsoff]	= ep->vsoff ;
	        	header[hi_vslen]	= ep->vslen ;
	        	header[hi_rtoff]	= ep->rtoff ;
	        	header[hi_rtlen]	= ep->rtlen ;
	        	header[hi_itoff]	= ep->itoff ;
	        	header[hi_itlen]	= ep->itlen ;
	        	header[hi_nvars]	= ep->nvars ;
	        	header[hi_nskip]	= ep->nskip ;
	        	bp += headsz ;
	        	bl -= headsz ;
			len = intconv(bp - hbuf) ;
		    } else {
			rs = SR_OVERFLOW ;
		    } /* end if */
		} /* end if (mkmagic) */
	    } else {
	        rs = SR_OVERFLOW ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (varhdr_rd) */

int varhdr_wr(varhdr *ep,cchar *hbuf,int hlen) noex {
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
	        if (bl >= vsz) ylikely {
	            memcopy(ep->vetu,bp,vsz) ;
	            if (ep->vetu[0] != VARHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (ep->vetu[1] != ENDIAN)) {
	                rs = SR_PROTOTYPE ;
		    }
	            bp += vsz ;
	            bl -= vsz ;
	        } else {
	            rs = SR_ILSEQ ;
		}
	        if (rs >= 0) ylikely {
	            if (bl >= headsz) ylikely {
	                const uint	*header = uintp(bp) ;
	                ep->fsz		= header[hi_fsz] ;
	                ep->wtime	= header[hi_wtime] ;
	                ep->ksoff	= header[hi_ksoff] ;
	                ep->kslen	= header[hi_kslen] ;
	                ep->vsoff	= header[hi_vsoff] ;
	                ep->vslen	= header[hi_vslen] ;
	                ep->rtoff	= header[hi_rtoff] ;
	                ep->rtlen	= header[hi_rtlen] ;
	                ep->itoff	= header[hi_itoff] ;
	                ep->itlen	= header[hi_itlen] ;
	                ep->nvars	= header[hi_nvars] ;
	                ep->nskip	= header[hi_nskip] ;
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
} /* end subroutine (varhdr_wr) */


/* local subroutines */

int varhdr::rd(char *rbuf,int rlen) noex {
    	return varhdr_rd(this,rbuf,rlen) ;
} /* end method (varhdr::rd) */

int varhdr::wr(cchar *wbuf,int wlen) noex {
    	return varhdr_wr(this,wbuf,wlen) ;
} /* end method (varhdr::wr) */


