/* intsat HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* Integer Saturation (clamping) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	INTSAT_INCLUDE
#define	INTSAT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdint.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

local inline int intsati(cint v) noex {
	return v ;
}

extern int intsatl	(clong)		noex ;
extern int intsatll	(clonglong)	noex ;

extern int intsatui	(cuint)		noex ;
extern int intsatul	(culong)	noex ;
extern int intsatull	(culonglong)	noex ;

EXTERNC_end

#ifdef	__cplusplus

local inline int intsat(cint v)		noex {
	return intsati(v) ;
} /* end */
local inline int intsat(clong v)	noex {
	return intsatl(v) ;
} /* end */
local inline int intsat(clonglong v)	noex {
	return intsatll(v) ;
} /* end */

local inline int intsat(cuint v)	noex {
	return intsatui(v) ;
} /* end */
local inline int intsat(culong v)	 noex {
	return intsatul(v) ;
} /* end */
local inline int intsat(culonglong v)	noex {
	return intsatull(v) ;
} /* end */

#endif /* __cplusplus */


#endif /* INTSAT_INCLUDE */


