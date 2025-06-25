/* recip_copyparts SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* copies parts of a file to another file */
/* last modified %G% version %I% */


/* revision history:

	= 1990-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	recip_copyparts

	Description:
	This subroutine copies part of one file (specified by a
	container type) to another file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"recip.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int recip_copyparts(recip *rp,int tfd,int sfd) noex {
	int		rs = SR_FAULT ;
	int		tlen = 0 ;
	if (rp) {
	    auto	getmo = recip_getmo ;
	    int		i ; /* used-afterwards */
	    int		off = 0 ;
	    int		moff{} ;
	    int		mlen ;
	    int		clen = 0 ;
	    bool	f_disjoint = false ;
	    rs = SR_OK ;
	    /* first, figure out if we can do a contiguous copy or not */
	    for (i = 0 ; (mlen = getmo(rp,i,&moff)) >= 0 ; i += 1) {
	        f_disjoint = (off != moff) ;
	        if (f_disjoint) break ;
	        off += mlen ;
	        clen += mlen ;
	    } /* end for */
	    /* OK, do the copying according to whether contiguous or not */
	    if (f_disjoint) {
	        int	len = 0 ;
	        off = 0 ;
	        for (i = 0 ; (mlen = getmo(rp,i,&moff)) >= 0 ; i += 1) {
	            if (mlen > 0) {
	                if (moff != off) {
			    off_t	uoff = moff ;
	                    rs = uc_seek(tfd,uoff,SEEK_SET) ;
	                }
	                if (rs >= 0) {
	                    if ((rs = uc_writedesc(sfd,tfd,mlen)) > 0) {
			        len = rs ;
	                        off += len ;
	                        tlen += len ;
			    }
		        }
	            } /* end if */
	            if (rs < 0) break ;
	        } /* end for */
	    } else {
	        rs = 0 ;
	        if (i > 0) {
	            rs = uc_writedesc(sfd,tfd,clen) ;
	            tlen = rs ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (recip_copyparts) */


