/* uss HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* SYSDIALER "uss" dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-04, David A­D­ Morano
	This was created as one of the first dialer modules for the
	SYSDIALER object.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	USS_INCLUDE
#define	USS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<logfile.h>
#include	<sysdialer.h>


#define	USS_MAGIC	31415926
#define	USS		struct uss_head
#define	USS_FL		struct uss_flags


struct uss_flags {
	uint		log:1 ;
} ; /* end struct */

struct uss_head {
	logfile		lh ;
	USS_FL		open ;
	uint		magic ;
	int		tlen ;
	int		fd ;
} ; /* end struct */

EXTERNC_begin

extern int uss_open(USS *,SYSDIALER_ARGS *,
		cchar *,cchar *,cchar **) const ;
extern int uss_reade(USS *,char *,int,int,int) const ;
extern int uss_recve(USS *,char *,int,int,int,int) const ;
extern int uss_recvfrome(USS *,char *,int,int,void *,int *,int,int) const ;
extern int uss_recvmsge(USS *,MSGHDR *,int,int,int) const ;
extern int uss_write(USS *,cchar *,int) const ;
extern int uss_send(USS *,cchar *,int,int) const ;
extern int uss_sendto(USS *,cchar *,int,int,void *,int) const ;
extern int uss_sendmsg(USS *,MSGHDR *,int) const ;
extern int uss_shutdown(USS *,int) const ;
extern int uss_close(USS *) const ;

EXTERNC_end


#endif /* USS_INCLUDE */


