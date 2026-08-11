/* babieshdr SUPPORT */
/* charset=ISO8859-1 */
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

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX */
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

#include	"babieshdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		headsz		= babieshdrh_overlast * szof(uint) ;
constexpr int		magicsz		= BABIESHDR_MAGICSIZE ;
constexpr int		vsz		= szof(uint) ;	/* VETU */
constexpr char		magicstr[]	= BABIESHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

/* read from the (header) object into the buffer */
int babieshdr_rd(babieshdr *ep,char *hbuf,int hlen) noex {
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
	    	    bp[0] = uchar(BABIESHDR_VERSION) ;
	    	    bp[1] = uchar(ENDIAN) ;
	    	    bp += vsz ;
	    	    bl -= vsz ;
	    	    if (bl >= headsz) ylikely {
	        	uint				*header = uintp(bp) ;
	        	header[babieshdrh_shmsz]	= ep->shmsz ;
	        	header[babieshdrh_dbsz]		= ep->dbsz ;
	        	header[babieshdrh_dbtime]	= ep->dbtime ;
	        	header[babieshdrh_wtime]	= ep->wtime ;
	        	header[babieshdrh_atime]	= ep->atime ;
	        	header[babieshdrh_acount]	= ep->acount ;
	        	header[babieshdrh_muoff]	= ep->muoff ;
	        	header[babieshdrh_musz]		= ep->musz ;
	        	header[babieshdrh_btoff]	= ep->btoff ;
	        	header[babieshdrh_btlen]	= ep->btlen ;
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
} /* end subroutine (babieshdr_rd) */

/* write to the (header) object from the contents of the buffer */
int babieshdr_wr(babieshdr *ep,cchar *hbuf,int hlen) noex {
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
		    uchar	ech = uchar(ENDIAN) ;
	            memcopy(ep->vetu,bp,vsz) ;
	            if (ep->vetu[0] != BABIESHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (ep->vetu[1] != ech)) {
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
	                ep->shmsz	= header[babieshdrh_shmsz] ;
	                ep->dbsz	= header[babieshdrh_dbsz] ;
	                ep->dbtime	= header[babieshdrh_dbtime] ;
	                ep->wtime	= header[babieshdrh_wtime] ;
	                ep->atime	= header[babieshdrh_atime] ;
	                ep->acount	= header[babieshdrh_acount] ;
	                ep->muoff	= header[babieshdrh_muoff] ;
	                ep->musz	= header[babieshdrh_musz] ;
	                ep->btoff	= header[babieshdrh_btoff] ;
	                ep->btlen	= header[babieshdrh_btlen] ;
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
} /* end subroutine (babieshdr_wr) */


/* local subroutines */

int babieshdr::rd(char *rbuf,int rlen) noex {
    	return babieshdr_rd(this,rbuf,rlen) ;
} /* end method (babieshdr::rd) */

int babieshdr::wr(cchar *wbuf,int wlen) noex {
    	return babieshdr_wr(this,wbuf,wlen) ;
} /* end method (babieshdr::wr) */


