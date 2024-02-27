/* uc_getdefaultproj SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* Solaris PROJECT DB access */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

	= 2018-10-03, David A.D. Morano
	Several typos in the introductory comments below have been
	fixed. Of course, it should not have taken twenty years to
	have seen (found) and have fixed these!

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Now for some fun facts on the (Sun) Solaris USER_ATTR and
	the PROJECT database access software (represented in part
	by this subroutine here).  If the databases have any sort
	of corruption, there is a danger that the Solaris access
	software will memory-access-fault and cause a core dump --
	as observed many times now. Care needs to taken that no
	entries in either the USER_ATTR or the PROJECT databases
	are corrupted or malformed in any way. Until the crappy
	access software is fixed, core dumps are possible if not
	guaranteed.

	Final note (if I may):

	I hate to say it, but debugging operating-system software
	without full ability to build or rebuild the OS itself is
	often quite difficult. And we can seldom actually patch up
	a binary OS even when we have and can correct the buggy
	source code involved. [update 2018-10-03] Linux now from
	Oracle (and maybe others) can live patch a completely running
	kernel on the fly along w/ its various system (shared-object)
	libraries.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |pid_t| */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<cstdlib>
#include	<cstring>
#include	<project.h>
#include	<usystem.h>
#include	<usupport.h>		/* |msleep(3u)| */
#include	<localmisc.h>


/* local defines */


/* local namespaces */


/* local typedefs */

#ifndef	TASKID_TYPEDEF
#define	TASKID_TYPEDEF	1
typedef pid_t		taskid_t ;
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#if	defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0)

int uc_getdefaultproj(PROJECT *pjp,char *rbuf,int rlen,cchar *name) noex {
	int		rs SR_FAULT ;
	int		to_again = utimeout[uto_again] ;
	bool		f_exit = false ;
	if (pjp && rbuf && name) {
	    rs = SR_INVALID ;
	    if ((rlen > 0) && name[0]) {
		PROJECT		*rp = nullptr ;
	        repeat {
	            rs = SR_OK ;
	            errno = 0 ;
	            rp = getdefaultproj(name,pjp,rbuf,(size_t) rlen) ;
	            if (rp == nullptr) {
		        rs = ((errno == 0) ? SR_NOENT : (- errno)) ;
	            }
	            if (rs < 0) {
	                switch (rs) {
	                case SR_AGAIN:
	                    if (to_again-- > 0) {
	                        msleep(1000) ;
		            } else {
			        f_exit = true ;
	                    }
		            break ;
	                case SR_INTR:
		            break ;
		        default:
		            f_exit = true ;
		            break ;
	                } /* end switch */
	            } /* end if (error) */
	        } until ((rs >= 0) || f_exit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getdefaultproj) */


#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */

int uc_getdefaultproj(PROJECT *pjp,char *rbuf,int rlen,cchar *name) noex {
	int		rs = SR_FAULT ;
	if (pjp && rbuf && name) {
	    rs = SR_INVALID ;
	    if ((rlen > 0) && name[0]) {
		rs = SR_NOSYS ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_getdefaultproj) */

#if	(! defined(SUBROUTINE_GETPROJID)) || (SUBROUTINE_GETPROJID == 0)
#define	SUBROUTINE_GETPROJID	1
projid_t getprojid(void) noex { 
	return 0 ; 
}
#endif /* (! defined(SUBROUTINE_GETPROJID)) || (SUBROUTINE_GETPROJID == 0) */

#if	(! defined(SUBROUTINE_GETTASKID)) || (SUBROUTINE_GETTASKID == 0)
#define	SUBROUTINE_GETTASKID	1
taskid_t gettaskid(void) noex { 
	return 0 ; 
}
taskid_t settaskid(projid_t,uint32_t) noex { 
	return 0 ; 
}
#endif /* (! defined(SUBROUTINE_GETTASKID)) || (SUBROUTINE_GETTASKID == 0) */


#endif /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */


