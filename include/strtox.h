/* strtox HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* numeric-character conversion */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTOX_INCLUDE
#define	STRTOX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>


EXTERNC_begin

extern int		strtoxi(cchar *,char **,int) noex ;	/* 32-bits */
extern uint		strtoxui(cchar *,char **,int) noex ;	/* 32-bits */

extern long		strtoxl(cchar *,char **,int) noex ;	/* 64-bits */
extern ulong		strtoxul(cchar *,char **,int) noex ;	/* 64-bits */

extern longlong		strtoxll(cchar *,char **,int) noex ;	/* 128-bits */
extern ulonglong	strtoxull(cchar *,char **,int) noex ;	/* 128-bits */

#ifdef	COMMENT /* this is a 256-bit integer; but it does not yet exist */
extern intmax		strtoxim(cchar *,char **,int) noex ;	/* 256-bits */
extern uintmax		strtoxuim(cchar *,char **,int) noex ;	/* 256-bits */
#endif /* COMMENT */

EXTERNC_end


#endif /* STRTOX_INCLUDE */


