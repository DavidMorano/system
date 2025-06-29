/* ucunlink SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX� library-3c */
/* POSIX� file-unlink */


/* revision history:

	= 1998-04-13, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/mman.h>		/* |shm_{x}(2)| */
#include	<semaphore.h>		/* |sem_{x}(2)| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<posname.h>		/* create POSIX� entity names */
#include	<localmisc.h>


/* local defines */

#if	defined(SYSHAS_PSHM) && (SYSHAS_PSHM > 0)
#define	F_PSHM		1
#else
#define	F_PSHM		0
#endif

#if	defined(SYSHAS_PSEM) && (SYSHAS_PSEM > 0)
#define	F_PSEM		1
#else
#define	F_PSEM		0
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mkexpandpath(char *,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

const bool		f_pshm = F_PSHM ;
const bool		f_psem = F_PSEM ;


/* exported variables */


/* exported subroutines */

int uc_unlink(cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	if (char *ebuf ; (rs = libmalloc_mp(&ebuf)) >= 0) {
	    if ((rs = mkexpandpath(ebuf,fname,-1)) > 0) {
		rs = u_unlink(ebuf) ;
	    } else if (rs == 0) {
		rs = u_unlink(fname) ;
	    }
	    rs1 = uc_libfree(ebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (uc_unlink) */

int uc_unlinkshm(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if_constexpr (f_pshm) {
		    cchar	*nn ;
		    if (posname pn ; (rs = pn.start(fname,-1,&nn)) >= 0) {
	                repeat {
	                    errno = 0 ;
	                    if ((rs = shm_unlink(nn)) < 0) {
		                rs = (- errno) ;
	                    }
	                } until (rs != SR_INTR) ;
		        rs1 = pn.finish ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (posname) */
		} else {
		    rs = SR_NOSYS ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_unlinkshm) */

int uc_unlinksem(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if_constexpr (f_psem) {
		    cchar	*nn ;
		    if (posname pn ; (rs = pn.start(fname,-1,&nn)) >= 0) {
	                repeat {
	                    errno = 0 ;
	                    if ((rs = sem_unlink(nn)) < 0) {
			        rs = (- errno) ;
		            }
	                } until (rs != SR_INTR) ;
		        rs1 = pn.finish ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (posname) */
		} else {
		    rs = SR_NOSYS ;
		} /* end if_constexpr (f_psem) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_unlinksem) */


