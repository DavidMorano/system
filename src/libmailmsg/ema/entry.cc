/* entry SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* E-Mail Address (entry) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	entry

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<char.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"ema.h"
#include	"entry.hh"


/* local defines */


/* imported namespaces */

using namespace::emaobj ;		/* namespace */


/* local typedefs */

typedef ema_ent		ent ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace emaobj {

   int entry_start(ema_ent *ep) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    rs = memclear(ep) ; /* dangerous */
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
		    ema *emap = cast_static<ema *>(ep->listp) ;
	            rs1 = ema_finish(emap) ;
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
	    cint	nsz = szof(ema) ;
	    ep->listp = nullptr ;
	    if (ema *nop{} ; (rs = uc_malloc(nsz,&nop)) >= 0) {
	        if ((rs = ema_start(nop)) >= 0) {
		    ema *emap = cast_static<ema *>(ep->listp) ;
	            if ((rs = ema_addents(nop,emap)) >= 0) {
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

} /* end namespace (emaobj) */

#if	COMMENT
static int entry_debugprint(ema_ent *ep,cchar *s) noex {
	if (ep == nullptr) return SR_FAULT ;
	if (s != nullptr)
	    debugprintf("entry_debugprint: s=%s\n",s) ;
	debugprintf("entry_debugprint: type=%u\n",ep->type) ;
	if (ep->op)
	    debugprintf("entry_debugprint: O %u %t\n",
	        ep->ol,ep->op,ep->ol) ;
	if (ep->ap)
	    debugprintf("entry_debugprint: A %u %t\n",
	        ep->al,ep->ap,ep->al) ;
	if (ep->rp)
	    debugprintf("entry_debugprint: R %u %t\n",
	        ep->rl,ep->rp,ep->rl) ;
	if (ep->cp)
	    debugprintf("entry_debugprint: C %u %t\n",
	        ep->cl,ep->cp,ep->cl) ;
	return SR_OK ;
}
/* end subroutine (entry_debugprint) */
#endif /* COMMENT */


