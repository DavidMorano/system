/* tcpmux HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "tcpmux" dialer module */
/* version %I% last-modified %G% */


/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	TCPMUX_INCLUDE
#define	TCPMUX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<sysdialer.h>


#define	TCPMUX_MAGIC	31415926
#define	TCPMUX		struct tcpmux_head


struct tcpmux_head {
	uint		magic ;
	int		fd ;
} ;

typedef	TCPMUX		tcpmux ;

EXTERNC_begin

extern int tcpmux_open(TCPMUX *,SYSDIALER_ARGS *,
		cchar *,cchar *,cchar **) noex ;
extern int tcpmux_reade(TCPMUX *,char *,int,int,int) noex ;
extern int tcpmux_recve(TCPMUX *,char *,int,int,int,int) noex ;
extern int tcpmux_recvfrome(TCPMUX *,char *,int,int,void *,int *,int,int) noex ;
extern int tcpmux_recvmsge(TCPMUX *,MSGHDR *,int,int,int) noex ;
extern int tcpmux_write(TCPMUX *,cchar *,int) noex ;
extern int tcpmux_send(TCPMUX *,cchar *,int,int) noex ;
extern int tcpmux_sendto(TCPMUX *,cchar *,int,int,void *,int) noex ;
extern int tcpmux_sendmsg(TCPMUX *,MSGHDR *,int) noex ;
extern int tcpmux_shutdown(TCPMUX *,int) noex ;
extern int tcpmux_close(TCPMUX *) noex ;

EXTERNC_end


#endif /* TCPMUX_INCLUDE */


