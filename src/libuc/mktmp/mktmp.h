/* mktmp HEADER */
/* lang=C20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MKTMP_INCLUDE
#define	MKTMP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>


EXTERNC_begin

extern int mktmpdir(char *,cchar *,mode_t) noex ;
extern int mktmpuserdir(char *,cchar *,cchar *,mode_t) noex ;

EXTERNC_end


#endif /* MKTMP_INCLUDE */


