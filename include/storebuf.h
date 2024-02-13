/* storebuf HEADER */
/* lang=C20 */

/* storage buffer manipulation subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STOREBUF_INCLUDE
#define	STOREBUF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


EXTERNC_begin

extern int storebuf_buf(char *,int,int,cchar *,int) noex ;
extern int storebuf_strw(char *,int,int,cchar *,int) noex ;
extern int storebuf_char(char *,int,int,int) noex ;
extern int storebuf_deci(char *,int,int,int) noex ;
extern int storebuf_decl(char *,int,int,long) noex ;
extern int storebuf_decll(char *,int,int,longlong) noex ;
extern int storebuf_decui(char *,int,int,uint) noex ;
extern int storebuf_decul(char *,int,int,ulong) noex ;
extern int storebuf_decull(char *,int,int,ulonglong) noex ;
extern int storebuf_hexi(char *,int,int,int) noex ;
extern int storebuf_hexl(char *,int,int,long) noex ;
extern int storebuf_hexll(char *,int,int,longlong) noex ;
extern int storebuf_hexui(char *,int,int,uint) noex ;
extern int storebuf_hexul(char *,int,int,ulong) noex ;
extern int storebuf_hexull(char *,int,int,ulonglong) noex ;

EXTERNC_end

#ifdef	__cplusplus

inline int storebuf_dec(char *bp,int bl,int i,int v) noex {
	return storebuf_deci(bp,bl,i,v) ;
}
inline int storebuf_dec(char *bp,int bl,int i,long v) noex {
	return storebuf_decl(bp,bl,i,v) ;
}
inline int storebuf_dec(char *bp,int bl,int i,longlong v) noex {
	return storebuf_decll(bp,bl,i,v) ;
}

inline int storebuf_dec(char *bp,int bl,int i,uint uv) noex {
	return storebuf_decui(bp,bl,i,uv) ;
}
inline int storebuf_dec(char *bp,int bl,int i,ulong uv) noex {
	return storebuf_decul(bp,bl,i,uv) ;
}
inline int storebuf_dec(char *bp,int bl,int i,ulonglong uv) noex {
	return storebuf_decull(bp,bl,i,uv) ;
}

inline int storebuf_hex(char *bp,int bl,int i,int v) noex {
	return storebuf_hexi(bp,bl,i,v) ;
}
inline int storebuf_hex(char *bp,int bl,int i,long v) noex {
	return storebuf_hexl(bp,bl,i,v) ;
}
inline int storebuf_hex(char *bp,int bl,int i,longlong v) noex {
	return storebuf_hexll(bp,bl,i,v) ;
}

inline int storebuf_hex(char *bp,int bl,int i,uint uv) noex {
	return storebuf_hexui(bp,bl,i,uv) ;
}
inline int storebuf_hex(char *bp,int bl,int i,ulong uv) noex {
	return storebuf_hexul(bp,bl,i,uv) ;
}
inline int storebuf_hex(char *bp,int bl,int i,ulonglong uv) noex {
	return storebuf_hexull(bp,bl,i,uv) ;
}

#endif /* __cplusplus */


#endif /* STOREBUF_INCLUDE */


