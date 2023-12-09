/* cfbin INCLUDE */
/* lang=C20 */

/* convert a digit c-string to its integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFBIN_INCLUDE
#define	CFBIN_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int cfbini(cchar *,int,int *) noex ;
extern int cfbinl(cchar *,int,long *) noex ;
extern int cfbinll(cchar *,int,longlong *) noex ;

extern int cfbinui(cchar *,int,uint *) noex ;
extern int cfbinul(cchar *,int,ulong *) noex ;
extern int cfbinull(cchar *,int,ulonglong *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CFBIN_INCLUDE */


