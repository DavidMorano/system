/* ctchars INCLUDE */
/* lang=C20 */


/* revision history:

	= 2023-10-14, David A­D­ Morano
	I wrote this just to have an extra value-digit conversion 
	facility available.

*/

/* Copyright © 2023 David A­D­ Morano.  All rights reserved. */

#ifndef	CTCHARS_INCLUDE
#define	CTCHARS_INCLUDE


#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int ctcharsi(char *,int,int,int) noex ;
extern int ctcharsl(char *,int,int,long) noex ;
extern int ctcharsll(char *,int,int,longlong) noex ;

extern int ctcharsui(char *,int,int,uint) noex ;
extern int ctcharsul(char *,int,int,ulong) noex ;
extern int ctcharsull(char *,int,int,ulonglong) noex ;

#ifdef	__cplusplus
}
#endif

#ifdef	__cplusplus

inline int ctchars(char *bp,int bl,int b,int v) noex {
	return ctcharsi(bp,bl,b,v) ;
}
inline int ctchars(char *bp,int bl,int b,long v) noex {
	return ctcharsl(bp,bl,b,v) ;
}
inline int ctchars(char *bp,int bl,int b,longlong v) noex {
	return ctcharsll(bp,bl,b,v) ;
}

inline int ctchars(char *bp,int bl,int b,uint v) noex {
	return ctcharsui(bp,bl,b,v) ;
}
inline int ctchars(char *bp,int bl,int b,ulong v) noex {
	return ctcharsul(bp,bl,b,v) ;
}
inline int ctchars(char *bp,int bl,int b,ulonglong v) noex {
	return ctcharsull(bp,bl,b,v) ;
}

#endif /* __cplusplus */


#endif /* CTCHARS_INCLUDE */


