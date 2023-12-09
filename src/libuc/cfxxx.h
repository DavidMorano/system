/* cfxxx INCLUDE */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFXXX_INCLUDE
#define	CFXXX_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int cfxxxi(cchar *,int,int *) noex ;
extern int cfxxxl(cchar *,int,long *) noex ;
extern int cfxxxll(cchar *,int,longlong *) noex ;

extern int cfxxxui(cchar *,int,uint *) noex ;
extern int cfxxxul(cchar *,int,ulong *) noex ;
extern int cfxxxull(cchar *,int,ulonglong *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CFXXX_INCLUDE */


