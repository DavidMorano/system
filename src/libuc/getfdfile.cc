/* getfdfile SUPPORT */
/* lang=C++20 */

/* get an FD out of an FD-File name (if there is one) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

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
	<0		error:
				SR_FAULT
				SR_INVALID
				SR_DOM
				SR_BADFD
				SR_EMPTY
				*other*

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<char.h>
#include	<mkchar.h>
#include	<stdfnames.h>
#include	<cfdec.h>
#include	<matstr.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"getfdfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */

static int	extfd(cchar *,int) noex ;


/* local variables */

constexpr int	ch_star = '*' ;

constexpr bool	isstar(int ch) noex {
	return (ch == ch_star) ;
}


/* exported subroutines */

int getfdfilex(cchar *fp,int fl) noex {
	int		rs = SR_FAULT ;
	int 		fd = -1 ;
	if (fp) {
	    rs = SR_INVALID ;
	    if (fl < 0) fl = strlen(fp) ;
	    if ((fl > 0) && fp[0]) {
		rs = SR_DOM ;
		if ((fl >= 2) && isstar(fp[0])) {
		    cint	ch1 = mkchar(fp[1]) ;
		    rs = SR_BADFD ;
		    if (isupperlatin(ch1)) {
	                if ((fd = matstr(stdfnames,fp,fl)) >= 0) {
	                    if (fd == stdfile_null) {
		                rs = SR_EMPTY ;
		            } else {
			        rs = SR_OK ;
			    }
			} /* end if (matstr) */
	            } else if (isdigitlatin(ch1)) {
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
}
/* end subroutine (getfdfile) */


/* local subroutines */

static int extfd(cchar *sp,int sl) noex {
	int		rs = SR_DOM ;
	int		fd = -1 ;
	if (sl < 0) sl = strlen(sp) ;
	if ((sl > 0) && isstar(*sp)) {
	   rs = SR_BADFD ;
	   sp += 1 ;
	   sl -= 1 ;
	   if (sl > 0) {
	       cint	ch = mkchar(sp[0]) ;
	       if (isdigitlatin(ch)) {
		    rs = cfdeci(sp,sl,&fd) ;
	        }
	    } /* end if (ok) */
	}
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (extfd) */


