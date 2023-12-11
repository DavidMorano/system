/* ucsysmisc */
/* lang=C++20 */

/* interface component for UNIX® library-3c */


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
#include	<climits>
#include	<usystem.h>


/* exported subroutines */

int uc_nprocessors(int w) noex {
	int		rs = SR_NOTSUP ;
	int		cmd = -1 ;
	int		n = 0 ;
	switch (w) {
	case 0:
#ifdef	_SC_NPROCESSORS_ONLN
	    rs = SR_OK ;
	    cmd = _SC_NPROCESSORS_ONLN ;
#endif /* _SC_NPROCESSORS_ONLN */
	    break ;
	case 1:
#ifdef	_SC_NPROCESSORS_CONF
	    rs = SR_OK ;
	    cmd = _SC_NPROCESSORS_CONF ;
#endif /* _SC_NPROCESSORS_CONF */
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
        } /* end switch */
	if (rs >= 0) {
	    long	rn ;
	    rs = uc_confsys(cmd,&rn) ;
	    n = (rn & INT_MAX) ;
	    if (rs == SR_INVALID) rs = SR_NOTSUP ;
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (uc_nprocessors) */

int uc_gethz(int w) noex {
	long	hz = -1 ;
	int	rs = SR_NOSYS ;
#if	defined(HZ)
	if ((hz < 0) && ((w == 0) || (w == 1))) {
	    rs = SR_OK ;
	    hz = HZ ;
	}
#endif
#if	defined(CLK_TCK) 
	if ((hz < 0) && ((w == 0) || (w == 2))) {
	    rs = SR_OK ;
	    hz = CLK_TCK ;
	}
#endif
#if	defined(_SC_CLK_TCK) 
	if ((hz < 0) && ((w == 0) || (w == 3))) {
	    rs = uc_confsys(_SC_CLK_TCK,&hz) ;
	}
#endif
	if (rs >= 0) rs = (hz & INT_MAX) ;
	return rs ;
}
/* end subroutine (uc_gethz) */

int uc_syspages(int w) noex {
	long		rn = 0 ;
	int		rs = SR_NOTSUP ;
	int		cmd = -1 ;
	int		n = 0 ;
	switch (w) {
	case 0:
#ifdef	_SC_AVPHYS_PAGES
	    rs = SR_OK ;
	    cmd = _SC_AVPHYS_PAGES ;
#endif /* _SC_AVPHYS_PAGES */
	    break ;
	case 1:
#ifdef	_SC_PHYS_PAGES
	    rs = SR_OK ;
	    cmd = _SC_PHYS_PAGES ;
#endif /* _SC_PHYS_PAGES */
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
        } /* end switch */
	if (rs >= 0) {
	    rs = uc_confsys(cmd,&rn) ;
	    n = (rn & INT_MAX) ;
	    if (rs == SR_INVALID) rs = SR_NOTSUP ;
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (uc_syspages) */


