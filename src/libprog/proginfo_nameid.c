/* proginfo_nameid SUPPORT */
/* lang=C++20 */

/* utility for KSH built-in commands */
/* version %I% last-modified %G% */

#define	CF_UNAME	0		/* perform UNAME function */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	proginfo_nameid{x}

	Description:
	These subroutines set the UNAME and IDS related fields in
	the PROGINFO object.

	Synopsis:
	int proginfo_nameid(proginfo *pip) noex

	Arguments:
	pip		program information pointer

	Returns:
	>=0	length of PR
	<0	error (system-returns)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<uinfo.h>
#include	<localmisc.h>

#include	"proginfo.hh"


/* local defines */

#ifndef	CF_UNAME
#define	CF_UNAME	0		/* perform UNAME function */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_uname = CF_UNAME ;


/* exported variables */


/* exported subroutines */

int proginfo_nameidbegin(PROGINFO *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    if_constexpr (f_uname) {
	        if ((rs = uname_start(&pip->un)) >= 0) {
	            pip->usysname = pip->un.sysname ;
	            pip->urelease = pip->un.release ;
	            pip->uversion = pip->un.version ;
	            pip->umachine = pip->un.machine ;
	            rs = ids_load(&pip->id)  ;
	            if (rs < 0) {
	                uname_finish(&pip->un) ;
	            }
	        }
	    } else {
	        rs = ids_load(&pip->id)  ;
	    } /* end if_constexpr (f_uname) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proginfo_nameidbegin) */

int proginfo_nameidend(PROGINFO *pip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pip) {
	    rs = SR_OK ;
	    {
	        rs1 = ids_release(&pip->id) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if_constexpr (f_uname) {
	        rs1 = uname_finish(&pip->un) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if_constexpr (f_uname) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (proginfo_nameidend) */


