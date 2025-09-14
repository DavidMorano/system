/* tcp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TCP_INCLUDE
#define	TCP_INCLUDE

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<sysdialer.h>


#define	TCP_MAGIC	31415926
#define	TCP		struct tcp_head


struct tcp_head {
	uint		magic ;
	int		fd ;
} ;

typedef	TCP		tcp ;

EXTERNC_begin

extern int tcp_open(TCP *,SYSDIALER_ARGS *,cchar *,cchar *,cchar **) noex ;
extern int tcp_reade(TCP *,char *,int,int,int) noex ;
extern int tcp_recve(TCP *,char *,int,int,int,int) noex ;
extern int tcp_recvfrome(TCP *,char *,int,int,void *,int *,int,int) noex ;
extern int tcp_recvmsge(TCP *,MSGHDR *,int,int,int) noex ;
extern int tcp_write(TCP *,cchar *,int) noex ;
extern int tcp_send(TCP *,cchar *,int,int) noex ;
extern int tcp_sendto(TCP *,cchar *,int,int,void *,int) noex ;
extern int tcp_sendmsg(TCP *,MSGHDR *,int) noex ;
extern int tcp_shutdown(TCP *,int) noex ;
extern int tcp_close(TCP *) noex ;

EXTERNC_end


#endif /* TCP_INCLUDE */


