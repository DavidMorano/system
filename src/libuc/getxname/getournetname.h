/* getournetname HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get the RPC net-name for a user */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This was written to collect this code into one subroutine.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETOUTNETNAME_INCLUDE
#define	GETOUTNETNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	getournetname(char *,int,cchar *) noex ;

EXTERNC_end


#endif /* GETOUTNETNAME_INCLUDE */


