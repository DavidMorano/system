/* cfb26 INCLUDE */
/* lang=C20 */

/* convert from base-26 */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFB26_INCLUDE
#define	CFB26_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int cfb26i(cchar *,int,int *) noex ;
extern int cfb26l(cchar *,int,long *) noex ;
extern int cfb26ll(cchar *,int,longlong *) noex ;

extern int cfb26ui(cchar *,int,uint *) noex ;
extern int cfb26ul(cchar *,int,ulong *) noex ;
extern int cfb26ull(cchar *,int,ulonglong *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CFB26_INCLUDE */


