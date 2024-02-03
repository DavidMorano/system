/* cta26 HEADER */
/* lang=C20 */

/* subroutines to convert an integer to a base-26 string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTA26_INCLUDE
#define	CTA26_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>


EXTERNC_begin

extern int cta26i(char *,int,int,int,int) noex ;
extern int cta26l(char *,int,int,int,long) noex ;
extern int cta26ll(char *,int,int,int,longlong) noex ;

extern int cta26ui(char *,int,int,int,uint) noex ;
extern int cta26ul(char *,int,int,int,ulong) noex ;
extern int cta26ull(char *,int,int,int,ulonglong) noex ;

EXTERNC_end


#endif /* CTA26_INCLUDE */


