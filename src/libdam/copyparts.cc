/* copyparts SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* copies parts of a file to another file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug prints */

/* revision history:

	= 1990-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	copyparts

	Description:
	This subroutine copies part of one file (specified by
	a container type) to another file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<recip.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	BUFLEN		256


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int copyparts(recip *rp,int tfd,int sfd) noex {
	struct md	*mop ;
	off_t	uoff ;

	uint	off ;

	int	rs = SR_OK ;
	int	i, clen, tlen ;
	int	len ;
	int	f_contiguous ;


/* first, figure out if we can do a contiguous copy or not */

	off = 0 ;
	clen = 0 ;
	for (i = 0 ; (rs = vecitem_get(&rp->mds,i,&mop)) >= 0 ; i += 1) {
	    if (mop == NULL) continue ;
	    if (off != mop->offset) break ;

	    off += mop->mlen ;
	    clen += mop->mlen ;

	} /* end for */

#if	CF_DEBUGS
	debugprintf("copyparts: search rs=%d\n",rs) ;
#endif

	f_contiguous = (rs == SR_NOTFOUND) ;

#if	CF_DEBUGS
	debugprintf("copyparts: f_contiguous=%u\n",f_contiguous) ;
#endif

/* OK, do the copying according to whether contiguous or not */

	if (! f_contiguous) {

	    off = 0 ;
	    tlen = 0 ;
	    for (i = 0 ; vecitem_get(&rp->mds,i,&mop) >= 0 ; i += 1) {
	        if (mop == NULL) continue ;
	        rs = 0 ;
	        if (mop->mlen > 0) {

	            if (mop->offset != off) {

	                off = mop->offset ;
			uoff = off ;
	                rs = u_seek(tfd,uoff,SEEK_SET) ;

#if	CF_DEBUGS
	                debugprintf("copyparts: u_seek() rs=%d\n",rs) ;
#endif

	            }

	            if (rs >= 0) {
	                rs = uc_copy(tfd,sfd,mop->mlen) ;
			len = rs ;
		    }

	        }

	        if (rs < 0)
	            break ;

	        if (len > 0) {
	            off += len ;
	            tlen += len ;
	        }

	    } /* end for */

	} else {

#if	CF_DEBUGS
	    debugprintf("copyparts: copy i=%d clen=%d\n",i,clen) ;
#endif

	    rs = 0 ;
	    if (i > 0) {
	        rs = uc_copy(tfd,sfd,clen) ;
	        tlen = rs ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("copyparts: ret rs=%d tlen=%d\n",rs,tlen) ;
#endif

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (copyparts) */


