/* taginfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* parse a tag (given a string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	taginfo

	Description:
	This subroutine parses a tag (given in a string) into its
	parts.

	Synopsis:
	int taginfo_parse(taginfo *op,cchar *sp,int sl) noex

	Arguments:
	op		object pointer
	sp		tag string
	sl		tag string length

	Returns:
	>=0		key-index (indenx to key beginning)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<cfdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"taginfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int taginfo_parse(taginfo *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		ki = 0 ; /* return-value */
	if (op && sp) {
	    cchar	*ssp = sp ;
	    /* initialize */
	    op->fnamelen = 0 ;
	    op->recoff = 0 ;
	    op->reclen = 0 ;
	    if (sl < 0) sl = lenstr(sp) ;
	    /* get the filename */
	    if (cchar *tp ; (tp = strnchr(sp,sl,':')) != nullptr) {
	        op->fnamelen = intconv(tp - sp) ;
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        /* get the tag offset */
	        if ((tp = strnchr(sp,sl,',')) != nullptr) {
		    cint tl = intconv(tp - sp) ;
		    if (ulong vv ; (rs = cfdecul(sp,tl,&vv)) >= 0) {
	                op->recoff = vv ;
	                sl -= intconv((tp + 1) - sp) ;
	                sp = (tp + 1) ;
	                if ((tp = strnchr(sp,sl,'\t')) != nullptr) {
	                    sl = intconv(tp - sp) ;
	                    ki = intconv((tp + 1) - ssp) ;
	                }
	                rs = cfdecul(sp,sl,&vv) ;
	                op->reclen = vv ;
	            } /* end if */
	        } else {
	            rs = SR_INVALID ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? ki : rs ;
} /* end subroutine (taginfo_parse) */


