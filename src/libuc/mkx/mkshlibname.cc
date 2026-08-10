/* mkshlibname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a Shared-Objext Name */
/* version %I% last-modified %G% */


/* revision history:

	- 2003-11-04, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkshlibname

	Description:
	This subroutine creates (makes) a Shared-Object Name from
	various components.

	Synopsis:
	int mkshlibname(char *rbuf,cchar *pre,cchar *base,cchar *ext) noex

	Arguments:
	rbuf		result buffer
	pre		optional filename prefix
	base		base-part of final filename
	ext		optional extension resulting filename

	Returns:
	>=0		length of resulting buffer
	<0		erro (system-return)

	See-also:
	mksofname
	mksoname
	mkshlibname

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |getustime(3u)| */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"mkshlibname.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */
 

/* local structures */


/* forward references */



/* local variables */

static bufsizevar		maxnamelen(bufsize_mn) ;


/* exported variables */


/* exported subroutines */

int mkshlibname(char *rbuf,cchar *pre,cchar *base,cchar *ext) noex {
    	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf && base) ylikely {
	    rs = SR_INVALID ;
	    if (base[0]) ylikely {
	        if ((rs = maxnamelen) >= 0) ylikely {
    	            storebuf sb(rbuf,rs) ;
	            if (pre && pre[0]) {
		        if ((rs = sb.str(pre)) >= 0) {
			    rs = sb.str("_") ;
			}
	            } /* end if (prefix) */
	            if (rs >= 0) ylikely {
			if ((rs = sb.str(base)) >= 0) ylikely {
			    if (ext && ext[0]) {
				if ((rs = sb.chr('.')) >= 0) {
				    rs = sb.str(ext) ;
				}
			    } /* end if (extension) */
		            rl = sb.idx ;
			} /* end if (base) */
		    } /* end if (ok) */
	        } /* end if (maxnamelen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (mkshlibname) */


