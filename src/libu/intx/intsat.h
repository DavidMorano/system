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
#include	<stddef.h>
#include	<stdint.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>


EXTERNC_begin

static inline int intsati(cint v) noex {
	return v ;
}

extern int intsatl(clong) noex ;
extern int intsatll(clonglong) noex ;

extern int intsatui(cuint) noex ;
extern int intsatul(culong) noex ;
extern int intsatull(culonglong) noex ;

EXTERNC_end

#ifdef	__cplusplus

static inline int intsat(cint v) noex {
	return intsati(v) ;
}
static inline int intsat(clong v) noex {
	return intsatl(v) ;
}
static inline int intsat(clonglong v) noex {
	return intsatll(v) ;
}

static inline int intsat(cuint v) noex {
	return intsatui(v) ;
}
static inline int intsat(culong v) noex {
	return intsatul(v) ;
}
static inline int intsat(culonglong v) noex {
	return intsatull(v) ;
}

#endif /* __cplusplus */


#endif /* INTSAT_INCLUDE */


