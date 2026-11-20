/* proginfo_base6 MODULE (Module-Implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program information */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */

/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

	= 2026-10-20, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	proginfo_base

	Description:
	This module provides a very basic set of simple subroutines
	for basic program management.

	Notes:
	1. The length (in bytes) of the program-name when printing
	error messages is limited to a maximum of |LOGIDLEN| bytes.
	Someone thought that this was a good idea.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<userinfo.h>		/* LIBDAM */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

module proginfo.base ;

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |sysword(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

statcexpr bool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

int proginfo::puinfo() noex {
    	int		rs = SR_OK ;
	DEBUGPRINTF("ent\n") ;
	if (! pf.uinfo) {
	    rs = puibeg() ;
	} /* end if (needed) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (proginfo::puinfo) */

int proginfo::puibeg() noex {
	cnothrow	nt{} ;
    	int		rs = SR_OK ;
	int		rv = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (! pf.uinfo) {
	    DEBUGPRINTF("-> userinfo::start\n") ;
	    if ((rs = userinfo::start) >= 0) {
	        DEBUGPRINTF("userinfo::start() rs=%d\n",rs) ;
		rv = rs ;
		if ((rs = puiloadbeg()) >= 0) {
		    pf.uinfo = true ;
		} /* end if (puiloadbeg) */
		if (rs < 0) {
		    userinfo::finish() ;
		} /* end if (error) */
	    } /* end if (userinfo_start) */
	    DEBUGPRINTF("userinfo::start-out rs=%d\n",rs) ;
	} /* end if (needed) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? rv : rs ;
} /* end method (proginfo::puibeg) */

int proginfo::puiend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	DEBUGPRINTF("ent\n") ;
	if (pf.uinfo) {
	    {
	        rs1 = userinfo::finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end */
	    {
		rs1 = puiloadend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end */
	    pf.uinfo = false ;
	} /* end if (present) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (proginfo::puiend) */

int proginfo::puiloadbeg() noex {
    	int rs = SR_OK ;
	if (pf.uinfo) {
	    rs = SR_OK ;
	} /* end if (non-null) */
    	return rs ;
} /* end method (proginfo::puiloadbeg) */

int proginfo::puiloadend() noex {
    	int rs = SR_OK ;
	if (pf.uinfo) {
	    rs = SR_OK ;
	} /* end if (non-null) */
    	return rs ;
} /* end method (proginfo::puiloadend) */


