/* ttihdr SUPPORT (Term-Translate-Index) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Termial-Translate-Index (TTI) file management (file header) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ttihdr

	Description:
        This subroutine manages the header for TERMTRANS lookup-table
        index-file.

	Synopsis:
	int ttihdr_rd(ttihdr *op,char *hbuf,hlen) noex
	int ttihdr_wd(ttihdr *op,cchar *hbuf,hlen) noex

	Arguments:
	- op		object pointer
	- hbuf		buffer containing object
	- hlen		length of buffer

	Returns:
	>=0		OK
	<0		error (system-return)

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
#include	<strn.h>		/* LIBUC */
#include	<mkmagic.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ttihdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsz,				/* file-size */
	hi_ctime,			/* creation-time */
	hi_rectab,			/* record-table */
	hi_reclen,			/* record-legnth */
	hi_ostrtab,			/* overflow-string-table */
	hi_ostrlen,			/* overflow-string-table length */
	hi_overlast
} ; /* end enum (his) */


/* forward references */


/* local variables */

constexpr int		headsz		= hi_overlast * szof(uint) ;
constexpr int		magicsz		= TTIHDR_MAGICSZ ;
constexpr int		vsz		= szof(uint) ;	/* VETU */
constexpr char		magicstr[]	= TTIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int ttihdr_rd(ttihdr *op,char *hbuf,int hlen) noex {
        int             rs = SR_FAULT ;
        int             len = 0 ;
        if (op && hbuf) ylikely {
            int         bl = hlen ;
            char        *bp = hbuf ;
            if (bl >= (magicsz + vsz)) ylikely {
                if ((rs = mkmagic(bp,magicsz,magicstr)) >= 0) ylikely {
                    bp += magicsz ;
                    bl -= magicsz ;
                    memcopy(bp,op->vetu,vsz) ;
                    bp[0] = uchar(TTIHDR_VERSION) ;
                    bp[1] = uchar(ENDIAN) ;
                    bp += vsz ;
                    bl -= vsz ;
                    if (bl >= headsz) ylikely {
                        uint    		*header = uintp(bp) ;
	        	header[hi_fsz]		= op->fsz ;
	        	header[hi_ctime]	= op->ctime ;
	        	header[hi_rectab]	= op->rectab ;
	        	header[hi_reclen]	= op->reclen ;
	        	header[hi_ostrtab]	= op->ostrlen ;
	        	header[hi_ostrlen]	= op->ostrlen ;
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
} /* end subroutine (ttihdr_rd) */

int ttihdr_wr(ttihdr *op,cchar *hbuf,int hlen) noex {
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
                    if (op->vetu[0] != TTIHDR_VERSION) {
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
	                op->ctime	= header[hi_ctime] ;
	                op->rectab	= header[hi_rectab] ;
	                op->reclen	= header[hi_reclen] ;
	                op->ostrtab	= header[hi_ostrtab] ;
	                op->ostrlen	= header[hi_ostrlen] ;
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
} /* end subroutine (ttihdr_wr) */


/* local subroutines */

int ttihdr::rd(char *rbuf,int rlen) noex {
    	return ttihdr_rd(this,rbuf,rlen) ;
} /* end method (ttihdr::rd) */

int ttihdr::wr(cchar *wbuf,int wlen) noex {
    	return ttihdr_wr(this,wbuf,wlen) ;
} /* end method (ttihdr::wr) */


