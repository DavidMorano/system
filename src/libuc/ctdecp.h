/* ctdecp INCLUDE */
/* lang=C20 */

/* subroutines to convert an integer to a decimal string /w precision */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

#ifndef	CTDECP_INCLUDE
#define	CTDECP_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int	ctdecpi(char *,int,int,int) noex ;
extern int	ctdecpl(char *,int,int,long) noex ;
extern int	ctdecpll(char *,int,int,longlong) noex ;

extern int	ctdecpui(char *,int,int,uint) noex ;
extern int	ctdecpul(char *,int,int,ulong) noex ;
extern int	ctdecpull(char *,int,int,ulonglong) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CTDECP_INCLUDE */


