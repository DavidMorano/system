/* cfoct INCLUDE */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFOCT_INCLUDE
#define	CFOCT_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int cfocti(cchar *,int,int *) noex ;
extern int cfoctl(cchar *,int,long *) noex ;
extern int cfoctll(cchar *,int,longlong *) noex ;

extern int cfoctui(cchar *,int,uint *) noex ;
extern int cfoctul(cchar *,int,ulong *) noex ;
extern int cfoctull(cchar *,int,ulonglong *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CFOCT_INCLUDE */


