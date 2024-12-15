/* cm HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* Connection-Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CM_INCLUDE
#define	CM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>		/* |MAXNAMELEN| */

#include	<systems.h>
#include	<sysdialer.h>


#define	CM_MAGIC	31815926
#define	CM		struct cm_head
#define	CM_ARGS		struct cm_arguments
#define	CM_INFO		struct cm_information


struct cm_arguments {
	cchar		*searchname ;
	cchar		*pr ;		/* program-root */
	cchar		*prn ;		/* program root-name */
	cchar		*nodename ;
	cchar		*domainname ;
	cchar		*hostname ;
	cchar		*username ;
	systems		*sp ;
	sysdialer	*dp ;
	int		timeout ;	/* connection timeout */
	int		options ;
} ;

struct cm_information {
	int		itype ;
	int		dflags ;
	char		dname[MAXNAMELEN + 1] ;	/* "sysdialer name" */
} ;

struct cm_head {
	void		*addr ;		/* remote address */
	void		*dobj ;		/* individual sysdialer object */
	cchar		*dname ;	/* sysdialer name */
	CM_ARGS		*ap ;
	sysdialer_ca	c ;
	uint		magic ;
	int		dsize ;		/* individual sysdialer object size */
	int		dflags ;	/* sysdialer flags */
	int		itype ;
	int		fd ;
	int		addrlen ;
} ;

typedef	CM		cm ;
typedef	CM_ARGS		cm_args ;
typedef	CM_INFO		cm_info ;

EXTERNC_begin

extern int cm_open(cm *,cmd_args *,cchar *,cchar *,mainv) noex ;
extern int cm_getinfo(cm *,cm_info *) noex ;
extern int cm_reade(cm *,char *,int,int,int) noex ;
extern int cm_recve(cm *,char *,int,int,int,int) noex ;
extern int cm_recvfrome(cm *,char *,int,int,void *,int *,int,int) noex ;
extern int cm_recvmsge(cm *,MSGHDR *,int,int,int) noex ;
extern int cm_write(cm *,cchar *,int) noex ;
extern int cm_send(cm *,cchar *,int,int) noex ;
extern int cm_sendto(cm *,cchar *,int,int,void *,int) noex ;
extern int cm_sendmsg(cm *,MSGHDR *,int) noex ;
extern int cm_poll(cm *,int) noex ;
extern int cm_localname(cm *,char *,int) noex ;
extern int cm_peername(cm *,char *,int) noex ;
extern int cm_shutdown(cm *,int) noex ;
extern int cm_close(cm *) noex ;

EXTERNC_end


#endif /* CM_INCLUDE */


