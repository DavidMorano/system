/* intsat INCLUDE */
/* lang=C20 */

/* Integer Saturation Addition */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 2000 David A�D� Morano.  All rights reserved. */

#ifndef	INTSAT_INCLUDE
#define	INTSAT_INCLUDE


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

extern int iaddsat(int,int) noex ;
extern long laddsat(long,long) noex ;
extern longlong lladdsat(longlong,longlong) noex ;
extern uint uaddsat(uint,uint) noex ;
extern ulong uladdsat(ulong,ulong) noex ;
extern ulonglong ulladdsat(ulonglong,ulonglong) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* INTSAT_INCLUDE */


