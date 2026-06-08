/* cfhexstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert from a HEX string to the value (string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CFHEXSTR_INCLUDE
#define	CFHEXSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int cfhexstr	(cchar *,int,uchar *)		noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int cfhexstr(cchar *sp,int sl,char *rbuf)	noex {
    	uchar *ubuf = cast_reinterpret<uchar *>(rbuf) ;
	return cfhexstr(sp,sl,ubuf) ;
} /* end subroutine (cfhexstr) */

#endif /* __cplusplus */


#endif /* CFHEXSTR_INCLUDE */


