/* splitfname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* split a file-name into its 'dirname' and its 'basename' */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

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
	fl		file-name length

	Returns:
	>=0		OK
	<0		error-code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<strn.h>		/* LIBUC |strnrchr(3uc)| */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"splitfname.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int splitfname_split(splitfname *op,cchar *fp,int fl) noex {
	int		rs = SR_FAULT ;
	int		bl = 0 ; /* return-value */
	if (op && fp) ylikely {
	    {
		strnul	ps(fp,fl) ;
	        DPRINTF("ent fl=%d f=>%s<\n",fl,ccp(ps)) ;
	    }
	    if (fl < 0) fl = lenstr(fp) ;
	    op->dp = fp ;
	    op->dl = fl ;
	    op->bp = fp ;
	    op->bl = fl ;
	    rs = SR_NOTDIR ;
	    if (cchar *tp = strnrchr(fp,fl,'/') ; tp) {
		op->bp = (tp + 1) ;
		op->bl = intconv((fp + fl) - (tp + 1)) ;
		op->dl = intconv(tp - fp) ;
		bl = op->bl ;
		rs = SR_OK ;
	    } /* end if */
	} /* end if (non-null) */
	DPRINTF("ent rs=%d bl=%d\n",rs,bl) ;
	return (rs >= 0) ? bl : rs ;
} /* end subroutine (splitfname_split) */


/* local subroutines */

int splitfname::operator () (cchar *fp,int fl) noex {
	int		rs = SR_BUGCHECK ;
	if (fp) ylikely {
	    rs = splitfname_split(this,fp,fl) ;
	}
	return rs ;
} /* end method (splitfname::operator) */


