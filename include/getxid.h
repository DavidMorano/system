/* getxid HEADER */
/* lang=C20 */

/* get the certain system IDs based on a specified name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This was written to collect this code into one subroutine.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETXID_INCLUDE
#define	GETXID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |gid_t| */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	getuid_name(cchar *,int) noex ;
extern int	getgid_name(cchar *,int) noex ;
extern int	getgid_group(cchar *,int) noex ;
extern int	getgid_def(cchar *,gid_t) noex ;
extern int	getpjid_name(cchar *,int) noex ;

EXTERNC_end


#endif /* GETXID_INCLUDE */


