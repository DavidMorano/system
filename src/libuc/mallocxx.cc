/* mallocxx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mallocxx

	Description:
	This subroutine is similar to |uc_malloc(3uc)| except that
	it takes a string argument and copies it into the newly
	allocated space.

	Synopsis:
	int mallocxx(int w,char **rpp) noex

	Arguments:
	w		which type of system bffer
	rpp		resulting pointer

	Returns:
	>=0		nominally -> (strlen(string) + 1)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sysval.hh>
#include	<mailvalues.hh>
#include	<localmisc.h>

#include	"mallocxx.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	uc_mallocsys(int,char **) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static sysval	pagesize(sysval_ps) ;


/* local subroutines */


/* exported variables */


/* exported subroutines */

int malloc_ma(char **rpp) noex {
	cint	w = getbufsize_ma ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_mn(char **rpp) noex {
	cint	w = getbufsize_mn ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_mp(char **rpp) noex {
	cint	w = getbufsize_mp ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_ml(char **rpp) noex {
	cint	w = getbufsize_ml ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_nn(char **rpp) noex {
	cint	w = getbufsize_nn ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_hn(char **rpp) noex {
	cint	w = getbufsize_hn ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_un(char **rpp) noex {
	cint	w = getbufsize_un ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_gn(char **rpp) noex {
	cint	w = getbufsize_gn ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_pn(char **rpp) noex {
	cint	w = getbufsize_pn ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_pw(char **rpp) noex {
	cint	w = getbufsize_pw ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_sp(char **rpp) noex {
	cint	w = getbufsize_sp ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_ua(char **rpp) noex {
	cint	w = getbufsize_ua ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_gr(char **rpp) noex {
	cint	w = getbufsize_gr ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_pj(char **rpp) noex {
	cint	w = getbufsize_pj ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_pr(char **rpp) noex {
	cint	w = getbufsize_pr ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_nw(char **rpp) noex {
	cint	w = getbufsize_nw ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_ho(char **rpp) noex {
	cint	w = getbufsize_ho ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_sv(char **rpp) noex {
	cint	w = getbufsize_sv ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_fs(char **rpp) noex {
	cint	w = getbufsize_fs ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_sn(char **rpp) noex {
	cint	w = getbufsize_sn ;
	return uc_mallocsys(w,rpp) ;
}
int malloc_zn(char **rpp) noex {
	cint	w = getbufsize_zn ;
	return uc_mallocsys(w,rpp) ;
}

int malloc_zi(char **rpp) noex {
	cint	w = getbufsize_zi ;
	return uc_mallocsys(w,rpp) ;
}

int malloc_ps(char **rpp) noex {
	int		rs = SR_FAULT ;
	int		sz = 0 ;
	if (rpp) ylikely {
	    if ((rs = pagesize) > 0) ylikely {
	        rs = uc_valloc((rs + 1),rpp) ;
	    } else if (rs <= 0) {
		*rpp = nullptr ;
		if (rs >= 0) rs = SR_NOSYS ;
	    } /* end if (pagesize) */
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
} /* end subroutine (malloc_ps) */

int malloc_mailaddr(char **rpp) noex {
    	cint		hmult = mailvalue.hostnamemult ;
    	cint		nmult = mailvalue.nodenamemult ;
	int		rs = SR_FAULT ;
	int		mal = 0 ;		/* mail-address length */
	if (rpp) ylikely {
	    int		w = getbufsize_hn ;
	    if ((rs = getbufsize(w)) >= 0) ylikely {
		cint	hnl = rs ;
		w = getbufsize_nn ;
		if ((rs = getbufsize(w)) >= 0) ylikely {
		    cint	nnl = rs ;
		    mal = ((hmult * hnl) + (nmult * nnl)) ;
		    rs = uc_malloc((mal+1),rpp) ;
		} /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? mal : rs ;
} /* end subroutine (mailoc_mailaddr) */


