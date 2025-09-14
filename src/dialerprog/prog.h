/* prog HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 2003-11-04, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROG_INCLUDE
#define	PROG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<logfile.h>
#include	<sysdialer.h>


#define	PROG		struct prog_head


struct prog_flags {
	uint		log:1 ;
} ;

struct prog_head {
	logfile		lh ;
	pid_t		pid ;
	PROG_FL		fl ;
	uint		magic ;
	int		fd ;
	int		tlen ;
	char		efname[MAXPATHLEN + 1] ;
} ;

EXTERNC_begin

extern int prog_open(PROG *,SYSDIALER_ARGS *,cc *,cc *,cc **) noex ;
extern int prog_reade(PROG *,char *,int,int,int) noex ;
extern int prog_recve(PROG *,char *,int,int,int,int) noex ;
extern int prog_recvfrome(PROG *,char *,int,int,void *,int *,int,int) noex ;
extern int prog_recvmsge(PROG *,MSGHDR *,int,int,int) noex ;
extern int prog_write(PROG *,cchar *,int) noex ;
extern int prog_send(PROG *,cchar *,int,int) noex ;
extern int prog_sendto(PROG *,cchar *,int,int,void *,int) noex ;
extern int prog_sendmsg(PROG *,MSGHDR *,int) noex ;
extern int prog_shutdown(PROG *,int) noex ;
extern int prog_close(PROG *) noex ;

EXTERNC_end


#endif /* PROG_INCLUDE */


