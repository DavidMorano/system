/* strtoxlonglong HEADER */
/* lang=C20 */

/* numeric-character conversion */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTOXLONGLONG_INCLUDE
#define	STRTOXLONGLONG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>


EXTERNC_begin

extern longlong strtolonglong(cchar *,char **,int) noex ;
extern ulonglong strtoulonglong(cchar *,char **,int) noex ;

EXTERNC_end


#endif /* STRTOXLONGLONG_INCLUDE */


