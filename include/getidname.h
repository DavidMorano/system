/* getidname HEADER */
/* lang=C20 */

/* get the certain system IDs based on a specified name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This was written to collect this code into one subroutine.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETIDNAME_INCLUDE
#define	GETIDNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<getuid.h>
#include	<getgid.h>


EXTERNC_begin

extern int	getpjid_name(cchar *,int) noex ;

EXTERNC_end


#endif /* GETIDNAME_INCLUDE */


