/* splitfname SUPPORT */
/* lang=C++20 */

/* split a file-name into its 'dirname' and its 'basename' */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	splitfname_split

	Description:
	This object (splitfname) splits a file-name into its 'dirname"
	and its 'basename'.

	Synopsis:
	int splitfname_split(splitfname *op,cchar *fp,int fl) noex

	Arguments:
	op		object pointer
	fp		file-name pointer
	dl		file-name length

	Returns:
	>=0		OK
	<0		error-code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<strn.h>		/* |strnrchr(3uc)| */
#include	<localmisc.h>

#include	"splitfname.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int splitfname_split(splitfname *op,cchar *fp,int fl) noex {
	int		rs = SR_FAULT ;
	if (op && fp) {
	    if (fl < 0) fl = strlen(fp) ;
	    op->dp = fp ;
	    op->dl = fl ;
	    op->bp = fp ;
	    op->bl = fl ;
	    if (cchar *tp ; (tp = strnrchr(fp,fl,'/')) != nullptr) {
		op->bp = (tp + 1) ;
		op->bl = ((fp + fl) - (tp + 1)) ;
		op->dl = (tp - fp) ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (splitfname_split) */


/* local subroutines */

int splitfname::operator () (cchar *fp,int fl) noex {
	int		rs = SR_BUGCHECK ;
	if (this) {
	    rs = splitfname_split(this,fp,fl) ;
	}
	return rs ;
}
/* end method (splitfname::operator) */


