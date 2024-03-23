/* mailmsg_enver SUPPORT */
/* lang=C++20 */

/* MAILMSG get-envelope */
/* version %I% last-modified %G% */

#define	CF_DIRECT	1		/* try the more direct approach */

/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets information about a MAILMSG envelope.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mailmsg.h>
#include	<mailmsgmatenv.h>
#include	<localmisc.h>

#include	"mailmsg_enver.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

#if	CF_DIRECT
#else
static int	isNotField(int) ;
#endif


/* local variables */

#if	CF_DIRECT
#else /* CF_DIRECT */
static const int	rsnofield[] = {
	SR_NOENT,
	SR_NOMSG,
	0
} ;
#endif /* CF_DIRECT */


/* exported variables */


/* exported subroutines */

#if	CF_DIRECT
int mailmsg_enver(mailmsg *op,int ei,mailmsg_enver *mep) noex {
	int		rs ;
	if ((rs = mailmsg_magic(op,mep)) >= 0) {
	    mailmsgenv		*ep ;
	    if ((rs = vecobj_get(op->elp,ei,&ep)) >= 0) {
	        mep->a.ep = ep->a.ep ;
	        mep->a.el = ep->a.el ;
	        mep->d.ep = ep->d.ep ;
	        mep->d.el = ep->d.el ;
	        mep->r.ep = ep->r.ep ;
	        mep->r.el = ep->r.el ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsg_enver) */
#else /* CF_DIRECT */
int mailmsg_enver(mailmsg *op,int ei,mailmsg_enver *mep) noex {
	int		rs ;
	if ((rs = mailmsg_magic(op,mep)) >= 0) {
	    cchar	*sp ;
	    memclear(mep) ;
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
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsg_enver) */
#endif /* CF_DIRECT */


/* provate subroutines */

#if	CF_DIRECT
#else /* CF_DIRECT */
static int isNotField(int rs) noex {
	return isOneOf(rsnofield,rs) ;
}
/* end subroutine (isNotField) */
#endif /* CF_DIRECT */


