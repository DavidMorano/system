/* ctxxx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines to convert an integer to a decimal string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written having been adapted (from
	memory) from something I wrote back in the early 1980s (for
	embedded work).  I had to write every ... last  ... thing
	myself back in the old days.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTXXX_INCLUDE
#define	CTXXX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int ctxxxsi	(char *,int,int,sint)		noex ;
extern int ctxxxsl	(char *,int,int,slong)		noex ;
extern int ctxxxsll	(char *,int,int,slonglong)	noex ;

extern int ctxxxui	(char *,int,int,uint)		noex ;
extern int ctxxxul	(char *,int,int,ulong)		noex ;
extern int ctxxxull	(char *,int,int,ulonglong)	noex ;

local inline int ctxxxi(char *bp,int bl,int b,int v)		noex {
	return ctxxxsi(bp,bl,b,v) ;
} /* end */
local inline int ctxxxl(char *bp,int bl,int b,long v)		noex {
	return ctxxxsl(bp,bl,b,v) ;
} /* end */
local inline int ctxxxll(char *bp,int bl,int b,longlong v)	noex {
	return ctxxxsll(bp,bl,b,v) ;
} /* end */

EXTERNC_end

#ifdef	__cplusplus

inline int ctxxx(char *bp,int bl,int b,int v)		noex {
	return ctxxxi(bp,bl,b,v) ;
} /* end */
inline int ctxxx(char *bp,int bl,int b,long v)		noex {
	return ctxxxl(bp,bl,b,v) ;
} /* end */
inline int ctxxx(char *bp,int bl,int b,longlong v)	noex {
	return ctxxxll(bp,bl,b,v) ;
} /* end */

inline int ctxxx(char *bp,int bl,int b,uint v)		noex {
	return ctxxxui(bp,bl,b,v) ;
} /* end */
inline int ctxxx(char *bp,int bl,int b,ulong v)		noex {
	return ctxxxul(bp,bl,b,v) ;
} /* end */
inline int ctxxx(char *bp,int bl,int b,ulonglong v)	noex {
	return ctxxxull(bp,bl,b,v) ;
} /* end */

#endif /* __cplusplus */


#endif /* CTXXX_INCLUDE */


