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

extern int intcmpsc	(char,char)		noex ;
extern int intcmpss	(short,short)		noex ;
extern int intcmpsi	(int,int)		noex ;
extern int intcmpsl	(long,long)		noex ;
extern int intcmpsll	(longlong,longlong)	noex ;

extern int intcmpuc	(uchar,uchar)		noex ;
extern int intcmpus	(ushort,ushort)		noex ;
extern int intcmpui	(uint,uint)		noex ;
extern int intcmpul	(ulong,ulong)		noex ;
extern int intcmpull	(ulonglong,ulonglong)	noex ;

EXTERNC_end


#ifdef	__cplusplus
inline int intcmp	(char v1,char v2)	noex {
    return intcmpsc(v1,v2) ;
}
inline int intcmp	(short v1,short v2)	noex {
    return intcmpss(v1,v2) ;
}
inline int intcmp	(int v1,int v2)		noex {
    return intcmpsi(v1,v2) ;
}
inline int intcmp	(long v1,long v2)	noex {
    return intcmpsl(v1,v2) ;
}
inline int intcmp	(longlong v1,longlong v2) noex {
    return intcmpsll(v1,v2) ;
}
#endif /* __cplusplus */

#ifdef	__cplusplus
inline int intcmp	(uchar v1,uchar v2)	noex {
    return intcmpuc(v1,v2) ;
}
inline int intcmp	(ushort v1,ushort v2)	noex {
    return intcmpus(v1,v2) ;
}
inline int intcmp	(uint v1,uint v2)	noex {
    return intcmpui(v1,v2) ;
}
inline int intcmp	(ulong v1,ulong v2)	noex {
    return intcmpul(v1,v2) ;
}
inline int intcmp	(ulonglong v1,ulonglong v2) noex {
    return intcmpull(v1,v2) ;
}
#endif /* __cplusplus */


#endif /* INTCMP_INCLUDE */


