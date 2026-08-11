/* pwihdr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PassWord Index file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

	= 2017-08-17, David A­D­ Morano
	I enhanced to use |hasValidMagic()|.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	pwihdr_rd
	pwihdr_wr

	Description:
	This subroutine reads from and write to a buffer which
	ropresents a PWI file header when written out to a file.

	Synopsis:
	int pwihdr_rd(pwdhdr *op,char *hbuf,int hlen) noex
	int pwihdr_wr(pwdhdr *op,cchar *hbuf,int hlen) noex

	Arguments:
	- op		object pointer
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

#include	"pwihdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		tabsize		= (pwihdr_overlast * szof(uint)) ;
constexpr int		magicsz		= PWIHDR_MAGICSIZE ;
constexpr int		vsz		= szof(uint) ;	/* VETU */
constexpr char		magicstr[]	= PWIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int pwihdr_rd(pwihdr *op,char *hbuf,int hlen) noex {
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
	    	    bp[0] = uchar(PWIHDR_VERSION) ;
	    	    bp[1] = uchar(ENDIAN) ;
	    	    bp += vsz ;
	    	    bl -= vsz ;
	    	    if (bl >= tabsize) ylikely {
	        	uint			*header = uintp(bp) ;
			header[pwihdr_fsz]	= op->fsz ;
			header[pwihdr_wrtime]	= op->wrtime ;
			header[pwihdr_wrcount]	= op->wrcount ;
			header[pwihdr_rectab]	= op->rectab ;
			header[pwihdr_reclen]	= op->reclen ;
			header[pwihdr_recsize]	= op->recsize ;
			header[pwihdr_strtab]	= op->strtab ;
			header[pwihdr_lenstr]	= op->lenstr ;
			header[pwihdr_strsize]	= op->strsize ;
			header[pwihdr_idxlen]	= op->idxlen ;
			header[pwihdr_idxsize]	= op->idxsize ;
			header[pwihdr_idxl1]	= op->idxl1 ;
			header[pwihdr_idxl3]	= op->idxl3 ;
			header[pwihdr_idxf]	= op->idxf ;
			header[pwihdr_idxfl3]	= op->idxfl3 ;
			header[pwihdr_idxun]	= op->idxun ;
	        	bp += tabsize ;
	        	bl -= tabsize ;
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
} /* end subroutine (pwihdr_rd) */

int pwihdr_wr(pwihdr *op,cchar *hbuf,int hlen) noex {
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
	            if (op->vetu[0] != PWIHDR_VERSION) {
	                rs = SR_PROTONOSUPPORT ;
		    }
	            if ((rs >= 0) && (op->vetu[1] != ENDIAN)) {
	                rs = SR_PROTOTYPE ;
		    }
	            bp += vsz ;
	            bl -= vsz ;
	            if ((rs >= 0) && (bl >= tabsize)) {
			const uint	*header = uintp(bp) ;
	                op->fsz		= header[pwihdr_fsz] ;
	                op->wrtime	= header[pwihdr_wrtime] ;
	                op->wrcount	= header[pwihdr_wrcount] ;
	                op->rectab	= header[pwihdr_rectab] ;
	                op->reclen	= header[pwihdr_reclen] ;
	                op->recsize	= header[pwihdr_recsize] ;
	                op->strtab	= header[pwihdr_strtab] ;
	                op->lenstr	= header[pwihdr_lenstr] ;
	                op->strsize	= header[pwihdr_strsize] ;
	                op->idxlen	= header[pwihdr_idxlen] ;
	                op->idxsize	= header[pwihdr_idxsize] ;
	                op->idxl1	= header[pwihdr_idxl1] ;
	                op->idxl3	= header[pwihdr_idxl3] ;
	                op->idxf	= header[pwihdr_idxf] ;
	                op->idxfl3	= header[pwihdr_idxfl3] ;
	                op->idxun	= header[pwihdr_idxun] ;
	                bp += tabsize ;
	                bl -= tabsize ;
			len = intconv(bp - hbuf) ;
	            } else {
	                rs = SR_ILSEQ ;
		    }
	        } else {
	            rs = SR_ILSEQ ;
	        } /* end if (ok) */
	    } else {
		rs = SR_ILSEQ ;
	    } /* end if (hasValidMagic) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (pwihdr_wr) */


/* local subroutines */

int pwihdr::rd(char *rbuf,int rlen) noex {
    	return pwihdr_rd(this,rbuf,rlen) ;
} /* end method (pwihdr::rd) */

int pwihdr::wr(cchar *wbuf,int wlen) noex {
    	return pwihdr_wr(this,wbuf,wlen) ;
} /* end method (pwihdr::wr) */


