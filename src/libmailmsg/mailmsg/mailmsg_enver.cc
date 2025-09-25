/* mailmsg_envget SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* MAILMSG get-envelope */
/* version %I% last-modified %G% */

#define	CF_DIRECT	1		/* try the more direct approach */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mailmsg

	Description:
	This subroutine gets information about a MAILMSG envelope.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mailmsgmatenv.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"mailmsg.h"
#include	"mailmsg_envget.h"

import libutil ;

/* local defines */

#ifndef	CF_DIRECT
#define	CF_DIRECT	1		/* try the more direct approach */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static inline bool	isNotField(int) noex ;


/* local variables */

constexpr int		rsnofield[] = {
	SR_NOENT,
	SR_NOMSG,
	0
} ;

cbool			f_direct = CF_DIRECT ;


/* exported variables */


/* exported subroutines */

int mailmsg_envget(mailmsg *op,int ei,mailmsg_envdat *mep) noex {
	int		rs ;
	if ((rs = mailmsg_magic(op,mep)) >= 0) {
	    memclear(mep) ;
	    if_constexpr (f_direct) {
	        if (void *vp{} ; (rs = vecobj_get(op->elp,ei,&vp)) >= 0) {
	            mmenvdat	*ep = (mmenvdat *) vp ;
	            mep->a.ep = ep->a.ep ;
	            mep->a.el = ep->a.el ;
	            mep->d.ep = ep->d.ep ;
	            mep->d.el = ep->d.el ;
	            mep->r.ep = ep->r.ep ;
	            mep->r.el = ep->r.el ;
	        } /* end if (vecobj_get) */
	    } else {
	        cchar	*sp{} ;
	        if ((rs = mailmsg_envaddress(op,ei,&sp)) >= 0) {
	            mep->a.el = rs ;
	            mep->a.ep = sp ;
	            if ((rs = mailmsg_envdate(op,ei,&sp)) >= 0) {
	                mep->d.el = rs ;
	                mep->d.ep = sp ;
	                if ((rs = mailmsg_envremote(op,ei,&sp)) >= 0) {
	                    mep->r.el = rs ;
	                    mep->r.ep = sp ;
	                } else if (isNotField(rs)) {
	                    rs = SR_OK ;
		        }
	            } /* end if (mailmsg_envdate) */
	        } /* end if (mailmsg_envaddress) */
	    } /* end if_constexpr (f_direct) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsg_envget) */


/* provate subroutines */

static bool isNotField(int rs) noex {
	return isOneOf(rsnofield,rs) ;
}
/* end subroutine (isNotField) */


