/* cfdec HEADER */
/* lang=C20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFDEC_INCLUDE
#define	CFDEC_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


EXTERNC_begin

extern int cfdeci(cchar *,int,int *) noex ;
extern int cfdecl(cchar *,int,long *) noex ;
extern int cfdecll(cchar *,int,longlong *) noex ;

extern int cfdecui(cchar *,int,uint *) noex ;
extern int cfdecul(cchar *,int,ulong *) noex ;
extern int cfdecull(cchar *,int,ulonglong *) noex ;

EXTERNC_end


#endif /* CFDEC_INCLUDE */


