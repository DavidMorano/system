/* ucsysmisc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* miscellaneous system information retrieval functions */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_nprocessors

	Description:
	This subroutine returns the number of CPUs:
	0. online, or
	1. configured, 
	on the current system.

	Synopsis:
	int uc_nprocessors(int w) noex

	Arguments:
	w		which number of CPUs to return: 
				0=online
				1=configured

	Returns:
	<0		error (generally not-supported)
	>=0		the number of CPUs requested


	Name:
	uc_gethz

	Description:
	Get the system statistics-clock frequency ("HZ").

	Synopsis:
	int uc_gethz(int w) noex

	Arguments:
	w		specifies which source:
				0 -> all
				1 -> 'HZ' define only
				2 -> 'CLK_TCK' define only
				3 -> |sysconf(3c)| 'CLK_TCK' only

	Returns:
	<0		error (generally not-supported)
	>=0		value of system HZ

	Notes:
	1. In the old days, people actually cared about what this
	value was. Now-a-days, the omly (pretty much) real use for
	this value is in evaluating the results of the |times(2)|
	system call. But that call has been depracated.


	Name:
	uc_syspages

	Description:
	This subroutine returns information about what the system
	virtual-memory (VM) facility is doing about both total and
	available memory pages.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<climits>
#include	<usystem.h>

#include	"ucsysmisc.h"


/* local defines */

#ifdef	_SC_NPROCESSORS_ONLN
#define	F_PROCAVAIL	1
#else
#define	F_PROCAVAIL	0
#define	_SC_NPROCESSORS_ONLN	-1
#endif

#ifdef	_SC_NPROCESSORS_CONF
#define	F_PROCALL	1
#else
#define	F_PROCALL	0
#define	_SC_NPROCESSORS_CONF	-1
#endif

#ifdef	HZ
#define	F_HZ		1
#else
#define	F_HZ		0
#define	HZ		0
#endif

#ifdef	CLK_TCK 
#define	F_CLK		1
#else
#define	F_CLK		0
#define	CLK_TCK		-1
#endif

#ifdef	_SC_CLK_TCK 
#define	F_SCTCK		1
#else
#define	F_SCTCK		0
#define	_SC_CLK_TCK 	-1
#endif

#ifdef	_SC_AVPHYS_PAGES
#define	F_PAGESAVAIL	1
#else
#define	F_PAGESAVAIL	0
#define	_SC_AVPHYS_PAGES	-1
#endif

#ifdef	_SC_PHYS_PAGES
#define	F_PAGESALL	1
#else
#define	F_PAGESALL	0
#define	_SC_PHYS_PAGES	-1
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_procavail 	= F_PROCAVAIL ;
constexpr bool		f_procall 	= F_PROCALL ;
constexpr bool		f_hz		= F_HZ ;
constexpr bool		f_clk		= F_CLK ;
constexpr bool		f_sctck		= F_SCTCK ;
constexpr bool		f_pagesavail	= F_PAGESAVAIL ;
constexpr bool		f_pagesall	= F_PAGESALL ;


/* exported variables */

libuc::ucpagesizer	ucpagesize ;


/* exported subroutines */

int uc_nprocessors(int w) noex {
	int		rs = SR_NOTSUP ;
	int		cmd = -1 ;
	int		n = 0 ;
	switch (w) {
	case 0:
	    if_constexpr (f_procavail) {
	        rs = SR_OK ;
	        cmd = _SC_NPROCESSORS_ONLN ;
	    } /* end if_constexpr (f_procavail) */
	    break ;
	case 1:
	    if_constexpr (f_procall) {
	        rs = SR_OK ;
	        cmd = _SC_NPROCESSORS_CONF ;
	    } /* end if_constexpr (f_procaall) */
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
        } /* end switch */
	if ((rs >= 0) && (cmd >= 0)) {
	    if (long rn{} ; (rs = uc_sysconfval(cmd,&rn)) >= 0) {
	        n = intsat(rn) ;
	    } else if (rs == SR_INVALID) {
	        rs = SR_NOTSUP ;
	    }
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (uc_nprocessors) */

int uc_gethz(int w) noex {
	int		hz = -1 ;
	int		rs = SR_NOSYS ;
	if_constexpr (f_hz) {
	    if ((hz < 0) && ((w == 0) || (w == 1))) {
	        rs = SR_OK ;
	        hz = HZ ;
	    }
	} /* end if_constexpr (f_hz) */
	if_constexpr (f_clk) {
	    if ((hz < 0) && ((w == 0) || (w == 2))) {
	        rs = SR_OK ;
	        hz = CLK_TCK ;
	    }
	} /* end if_constexpr (f_clk) */
	if_constexpr (f_sctck) {
	    if ((hz < 0) && ((w == 0) || (w == 3))) {
	        cint	cmd = _SC_CLK_TCK ;
	        rs = uc_sysconfval(cmd,nullptr) ;
		hz = rs ;
	    }
	} /* end if_constexpr (f_sctck) */
	return (rs >= 0) ? hz : rs ;
}
/* end subroutine (uc_gethz) */

int uc_syspages(int w) noex {
	int		rs = SR_NOTSUP ;
	int		cmd = -1 ;
	int		n = 0 ;
	switch (w) {
	case 0:
	    if_constexpr (f_pagesavail) {
	        rs = SR_OK ;
	        cmd = _SC_AVPHYS_PAGES ;
	    } /* end if_constexpr (f_pagesavail) */
	    break ;
	case 1:
	    if_constexpr (f_pagesall) {
	        rs = SR_OK ;
	        cmd = _SC_PHYS_PAGES ;
	    } /* end if_constexpr (f_pagesall) */
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
        } /* end switch */
	if ((rs >= 0) && (cmd >= 0)) {
	    if (long rn{} ; (rs = uc_sysconfval(cmd,&rn)) >= 0) {
	        n = intsat(rn) ;
	    } else if (rs == SR_INVALID) {
		rs = SR_NOTSUP ;
	    }
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (uc_syspages) */

int uc_pagesize() noex {
	cint		cmd = _SC_PAGESIZE ;
	return uc_sysconfval(cmd,nullptr) ;
}
/* end subroutine (uc_pagesize) */


/* local subrouties */

namespace libuc {
    ucpagesizer::operator int () noex {
    	int		rs ;
	if ((rs = pagesize) == 0) {
	    cint	cmd = _SC_PAGESIZE ;
	    if ((rs = uc_sysconfval(cmd,nullptr)) >= 0) {
		pagesize = rs ;
	    }
	}
    	return rs ;
    } /* end method (ucpagesizer::operator) */
    ucnprocesser::operator int () noex {
	return uc_nprocessors(0) ;
    } /* end method (ucprocesser::operator) */
    int ucnprocesser::operator () (int w) noex {
	return uc_nprocessors(w) ;
    } /* end method (ucprocesser::operator) */
} /* end namespace (libuc) */


