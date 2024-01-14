/* sysusers HEADER */
/* lang=C20 */

/* system user-entry enumeration */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSUSERS_INCLUDE
#define	SYSUSERS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>
#include	<ucpasswd.h>


#define	SYSUSERS		struct sysusers_head
#define	SYSUSERS_MAGIC		0x88776217
#define	SYSUSERS_FNAME		"/sys/users"


struct sysusers_head {
	filemap		b ;
	uint		magic ;
} ;

typedef SYSUSERS	sysusers ;
typedef PASSWD		sysusers_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int sysusers_open(sysusers *,const char *) noex ;
extern int sysusers_close(sysusers *) noex ;
extern int sysusers_readent(sysusers *,sysusers_ent *,char *,int) noex ;
extern int sysusers_reset(sysusers *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSUSERS_INCLUDE */


