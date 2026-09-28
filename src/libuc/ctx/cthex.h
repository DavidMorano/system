/* cthex HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* subroutines to convert an integer to a HEX string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CTHEX_INCLUDE
#define	CTHEX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


EXTERNC_begin

extern int cthexsc	(char *,int,schar)		noex ;
extern int cthexss	(char *,int,sshort)		noex ;
extern int cthexsi	(char *,int,sint)		noex ;
extern int cthexsl	(char *,int,slong)		noex ;
extern int cthexsll	(char *,int,slonglong)		noex ;

extern int cthexuc	(char *,int,uchar)		noex ;
extern int cthexus	(char *,int,ushort)		noex ;
extern int cthexui	(char *,int,uint)		noex ;
extern int cthexul	(char *,int,ulong)		noex ;
extern int cthexull	(char *,int,ulonglong)		noex ;

local inline int cthexc(char *bp,int bl,char v)		noex {
	return cthexsc(bp,bl,v) ;
} /* end */
local inline int cthexs(char *bp,int bl,short v)	noex {
	return cthexss(bp,bl,v) ;
} /* end */
local inline int cthexi(char *bp,int bl,int v)		noex {
	return cthexsi(bp,bl,v) ;
} /* end */
local inline int cthexl(char *bp,int bl,long v)		noex {
	return cthexsl(bp,bl,v) ;
} /* end */
local inline int cthexll(char *bp,int bl,longlong v)	noex {
	return cthexsll(bp,bl,v) ;
} /* end */

EXTERNC_end

#ifdef	__cplusplus

inline int cthex(char *bp,int bl,int v)			noex {
	return cthexi(bp,bl,v) ;
} /* end */
inline int cthex(char *bp,int bl,long v)		noex {
	return cthexl(bp,bl,v) ;
} /* end */
inline int cthex(char *bp,int bl,longlong v)		noex {
	return cthexll(bp,bl,v) ;
} /* end */

inline int cthex(char *bp,int bl,uchar v)		noex {
	return cthexuc	(bp,bl,v) ;
} /* end */
inline int cthex(char *bp,int bl,ushort v)		noex {
	return cthexus	(bp,bl,v) ;
} /* end */
inline int cthex(char *bp,int bl,uint v)		noex {
	return cthexui	(bp,bl,v) ;
} /* end */
inline int cthex(char *bp,int bl,ulong v)		noex {
	return cthexul	(bp,bl,v) ;
} /* end */
inline int cthex(char *bp,int bl,ulonglong v)		noex {
	return cthexull	(bp,bl,v) ;
} /* end */

#endif /* __cplusplus */


#endif /* CTHEX_INCLUDE */


