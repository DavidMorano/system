/* snx HEADER */
/* lang=C20 */

/* counted c-string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNX_INCLUDE
#define	SNX_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>

#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<snxxx.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int snfsflags(char *,int,ulong) noex ;
extern int snopenflags(char *,int,int) noex ;
extern int snpollflags(char *,int,int) noex ;
extern int snxtilook(char *,int,int) noex ;
extern int sninetaddr(char *,int,int,cchar *) noex ;
extern int snsigabbr(char *,int,uint) noex ;
extern int snabbr(char *,int,cchar *,int) noex ;
extern int snshellunder(char *,int,pid_t,cchar *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SNX_INCLUDE */


