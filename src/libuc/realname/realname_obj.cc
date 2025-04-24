/* realname_obj SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manipulate real names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-13, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	realname

	Description:
	This modules manipulates "real names" (that is like the
	real name of a person) in various ways.

	Notes:
	Oh boy!  We want to be careful with those foreign names that
	have funny article-preposition (whatever?) words in front of
	their last names.  Some examples, of these might be: a, da, 
	de, do, du, mc, mac, o, ou, von, van, dell.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<new>
#include	<usystem.h>
#include	<sbuf.h>
#include	<storeitem.h>
#include	<dstr.h>
#include	<six.h>
#include	<snwcpyx.h>
#include	<toxc.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |REALNAME| */

#include	"realname.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int realname_esther::operator () () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case 0:
	        rs = realname_start(op,nullptr,0) ;
	        break ;
	    } /* end switch */
	}
	return rs ;
}
    
int realname_esther::operator () (cchar *sp,int sl) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case 0:
	        rs = realname_start(op,sp,sl) ;
	        break ;
	    } /* end switch */
	}
	return rs ;
}

int realname_esther::operator () (cchar **sa,int sn) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case 0:
		rs = SR_FAULT ;
		if (sa) {
		    rs = SR_INVALID ;
		    if (sn > 0) {
		        if ((rs = realname_start(op,nullptr,0)) >= 0) {
		            rs = realname_loadpieces(op,sa,sn) ;
			    if (rs < 0) {
			        realname_finish(op) ;
			    }
		        } /* end if (realname_start) */
		    } /* end if (valid) */
		} /* end if (non-null) */
	        break ;
	    } /* end switch */
	}
	return rs ;
}

int realname_esther::operator () (dstr *dsp) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case 0:
		rs = SR_FAULT ;
		if (dsp) {
		    if ((rs = realname_start(op,nullptr,0)) >= 0) {
		        rs = realname_loadparts(op,dsp) ;
			if (rs < 0) {
			    realname_finish(op) ;
			}
		    } /* end if (realname_start) */
		} /* end if (non-null) */
	        break ;
	    } /* end switch */
	}
	return rs ;
}

void realname::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("realname",rs,"fini-finish") ;
	}
}


