/* mkpathrooted SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a file rooted from the present-working-directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkpathrooted

	Description:
	This subroutine examines the source file or path given
	and returns either the original source-path as it is
	or an absolute-path version it, depending on whether the
	source as already absolute or not.

	Synopsis:
	int mkpathrooted(char *rbuf,cchar *sbuf) noex

	Arguments:
	rbuf		result buffer
	sbuf		source buffer

	Returns:
	>=0		length of resulting c-string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<mkpath.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"mkx.h"
#include	"mkpathrooted.h"


/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkpathrooted(char *rbuf,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf && fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
	        if (fn[0] != '/') {
	            if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) ylikely {
	                cint	plen = rs ;
	                if ((rs = getpwd(pbuf,plen)) >= 0) ylikely {
	                    rs = mkpath(rbuf,pbuf,fn) ;
		            rl = rs ;
	                } /* end if (getpwd) */
	                rs1 = lm_free(pbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (rooting file) */
	        } else {
		    rs = mkpath(rbuf,fn) ;
		    rl = rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end if (mkpathrooted) */


