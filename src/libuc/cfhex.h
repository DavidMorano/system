/* cfhex INCLUDE */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFHEX_INCLUDE
#define	CFHEX_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int cfhexi(cchar *,int,int *) noex ;
extern int cfhexl(cchar *,int,long *) noex ;
extern int cfhexll(cchar *,int,longlong *) noex ;

extern int cfhexui(cchar *,int,uint *) noex ;
extern int cfhexul(cchar *,int,ulong *) noex ;
extern int cfhexull(cchar *,int,ulonglong *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CFHEX_INCLUDE */


