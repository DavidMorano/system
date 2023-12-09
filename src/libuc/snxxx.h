/* snxxx INCLUDE */
/* lang=C20 */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNXXX_INCLUDE
#define	SNXXX_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>

#ifdef	__cplusplus
extern "C" {
#endif

extern int snsds(char *,int,cchar *,cchar *) noex ;
extern int snscs(char *,int,cchar *,cchar *) noex ;
extern int snses(char *,int,cchar *,cchar *) noex ;
extern int snsdd(char *,int,cchar *,uint) noex ;
extern int snddd(char *,int,uint,uint) noex ;
extern int snsd(char *,int,cchar *,uint) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SNXXX_INCLUDE */


