/* pretime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* set or get some program (process) data */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pretime

	Description:
	Manipulated system time management.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/time.h>		/* for |gettimeofday(3c)| */
#include	<sys/timeb.h>		/* for |ftime(3c)| */
#include	<dlfcn.h>
#include	<cerrno>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<tmz.hh>
#include	<localmisc.h>

#include	"pretime.h"


/* local defines */

#define	PRETIME		struct pretime_head

#define	VARBASETIME	"LIBPRETIME_BASETIME"

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

#define	NDF		"libpretime.nd"


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int	(*gettimeofday_t)(TIMEVAL *,void *) noex ;
}


/* external subroutines */

extern int	snwcpy(char *,int,const char *,int) ;
extern int	sncpy1(char *,int,const char *) ;
extern int	sncpy1w(char *,int,const char *,int) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	msleep(int) ;
extern int	isNotPresent(int) ;

extern char	*strwcpy(char *,const char *,int) ;
extern char	*strdcpy1(char *,int,const char *) ;
extern char	*strdcpy1w(char *,int,const char *,int) ;
extern char	*timestr_logz(time_t,char *) ;


/* external variables */


/* local structures */

extern "C" {
    struct pretime_head {
	long		off ;
	time_t		(*func_time)(time_t *) ;
	int		(*func_gettimeofday)(TIMEVAL *,void *) ;
	int		(*func_ftime)(TIMEB *) ;
	volatile int	f_init ;
	volatile int	f_initdone ;
	int		serial ;
    } ; /* end struct (pretime_head) */
}


/* forward references */

static int pretime_loadsyms(PRETIME *) noex ;


/* local variables */

static PRETIME		pretime_data ; /* zero-initialized */

static constexpr cchar	*syms[] = {
	"time",
	"gettimeofday",
	"ftime",
	nullptr
} ;

enum syms {
	sym_time,
	sym_gettimeofday,
	sym_ftime,
	sym_overlast
} ;


/* exported variables */


/* exported subroutines */

int pretime_init() noex {
	PRETIME		*op = &pretime_data ;
	int		rs = SR_OK ;
	int		f = false ;
	if (! op->f_init) {
	    op->f_init = true ;
	    if (op->func_time == nullptr) {
	        if ((rs = pretime_loadsyms(op)) >= 0) {
	            cchar	*cp ;
	            if ((cp = getenv(VARBASETIME)) != nullptr) {
	                TMZ		z ;
	                if ((rs = tmz_xtoucht(&z,cp,-1)) >= 0) {
		            struct tm	ts ;
		            memset(&ts,0,sizeof(struct tm)) ;
		            if ((rs = tmz_gettm(&z,&ts)) >= 0) {
		                time_t	ti_base ;
		                if ((rs = uc_mktime(&ts,&ti_base)) >= 0) {
			            time_t ti_now = (*op->func_time)(nullptr) ;
			            op->off = (ti_base-ti_now) ;
			            f = true ;
		                }
		            }
	                }
	            } /* end if (base-time) */
		    if (rs >= 0) {
	                op->f_initdone = true ;
		    } else {
	                op->f_init = false ;
		    }
	        } /* end if (load-syms) */
	    } /* end if (needed initialization) */
	} else {
	    while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	        rs = msleep(1) ;
	        if (rs == SR_INTR) rs = SR_OK ;
	    }
	    if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pretime_init) */

void pretime_fini() noex {
	PRETIME		*op = &pretime_data ;
	if (op->f_initdone) {
	    op->f_initdone = false ;
	    op->f_init = false ;
	}
}
/* end subroutine (pretime_fini) */

int pretime_serial() noex {
	PRETIME		*op = &pretime_data ;
	int		rs ;
	if ((rs = pretime_init()) >= 0) {
	   rs = op->serial++ ;
	}
	return rs ;
}
/* end subroutine (pretime_serial) */

int pretime_getoff(long *offp) noex {
	PRETIME		*op = &pretime_data ;
	int		rs ;
	if (offp == nullptr) return SR_FAULT ;
	if ((rs = pretime_init()) >= 0) {
	    if (offp != nullptr) *offp = op->off ;
	}
	return rs ;
}
/* end subroutine (pretime_getoff) */

int pretime_modtime(time_t *timep) noex {
	PRETIME		*op = &pretime_data ;
	int		rs ;
	if (timep == nullptr) return SR_FAULT ;
	if ((rs = pretime_init()) >= 0) {
	    time_t	t = (*op->func_time)(nullptr) ;
	    t += op->off ;
	    *timep = t ;
	} /* end if (init) */
	return rs ;
}
/* end subroutine (pretime_modtime) */

int pretime_modtv(TIMEVAL *tvp,void *dummy) noex {
	PRETIME		*op = &pretime_data ;
	int		rs ;
	if (tvp == nullptr) return SR_FAULT ;
	tvp->tv_sec = 0 ;
	tvp->tv_usec = 0 ;
	if ((rs = pretime_init()) >= 0) {
	    if ((*op->func_gettimeofday)(tvp,dummy) >= 0) {
	        tvp->tv_sec += op->off ;
	    } else {
		rs = (-errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (pretime_modtv) */

int pretime_modts(TIMESPEC *tsp) noex {
	PRETIME		*op = &pretime_data ;
	int		rs ;
	if (tsp == nullptr) return SR_FAULT ;
	if ((rs = pretime_init()) >= 0) {
	    TIMEVAL	tv ;
	    if ((*op->func_gettimeofday)(&tv,nullptr) >= 0) {
		tsp->tv_sec = tv.tv_sec ;
		tsp->tv_nsec = (tv.tv_usec * 1000) ;
	        tsp->tv_sec += op->off ;
	    } else {
		rs = (-errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (pretime_modts) */

int pretime_modtimeb(TIMEB *tbp) noex {
	PRETIME		*op = &pretime_data ;
	int		rs ;
	if (tbp == nullptr) return SR_FAULT ;
	memset(tbp,0,sizeof(TIMEB)) ;
	if ((rs = pretime_init()) >= 0) {
	    if ((*op->func_ftime)(tbp) >= 0) {
	        tbp->time += op->off ;
	    } else {
		rs = (-errno) ;
	    }
	}
	return rs ;
}
/* end subroutine (pretime_modtimeb) */


/* private subroutines */

static int pretime_loadsyms(PRETIME *op) noex {
	int		rs = SR_OK ;
	void		*sp ;
	for (int i = 0 ; (rs >= 0) && (syms[i] != nullptr) ; i += 1) {
	    if ((sp = dlsym(RTLD_NEXT,syms[i])) != nullptr) {
	        switch (i) {
	        case sym_time:
	            op->func_time = (time_t (*)(time_t *)) sp ;
		    break ;
	        case sym_gettimeofday:
	            op->func_gettimeofday = (gettimeofday_t) sp ;
		    break ;
	        case sym_ftime:
	            op->func_ftime = (int (*)(TIMEB *)) sp ;
		    break ;
	        } /* end switch */
	    } else {
		rs = SR_LIBACC ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (pretime_loadsyms) */


