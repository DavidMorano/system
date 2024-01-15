/* cfchars HEADER */
/* lang=C20 */

/* convert a digit c-string to its integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFCHARS_INCLUDE
#define	CFCHARS_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


EXTERNC_begin

extern int cfcharsi(cchar *,int,int *) noex ;
extern int cfcharsl(cchar *,int,long *) noex ;
extern int cfcharsll(cchar *,int,longlong *) noex ;

extern int cfcharsui(cchar *,int,uint *) noex ;
extern int cfcharsul(cchar *,int,ulong *) noex ;
extern int cfcharsull(cchar *,int,ulonglong *) noex ;

EXTERNC_end


#endif /* CFCHARS_INCLUDE */


