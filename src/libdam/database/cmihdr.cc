/* cmihdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* index for Commandment-entry file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-23, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmihdr

	Description:
	This subroutine reads and write a commandment-entry index file.

	Synopsis:
	int cmihdr_rd(cmihdr *op,char *hbuf,int hlen) noex
	int cmihdr_wr(cmihdr *op,cchar *hbuf,int hlen) noex

	Arguments:
	- op		object pointer
	- f		read=1, write=0
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
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

#include	"cmihdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_dbsz,			/* DB file size */
	hi_dbtime,			/* DB modification-time */
	hi_idxsize,			/* IDX file size */
	hi_idxtime,			/* IDX modification-time */
	hi_vioff,			/* key-string table */
	hi_vilen,
	hi_vloff,			/* key-string table */
	hi_vllen,
	hi_nents,			/* number of entries */
	hi_maxent,			/* maximum commandment-number */
	hi_overlast
} ; /* end enum */


/* forward references */


/* local variables */

constexpr int		headsz		= hi_overlast * szof(uint) ;
constexpr int		magicsz		= CMIHDR_MAGICSIZE ;
constexpr int		vsz		= szof(uint) ;	/* VETU */
constexpr char		magicstr[]	= CMIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

/* read from to (header) object (to buffer) */
int cmihdr_rd(cmihdr *op,char *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) ylikely {
	    int		bl = hlen ;
	    char	*bp = hbuf ;
	    rs = SR_INVALID ;
	    if (bl >= (magicsz + vsz)) ylikely {
	        if ((rs = mkmagic(bp,magicsz,magicstr)) >= 0) ylikely {
	            bp += magicsz ;
	            bl -= magicsz ;
	    	    memcopy(bp,op->vetu,vsz) ;
	    	    bp[0] = uchar(CMIHDR_VERSION) ;
	    	    bp[1] = uchar(ENDIAN) ;
	    	    bp += vsz ;
	    	    bl -= vsz ;
	    	    if (bl >= headsz) ylikely {
	        	uint			*header = uintp(bp) ;
	        	header[hi_dbsz]		= op->dbsz ;
	        	header[hi_dbtime]	= op->dbtime ;
	        	header[hi_idxsize]	= op->idxsize ;
	        	header[hi_idxtime]	= op->idxtime ;
	        	header[hi_vioff]	= op->vioff ;
	        	header[hi_vilen]	= op->vilen ;
	        	header[hi_vloff]	= op->vloff ;
	        	header[hi_vllen]	= op->vllen ;
	        	header[hi_nents]	= op->nents ;
	        	header[hi_maxent]	= op->maxent ;
	        	bp += headsz ;
	        	bl -= headsz ;
			len = intconv(bp - hbuf) ;
	            } else {
	                rs = SR_OVERFLOW ;
	            } /* end if */
	        } /* end if (mkmagic) */
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (cmihdr_rd) */

/* write to (header) object (from buffer) */
int cmihdr_wr(cmihdr *op,cchar *hbuf,int hlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && hbuf) ylikely {
	    int		bl = hlen ;
	    cchar	*bp = hbuf ;
	    if ((bl > magicsz) && hasValidMagic(bp,magicsz,magicstr)) {
		rs = SR_OK ;
	        bp += magicsz ;
	        bl -= magicsz ;
		/* read out the VETU information */
	        if (bl >= 4) {
		    uchar	ech = uchar(ENDIAN) ;
	            memcopy(op->vetu,bp,vsz) ;
	            if (op->vetu[0] != CMIHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (op->vetu[1] != ech)) {
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
	                op->dbsz	= header[hi_dbsz] ;
	                op->dbtime	= header[hi_dbtime] ;
	                op->idxsize	= header[hi_idxsize] ;
	                op->idxtime	= header[hi_idxtime] ;
	                op->vioff	= header[hi_vioff] ;
	                op->vilen	= header[hi_vilen] ;
	                op->vloff	= header[hi_vloff] ;
	                op->vllen	= header[hi_vllen] ;
	                op->nents	= header[hi_nents] ;
	                op->maxent	= header[hi_maxent] ;
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
} /* end subroutine (cmihdr_wr) */


/* local subroutines */

int cmihdr::rd(char *rbuf,int rlen) noex {
    	return cmihdr_rd(this,rbuf,rlen) ;
} /* end method (cmihdr::rd) */

int cmihdr::wr(cchar *wbuf,int wlen) noex {
    	return cmihdr_wr(this,wbuf,wlen) ;
} /* end method (cmihdr::wr) */


