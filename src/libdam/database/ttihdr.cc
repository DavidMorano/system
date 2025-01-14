/* ttihdr SUPPORT */
/* encoding=ISO8859-1 */
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

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
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
#include	<hasx.h>
#include	<localmisc.h>

#include	"ttihdr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

enum his {
	hi_fsize,			/* file-size */
	hi_ctime,			/* creation-time */
	hi_rectab,			/* record-table */
	hi_reclen,			/* record-legnth */
	hi_ostrtab,			/* overflow-string-table */
	hi_ostrlen,			/* overflow-string-table length */
	hi_overlast
} ;


/* forward references */


/* local variables */

constexpr int		headsize = hi_overlast * szof(uint) ;
constexpr int		magicsize = TTIHDR_MAGICSIZE ;
constexpr char		magicstr[] = TTIHDR_MAGICSTR ;


/* exported variables */


/* exported subroutines */

int ttihdr_rd(ttihdr *op,char *hbuf,int hlen) noex {
        int             rs = SR_FAULT ;
        int             len = 0 ;
        if (op && hbuf) {
            int         bl = hlen ;
            char        *bp = hbuf ;
            if (bl >= (magicsize + 4)) {
                if ((rs = mkmagic(bp,magicsize,magicstr)) >= 0) {
                    bp += magicsize ;
                    bl -= magicsize ;
                    memcpy(bp,op->vetu,4) ;
                    bp[0] = TTIHDR_VERSION ;
                    bp[1] = ENDIAN ;
                    bp += 4 ;
                    bl -= 4 ;
                    if (bl >= headsize) {
                        uint    *header = uintp(bp) ;
	        	header[hi_fsize] = op->fsize ;
	        	header[hi_ctime] = op->ctime ;
	        	header[hi_rectab] = op->rectab ;
	        	header[hi_reclen] = op->reclen ;
	        	header[hi_ostrtab] = op->ostrlen ;
	        	header[hi_ostrlen] = op->ostrlen ;
                        bp += headsize ;
                        bl -= headsize ;
                        len = (bp - hbuf) ;
                    } else {
                        rs = SR_OVERFLOW ;
                    } /* end if */
                } /* end if (mkmagic) */
            } else {
                rs = SR_OVERFLOW ;
            }
        } /* end if (non-null) */
        return (rs >= 0) ? len : rs ;
}
/* end subroutine (ttihdr_rd) */

int ttihdr_wr(ttihdr *op,cchar *hbuf,int hlen) noex {
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
                if (bl >= 4) {
                    memcpy(op->vetu,bp,4) ;
                    if (op->vetu[0] != TTIHDR_VERSION) {
                        rs = SR_PROTONOSUPPORT ;
                    }
                    if ((rs >= 0) && (op->vetu[1] != ENDIAN)) {
                        rs = SR_PROTOTYPE ;
                    }
                    bp += 4 ;
                    bl -= 4 ;
                } else {
                    rs = SR_ILSEQ ;
                }
	        if (rs >= 0) {
	            if (bl >= headsize) {
	                uint	*header = uintp(bp) ;
	                op->fsize = header[hi_fsize] ;
	                op->ctime = header[hi_ctime] ;
	                op->rectab = header[hi_rectab] ;
	                op->reclen = header[hi_reclen] ;
	                op->ostrtab = header[hi_ostrtab] ;
	                op->ostrlen = header[hi_ostrlen] ;
	                bp += headsize ;
	                bl -= headsize ;
		        len = (bp - hbuf) ;
	            } else {
	                rs = SR_ILSEQ ;
	            }
	        } /* end if (ok) */
	    } else {
		rs = SR_ILSEQ ;
	    } /* end if (hasValidMagic) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ttihdr_wr) */


