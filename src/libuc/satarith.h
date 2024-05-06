/* satarith HEADER */
/* lang=C20 */

/* Integer Saturation Addition */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	SATARITH_INCLUDE
#define	SATARITH_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<stdint.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>


EXTERNC_begin

extern int iaddsat(int,int) noex ;
extern long laddsat(long,long) noex ;
extern longlong lladdsat(longlong,longlong) noex ;

extern uint uaddsat(uint,uint) noex ;
extern ulong uladdsat(ulong,ulong) noex ;
extern ulonglong ulladdsat(ulonglong,ulonglong) noex ;

EXTERNC_end


#endif /* SATARITH_INCLUDE */


