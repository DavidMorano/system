/* usupport_strnwcpy HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® 
	of what we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRNWCPY_INCLUDE
#define	USUPPORTSTRNWCPY_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern char *strnwcpybc(char *,int,cchar *,int = -1) noex ;
    extern char *strnwcpylc(char *,int,cchar *,int = -1) noex ;
    extern char *strnwcpyuc(char *,int,cchar *,int = -1) noex ;
    extern char *strnwcpyfc(char *,int,cchar *,int = -1) noex ;
    inline char *strnwcpy(char *dp,int dl,cchar *sp,int sl = -1) noex {
	return strnwcpybc(dp,dl,sp,sl) ;
    } /* end subroutine */
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTSTRNWCPY_INCLUDE */


