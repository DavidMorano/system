/* fileliner SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* determine the number of lines in a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fileliner

	Description:
	Given a file-name we determine the number of lines the file
	has.  An optional comment character can be specified.  When
	a comment character is specified, only lines with some
	content located before the comment character will count as
	a line.  Note that a comment characters is indeed just a
	single characters, so things like specifying C-language
	comments are not possible.

	Synopsis:
	int fileliner(cchar *fname,int cc) noex

	Arguments:
	fname		file-path to check
	cc		optional comment character (when non-zero)

	Returns:
	<0		error
	>=0		number of lines in the file

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<six.h>			/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"fileliner.h"

#pragma		GCC dependency		"mod/ucstream.ccm"

import ucstream ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int reader(char *,int,cc *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int fileliner(cchar *fname,int cn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	        if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) ylikely {
		    cint	llen = rs ;
		    {
		        rs = reader(lbuf,llen,fname,cn) ;
		        n = rs ;
		    }
		    rs1 = lm_free(lbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (filelines) */


/* local subroutines */

local int reader(char *lbuf,int llen,cc *fn,int cn) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (ucstream sf ; (rs = sf.open(fn,"r")) >= 0) ylikely {
	    while ((rs = sf.readln(lbuf,llen)) > 0) {
		if (cn > 0) {
		    if (cint ll = rmeol(lbuf,rs) ; ll > 0) {
			cauto sw = siskipwhite ;
			if (int si ; (si = sw(lbuf,ll)) >= 0) {
			    if (ll > si) {
				if (lbuf[si] != cn) n += 1 ;
			    }
			}
		    } /* end if (rmeol) */
		} else {
		    n += 1 ;
		}
	    } /* end while */
	    rs1 = sf.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ucstream) */
	return (rs >= 0) ? rs : n ;
} /* end subroutine (reader) */


