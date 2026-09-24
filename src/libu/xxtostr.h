/* xxtostr HEADER (X-To-String) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines to convert an integer to a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2024-01-15, David A-D- Morano
	Added a comment below.

*/

/* Copyright (c) 1998,2024 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	{x}tostr

	Description:
	These subroutines convert integers (either signed or unsigned
	of various sized types) into their c-string digit
	representations.

	The following subroutines are provoided for base-10 conversions
	of the various types:

	name		signed	size
	itostr		yes	32-bit
	ltostr		yes	64-bit
	lltostr		yes	128-bit
	uitostr		no	32-bit
	ultostr		no	64-bit
	ulltostr	no	128-bit

*******************************************************************************/

#ifndef	XXTOSTR_INCLUDE
#define	XXTOSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU extended integer types */


EXTERNC_begin

extern char *sitostr(sint,		char *) noex ;
extern char *sltostr(slong,		char *) noex ;
extern char *slltostr(slonglong,	char *) noex ;

extern char *uitostr(uint,		char *) noex ;
extern char *ultostr(ulong,		char *) noex ;
extern char *ulltostr(ulonglong,	char *) noex ;

local inline char *itostr(int v,char *endp) noex {
    	return sitostr(v,endp) ;
} /* end subroutine */
local inline char *ltostr(long v,char *endp) noex {
    	return sltostr(v,endp) ;
} /* end subroutine */
local inline char *lltostr(longlong v,char *endp) noex {
    	return slltostr(v,endp) ;
} /* end subroutine */

EXTERNC_end

#ifdef	__cplusplus

extern char *sitostr(sint,		char *,int) noex ;
extern char *sltostr(slong,		char *,int) noex ;
extern char *slltostr(slonglong,	char *,int) noex ;

extern char *uitostr(uint,		char *,int) noex ;
extern char *ultostr(ulong,		char *,int) noex ;
extern char *ulltostr(ulonglong,	char *,int) noex ;

inline char *xtostr(sint sv,		char *endp,int b = 10) noex {
    return sitostr(sv,endp,b) ;
} /* end */
inline char *xtostr(slong sv,		char *endp,int b = 10) noex {
    return sltostr(sv,endp,b) ;
} /* end */
inline char *xtostr(slonglong sv,	char *endp,int b = 10) noex {
    return slltostr(sv,endp,b) ;
} /* end */

inline char *xtostr(uint uv,		char *endp,int b = 10) noex {
    return uitostr(uv,endp,b) ;
} /* end */
inline char *xtostr(ulong uv,		char *endp,int b = 10) noex {
    return ultostr(uv,endp,b) ;
} /* end */
inline char *xtostr(ulonglong uv,	char *endp,int b = 10) noex {
    return ulltostr(uv,endp,b) ;
} /* end */

#endif /* __cplusplus */


#endif /* XXTOSTR_INCLUDE */


