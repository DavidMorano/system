/* sysgroup HEADER */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSGROUP_INCLUDE
#define	SYSGROUP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>
#include	<grp.h>			/* <- money shot */


#define	SYSGROUP		struct sysgroup_head
#define	SYSGROUP_MAGIC		0x88776216
#define	SYSGROUP_FNAME		"/sysdb/group"


struct sysgroup_head {
	uint		magic ;
	filemap		b ;
} ;

typedef struct sysgroup_head	sysgroup ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int sysgroup_open(SYSGROUP *,const char *) noex ;
extern int sysgroup_close(SYSGROUP *) noex ;
extern int sysgroup_readent(SYSGROUP *,struct group *,char *,int) noex ;
extern int sysgroup_reset(SYSGROUP *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSGROUP_INCLUDE */


