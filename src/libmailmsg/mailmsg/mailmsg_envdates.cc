/* mailmsg_envdates SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extract the environment date (if any) from a message */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Object:
	mailmsg

	Description:
	This subroutine searches through the various envelope dates
	and returns the earliest one found.  Message header dates
	are ignored.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
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

int mailmsg_envdates(mailmsg *op,TIMEB *nowp,char *zname,time_t *rp) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if ((rs = mailmsg_magic(op,nowp,zname,rp)) >= 0) {
	    if (dater d ; (rs = dater_start(&d,nowp,zname,-1)) >= 0) {
	        cauto	mef = mailmsg_envdate ;
	        time_t	posttime = 0 ;
	        cchar	*sp ;
	        for (int i = 0 ; (rs = mef(op,i,&sp)) >= 0 ; i += 1) {
	            if (sp) {
		        if (cint sl = rs ; sl > 0) {
	                    if ((rs = dater_setstd(&d,sp,sl)) >= 0) {
	                        time_t envtime ; 
				if ((rs = dater_gettime(&d,&envtime)) >= 0) {
	                            n += 1 ;
	                            if (posttime != 0) {
	                                if (envtime < posttime) {
	                                    posttime = envtime ;
				        }
	                            } else {
	                                posttime = envtime ;
			            }
				} /* end if (dater_gettime) */
	                    } /* end if (had a good date) */
	                } /* end if (we got a good date) */
		    } /* end if (non-null) */
	            if (rs < 0) break ;
	        } /* end for (looping through envelopes) */
	        if (rp) {
	            *rp = posttime ;
	        } /* end if */
	        rs1 = dater_finish(&d) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (dater) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailmsg_envdates) */


