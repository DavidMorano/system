/* intsat HEADER */
/* lang=C20 */

/* Integer Saturation Addition */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	INTSAT_INCLUDE
#define	INTSAT_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<stdint.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


EXTERNC_begin

extern int intsatl(long) noex ;
extern int intsatll(longlong) noex ;

extern int intsatul(ulong) noex ;
extern int intsatull(ulonglong) noex ;

extern int iaddsat(int,int) noex ;
extern long laddsat(long,long) noex ;
extern longlong lladdsat(longlong,longlong) noex ;

extern uint uaddsat(uint,uint) noex ;
extern ulong uladdsat(ulong,ulong) noex ;
extern ulonglong ulladdsat(ulonglong,ulonglong) noex ;

EXTERNC_end

#ifdef	__cplusplus

static inline int intsat(long v) noex {
	return intsatl(v) ;
}
static inline int intsat(longlong v) noex {
	return intsatll(v) ;
}

static inline int intsat(ulong v) noex {
	return intsatul(v) ;
}
static inline int intsat(ulonglong v) noex {
	return intsatull(v) ;
}

#endif /* __cplusplus */


#endif /* INTSAT_INCLUDE */


