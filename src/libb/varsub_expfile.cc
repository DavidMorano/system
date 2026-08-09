/* varsub_expfile SUPPORT */
/* varsub_addvec SUPPORT */
/* lang=C++20 */

/* module to handle variable substitution in strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This code was originally written.

	= 1999-07-12, David A­D­ Morano
	Believe it or not I did not like the treatment that zero
	length values were getting!  I modified the |varsub_add|
	subroutine to allow zero-length values in the default case.

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	varsub

	Description:
	This module performs substitutions on strings that have
	variable substitution escapes of some sort in them. The
	variable substitution escapes look like environment variable
	use within (for example) the Bourne and Korn shells.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<strwcmp.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strnxchr.h>		/* LIBUC */
#include	<strnxcmp.h>		/* LIBUC |strnncmp(3uc)| */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<varsub.h>		/* LIBDAM */
#include	<bfile.h>		/* LIBB */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */
import varsub_util ;

/* local defines */

#define	VS		varsub
#define	VS_NLINES	10


/* imported namespaces */

using varsub_ns::varsub_sort ;		/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	varsub_expfiler	(VS *,bfile *,bfile *) noex ;
local int	varsub_writebuf	(VS *,bfile *,buffer *) noex ;


/* local variables */

static bufsizevar	maxlinelen(bufsize_ml) ;


/* exported variables */


/* exported subroutines */

int varsub_expfile(VS *op,bfile *ifp,bfile *ofp) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = varsub_magic(op,ifp,ofp)) >= 0) ylikely {
	    if ((rs = varsub_sort(op)) >= 0) ylikely {
		rs = varsub_expfiler(op,ifp,ofp) ;
		wlen = rs ;
	    } /* end if (varsub_sort) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (varsub_expfile) */


/* private subroutines */

local int varsub_expfiler(VS *op,bfile *ifp,bfile *ofp) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) ylikely {
	    cint	llen = rs ;
	    if ((rs = maxlinelen) >= 0) ylikely {
	        cint	stlen = (VS_NLINES * rs) ;
	        op->badline = -1 ;
	        if (buffer b ; (rs = b.start(stlen)) >= 0) {
		    int		nlines = 0 ;
	            while ((rs = ifp->readln(lbuf,llen)) > 0) {
	                int	len = rs ;
	                if (lbuf[len-1] == '\n') nlines += 1 ;
	                if ((rs = varsub_expbuf(op,&b,lbuf,len)) >= 0) {
	                    if (nlines >= VS_NLINES) {
	                        nlines = 0 ;
	                        rs = varsub_writebuf(op,ofp,&b) ;
			        wlen += rs ;
			    }
	                } /* end if (flush) */
	                if (rs < 0) break ;
	            } /* end while (reading file lines) */
	            if ((rs >= 0) && (nlines > 0)) {
	                rs = varsub_writebuf(op,ofp,&b) ;
		        wlen += rs ;
	            } /* end if */
	            rs1 = b.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (buffer) */
	    } /* end if (maxlinelen) */
	    rs1 = lm_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (varsub_expfiler) */

local int varsub_writebuf(VS *op,bfile *ofp,buffer *bufp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (op) ylikely {
	    if (cchar *bp{} ; (rs = bufp->get(&bp)) > 0) ylikely {
	        if ((rs = ofp->write(bp,rs)) >= 0) {
	            wlen += rs ;
	            bufp->reset() ;
	        }
	    } /* end if (buffer_get) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (varsub_writebuf) */


