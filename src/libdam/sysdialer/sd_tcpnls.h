/* tcpnls HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "tcpnls" dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	TCPNLS_INCLUDE
#define	TCPNLS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<sysdialer.h>


#define	TCPNLS_MAGIC	31415926
#define	TCPNLS		struct tcpnls_head


struct tcpnls_head {
	time_t		opentime ;
	uint		magic ;
	int		fd ;
} ; /* end struct */

EXTERNC_begin

extern int tcpnls_open(TCPNLS *,SYSDIALER_ARGS *,
		cchar *,cchar *,cchar **) noex ;
extern int tcpnls_reade(TCPNLS *,char *,int,int,int) noex ;
extern int tcpnls_recve(TCPNLS *,char *,int,int,int,int) noex ;
extern int tcpnls_recvfrome(TCPNLS *,char *,int,int,void *,int *,int,int) noex ;
extern int tcpnls_recvmsge(TCPNLS *,MSGHDR *,int,int,int) noex ;
extern int tcpnls_write(TCPNLS *,cchar *,int) noex ;
extern int tcpnls_send(TCPNLS *,cchar *,int,int) noex ;
extern int tcpnls_sendto(TCPNLS *,cchar *,int,int,void *,int) noex ;
extern int tcpnls_sendmsg(TCPNLS *,MSGHDR *,int) noex ;
extern int tcpnls_shutdown(TCPNLS *,int) noex ;
extern int tcpnls_close(TCPNLS *) noex ;

EXTERNC_end


#endif /* TCPNLS_INCLUDE */


