/* ushm SUPPORT (UNIX® System V IPC - Shared Memory) */
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
	u_shmget
	u_shmat
	u_shmctl
	u_shmdt

	Description:
	UNIX® System V Inter-Process-Communication (IPC)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/ipc.h>
#include	<sys/shm.h>
#include	<unistd.h>
#include	<climits>		/* |INT_MAX| */
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdint>		/* |intptr_t| */
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

int u_shmget(key_t key,size_t sz,int msgflag) noex {
	int		rs ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nospc = utimeout[uto_nospc] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = shmget(key,sz,msgflag)) < 0) rs = (- errno) ;
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
/* end subroutine (u_shmget) */

int u_shmat(int shmid,void *shmaddr,int flags,void **app) noex {
	int		rs = SR_FAULT ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		to_mfile = utimeout[uto_mfile] ;
	bool		f_exit = false ;
	if (shmaddr && app) {
	    repeat {
	        *app = shmat(shmid,shmaddr,flags) ;
		if (intptr_t ui ; (ui = reinterpret_cast<long>(*app)) == -1) {
	            rs = (- errno) ;
		} else if (ui < 0) {
		    rs = SR_NOANODE ;
		} else {
		    rs = int(ui & INT_MAX) ;
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
/* end subroutine (u_shmat) */

int u_shmctl(int shmid,int cmd,SHMIDDS *buf) noex {
	int		rs ;
	int		to_nomem = utimeout[uto_nomem] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = shmctl(shmid,cmd,buf)) < 0) rs = (- errno) ;
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
	return rs ;
}
/* end subroutine (u_shmctl) */

int u_shmdt(void *shmaddr) noex {
	int		rs ;
	repeat {
	    if ((rs = shmdt(shmaddr)) < 0) rs = (- errno) ;
	} until (rs != SR_INTR) ;
	return rs ;
}
/* end subroutine (u_shmdt) */


