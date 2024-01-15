/* intfloor HEADER */
/* lang=C20 */

/* Integer-Flooring */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	INTFLOOR_INCLUDE
#define	INTFLOOR_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<stdint.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


EXTERNC_begin

extern int		ifloor(int,int) noex ;
extern long		lfloor(long,int) noex ;
extern longlong		llfloor(longlong,int) noex ;
extern off_t		offloor(off_t,int) noex ;
extern intptr_t		ipfloor(intptr_t,int) noex ;

extern uint		ufloor(uint,int) noex ;
extern ulong		ulfloor(ulong,int) noex ;
extern ulonglong	ullfloor(ulonglong,int) noex ;
extern size_t		szfloor(size_t,int) noex ;
extern uintptr_t	uipfloor(uintptr_t,int) noex ;

EXTERNC_end


#endif /* INTFLOOR_INCLUDE */


