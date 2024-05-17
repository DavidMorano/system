/* intrem HEADER */
/* lang=C20 */

/* Integer-Remaider */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	INTREM_INCLUDE
#define	INTREM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


EXTERNC_begin

extern int		irem(int,int) noex ;
extern long		lrem(long,long) noex ;
extern longlong		llrem(longlong,longlong) noex ;
extern uint		urem(uint,uint) noex ;
extern ulong		ulrem(ulong,ulong) noex ;
extern ulonglong	ullrem(ulonglong,ulonglong) noex ;

EXTERNC_end


#endif /* INTREM_INCLUDE */


