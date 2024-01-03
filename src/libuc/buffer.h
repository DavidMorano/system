/* buffer HEADER */
/* lang=C20 */

/* variable-length buffer management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BUFFER_INCLUDE
#define	BUFFER_INCLUDE


#include	<envstandards.h>
#include	<stdarg.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<stdintx.h>


#define	BUFFER		struct buffer_head


struct buffer_head {
	char		*buf ;		/* the "buffer" */
	int		startlen ;	/* saved for expansion purposes */
	int		len ;		/* occupied length */
	int		e ;		/* current buffer extent */
} ;

typedef BUFFER		buffer ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	buffer_start(buffer *,int) noex ;
extern int	buffer_reset(buffer *) noex ;
extern int	buffer_adv(buffer *,int) noex ;
extern int	buffer_char(buffer *,int) noex ;
extern int	buffer_strw(buffer *,cchar *,int) noex ;
extern int	buffer_buf(buffer *,cchar *,int) noex ;
extern int	buffer_deci(buffer *,int) noex ;
extern int	buffer_decl(buffer *,long) noex ;
extern int	buffer_decll(buffer *,longlong) noex ;
extern int	buffer_decui(buffer *,uint) noex ;
extern int	buffer_decul(buffer *,ulong) noex ;
extern int	buffer_decull(buffer *,ulonglong) noex ;
extern int	buffer_hexc(buffer *,int) noex ;
extern int	buffer_hexi(buffer *,int) noex ;
extern int	buffer_hexl(buffer *,long) noex ;
extern int	buffer_hexll(buffer *,longlong) noex ;
extern int	buffer_hexuc(buffer *,uint) noex ;
extern int	buffer_hexui(buffer *,uint) noex ;
extern int	buffer_hexul(buffer *,ulong) noex ;
extern int	buffer_hexull(buffer *,ulonglong) noex ;
extern int	buffer_printf(buffer *,cchar *,...) noex ;
extern int	buffer_vprintf(buffer *,cchar *,va_list) noex ;
extern int	buffer_getprev(buffer *) noex ;
extern int	buffer_get(buffer *,cchar **) noex ;
extern int	buffer_finish(buffer *) noex ;

#ifdef	__cplusplus
}
#endif

#ifdef	__cplusplus

inline int buffer_dec(buffer *op,int v) noex {
	return buffer_deci(op,v) ;
}
inline int buffer_dec(buffer *op,long v) noex {
	return buffer_decl(op,v) ;
}
inline int buffer_dec(buffer *op,longlong v) noex {
	return buffer_decll(op,v) ;
}

inline int buffer_dec(buffer *op,uint v) noex {
	return buffer_decui(op,v) ;
}
inline int buffer_dec(buffer *op,ulong v) noex {
	return buffer_decul(op,v) ;
}
inline int buffer_dec(buffer *op,ulonglong v) noex {
	return buffer_decull(op,v) ;
}

inline int buffer_hex(buffer *op,int v) noex {
	return buffer_hexi(op,v) ;
}
inline int buffer_hex(buffer *op,long v) noex {
	return buffer_hexl(op,v) ;
}
inline int buffer_hex(buffer *op,longlong v) noex {
	return buffer_hexll(op,v) ;
}

inline int buffer_hex(buffer *op,uint v) noex {
	return buffer_hexui(op,v) ;
}
inline int buffer_hex(buffer *op,ulong v) noex {
	return buffer_hexul(op,v) ;
}
inline int buffer_hex(buffer *op,ulonglong v) noex {
	return buffer_hexull(op,v) ;
}

#endif /* __cplusplus */


#endif /* BUFFER_INCLUDE */


