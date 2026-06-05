/* ffbs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Find-First-Bit-Set in various sized integers */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FFBS_INCLUDE
#define	FFBS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int	ffbsi	(int)		noex ;
extern int	ffbsl	(long)		noex ;
extern int	ffbsll	(longlong)	noex ;

extern int	ffbsui	(uint)		noex ;
extern int	ffbsul	(ulong)		noex ;
extern int	ffbsull	(ulonglong)	noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int ffbs(int v)			noex {
	return ffbsi(v) ;
}
inline int ffbs(long v)			noex {
	return ffbsl(v) ;
}
inline int ffbs(longlong v)		noex {
	return ffbsll(v) ;
}

inline int ffbs(uint uv)		noex {
	return ffbsui(uv) ;
}
inline int ffbs(ulong uv)		noex {
	return ffbsul(uv) ;
}
inline int ffbs(ulonglong uv)		noex {
	return ffbsull(uv) ;
}

#endif /* __cplusplus */


#endif /* FFBS_INCLUDE */


