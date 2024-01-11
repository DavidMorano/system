/* entry SUPPORT */
/* lang=C++20 */

/* E-Mail Address (entry) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<usupport.h>		/* for |memclear(3u)| */
#include	<mallocstuff.h>
#include	<strn.h>
#include	<char.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"ema.h"
#include	"entry.hh"


/* local defines */


/* local namespaces */

using namespace::emaobj ;		/* namespace */
using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef ema_ent		ent ;


/* external subroutines */


/* external variables */


/* local structures */


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace emaobj {

   int entry_start(ema_ent *ep) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    memclear(ep) ;
	    ep->type = ematype_reg ;
	}
	return rs ;
    } /* end subroutine (entry_start) */

   int entry_finish(ema_ent *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->op != nullptr) {
	        rs1 = uc_free(ep->op) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->op = nullptr ;
	    }
	    if (ep->ap != nullptr) {
	        rs1 = uc_free(ep->ap) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->ap = nullptr ;
	    }
	    if (ep->rp != nullptr) {
	        rs1 = uc_free(ep->rp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->rp = nullptr ;
	    }
	    if (ep->cp != nullptr) {
	        rs1 = uc_free(ep->cp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->cp = nullptr ;
	    }
	    if (ep->listp != nullptr) {
		{
	            rs1 = ema_finish(ep->listp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(ep->listp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        ep->listp = nullptr ;
	    } /* end if (recursive free-up) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (entry_finish) */

   int entry_startload(ema_ent *ep,ema_ent *oep) noex {
	int		rs = SR_OK ;
	*ep = *oep ;
	if (oep->op != nullptr) {
	    ep->op = mallocstrw(oep->op,oep->ol) ;
	}
	if (oep->ap != nullptr) {
	    ep->ap = mallocstrw(oep->ap,oep->al) ;
	}
	if (oep->rp != nullptr) {
	    ep->rp = mallocstrw(oep->rp,oep->rl) ;
	}
	if (oep->cp != nullptr) {
	    ep->cp = mallocstrw(oep->cp,oep->cl) ;
	}
	if (oep->listp != nullptr) {
	    ema		*nop = nullptr ; /* LINT assignment */
	    cint	size = sizeof(ema) ;
	    ep->listp = nullptr ;
	    if ((rs = uc_malloc(size,&nop)) >= 0) {
	        if ((rs = ema_start(nop)) >= 0) {
	            if ((rs = ema_addents(nop,oep->listp)) >= 0) {
	                ep->listp = nop ;
	            }
	            if (rs < 0) {
	                ema_finish(nop) ;
		    }
	        } /* end if (ema_start) */
	        if (rs < 0) {
	            uc_free(nop) ;
		}
	    } /* end if (allocation) */
	} /* end if (non-nullptr) */
	return rs ;
    } /* end subroutine (entry_startload) */

}


