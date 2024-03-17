/* mailmsg HEADER */
/* lang=C20 */

/* message parsing object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSG_INCLUDE
#define	MAILMSG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<vecobj.h>
#include	<strpack.h>
#include	<localmisc.h>


#define	MAILMSG_MAGIC		0x97634587
#define	MAILMSG			struct mailmsg_head
#define	MAILMSG_ENV		struct mailmsg_env
#define	MAILMSG_HDR		struct mailmsg_hdr
#define	MAILMSG_MAXLINELEN	MAX((5 * LINEBUFLEN),MAXPATHLEN)


struct mailmsg_env {
	vecobj		insts ;
} ;

struct mailmsg_hdr {
	vecobj		names ;
	int		lastname ;	/* index of last HDR-name */
} ;

struct mailmsg_head {
	strpack		stores ;
	MAILMSG_ENV	envs ;
	MAILMSG_HDR	hdrs ;
	uint		magic ;
	int		msgstate ;
} ;

typedef MAILMSG		mailmsg ;

EXTERNC_begin

extern int mailmsg_start(mailmsg *) noex ;
extern int mailmsg_loadline(mailmsg *,cchar *,int) noex ;
extern int mailmsg_envcount(mailmsg *) noex ;
extern int mailmsg_envaddress(mailmsg *,int,cchar **) noex ;
extern int mailmsg_envdate(mailmsg *,int,cchar **) noex ;
extern int mailmsg_envremote(mailmsg *,int,cchar **) noex ;
extern int mailmsg_hdrcount(mailmsg *,cchar *) noex ;
extern int mailmsg_hdrikey(mailmsg *,int,cchar **) noex ;
extern int mailmsg_hdriline(mailmsg *,cchar *,int,int,cchar **) noex ;
extern int mailmsg_hdrival(mailmsg *,cchar *,int,cchar **) noex ;
extern int mailmsg_hdrval(mailmsg *,cchar *,cchar **) noex ;
extern int mailmsg_finish(mailmsg *) noex ;

EXTERNC_end


#endif /* MAILMSG_INCLUDE */


