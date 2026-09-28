/* ctdec HEADER */
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

#ifndef	CTDEC_INCLUDE
#define	CTDEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */

#include	<ctdecf.h>		/* <- extra convenience */
#include	<ctdecp.h>		/* <- extra convenience */


EXTERNC_begin

extern int ctdecsi	(char *,int,sint)		noex ;
extern int ctdecsl	(char *,int,slong)		noex ;
extern int ctdecsll	(char *,int,slonglong)		noex ;

extern int ctdecui	(char *,int,uint)		noex ;
extern int ctdecul	(char *,int,ulong)		noex ;
extern int ctdecull	(char *,int,ulonglong)		noex ;

local inline int ctdeci(char *bp,int bl,int v)		noex {
	return ctdecsi(bp,bl,v) ;
} /* end */
local inline int ctdecl(char *bp,int bl,long v)		noex {
	return ctdecsl(bp,bl,v) ;
} /* end */
local inline int ctdecll(char *bp,int bl,longlong v)	noex {
	return ctdecsll(bp,bl,v) ;
} /* end */

EXTERNC_end

#ifdef	__cplusplus

inline int ctdec(char *bp,int bl,int v)			noex {
	return ctdeci(bp,bl,v) ;
} /* end */
inline int ctdec(char *bp,int bl,long v)		noex {
	return ctdecl(bp,bl,v) ;
} /* end */
inline int ctdec(char *bp,int bl,longlong v)		noex {
	return ctdecll(bp,bl,v) ;
} /* end */

inline int ctdec(char *bp,int bl,uint v)		noex {
	return ctdecui(bp,bl,v) ;
} /* end */
inline int ctdec(char *bp,int bl,ulong v)		noex {
	return ctdecul(bp,bl,v) ;
} /* end */
inline int ctdec(char *bp,int bl,ulonglong v)		noex {
	return ctdecull(bp,bl,v) ;
} /* end */

#endif /* __cplusplus */


#endif /* CTDEC_INCLUDE */


