/* mailmsg_envtimes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extract all environment times (if any) from a message */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailmsg_envtimes

	Description:
	This subroutine will extract all of the times (from the
	dates) out of all envelopes that may be present in the
	message.

	Symopsis:
	int mailmsg_envtimes(mailmsg *op,dater *dp,time_t *ta,int nents) noex

	Arguments:
	op		pointer to MAILMSG object
	dp		pointer to DATER object
	ta		array of 'time_t' entries
	nents		number of entries in 'time_t' array

	Returns:
	>=0		number of entries returned
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<dater.h>
#include	<localmisc.h>

#include	"mailmsg.h"


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

int mailmsg_envtimes(mailmsg *op,dater *dp,time_t *ta,int nents) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = mailmsg_magic(op,dp,ta)) >= 0) {
	    cauto	mef = mailmsg_envdate ;
	    cchar	*cp ;
	    for (int i = 0 ; (rs = mef(op,i,&cp)) >= 0 ; i += 1) {
	        if (cp && (cp[0] != '\0')) {
		    if ((rs = dater_setstd(dp,cp,-1)) >= 0) {
		        time_t	t ;
		        if ((rs = dater_gettime(dp,&t)) >= 0) {
	                    if (n < nents) ta[n++] = t ;
		        }
		    } else if (rs == SR_INVALID) {
		        rs = SR_OK ;
	            } /* end if (got a date) */
	        } /* end if (we got a good date) */
	        if (n >= nents) break ;
	        if (rs < 0) break ;
	    } /* end for (looping through envelopes) */
	    if ((n > 0) || (rs == SR_NOTFOUND)) {
	        rs = SR_OK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsg_envtimes) */


