/* lkmail HEADER (Lock-Mail) */
/* charset=ISO8859-1 */
/* lang=C20 */


/* revision history:

	= 1998-02-05, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LKMAIL_INCLUDE
#define	LKMAIL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<time.h>		/* CSTD |time_t| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#define	LKMAIL		struct lkmail_head
#define	LKMAIL_IDS	struct lkmail_holdids
#define	LKMAIL_MAGIC	0x95437651


struct lkmail_holdids {
	uid_t		uid, euid, uid_maildir ;
	gid_t		gid, egid, gid_maildir ;
} ; /* end struct */

struct lkmail_head {
	char		*lockfname ;
	LKMAIL_IDS	id ;
	uint		magval ;
	int		lfd ;
} ; /* end struct */

typedef	LKMAIL		lkmail ;
typedef	LKMAIL_IDS	lkmail_ids ;

EXTERNC_begin

extern int lkmail_start(lkmail *,lkmail_ids *,cchar *) noex ;
extern int lkmail_create(lkmail *) noex ;
extern int lkmail_unlink(lkmail *) noex ;
extern int lkmail_old(lkmail *,time_t,int) noex ;
extern int lkmail_finish(lkmail *) noex ;

EXTERNC_end


#endif /* LKMAIL_INCLUDE */


