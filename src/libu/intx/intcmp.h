/* intcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* Integer-Compare */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	INTCMP_INCLUDE
#define	INTCMP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern int	intcmpc(uchar,uchar)		noex ;
extern int	intcmps(ushort,ushort)		noex ;
extern int	intcmpi(uint,uint)		noex ;
extern int	intcmpl(ulong,ulong)		noex ;
extern int	intcmpll(ulonglong,ulonglong)	noex ;

EXTERNC_end


#ifdef	__cplusplus
inline int intcmp(uchar v1,uchar v2) noex {
    return intcmpc(v1,v2) ;
}
inline int intcmp(ushort v1,ushort v2) noex {
    return intcmps(v1,v2) ;
}
inline int intcmp(uint v1,uint v2) noex {
    return intcmpi(v1,v2) ;
}
inline int intcmp(ulong v1,ulong v2) noex {
    return intcmpl(v1,v2) ;
}
inline int intcmp(ulonglong v1,ulonglong v2) noex {
    return intcmpll(v1,v2) ;
}
#endif /* __cplusplus */


#endif /* INTCMP_INCLUDE */


