/* pinghost SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* a little object to hold ping-host names */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 2001-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pinghost

	Description:
	This object serves as an entry (for a collection) of a
	'pinghost'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pinghost.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PH	pinghost

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int pinghost_start(PH *op,cchar *hp,int hl,int mval,int to) noex {
	int		rs = SR_FAULT ;
	DEBUGPRINTF("ent h=%s\n",hp) ;
	if (op && hp) {
	    memclear(op) ;
	    op->intminping = mval ;
	    op->to = to ;
	    if (cchar *cp ; (rs = mem.strw(hp,hl,&cp)) >= 0) {	
	        op->name = cp ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pinghost_start) */

int pinghost_finish(PH *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	DEBUGPRINTF("ent h=%s\n",op->name) ;
	if (op) {
	    rs = SR_OK ;
	    if (op->name) {
	        voidp vp = voidp(op->name) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->name = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pinghost_finish) */


