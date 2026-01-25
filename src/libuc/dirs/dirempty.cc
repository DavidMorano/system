/* dirempty SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* is the given directory empty? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dirempty

	Description:
	We test if the given directory is empty.

	Synopsis:
	int dirempty(cchar *dname)

	Arguments:
	dname		directory name (as a string)

	Returns:
	>0		is empty
	==0		not empty
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<fsdir.h>
#include	<hasx.h>		/* |hasNotDots(3uc)| */
#include	<localmisc.h>

#include	"dirempty.h"


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

int dirempty(cchar *dname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = true ;
	if (dname) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0]) ylikely {
		if (char *nbuf ; (rs = lm_mn(&nbuf)) >= 0) ylikely {
		    cint	nlen = rs ;
	            if (fsdir d ; (rs = d.open(dname)) >= 0) ylikely {
	                fsdir_ent	de ;
	                while ((rs = d.read(&de,nbuf,nlen)) > 0) {
			    if (hasNotDots(de.name,rs)) {
				f = false ;
		            } /* end (not dots) */
		            if (!f) break ;
	                } /* end while */
	                rs1 = d.close ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (fsdir) */
		    rs1 = lm_free(nbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (dirempty) */


