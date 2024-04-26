/* usem SUPPORT (UNIX® System V IPC - Semaphores) */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_semget
	u_semop
	u_semctl

	Description:
	UNIX® System V Inter-Process-Communication (IPC)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/sem.h>
#include	<unistd.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |intptr_t| */
#include	<cstdarg>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"uipc.h"


/* local defines */


/* imported namespaces */

using namespace	uipc ;			/* namespace */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_semget(key_t key,int nsems,int semflag) noex {
	int		rs ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nospc = utimeout[uto_nospc] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = semget(key,nsems,semflag)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_AGAIN:
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (u_semget) */

int u_semop(int semid,SEMBUF *sops,size_t nsops) noex {
	int		rs = SR_FAULT ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		to_mfile = utimeout[uto_mfile] ;
	bool		f_exit = false ;
	if (sops) {
	    repeat {
	        if ((rs = semop(semid,sops,nsops)) < 0) {
		    rs = (- errno) ;
		}
	        if (rs < 0) {
	            switch (rs) {
	            case SR_NOMEM:
	                if (to_nomem-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_NOSPC:
	                if (to_nospc-- > 0) {
			    msleep(1000) ;
		        } else {
			    f_exit = true ;
		        }
	                break ;
	            case SR_MFILE:
	                if (to_mfile-- > 0) {
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
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (u_semop) */

int u_semctl(int semid,int semnum,int cmd,...) noex {
	va_list		ap ;
	int		rs ;
	int		to_nomem = utimeout[uto_nomem] ;
	bool		f_exit = false ;
	SEMUN		*unp ;
	va_begin(ap,cmd) ;
	unp = (SEMUN *) va_arg(ap,SEMUN *) ;
	repeat {
	    if ((rs = semctl(semid,semnum,cmd,unp)) < 0) {
		rs = (- errno) ;
	    }
	    if (rs < 0) {
	        switch (rs) {
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
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
	va_end(ap) ;
	return rs ;
}
/* end subroutine (u_semctl) */


