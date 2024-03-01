/* getgroupname HEADER */
/* lang=C20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETGROUPNAME_INCLUDE
#define	GETGROUPNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |gid_t| */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	getgroupname(char *,int,gid_t) noex ;

EXTERNC_end


#endif /* GETGROUPNAME_INCLUDE */


