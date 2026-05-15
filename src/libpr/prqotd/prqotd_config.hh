/* prqotd_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	prqotd::config

	Description:
	This subroutine sort of forms the back-end maintenance end
	of the QOTD mechansim.

	Synopsis:
	int prqotd::config_start(cchar *pr,int mjd,int of,int to) noex

	Arguments:
	pr		program-root
	mjd		modified-julian-day
	of		open-flags
	to		time-out

	Returns:
	>=0		FD of QOTD
	<0		error (system-return)

*******************************************************************************/

#ifndef	PRQOTDCONFIG_INCLUDE
#define	PRQOTDCONFIG_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"prqotd.h"
#include	"prqotd_subinfo.hh"


namespace prqotd {
    const int		config_magicval = 0x8932170 ;
} /* end namespace (prqotd) */

namespace prqotd {
    struct config {
	subinfo		*sip ;		/* supplied argument */
	paramfile	*pfp ;		/* allocated */
	expcook		*ecp ;		/* allocated */
	uint		magval ;
	uint		f_p:1 ;
	uint		f_cooks:1 ;
    } ; /* end struct */
} /* end namespace (prqotd) */

namespace prqotd {
    extern int	config_start		(config *,subinfo *,cchar *) noex ;
    extern int	config_finish		(config *) noex ;
    extern int	config_read		(config *) noex ;
    extern int	config_check		(config *) noex ;
} /* end namespace (prqotd) */


#endif /* __cplusplus */
#endif /* PRQOTDCONFIG_INCLUDE */


