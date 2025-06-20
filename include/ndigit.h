/* ndigit HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* calculate the number of digits in a given integer given a number-base */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NDIGIT_INCLUDE
#define	NDIGIT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>


EXTERNC_begin

extern int ndigiti(int,int) noex ;
extern int ndigitl(long,int) noex ;
extern int ndigitll(longlong,int) noex ;

extern int ndigitui(uint,int) noex ;
extern int ndigitul(ulong,int) noex ;
extern int ndigitull(ulonglong,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int ndigit(int v,int b) noex {
	return ndigiti(v,b) ;
}
inline int ndigit(long v,int b) noex {
	return ndigitl(v,b) ;
}
inline int ndigit(longlong v,int b) noex {
	return ndigitll(v,b) ;
}

inline int ndigit(uint v,int b) noex {
	return ndigitui(v,b) ;
}
inline int ndigit(ulong v,int b) noex {
	return ndigitul(v,b) ;
}
inline int ndigit(ulonglong v,int b) noex {
	return ndigitull(v,b) ;
}

#endif /* __cplusplus */


#endif /* NDIGIT_INCLUDE */


