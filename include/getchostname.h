/* getchostname HEADER */
/* lang=C20 */

/* get a cannical host-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETCHOSTNAME_INCLUDE
#define	GETCHOSTNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int getchostname(cchar *,char *) noex ;
extern int getcname(cchar *,char *) noex ;

EXTERNC_end


#endif /* GETCHOSTNAME_INCLUDE */


