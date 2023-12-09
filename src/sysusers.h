/* sysusers */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSUSERS_INCLUDE
#define	SYSUSERS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>
#include	<pwd.h>			/* <- money shot */


#define	SYSUSERS		struct sysusers_head
#define	SYSUSERS_MAGIC		0x88776217
#define	SYSUSERS_FNAME		"/sysdb/users"


struct sysusers_head {
	uint		magic ;
	filemap		b ;
} ;

typedef struct sysusers_head	sysusers ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int sysusers_open(SYSUSERS *,const char *) noex ;
extern int sysusers_close(SYSUSERS *) noex ;
extern int sysusers_readent(SYSUSERS *,struct passwd *,char *,int) noex ;
extern int sysusers_reset(SYSUSERS *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSUSERS_INCLUDE */


