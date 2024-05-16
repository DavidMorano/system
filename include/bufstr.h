/* bufstr SUPPORT */
/* lang=C++20 */

/* string buffer object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BUFSTR_INCLUDE
#define	BUFSTR_INCLUDE


#define	BUFSTR		struct bufstr_head
#define	BUFSTR_LEN	100		/* default value */


struct bufstr_head {
	char		*dbuf ;		/* dynamic- buffer */
	int		len ;		/* index (active length) */
	int		dlen ;		/* extent */
	char		sbuf[BUFSTR_LEN + 1] ; /* static-buffer */
} ;

typedef	BUFSTR		bufstr ;

EXTERNC_begin

extern int	bufstr_start(bufstr *) noex ;
extern int	bufstr_char(bufstr *,int) noex ;
extern int	bufstr_strw(bufstr *,cchar *,int) noex ;
extern int	bufstr_buf(bufstr *,cchar *,int) noex ;
extern int	bufstr_get(bufstr *,cchar **) noex ;
extern int	bufstr_finish(bufstr *) noex ;

EXTERNC_end


#endif /* bufstr_INCLUDE */


