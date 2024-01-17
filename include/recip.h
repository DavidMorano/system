/* recip HEADER */
/* lang=C20 */

/* recipient object for DMAIL¦DMAILBOX */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RECIP_INCLUDE
#define	RECIP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<vecitem.h>
#include	<localmisc.h>


#define	RECIP_MAGIC	0x73169284
#define	RECIP		struct recip_head
#define	RECIP_FL	struct recip_flags
#define	RECIP_ENT	struct recip_ent


struct recip_ent {
	int		moff ;
	int		mlen ;
} ;

struct recip_flags {
	uint		user:1 ;	/* is an actual user */
} ;

struct recip_head {
	vecitem		*mdp ;		/* message delivery entries */
	cchar		*recipient ;	/* recipient name */
	cchar		*name ;		/* recipient real-name (if known) */
	cchar		*maildname ;	/* recipient maildir */
	RECIP_FL	f ;		/* flags */
	uid_t		uid ;
	uint		magic ;
	int		mbo ;		/* mailbox-message offset */
	int		n ;		/* number of deliveries */
	int		ds ;		/* delivery status */
} ;

typedef RECIP		recip ;

EXTERNC_begin

extern int	recip_start(RECIP *,cchar *,int) noex ;
extern int	recip_get(RECIP *,cchar **) noex ;
extern int	recip_setuser(RECIP *,uid_t) noex ;
extern int	recip_setname(RECIP *,cchar *,int) noex ;
extern int	recip_setmailspool(RECIP *,cchar *,int) noex ;
extern int	recip_mbo(RECIP *,int) noex ;
extern int	recip_ds(RECIP *,int) noex ;
extern int	recip_mo(RECIP *,int,int) noex ;
extern int	recip_match(RECIP *,cchar *,int) noex ;
extern int	recip_getmbo(RECIP *) noex ;
extern int	recip_getmo(RECIP *,int,int *) noex ;
extern int	recip_getname(RECIP *,cchar **) noex ;
extern int	recip_getmailspool(RECIP *,cchar **) noex ;
extern int	recip_getuser(RECIP *,uid_t *) noex ;
extern int	recip_isuser(RECIP *) noex ;
extern int	recip_finish(RECIP *) noex ;

EXTERNC_end


#endif /* RECIP_INCLUDE */


