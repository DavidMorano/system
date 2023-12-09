/* sysgroups INCLUDE */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSGROUPS_INCLUDE
#define	SYSGROUPS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>
#include	<grp.h>			/* <- money shot */


#define	SYSGROUPS		struct sysgroups_head
#define	SYSGROUPS_MAGIC		0x88776215
#define	SYSGROUPS_FNAME		"/sysdb/groups"


struct sysgroups_head {
	uint		magic ;
	filemap		b ;
} ;

typedef struct sysgroups_head	sysgroups ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int sysgroups_open(SYSGROUPS *,const char *) noex ;
extern int sysgroups_close(SYSGROUPS *) noex ;
extern int sysgroups_readent(SYSGROUPS *,struct group *,char *,int) noex ;
extern int sysgroups_reset(SYSGROUPS *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSGROUPS_INCLUDE */


