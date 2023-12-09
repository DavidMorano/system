/* cfnum INCLUDE */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFNUM_INCLUDE
#define	CFNUM_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int cfnumi(cchar *,int,int *) noex ;
extern int cfnuml(cchar *,int,long *) noex ;
extern int cfnumll(cchar *,int,longlong *) noex ;

extern int cfnumui(cchar *,int,uint *) noex ;
extern int cfnumul(cchar *,int,ulong *) noex ;
extern int cfnumull(cchar *,int,ulonglong *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CFNUM_INCLUDE */


