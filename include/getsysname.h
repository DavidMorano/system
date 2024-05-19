/* getsysname HEADER */
/* lang=C20 */

/* get the best approximation of the username of the current user */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	GETSYSNAME_INCLUDE
#define	GETSYSNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>


EXTERNC_begin

extern int	getsysname(char *,int) noex ;

EXTERNC_end


#endif /* GETSYSNAME_INCLUDE */


