/* getfdfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get an FD out of an FD-File name (if there is one) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Name:
	getfdfile

	Description:
	This subroutine tries to divine a file-descriptor (FD) from
	a (so-called) File-Descriptor filename.

	Synopsis:
	int getfdfile(cchar *fp,int fl) noex

	Arguments:
	fp		file-name c-string pointer
	fl		file-name c-string length

	Returns:
	>=0		a file-descriptor (FD) number 
	<0		error: (system-return)
				SR_FAULT
				SR_INVALID
				SR_DOM
				SR_BADF
				SR_EMPTY
				*other*

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |cfdec(3u)| */
#include	<stdfnames.h>		/* LIBU |stdfname(3u)| */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"getfdfile.h"		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |getlenstr(3u)| */
import ureserve ;			/* |is{x}latin(3u)| */

/* local defines */

#define	ISUPP(ch)	isupperlatin(ch)
#define	ISDIG(ch)	isdigitlatin(ch)


/* imported namespaces */

using libu::cfdec ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

local int	extfd(cchar *,int) noex ;

constexpr bool	isstar(int ch) noex attrconst {
	return (ch == '*') ;
} /* end subroutine (isstart) */


/* local variables */


/* exported variables */


/* exported subroutines */

int getfdfile(cchar *fp,int µfl) noex {
	int		rs = SR_FAULT ;
	int 		fd = -1 ;
	if (int fl = getlenstr(fp,µfl) ; fl >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((fl > 0) && fp[0]) ylikely {
		rs = SR_DOM ;
		if ((fl >= 2) && isstar(fp[0])) {
		    if (cint ch1 = mkchar(fp[1]) ; ISUPP(ch1)) {
		        rs = SR_BADF ;
	                if ((fd = getstdfname(fp,fl)) >= 0) {
	                    if (fd == stdfile_null) {
		                rs = SR_EMPTY ;
		            } else if (fd <= FD_STDLOG) {
			        rs = SR_OK ;
			    } else if (fd == stdfile_minus) {
				rs = SR_OK ;
				fd = FD_STDIN ;
			    } /* end if */
			} /* end if (getstdfname) */
	            } else if (ISDIG(ch1)) {
			rs = extfd(fp,fl) ;
			fd = rs ;
		    } /* end if (FD decision) */
		} else if ((fl == 1) && (fp[0] == '-')) {
		    rs = SR_OK ;
		    fd = FD_STDIN ;
		} /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (getfdfile) */


/* local subroutines */

local int extfd(cchar *sp,int sl) noex {
	int		rs = SR_DOM ;
	int		fd = -1 ;
	if ((sl > 0) && isstar(*sp)) {
	   rs = SR_BADF ;
	   sp += 1 ;
	   sl -= 1 ;
	   if (sl > 0) {
	       if (cint ch = mkchar(sp[0]) ; ISDIG(ch)) {
		    rs = cfdec(sp,sl,&fd) ;
	        }
	    } /* end if (ok) */
	} /* end if (has leading star) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (extfd) */


