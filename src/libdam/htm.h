/* htm HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* hack to output HTML */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HTM_INCLUDE
#define	HTM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |off_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdarg.h>
#include	<shio.h>


#define	HTM_MAGIC		0x43628193
#define	HTM			struct htm_head
#define	HTM_FL			struct htm_flags

#define	HTM_FBUFWHOLE		(1<<0)
#define	HTM_FBUFLINE		(1<<1)
#define	HTM_FBUFNONE		(1<<2)
#define	HTM_FBUFDEF		(1<<3)


struct htm_flags {
	uint		dummy:1 ;
} ;

struct htm_head {
	shio		*ofp ;
	char		*lbuf ;
	HTM_FL		fl ;		/* currently unused! */
	uint		magic ;
	int		llen ;
	int		wlen ;
} ;

typedef	HTM		htm ;
typedef	HTM_FL		htm_fl ;

EXTERNC_begin

extern int htm_start(htm *,shio *,cchar *) noex ;
extern int htm_finish(htm *) noex ;
extern int htm_headbegin(htm *,cchar *) noex ;
extern int htm_headend(htm *) noex ;
extern int htm_bodybegin(htm *,cchar *) noex ;
extern int htm_bodyend(htm *) noex ;
extern int htm_tagbegin(htm *,cchar *,cchar *,cchar *,cchar *(*)[2]) noex ;
extern int htm_tagend(htm *,cchar *) noex ;
extern int htm_abegin(htm *,cchar *,cchar *,cchar *,cchar *) noex ;
extern int htm_aend(htm *) noex ;

extern int htm_textbegin(htm *,cchar *,cchar *,cchar *,
			int,int,cchar *(*)[2]) noex ;
extern int htm_textend(htm *) noex ;

extern int htm_hr(htm *,cchar *,cchar *) noex ;
extern int htm_br(htm *,cchar *,cchar *) noex ;
extern int htm_img(htm *,cchar *,cchar *,cchar *,cchar *,cchar *,int,int) noex ;

extern int htm_write(htm *,cvoid *,int) noex ;
extern int htm_printline(htm *,cchar *,int) noex ;
extern int htm_vprintf(htm *,cchar *,va_list) noex ;
extern int htm_printf(htm *,cchar *,...) noex ;
extern int htm_putc(htm *,int) noex ;
extern int htm_seek(htm *,off_t,int) noex ;
extern int htm_flush(htm *) noex ;
extern int htm_reserve(htm *,int) noex ;

EXTERNC_end


#endif /* HTM_INCLUDE */


