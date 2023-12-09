/* ctb26 INCLUDE */
/* lang=C20 */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTB26_INCLUDE
#define	CTB26_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int ctb26i(char *,int,int,int,int) noex ;
extern int ctb26l(char *,int,int,int,long) noex ;
extern int ctb26ll(char *,int,int,int,longlong) noex ;

extern int ctb26ui(char *,int,int,int,uint) noex ;
extern int ctb26ul(char *,int,int,int,ulong) noex ;
extern int ctb26ull(char *,int,int,int,ulonglong) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CTB26_INCLUDE */


