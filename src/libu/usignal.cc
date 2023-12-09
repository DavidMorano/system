/* usignal */
/* lang=C++20 */

/* UNIX signal handling */
/* translation layer interface for UNIX® equivalents */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Notes:
	|u_sigprocmask(3u)|
	Set the process signal mask (to something).  When there are more than
	one thread in the proces, this only sets the signal mask of the thread
	it is called from.
        A cousin of this system call is the call |pt_sigmask(3u)|, which is
	supposed to be for thread specific use.


*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<signal.h>
#include	<unistd.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"usupport.h"


/* exported subroutines */

int u_kill(pid_t pid,int sig) noex {
	int		rs ;
	if ((rs = kill(pid,sig)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_kill) */

int u_sigaction(int sn,SIGACTION *nsp,SIGACTION *osp) noex {
	int		rs ;
	if ((rs = sigaction(sn,nsp,osp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_sigaction) */

int u_sigaltstack(const stack_t *ssp,stack_t *ossp) noex {
	int		rs ;
	if ((rs = sigaltstack(ssp,ossp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_sigaltstack) */

int u_sigpending(sigset_t *ssp) noex {
	int		rs ;
	if ((rs = sigpending(ssp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_sigpending) */

int u_sigprocmask(int how,sigset_t *setp,sigset_t *osetp) noex {
	int		rs ;
	if ((rs = sigprocmask(how,setp,osetp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_sigprocmask) */

int u_sigsuspend(const sigset_t *ssp) noex {
	int		rs ;
	int		to_intr = utimeout[uto_intr] ;
	int		f_exit = FALSE ;
	repeat {
	    rs = SR_OK ;
	    if (sigsuspend(ssp) == -1) rs = (- errno) ;
	    if (rs < 0) {
		switch (rs) {
	        case SR_INTR:
	            if (to_intr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = TRUE ;
		    }
	            break ;
		default:
		    f_exit = TRUE ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (u_sigsuspend) */

int u_pause() noex {
	int		rs = SR_OK ;
	if (pause() == -1) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_pause) */

int u_alarm(const uint secs) noex {
	uint		rem ;
	int		rs ;
	int		sec = (secs & INT_MAX) ;
	if ((rem = alarm(sec)) < 0) {
	    rs = (- errno) ;
	}
	if (rs >= 0) rs = (rem & INT_MAX) ;
	return rs ;
}
/* end subroutine (u_alarm) */

/* these below are NOT on all systems (like MacOS Darwin!) */

int u_sigsend(idtype_t idtype,id_t id,int sig) noex {
	int		rs ;
	if ((rs = sigsend(idtype,id,sig)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_sigsend) */

int u_sigsendset(procset_t *psp,int sig) noex {
	int		rs ;
	if ((rs = sigsendset(psp,sig)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (u_sigsendset) */

int u_sigwait(const sigset_t *ssp,int *rp) noex {
	int		rs ;
	int		sig ;
	if ((rs = sigwait(ssp,&sig)) < 0) {
	    rs = (- errno) ;
	}
	if (rp != NULL) *rp = sig ;
	sig &= INT_MAX ;
	return (rs >= 0) ? sig : rs ;
}
/* end subroutine (u_sigwait) */


