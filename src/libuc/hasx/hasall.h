/* hasall HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* has a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASALL_INCLUDE
#define	HASALL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

EXTERNC_begin

extern bool hasallalpha(cchar *,int) noex ;
extern bool hasallalnum(cchar *,int) noex ;
extern bool hasallwhite(cchar *,int) noex ;
extern bool hasalllc(cchar *,int) noex ;
extern bool hasalluc(cchar *,int) noex ;
extern bool hasalldig(cchar *,int) noex ;
extern bool hasallhdrkey(cchar *,int) noex ;
extern bool hasallbase(cchar *,int,int) noex ;
extern bool hasallchr(cchar *,int,int) noex ;

EXTERNC_end


#endif /* HASALL_INCLUDE */


