/* ucsigx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-28, David A­D­ Morano
	How did we get along without this for over 10 years?

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	ucsigx

	Description:
	These are various signal-handling (C-Library) subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<usysflag.h>
#include	<timespec.h>
#include	<localmisc.h>

#include	"ucsigx.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int uc_sigdefault(int sn) noex {
	int		rs = SR_OK ;
	if (sig_t ret ; (ret = signal(sn,SIG_DFL)) == SIG_ERR) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_sigdefault) */

int uc_sigignore(int sn) noex {
	int		rs = SR_OK ;
	if (sig_t ret ; (ret = signal(sn,SIG_IGN)) == SIG_ERR) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_sigignore) */

int uc_sighold(int sn) noex {
	int		rs ;
	if_constexpr (f_darwin) {
	    rs = SR_NOSYS ;
	} else {
	    if ((rs = sighold(sn)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if_constexpr (f_darwin) */
	return rs ;
}
/* end subroutine (uc_sighold) */

int uc_sigrelease(int sn) noex {
	int		rs ;
	if_constexpr (f_darwin) {
	    rs = SR_NOSYS ;
	} else {
	    if ((rs = sigrelse(sn)) < 0) {
	        rs = (- errno) ;
	    }
	} /* end if_constexpr (f_darwin) */
	return rs ;
}
/* end subroutine (uc_sigrelease) */

int uc_sigpause(int sn) noex {
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = sigpause(sn)) < 0) {
		rs = (- errno) ;
	        switch (rs) {
	        case SR_INTR:
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (uc_sigpause) */

int uc_sigqueue(pid_t pid,int sn,CSIGVAL val) noex {
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = sigqueue(pid,sn,val)) < 0) {
		rs = (- errno) ;
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
	return rs ;
}
/* end subroutine (uc_sigqueue) */

int uc_sigwaitinfo(const sigset_t *ssp,siginfo_t *sip) noex {
	int		rs = SR_FAULT ;
	if (ssp && sip) {
	    bool	f_exit = false ;
	    repeat {
	        if ((rs = sigwaitinfo(ssp,sip)) < 0) {
		    rs = (- errno) ;
	            switch (rs) {
	            case SR_INTR:
		        break ;
	            case SR_AGAIN:
		    default:
		        f_exit = true ;
		        break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	    if (rs >= 0) {
	        rs = sip->si_signo ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_sigwaitinfo) */

int uc_sigtimedwait(const sigset_t *ssp,siginfo_t *sip,CTIMESPEC *tsp) noex {
	int		rs = SR_FAULT ;
	if (ssp && sip) {
	    TIMESPEC	ts ;
	    bool	f_exit = false ;
	    if (tsp == nullptr) {
	        tsp = &ts ;
	        timespec_load(&ts,0,0) ;
	    }
	    repeat {
	        if ((rs = sigtimedwait(ssp,sip,tsp)) < 0) {
		    rs = (- errno) ;
	            switch (rs) {
	            case SR_INTR:
		        break ;
	            case SR_AGAIN:
		    default:
		        f_exit = true ;
		        break ;
	            } /* end switch */
	        } /* end if (error) */
	    } until ((rs >= 0) || f_exit) ;
	    if (rs >= 0) {
	        rs = sip->si_signo ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_sigtimedwait) */

int uc_sigwaitinfoto(const sigset_t *ssp,siginfo_t *sip,CTIMESPEC *tsp) noex {
	int		rs = SR_FAULT ;
	if (ssp && sip) {
	    if (tsp) {
	         rs = uc_sigtimedwait(ssp,sip,tsp) ;
	    } else {
	         rs = uc_sigwaitinfo(ssp,sip) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_sigwaitinfoto) */

int uc_raise(int sig) noex {
	int		rs ;
	if ((rs = raise(sig)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_raise) */

int uc_pause() noex {
	int		rs ;
	if ((rs = pause()) < 0) {
	    rs = (- errno) ;
	    switch (rs) {
	    case SR_INTR:
		rs = SR_OK ;
		break ;
	    } /* end switch */
	} /* end if */
	return rs ;
}
/* end subroutine (uc_pause) */


