/* bpihdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* index for bible-paragraph file */
/* version %I% last-modified %G% */


/* revision history:

	= 2007-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-17, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 2007,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bpihdr

	Description:
	This subroutine reads and write a bible-verse-index file.

	Synopsis:
	int bpihdr_rd(bpihdr *op,char *hbuf,int hlen) noex
	int bpihdr_wr(bpihdr *op,cchar *hbuf,int hlen) noex

	Arguments:
	- op		object pointer
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<mkmagic.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bpihdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsz,				/* file size */
	hi_wtime,			/* creation time */
	hi_vioff,			/* key-string table */
	hi_vilen,
	hi_nverses,
	hi_nzverses,
	hi_maxbook,
	hi_maxchap,
	hi_overlast
} ; /* end enum (his) */


/* forward references */


/* local variables */

constexpr int		headsize	= hi_overlast * szof(uint) ;
constexpr int		magicsize	= BPIHDR_MAGICSIZE ;
constexpr int		vsz		= szof(uint) ;	/* VETU */
constexpr char		magicstr[]	= BPIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int bpihdr_rd(bpihdr *op,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) {
	    int		bl = hlen ;
	    char	*bp = hbuf ;
	    rs = SR_INVALID ;
	    if (bl >= (magicsize + vsz)) {
	        if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
	            bp += magicsize ;
	            bl -= magicsize ;
	            memcopy(bp,op->vetu,vsz) ;
	            bp[0] = uchar(BPIHDR_VERSION) ;
	            bp[1] = uchar(ENDIAN) ;
	            bp += vsz ;
	            bl -= vsz ;
	            if (bl >= headsize) {
	                uint	*header = (uint *) bp ;
	                header[hi_fsz]		= op->fsz ;
	                header[hi_wtime]	= op->wtime ;
	                header[hi_vioff]	= op->vioff ;
	                header[hi_vilen]	= op->vilen ;
	                header[hi_nverses]	= op->nverses ;
	                header[hi_nzverses]	= op->nzverses ;
	                header[hi_maxbook]	= op->maxbook ;
	                header[hi_maxchap]	= op->maxchap ;
	                bp += headsize ;
	                bl -= headsize ;
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
} /* end subroutine (bpihdr) */

int bpihdr_wr(bpihdr *op,cchar *hbuf,int hlen) noex {
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
	        if (bl >= vsz) {
	            memcopy(op->vetu,bp,vsz) ;
	            if (op->vetu[0] != BPIHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
	            }
	            if ((rs >= 0) && (op->vetu[1] != ENDIAN)) {
	                rs = SR_PROTOTYPE ;
	            }
	            bp += vsz ;
	            bl -= vsz ;
	        } else {
	            rs = SR_ILSEQ ;
	        } /* end if */
	        if (rs >= 0) {
	            if (bl >= headsize) {
	                const uint	*header = uintp(bp) ;
	                op->fsz		= header[hi_fsz] ;
	                op->wtime	= header[hi_wtime] ;
	                op->vioff	= header[hi_vioff] ;
	                op->vilen	= header[hi_vilen] ;
	                op->nverses	= header[hi_nverses] ;
	                op->nzverses	= header[hi_nzverses] ;
	                op->maxbook	= header[hi_maxbook] ;
	                op->maxchap	= header[hi_maxchap] ;
	                bp += headsize ;
	                bl -= headsize ;
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
} /* end subroutine (bpihdr_wr) */
	

/* local subroutines */

int bpihdr::rd(char *rbuf,int rlen) noex {
    	return bpihdr_rd(this,rbuf,rlen) ;
} /* end method (bpihdr::rd) */

int bpihdr::wr(cchar *wbuf,int wlen) noex {
    	return bpihdr_wr(this,wbuf,wlen) ;
} /* end method (bpihdr::wr) */



