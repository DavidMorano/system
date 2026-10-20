/* cm_caller SUPPORT (Communications-Connection Manager) */
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
	cm

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sysdialer.h>		/* LIBUC */

#include	"cm_defs.h"


namespace cm_ns {
extern "C" {
    typedef int (*open_f)	(void *,SD_ARGS *,ccp,ccp,mainv) noex ;
    typedef int (*close_f)	(void *) noex ;
    typedef int (*shutdown_f)	(void *,int) noex ;
    typedef int (*reade_f)	(void *,void *,int,int,int) noex ;
    typedef int (*recve_f)	(void *,void *,int,int,int,int) noex ;
    typedef int (*write_f)	(void *,cvoid *,int) noex ;
    } /* end extern (C) */
} /* end namespace (cm_ns) */

namespace cm_ns {
    struct caller {
    	open_f		open ;
    	close_f		close ;
    	shutdown_f	shutdown ;
    	reade_f		reade ;
    	recve_f		recve ;
    	write_f		write ;
    } ; /* end struct (caller) */
} /* end namespace (cm_ns) */


