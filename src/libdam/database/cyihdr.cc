/* cyihdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Calendar-Year-Index DB header (text-index for CYI-INDEX file) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-22, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	cyihdr

	Description:
	This ojbect represents the data from the header of CYI 
	database file.

	Synopsis:
	int cyihdr_rd(cyihdr *op,char *hbuf,int hlen) noex
	int cyihdr_wr(cyihdr *op,cchar *hbuf,int hlen) noex

	Arguments:
	- op		object pointer
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
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<mkmagic.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"cyihdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsz,				/* file size */
	hi_wtime,			/* creation time */
	hi_diroff,			/* directory-name */
	hi_caloff,			/* calendar-name */
	hi_vioff,			/* key-string table */
	hi_vilen,
	hi_vloff,			/* key-string table */
	hi_vllen,
	hi_nentries,
	hi_nskip,			/* used in hash-collision algorithm */
	hi_year,			/* the year index was created */
	hi_overlast
} ; /* end enum */


/* forward references */


/* local variables */

constexpr int		headsz		= hi_overlast * szof(uint) ;
constexpr int		magicsz		= CYIHDR_MAGICSZ ;
constexpr int		vsz		= szof(uint) ;	/* VETU */
constexpr char		magicstr[]	= CYIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int cyihdr_rd(cyihdr *op,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) ylikely {
	    int		bl = hlen ;
	    char	*bp = hbuf ;
	    if (bl >= (magicsz + vsz)) ylikely {
	        if ((rs = mkmagic(bp,magicsz,magicstr)) >= 0) ylikely {
	            bp += magicsz ;
	            bl -= magicsz ;
	    	    memcopy(bp,op->vetu,vsz) ;
	    	    bp[0] = uchar(CYIHDR_VERSION) ;
	    	    bp[1] = uchar(ENDIAN) ;
	    	    bp += vsz ;
	    	    bl -= vsz ;
	    	    if (bl >= headsz) ylikely {
	        	uint			*header = uintp(bp) ;
	        	header[hi_fsz]		= op->fsz ;
	        	header[hi_wtime]	= op->wtime ;
	        	header[hi_diroff]	= op->diroff ;
	        	header[hi_caloff]	= op->caloff ;
	        	header[hi_vioff]	= op->vioff ;
	        	header[hi_vilen]	= op->vilen ;
	        	header[hi_vloff]	= op->vloff ;
	        	header[hi_vllen]	= op->vllen ;
	        	header[hi_nentries]	= op->nentries ;
	        	header[hi_nskip]	= op->nskip ;
	        	header[hi_year]		= op->year ;
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
} /* end subroutine (cyihdr_rd) */

int cyihdr_wr(cyihdr *op,cchar *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) ylikely {
	    int		bl = hlen ;
	    cchar	*bp = hbuf ;
	    if ((bl > magicsz) && hasValidMagic(bp,magicsz,magicstr)) ylikely {
		rs = SR_OK ;
	        bp += magicsz ;
	        bl -= magicsz ;
		/* read out the VETU information */
		if (bl >= vsz) ylikely {
	            memcopy(op->vetu,bp,vsz) ;
	            if (op->vetu[0] != CYIHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (op->vetu[1] != ENDIAN)) {
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
	                op->fsz		= header[hi_fsz] ;
	                op->wtime	= header[hi_wtime] ;
	                op->diroff	= header[hi_diroff] ;
	                op->caloff	= header[hi_caloff] ;
	                op->vioff	= header[hi_vioff] ;
	                op->vilen	= header[hi_vilen] ;
	                op->vloff	= header[hi_vloff] ;
	                op->vllen	= header[hi_vllen] ;
	                op->nentries	= header[hi_nentries] ;
	                op->nskip	= header[hi_nskip] ;
	                op->year	= header[hi_year] ;
	                bp += headsz ;
	        	bl -= headsz ;
			len = intconv(bp - hbuf) ;
	            } else {
	                rs = SR_ILSEQ ;
	            }
	        } /* end if (ok) */
	    } else {
	        rs = SR_ILSEQ ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (cyihdr_wr) */


/* local subroutines */

int cyihdr::rd(char *rbuf,int rlen) noex {
    	return cyihdr_rd(this,rbuf,rlen) ;
} /* end method (cyihdr::rd) */

int cyihdr::wr(cchar *wbuf,int wlen) noex {
    	return cyihdr_wr(this,wbuf,wlen) ;
} /* end method (cyihdr::wr) */


