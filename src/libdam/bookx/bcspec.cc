/* bcspec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load a Bible-Citation-Specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-02-07, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bcspec

	Description:
	This object parses and loads a given Bible-Citation-Specification
	string into itself.  The given citation-specification string
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
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<climits>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<estrings.h>		/* LIBU */
#include	<cfdec.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"bcspec.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	siourbrk(cchar *,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int bcspec_load(bcspec *op,cchar *sbuf,int slen) noex {
    	BCSPEC		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && sbuf) {
	    cchar	*sp{} ;
	    rs = memclear(hop) ;
	    op->c = 1 ;
	    op->v = 1 ;
	    if (int v, si, sl ; (sl = sfshrink(sbuf,slen,&sp)) > 0) {
	        int	ch = mkchar(sp[0]) ;
	        if (isalphalatin(ch)) {
		    op->sp = sp ;
	  	    op->sl = sl ;
	            if ((si = siourbrk(sp,sl,true)) > 0) {
	  	        op->sl = si ;
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
		        op->b = schar(v) ;
	            }
	        } else {
	            rs = SR_DOM ;
	        } /* end if */
	        if ((rs >= 0) && (sl > 0)) {
		    if (cchar *tp = strnchr(sp,sl,':') ; tp) {
			cint tl = intconv(tp - sp) ;
		        if ((rs = cfdeci(sp,tl,&v)) >= 0) {
			    op->c = schar(v) ;
			    sl -= intconv((tp + 1) - sp) ;
			    sp = (tp+1) ;
			    if (sl > 0) {
		    	        rs = cfdeci(sp,sl,&v) ;
		    	        op->v = schar(v) ;
			    }
		        } /* end if (cfdec) */
		    } else {
		        rs = cfdeci(sp,sl,&v) ;
		        op->c = schar(v) ;
		    }
	        } /* end if */
	    } else {
	        rs = SR_DOM ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (bcspec_load) */


/* local subroutines */

local int siourbrk(cchar *sp,int sl,int f_dig) noex {
	int		i = -1 ; /* used afterwards */
	bool		f = false ;
	for (i = 0 ; i < sl ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    if (f_dig) {
		f = isdigitlatin(ch) ;
	    } else {
		f = isalphalatin(ch) ;
	    }
	    f = f || (ch == ':') ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
} /* end subroutine (siourbrk) */


