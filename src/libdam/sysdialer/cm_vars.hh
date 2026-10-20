/* cm_vars HEADER (Communications-Connection Manager) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Connection Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cm_opener

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#ifndef	CMVARS_INCLUDE
#define	CMVARS_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<fieldterms.h>		/* LIBUC */

#include	"cm_defs.h"


namespace cm_ns {
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	int		usernamelen ;
	int		nodenamelen ;
	int		hostnamelen ;
	int		maxarg ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace (cm_ns) */

namespace cm_ns {
    extern char		terms[fieldterms_termsize] ;
    extern vars		var ;
} /* end namespace (cm_ns) */


#endif /* __cplusplus */
#endif /* CMVARS_INCLUDE */


