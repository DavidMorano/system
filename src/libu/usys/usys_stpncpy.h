/* usys_stpncpy HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	stpncpy

	Description:
	These are extensions to the standard C-lamguage library
	subroutine |stpncpy(3c)|.  The destination buffer is
	zero-filled.

	Synopsis:
	char *stpncpy(char *dbuf,cchar *sp ...,size_t n) noex

	Arguments:
	dbuf		destination buffer pointer
	sp		source c-string pointer
	n		size of destination buffer in bytes

	Returns:
	-		pointer to the last byte in the destination buffer

	Notes:
	I hate these stupid interfaces that feature the 'size_t' type.
	That type is:
	a) bigger than necessary (how many c-strings are over 4G-Bytes?)
	b) is unsigned, so given a special value (as negative) is not
	possible.
	Gag!

*******************************************************************************/

#ifndef	USYSSTPNCPY_INCLUDE
#define	USYSSTPNCPY_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern char *stpncpyx(char *,int,int,...) noex ;

extern char *stpncpy1(char *,cc *,size_t) noex ;
extern char *stpncpy2(char *,cc *,cc *,size_t) noex ;
extern char *stpncpy3(char *,cc *,cc *,cc *,size_t) noex ;
extern char *stpncpy4(char *,cc *,cc *,cc *,cc *,size_t) noex ;
extern char *stpncpy5(char *,cc *,cc *,cc *,cc *,cc *,size_t) noex ;
extern char *stpncpy6(char *,cc *,cc *,cc *,cc *,cc *,cc *,size_t) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline char *stpncpy(char *dp,
		cc *s1,cc *s2,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,2,s1,s2) ;
} /* end */
inline char *stpncpy(char *dp,
		cc *s1,cc *s2,cc *s3,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,3,s1,s2,s3) ;
} /* end */
inline char *stpncpy(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,4,s1,s2,s3,s4) ;
} /* end */
inline char *stpncpy(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,5,s1,s2,s3,s4,s5) ;
} /* end */
inline char *stpncpy(char *dp,
		cc *s1,cc *s2,cc *s3,cc *s4,cc *s5,cc *s6,size_t n) noex {
    	cint dl = intconv(n) ;
	return stpncpyx(dp,dl,6,s1,s2,s3,s4,s5,s6) ;
} /* end */
#endif /* __cplusplus */


#endif /* USYSSTPNCPY_INCLUDE */


