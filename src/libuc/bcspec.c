/* bcspec */
/* lang=C20 */

/* load a Bible-Citation-Specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-02-07, David A�D� Morano
	This subroutine was originally written.  

*/

/* Copyright � 2013 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This object parses and loads a given Bible-Citation-Specification
	string into itself. The given citation-specification string
	looks like:

		<bookname>[:]<chapter>[:<verse>]
	or
		<booknum>:<chapter>[:<verse>]

	Examples are:

		mat5:1
		mat:5:1
		40:5:1

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<char.h>
#include	<mkchar.h>
#include	<cfdec.h>
#include	<estrings.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"bcspec.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	siourbrk(cchar *,int,int) noex ;


/* local variables */


/* exported subroutines */

int bcspec_load(bcspec *op,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	if (op && sbuf) {
	    int		sl ;
	    cchar	*sp ;
	    rs = SR_OK ;
	    memclear(op) ;
	    op->c = 1 ;
	    op->v = 1 ;
	    if ((sl = sfshrink(sbuf,slen,&sp)) > 0) {
	        int	si ;
	        int	v ;
	        int	ch = mkchar(sp[0]) ;
	        if (isalphalatin(ch)) {
		    op->np = sp ;
	  	    op->nl = sl ;
	            if ((si = siourbrk(sp,sl,true)) > 0) {
	  	        op->nl = si ;
		        sp += si ;
		        sl -= si ;
		        if (sl > 0) {
			    ch = mkchar(sp[0]) ;
		 	    if (ch == ':') {
			        sp += 1 ;
			        sl -= 1 ;
			    }
		        }
	            } else {
		        sl = 0 ;
		    }
	        } else if (isdigitlatin(ch)) {
	            if ((si = siourbrk(sp,sl,true)) > 0) {
		        rs = cfdeci(sp,si,&v) ;
		        op->b = v ;
	            }
	        } else {
	            rs = SR_DOM ;
	        }
	        if ((rs >= 0) && (sl > 0)) {
		    cchar	*tp ;
		    if ((tp = strnchr(sp,sl,':')) != NULL) {
		        if ((rs = cfdeci(sp,(tp-sp),&v)) >= 0) {
			    op->c = v ;
			    sl -= ((tp+1)-sp) ;
			    sp = (tp+1) ;
			    if (sl > 0) {
		    	        rs = cfdeci(sp,sl,&v) ;
		    	        op->v = v ;
			    }
		        }
		    } else {
		        rs = cfdeci(sp,sl,&v) ;
		        op->c = v ;
		    }
	        }
	    } else {
	        rs = SR_DOM ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bcspec_load) */


/* local subroutines */

static int siourbrk(cchar *sp,int sl,int f_dig) noex {
	int		i = -1 ;
	int		ch ;
	bool		f = false ;
	for (i = 0 ; i < sl ; i += 1) {
	    ch = mkchar(sp[i]) ;
	    if (f_dig) {
		f = isdigitlatin(ch) ;
	    } else {
		f = isalphalatin(ch) ;
	    }
	    f = f || (ch == ':') ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (siourbrk) */


