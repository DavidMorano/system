/* udp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "udp" dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UDP_INCLUDE
#define	UDP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<sysdialer.h>


#define	UDP_MAGIC	31415926
#define	UDP		struct udp_head


struct udp_head {
	uint		magic ;
	int		fd ;
} ; /* end struct */

EXTERNC_begin

extern int udp_open(UDP *,SYSDIALER_ARGS *,cchar *,cchar *,cchar **) noex ;
extern int udp_reade(UDP *,char *,int,int,int) noex ;
extern int udp_recve(UDP *,char *,int,int,int,int) noex ;
extern int udp_recvfrome(UDP *,char *,int,int,void *,int *,int,int) noex ;
extern int udp_recvmsge(UDP *,MSGHDR *,int,int,int) noex ;
extern int udp_write(UDP *,cchar *,int) noex ;
extern int udp_send(UDP *,cchar *,int,int) noex ;
extern int udp_sendto(UDP *,cchar *,int,int,void *,int) noex ;
extern int udp_sendmsg(UDP *,MSGHDR *,int) noex ;
extern int udp_shutdown(UDP *,int) noex ;
extern int udp_close(UDP *) noex ;

EXTERNC_end


#endif /* UDP_INCLUDE */


