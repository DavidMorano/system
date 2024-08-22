/* filebinary SUPPORT */
/* lang=C++20 */

/* determine if the named file is a binary file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2018-10-01, David A.D. Morano
	I refactored for some clarity.

	= 2018-10-02, David A.D. Morano
	I changed to use buffered file operation.

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	filebinary

	Description:
	Given a file-name we determine if it is a binary file.  In
	other words, we detemine if the passed file (referenced by
	filename) contains binary data or not.  Binary data is any
	data that is not a normal character in each byte position
	of the file.  Non-normal characters are any characters lying
	within the control-0 or control-1 range of the ISO-8859-1
	(otherwise known as ISO Latin-1) character set.  We also
	recognize as normal characters all of the old C-language
	escape characters (not escaping an existing normal character).
	The present C-language escapes which we recognize are:

	\a \b \f \n \r \t \v

	Enjoy.
	
	Synopsis:
	int filebinary(cchar *fname) noex

	Arguments:
	fname		file-path to check

	Returns:
	>0		is an object file
	==0		not an object file
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<usystem.h>
#include	<ascii.h>
#include	<linebuffer.h>
#include	<bfile.h>
#include	<localmisc.h>

#include	"filebinary.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static bool	hasbinary(cchar *,int) noex ;
static bool	isbinary(int) noex ;
static bool	isc0(int) noex ;
static bool	isc1(int) noex ;


/* local variables */

static constexpr cchar	allowed[] = "\a\b\f\n\r\t\v" ;


/* exported variables */


/* exported subroutines */

int filebinary(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        linebuffer	lb ;
	        if ((rs = lb.start) >= 0) {
	            bfile	ifile, *ifp = &ifile ;
	            if ((rs = bopen(ifp,fname,"r",0666)) >= 0) {
	                USTAT	sb ;
	                if ((rs = bstat(ifp,&sb)) >= 0) {
		            if (S_ISREG(sb.st_mode)) {
				cint	ll = lb.llen ;
				char	*lp = lb.lbuf ;
		                while ((rs = breadln(ifp,lp,ll)) > 0) {
		                    f = hasbinary(lp,rs) ;
		                    if (f) break ;
		                } /* end while */
		            } else {
		                f = true ; /* whatever! */
		            }
	                } /* end if (bstat) */
	                rs1 = bclose(ifp) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (open) */
		    rs1 = lb.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (linebuffer) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (filebinary) */


/* local subroutines */

static bool hasbinary(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl-- && sp[0]) {
	    f = isbinary(sp[0]) ;
	    if (f) break ;
	    sp += 1 ;
	} /* end while */
	return f ;
}
/* end subroutine (hasbinary) */

static bool isbinary(int ch) noex {
	bool		f = false ;
	ch &= UCHAR_MAX ;
	if (isc0(ch)) {
	    f = (ch != '\n') && (strchr(allowed,ch) == nullptr) ;
	} else if (isc1(ch)) {
	    f = true ;
	}
	return f ;
}
/* end subroutine (isbinary) */

static bool isc0(int ch) noex {
	return (ch < 0x20) ;
}
/* end subroutine (isc0) */

static bool isc1(int ch) noex {
	return ((ch >= 0x80) && (ch < 0xA0)) ;
}
/* end subroutine (isc1) */


