/* mknpathexp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make an expanded path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	mknpathexp

	Description:
	This subroutine creates a resolved filename path from the
	coded form.

	Synopsis:
	int mknpathexp(char *rbuf,int rlen,cchar *pp,int pl) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	pp		source path pointer
	pl		source path length

	Returns:
	>0		expansion
	==0		no expansion
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mkx.h"
#include	"mknpathxx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lensr(3u)| + |getlenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mknpathexp(char *rbuf,int rlen,cchar *pp,int µpl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf && pp) ylikely {
	    rbuf[0] = '\0' ;
	    if (int pl ; (pl = getlenstr(pp,µpl)) > 0) ylikely {
	        if ((rs = mknpathuser(rbuf,rlen,np,pp,pl)) == 0) ylikely {
	            rs = mknpathvar(rbuf,rlen,pp,pl) ;
		    rl = rs ;
		}
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (mknpathexp) */


