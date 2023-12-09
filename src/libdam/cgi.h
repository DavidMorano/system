/* cgi */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CGI_INCLUDE
#define	CGI_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<stdarg.h>
#include	<shio.h>
#include	<localmisc.h>


#define	CGI_MAGIC	0x43628195
#define	CGI		struct cgi_head
#define	CGI_FL		struct cgi_flags

#define	CGI_FBUFWHOLE		(1<<0)
#define	CGI_FBUFLINE		(1<<1)
#define	CGI_FBUFNONE		(1<<2)
#define	CGI_FBUFDEF		(1<<3)


struct cgi_flags {
	uint		dummy:1 ;
} ;

struct cgi_head {
	uint		magic ;
	CGI_FL		f ;
	SHIO		*ofp ;
	int		wlen ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	cgi_start(CGI *,SHIO *) noex ;
extern int	cgi_finish(CGI *) noex ;
extern int	cgi_eoh(CGI *) noex ;
extern int	cgi_hdrdate(CGI *,time_t) noex ;
extern int	cgi_hdr(CGI *,cchar *,cchar *,int) noex ;
extern int	cgi_write(CGI *,const void *,int) noex ;
extern int	cgi_printline(CGI *,cchar *,int) noex ;
extern int	cgi_vprintf(CGI *,cchar *,va_list) noex ;
extern int	cgi_printf(CGI *,cchar *,...) noex ;
extern int	cgi_putc(CGI *,int) noex ;
extern int	cgi_seek(CGI *,offset_t,int) noex ;
extern int	cgi_flush(CGI *) noex ;
extern int	cgi_reserve(CGI *,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CGI_INCLUDE */


