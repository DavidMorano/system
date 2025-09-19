/* cgi HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CGI_INCLUDE
#define	CGI_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |off_t| */
#include	<stdarg.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<shio.h>


#define	CGI_MAGIC	0x43628195
#define	CGI		struct cgi_head
#define	CGI_FL		struct cgi_flags
/* options */
#define	CGI_FBUFWHOLE		(1<<0)
#define	CGI_FBUFLINE		(1<<1)
#define	CGI_FBUFNONE		(1<<2)
#define	CGI_FBUFDEF		(1<<3)


struct cgi_flags {
	uint		dummy:1 ;
} ;

struct cgi_head {
	shio		*ofp ;
	CGI_FL		fl ;		/* currently unused! */
	uint		magic ;
	int		wlen ;
} ;

typedef	CGI		cgi ;
typedef	CGI_FL		cgi_fl ;

EXTERNC_begin

extern int	cgi_start(cgi *,shio *) noex ;
extern int	cgi_finish(cgi *) noex ;
extern int	cgi_eoh(cgi *) noex ;
extern int	cgi_hdrdate(cgi *,time_t) noex ;
extern int	cgi_hdr(cgi *,cchar *,cchar *,int) noex ;
extern int	cgi_write(cgi *,const void *,int) noex ;
extern int	cgi_println(cgi *,cchar *,int) noex ;
extern int	cgi_vprintf(cgi *,cchar *,va_list) noex ;
extern int	cgi_printf(cgi *,cchar *,...) noex ;
extern int	cgi_putc(cgi *,int) noex ;
extern int	cgi_seek(cgi *,off_t,int) noex ;
extern int	cgi_flush(cgi *) noex ;
extern int	cgi_reserve(cgi *,int) noex ;

EXTERNC_end


#endif /* CGI_INCLUDE */


