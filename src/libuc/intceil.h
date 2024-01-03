/* intceil HEADER */
/* lang=C20 */

/* Integer-Ceiling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	INTCEIL_INCLUDE
#define	INTCEIL_INCLUDE 


#include	<envstandards.h>
#include	<sys/types.h>
#include	<stdint.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int		iceil(int,int) noex ;
extern long		lceil(long,int) noex ;
extern longlong		llceil(longlong,int) noex ;
extern off_t		ofceil(off_t,int) noex ;
extern intptr_t		ipceil(intptr_t,int) noex ;

extern uint		uceil(uint,int) noex ;
extern ulong		ulceil(ulong,int) noex ;
extern ulonglong	ullceil(ulonglong,int) noex ;
extern size_t		szceil(size_t,int) noex ;
extern uintptr_t	uipceil(uintptr_t,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* INTCEIL_INCLUDE */


