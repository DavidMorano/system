/* localmisc_mem HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscellaneous pre-processor definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is a collection of miscellaneious pre-processor
	defintions that were eidely used on the various embedded
	code projects and also on many of the support tool projects.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOCALMISCMEM_INCLUDE
#define	LOCALMISCMEM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stddef.h>		/* CSTD |size_t| */
#include	<string.h>		/* CSTD |memset(3c)| */


#ifdef	__cplusplus
#else /* __splusplus */
#ifndef	SUBROUTINE_MEMCLEAR
#define	SUBROUTINE_MEMCLEAR
local inline int memclear(void *objp,int sz) noex {
    	csize	objs = (size_t) sz ;
    	memset(objp,0,objs) ;
	return sz ;
} /* end subroutine */
#endif /* SUBROUTINE_MEMCLEAR */
#endif /* __splusplus */


#endif /* LOCALMISCMEM_INCLUDE */


