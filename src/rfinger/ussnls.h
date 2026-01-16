/* ussnls HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "ussnls" dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USSNLS_INCLUDE
#define	USSNLS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<logfile.h>
#include	<sysdialer.h>


#define	USSNLS_MAGIC		31415926
#define	USSNLS			struct ussnls_head
#define	USSNLS_FL		struct ussnls_flags


struct ussnls_flags {
	uint		log:1 ;
} ; /* end struct */

struct ussnls_head {
	logfile		lh ;
	USSNLS_FL	fl, open ;
	uint		magic ;
	int		tlen ;
	int		fd ;
} ; /* end struct */

EXTERNC_end

extern int ussnls_open(USSNLS *,SYSDIALER_ARGS *,
		cchar *,cchar *,cchar **) noex ;
extern int ussnls_reade(USSNLS *,char *,int,int,int) noex ;
extern int ussnls_recve(USSNLS *,char *,int,int,int,int) noex ;
extern int ussnls_recvfrome(USSNLS *,char *,int,int,void *,int *,int,int) noex ;
extern int ussnls_recvmsge(USSNLS *,MSGHDR *,int,int,int) noex ;
extern int ussnls_write(USSNLS *,cchar *,int) noex ;
extern int ussnls_send(USSNLS *,cchar *,int,int) noex ;
extern int ussnls_sendto(USSNLS *,cchar *,int,int,void *,int) noex ;
extern int ussnls_sendmsg(USSNLS *,MSGHDR *,int) noex ;
extern int ussnls_shutdown(USSNLS *,int) noex ;
extern int ussnls_close(USSNLS *) noex ;

EXTERNC_end


#endif /* USSNLS_INCLUDE */


