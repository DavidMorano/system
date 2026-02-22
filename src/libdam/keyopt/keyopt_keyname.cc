/* keyopt_keyname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* paramater option manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	keyopt

	Description:
	This object is similar to the PARAMOPT object in some ways.
	This object does not provide (natually) any exponetial
	enumeration of the gathered options (like PARAMOPT does).
	Rather it allows several options (different options) to be
	specified together (or strung together) separated by commas.
	This is actually quite natural for most applications.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strncmp(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<sfx.h>
#include	<strn.h>
#include	<strkeycmp.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<localmisc.h>

#include	"keyopt.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	CUR		keyopt_cur
#define	VAL		keyopt_val
#define	NAM		keyopt_na


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace keyopt_keyname {
   int keyname_incri(NAM *pp) noex {
	VAL		*vp ;
	int		rs = SR_NOTFOUND ;
	if (pp->next != nullptr) {
	    rs = keyname_incri(pp->next) ;
	}
	/* increment if at bottom or if previous carried */
	if (rs == SR_NOTFOUND) {
	    vp = pp->current ;
	    if (vp->next == nullptr) {
	        pp->current = pp->head ;
	        rs = -1 ;
	    } else {
	        pp->current = vp->next ;
	        rs = 0 ;
	    } /* end if */
	} /* end if (not found) */
	return rs ;
    } /* end subroutine (keyname_incri) */
    int keyname_findv(NAM *pp,cchar *vbuf,int vlen,VAL **rp) noex {
	VAL		*vp ;
	bool		f = false ;
	if (vlen < 0) vlen = lenstr(vbuf) ;
	for (vp = pp->head ; vp != nullptr ; vp = vp->next) {
	    f = (strncmp(vp->value,vbuf,vlen) == 0) ;
	    f = f && (vp->value[vlen] == '\0') ;
	    if (f) break ;
	} /* end for */
	if (rp) *rp = vp ;
	return (f) ? SR_OK : SR_NOTFOUND ;
    } /* end subroutine (keyname_findv) */
} /* end namespace (keyopt_keyname) */


