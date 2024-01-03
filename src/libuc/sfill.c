/* sfill */
/* lang=C20 */

/* text fill */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object mediates filling out text to an output file
	using the BFILE package.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<string.h>
#include	<usystem.h>
#include	<estrings.h>
#include	<bfile.h>
#include	<fifostr.h>
#include	<localmisc.h>

#include	"sfill.h"


/* local defines */

#ifndef	WORDBUFLEN
#define	WORDBUFLEN	100
#endif


/* external subroutines */

extern int	bwriteblanks(bfile *,int) noex ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int sfill_start(sfill *op,int indent,bfile *ofp) noex {
	int		rs = SR_FAULT ;
	if (op && ofp) {
	    memset(op,0,sizeof(SFILL)) ;
	    op->ofp = ofp ;
	    op->indent = indent ;
	    rs = fifostr_start(&op->sq) ;
	}
	return rs ;
}
/* end subroutine (sfill_start) */

int sfill_finish(sfill *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    rs1 = fifostr_finish(&op->sq) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sfill_finish) */

int sfill_remaining(sfill *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->clen ;
	}
	return rs ;
}
/* end subroutine (sfill_remaining) */

int sfill_proc(sfill *op,int olinelen,cchar *linebuf,int linelen) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op && linebuf) {
	    cchar	*sp = linebuf ;
	    cchar	*cp ;
	    int		sl = linelen ;
	    int		cl ;
	    rs = SR_OK ;
	    while ((cl = nextfield(sp,sl,&cp)) > 0) {
	        rs = fifostr_add(&op->sq,cp,cl) ;
	        op->clen += (cl + 1) ;
	        sl -= ((cp + cl) - sp) ;
	        sp = (cp + cl) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if (rs >= 0) {
	        while ((rs >= 0) && (op->clen > olinelen)) {
	            rs = sfill_wline(op,olinelen) ;
	            wlen += rs ;
	        } /* end while */
	    } /* end if */
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (sfill_proc) */

int sfill_wline(sfill *op,int olinelen) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op) {
	    bfile	*ofp = op->ofp ;
	    int		cl ;
	    int		c_words = 0 ;
	    cchar	*cp ;
	    char	wordbuf[WORDBUFLEN + 1] ;
	    rs = SR_OK ;
	    if (op->indent > 0) {
	        rs = bwriteblanks(ofp,op->indent) ;
	        wlen += rs ;
	    }
	    while ((rs >= 0) && (wlen < olinelen)) {
	        cp = wordbuf ;
	        cl = fifostr_headread(&op->sq,wordbuf,WORDBUFLEN) ;
	        if (cl == SR_NOENT) break ;
	        rs = cl ;
	        if (rs < 0) break ;
	        if ((wlen + (cl + 1)) > olinelen) break ;
	        if (c_words++ > 0) {
	            rs = bputc(ofp,' ') ;
	            wlen += 1 ;
	        }
	        if (rs >= 0) {
	            rs = bwrite(ofp,cp,cl) ;
	            wlen += rs ;
	        }
	        if (rs >= 0) {
	            rs = fifostr_remove(&op->sq,NULL,0) ;
	        }
	        if (rs < 0) break ;
	    } /* end while (getting words) */
	    if (rs >= 0) {
	        rs = bputc(ofp,'\n') ;
	        wlen += 1 ;
	    }
	    if (rs >= 0) {
	        op->clen -= wlen ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (sfill_wline) */


