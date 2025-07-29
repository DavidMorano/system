/* mkaltext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a file-name w/ an alternate extension */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkaltext

	Description:
	We take a file-name and create a new file-name replacing
	the existing extension with the new (alternative) extension
	supplied.

	Synopsis:
	int mkaltext(char *dbuf,cchar *name,cchar *ext) noex

	Arguments:
	dbuf		result buffer pointer
	name		source base-file-name
	ext		alternative extension

	Returns:
	>=0		length of result string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strrchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<bufsizevar.hh>
#include	<sbuf.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mkaltext(char *dbuf,cchar *name,cchar *ext) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf && name && ext) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        if (cchar *tp ; (tp = strrchr(name,'.')) != nullptr) ylikely {
	            if (tp[1] != '\0') ylikely {
			if ((rs = maxpathlen) >= 0) ylikely {
	                    cint	dlen = rs ;
	                    if (sbuf alt ; (rs = alt.start(dbuf,dlen)) >= 0) {
		                {
				    cint	nl = intconv(tp - name) ;
	                            alt.strw(name,nl) ;
	                            alt.chr('.') ;
	                            alt.strw(ext,-1) ;
		                }
	                        len = alt.finish ;
	                        if (rs >= 0) rs = len ;
	                    } /* end if (sbuf) */
			} /* end if (maxpathlen) */
	            } /* end if (had extension) */
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkaltext) */


