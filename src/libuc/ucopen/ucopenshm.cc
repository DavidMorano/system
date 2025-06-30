/* ucopenshm SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open POSIX® share-memory (w/ time-out) */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_openshmto
	uc_openshm

	Description:
	This subroutine opens a POSIX® shared-memory (|sem(3rt)|)
	object but with a time-out. What does this mean to have a
	time-out while trying to open a shared memory segment? It
	means that if the segment is access protected, we continue
	to try until the time-out value has expired.

	Synopsis:
	int uc_openshmto(cchar *shmname,int of,mode_t om,int to) noex
	int uc_openshm(cchar *shmname,int of,mode_t om) noex

	Arguments:
	shmname		c-string naming shared-memory-segment
	of		open flags
	om		open mode
	to		time-out

	Returns:
	>=0		OK (file-descriptor)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |mode_t| */
#include	<sys/mman.h>		/* SHM asks for this */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<errtimer.hh>
#include	<posname.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	ucopenshm(cc *,int,mode_t) noex ;


/* local variables */

const int	onethousand = 1000 ;	/* One-Thousand */


/* exported variables */


/* exported subroutines */

int uc_openshmto(cchar *shmname,int of,mode_t om,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	if (to < 0) to = INT_MAX ;
	if (shmname) {
	    rs = SR_INVALID ;
	    if (shmname[0]) {
	        cchar	*nn ;
	        if (posname pn ; (rs = pn.start(shmname,-1,&nn)) >= 0) {
	            for (int i = 0 ; to-- >= 0 ; i += 1) {
	                if (i > 0) {
			    rs = msleep(onethousand) ;
			}
			if (rs >= 0) {
		            rs = ucopenshm(nn,of,om) ;
	                    fd = rs ;
			}
	                if (rs != SR_ACCESS) break ;
	            } /* end for */
	            if (rs == SR_ACCESS) rs = SR_TIMEDOUT ;
		    rs1 = pn.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (posname) */
	        if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openshmto) */

int uc_openshm(cchar *shmname,int of,mode_t om) noex {
    	return uc_openshmto(shmname,of,om,-1) ;
}
/* end subroutine (uc_openshm) */


/* local subroutines */

static int ucopenshm(cc *name,int of,mode_t om) noex {
	errtimer	to_mfile = utimeout[uto_mfile] ;
	errtimer	to_nfile = utimeout[uto_nfile] ;
	errtimer	to_nomem = utimeout[uto_nomem] ;
	errtimer	to_nospc = utimeout[uto_nospc] ;
        reterr          r ;
    	int		rs ;
        repeat {
	    errno = 0 ;
	    if ((rs = shm_open(name,of,om)) < 0) {
                rs = (- errno) ;
                r(rs) ;                 /* <- default causes exit */
	        switch (rs) {
	        case SR_MFILE:
                    r = to_mfile(rs) ;
		    break ;
	        case SR_NFILE:
                    r = to_nfile(rs) ;
		    break ;
		case SR_NOMEM:
                    r = to_nomem(rs) ;
		    break ;
	        case SR_NOSPC:
                    r = to_nospc(rs) ;
		    break ;
	        case SR_INTR:
		    r(false) ;
	            break ;
	        } /* end switch */
		rs = r ;
	    } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
}
/* end subroutine (ucopenshm) */


