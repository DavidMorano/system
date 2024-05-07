/* mknoise SUPPORT */
/* lang=C++20 */

/* retrieve process ID related noise from the system */
/* version %I% last-modified %G% */

#define	CF_STAT		1		/* |stat(2)| the file also */

/* revision history:

	= 2002-07-13, David A­D­ Morano
	This is just a quick hack to get some additional noise from
	a UNIX® system that is fairly portable (a big problem). I
	really failed here because the most noise we get is from
	the presence of the current processes on the machine and
	this is very non-portable on systems not running System V.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mknoise

	Description:
	This subroutine acquires some of the process noise in the
	system.  It basically just looks at the process IDs that
	exist at the moment of the call and returns a random sequence
	of data based on that.

	Synopsis:
	int mknoise(uint *a,int n) noex

	Arguments:
	a	array of integers to receive the noise
	n	number of integers supplied by caller

	Returns:
	>=0	number of intergers containing random data being returned
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fsdir.h>
#include	<cfdec.h>
#include	<strwcpy.h>
#include	<sha1.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */

#define	NENTS		1000


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct noisedata {
	int		maxi ;
	int		i ;
	int		c ;
	int		pass ;
	ushort		buf[NENTS + 1] ;
} ; /* end struct (noisedata) */


/* forward references */

static int	noisedata_start(noisedata *) noex ;
static int	noisedata_add(noisedata *,uint) noex ;
static int	noisedata_finish(noisedata *) noex ;
 
static int	noisedata_addtime(noisedata *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mknoise(uint *a,int n) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (a) {
	    rs = SR_INVALID ;
	    if (n > 0) {
	        noisedata	nd ;
	        if ((rs = noisedata_start(&nd)) >= 0) {
	            if ((rs = noisedata_addtime(&nd)) >= 0) {
	                if (nd.c > 0) {
	                    SHA1	d ;
	                    if ((rs = sha1_start(&d)) >= 0) {
	                        int	sz = nd.maxi * int(sizeof(ushort)) ;
	                        sha1_update(&d,(char *) nd.buf,sz) ;
	                        if (n >= 5) {
	                            sha1_digest(&d,(uchar *) a) ;
	                            n = 5 ;
	                        } else {
	                            uint	aa[5] ;
	                            sha1_digest(&d,(uchar *) aa) ;
	                            sz = n * int(sizeof(uint)) ;
	                            memcpy(a,aa,sz) ;
	                        }
	                        rs1 = sha1_finish(&d) ;
				if (rs >= 0) rs = rs1 ;
	                    } /* end if (sha1) */
	                } /* end if (positive) */
	            } /* end if (noisedata_addtime) */
	            rs1 = noisedata_finish(&nd) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (noisedata) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs > 0) ? n : rs ;
}
/* end subroutine (mknoise) */


/* local subroutines */

static int noisedata_addtime(noisedata *ndp) noex {
	TIMEVAL		tv ;
	int		rs ;
	if ((rs = uc_gettimeofday(&tv,nullptr)) >= 0) {
	    noisedata_add(ndp,(uint) (tv.tv_sec >> 16)) ;
	    noisedata_add(ndp,(uint) (tv.tv_sec >> 0)) ;
	    noisedata_add(ndp,(uint) (tv.tv_usec >> 16)) ;
	    noisedata_add(ndp,(uint) (tv.tv_usec >> 0)) ;
	}
	return rs ;
}
/* end subroutine (noisedata_addtime) */

static int noisedata_start(noisedata *ndp) noex {
	int		rs = SR_FAULT ;
	if (ndp) {
	    rs = SR_OK ;
	    ndp->i = 0 ;
	    ndp->c = 0 ;
	    ndp->maxi = 0 ;
	    ndp->pass = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (noisedata_start) */

static int noisedata_finish(noisedata *ndp) noex {
	int		rs = SR_FAULT ;
	if (ndp) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < ndp->maxi ; i += 1) {
	        ndp->buf[i] = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (noisedata_finish) */

static int noisedata_add(noisedata *ndp,uint v) noex {
	ndp->buf[ndp->i] ^= v ;
	ndp->i = (ndp->i + 1) % NENTS ;
	if (ndp->i > ndp->maxi) {
	    ndp->maxi = ndp->i ;
	}
	if (ndp->i == 0) {
	    ndp->pass += 1 ;
	}
	ndp->c += 1 ;
	return 0 ;
}
/* end subroutine (noisedata_add) */


