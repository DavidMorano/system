/* cfdecmf HEADER */
/* lang=C20 */

/* Convert-From-Decimal-Multiply-Factor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFDECMF_INCLUDE
#define	CFDECMF_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int cfdecmfi(cchar *,int,int *) noex ;
extern int cfdecmfl(cchar *,int,long *) noex ;
extern int cfdecmfll(cchar *,int,longlong *) noex ;

extern int cfdecmfui(cchar *,int,uint *) noex ;
extern int cfdecmful(cchar *,int,ulong *) noex ;
extern int cfdecmfull(cchar *,int,ulonglong *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CFDECMF_INCLUDE */


