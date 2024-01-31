/* sdprog HEADER */
/* lang=C20 */

/* SYSDIALER "sdprog" dialer */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-11-04, David A�D� Morano
	This subroutine was originally written.


*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	SDPROG_INCLUDE
#define	SDPROG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<logfile.h>
#include	<localmisc.h>

#include	"sysdialer.h"


#define	SDPROG		struct sdprog_head
#define	SDPROG_FL	struct sdprog_flags
#define	SDPROG_A	SYSDIALER_ARGS


struct sdprog_flags {
	uint		log:1 ;
} ;

struct sdprog_head {
	logfile		lh ;
	pid_t		pid ;
	uint		magic ;
	int		fd ;
	int		tlen ;
	SDPROG_FL	f ;
	char		efname[MAXPATHLEN + 1] ;
} ;

typedef SDPROG		sdprog ;

EXTERNC_begin

extern int sdprog_open(sdprog *,SDPROG_A *,cchar *,cchar *,cchar **) noex ;
extern int sdprog_reade(sdprog *,char *,int,int,int) noex ;
extern int sdprog_recve(sdprog *,char *,int,int,int,int) noex ;
extern int sdprog_recvfrome(sdprog *,char *,int,int,void *,int *,int,int) noex ;
extern int sdprog_recvmsge(sdprog *,MSGHDR *,int,int,int) noex ;
extern int sdprog_write(sdprog *,cchar *,int) noex ;
extern int sdprog_send(sdprog *,cchar *,int,int) noex ;
extern int sdprog_sendto(sdprog *,cchar *,int,int,void *,int) noex ;
extern int sdprog_sendmsg(sdprog *,MSGHDR *,int) noex ;
extern int sdprog_shutdown(sdprog *,int) noex ;
extern int sdprog_close(sdprog *) noex ;

EXTERNC_end


#endif /* SDPROG_INCLUDE */


