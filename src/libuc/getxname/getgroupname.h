/* getgroupname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a group name from the system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETGROUPNAME_INCLUDE
#define	GETGROUPNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |gid_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	getgroupname(char *,int,gid_t) noex ;

EXTERNC_end


#endif /* GETGROUPNAME_INCLUDE */


