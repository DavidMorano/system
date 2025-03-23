/* openshm SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* open a share-memory file in shared memory (really private) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	openshmtmpx
	openshmtmp

	Description:
	This subroutine creates and opens a POSIX® shared-memory
	temporary object.  The POSIX® SHM-type name of the object
	created is returned to the caller.

	Synopsis:
	int openshmtmpx(mode_t om) noex
	int openshmtmp(char *rbuf,int rlen,mode_r om) noex

	Arguments:
	rbuf		suppled buffer to receive the resulting SHM name
	rlen		length of suppled buffer; should be at least SHMNAMELEN
	om		SHM object creation opermissions

	Returns:
	>=0		file descriptor to program STDIN and STDOUT
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<ctime>			/* <- |time(2)| */
#include	<usystem.h>
#include	<syswords.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<sigblocker.h>
#include	<cthex.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"openshm.h"


/* local defines */

#define	EBUFLEN		(2 * szof(ulong))
#define	NTRIES		1000

#ifndef	SHMNAME_MAX
#define	SHMNAME_MAX	14		/* historic value (in decimal) */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	randinit(ulong *) noex ;
static int	mkshmname(char *,int,ulong) noex ;


/* local variables */

static bufsizevar	maxnamelen(getbufsize_mn) ;

static cint		tmplen = strlen(sysword.w_tmpdir) ;


/* exported variables */


/* exported subroutines */

int openshmtmpx(mode_t om) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;
	if (char *sbuf ; (rs = malloc_mn(&sbuf)) >= 0) {
	    cint	slen = rs ;
	    {
		rs = openshmtmp(sbuf,slen,om) ;
		fd = rs ;
	    }
	    rs1 = uc_free(sbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openshmtmpx) */

int openshmtmp(char *rbuf,int rlen,mode_t om) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (rbuf) {
	    if ((rs = maxnamelen) >= 0) {
		if (rlen < 0) rlen = rs ;
	        if (ulong rv{} ; (rs = randinit(&rv)) >= 0) {
	            if (sigblocker b ; (rs = b.start) >= 0) {
	                cint	of = (O_CREAT | O_EXCL | O_RDWR) ;
	                cint	ntries = NTRIES ;
	                for (int i = 0 ; i < ntries ; i += 1) {
	                    if ((rs = mkshmname(rbuf,rlen,rv)) >= 0) {
	                        rs = uc_openshm(rbuf,of,om) ;
	                        fd = rs ;
	                    }
		            rv += 1 ;
		            if (rs != SR_EXISTS) break ;
	                } /* end for */
	                if (rs >= 0) {
		            uc_unlinkshm(rbuf) ;
	 	        }
	                rs1 = b.finish ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (sigblock) */
		    if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
	        } /* end if (randinit) */
	    } /* end if (maxnamelen) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openshmtmp) */


/* local subroutines */

static int randinit(ulong *rvp) noex {
	int		rs = SR_FAULT ;
	if (rvp) {
	    custime	dt = getustime ;
	    if ((rs = ucpid) >= 0) {
	        ulong	rv = rs ;
	        ulong	sv = ulong(dt) ;
	        rv += (sv << 8) ;
	        rv += sv ;
	        *rvp = rv ;
	        rs = int(rv & INT_MAX) ;
	    } /* end if (ucpid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (randinit) */

static int mkshmname(char *rbuf,int rlen,ulong rv) noex {
	cint		elen = EBUFLEN ;
	int		rs ;
	cchar		*tpre = sysword.w_tmpdir ;
	char		ebuf[EBUFLEN+1] ;
	if ((rs = cthex(ebuf,elen,rv)) >= 0) {
	    cint	el = (SHMNAME_MAX - tmplen) ;
	    cint	i = (rs >= 16) ? ((rs - 16)+6) : 0 ;
	    rs = sncpy2w(rbuf,rlen,tpre,(ebuf + i),el) ;
	}
	return rs ;
}
/* end subroutine (mkshmname) */


