/* getproto SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a protocol number given a protocol name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getproto_name

	Description:
	Retrieve a protocol number given a protocol name.

	Synopsis:
	int getproto_name(cchar *pp,int pl) noex

	Arguments:
	pp		protocol name string pointer
	pl		protocol name string length

	Returns:
	>=0		protocol number
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucgetpr.h>		/* LIBUC */
#include	<nulstr.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"getproto.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getproto_name(cchar *pnp,int pnl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		proto = 0 ; /* return-value */
	if (pnp) ylikely {
	    rs = SR_INVALID ;
	    if (pnp[0]) ylikely {
	        cchar	*pname{} ;
	        if (nulstr n ; (rs = n.start(pnp,pnl,&pname)) >= 0) ylikely {
	    	    if (char *prbuf ; (rs = lm_pr(&prbuf)) >= 0) ylikely {
	                ucentpr		pr ;
	                cint		prlen = rs ;
	                if ((rs = uc_getprnam(&pr,prbuf,prlen,pname)) >= 0) {
	                    proto = pr.p_proto ;
	                }
		        rs1 = lm_free(prbuf) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	            rs1 = n.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? proto : rs ;
} /* end subroutine (getproto_name) */


