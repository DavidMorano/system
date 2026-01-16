/* ussmux HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "ussmux" dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USSMUX_INCLUDE
#define	USSMUX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<logfile.h>
#include	<sysdialer.h>


#define	USSMUX_MAGIC		31415926
#define	USSMUX			struct ussmux_head
#define	USSMUX_FL		struct ussmux_flags


struct ussmux_flags {
	uint		log:1 ;
} ; /* end struct (ussmux_flags) */

struct ussmux_head {
	logfile		lh ;
	USSMUX_FL	fl, open ;
	uint		magic ;
	int		tlen ;
	int		fd ;
} ; /* end struct (ussmux_head) */

EXTERNC_begin

extern int ussmux_open(USSMUX *,SYSDIALER_ARGS *,
		cchar *,cchar *,cchar **) noex ;
extern int ussmux_reade(USSMUX *,char *,int,int,int) noex ;
extern int ussmux_recve(USSMUX *,char *,int,int,int,int) noex ;
extern int ussmux_recvfrome(USSMUX *,char *,int,int,void *,int *,int,int) noex ;
extern int ussmux_recvmsge(USSMUX *,MSGHDR *,int,int,int) noex ;
extern int ussmux_write(USSMUX *,cchar *,int) noex ;
extern int ussmux_send(USSMUX *,cchar *,int,int) noex ;
extern int ussmux_sendto(USSMUX *,cchar *,int,int,void *,int) noex ;
extern int ussmux_sendmsg(USSMUX *,MSGHDR *,int) noex ;
extern int ussmux_shutdown(USSMUX *,int) noex ;
extern int ussmux_close(USSMUX *) noex ;

EXTERNC_end


#endif /* USSMUX_INCLUDE */


