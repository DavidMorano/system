/* sysprojects */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSPROJECTS_INCLUDE
#define	SYSPROJECTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>
#include	<project.h>		/* <- money shot */


#define	SYSPROJECTS		struct sysprojects_head
#define	SYSPROJECTS_MAGIC	0x88776216
#define	SYSPROJECTS_FNAME	"/sysdb/projects"


struct sysprojects_head {
	uint		magic ;
	filemap		b ;
} ;

typedef struct sysprojects_head	sysprojects ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int sysprojects_open(SYSPROJECTS *,const char *) noex ;
extern int sysprojects_close(SYSPROJECTS *) noex ;
extern int sysprojects_readent(SYSPROJECTS *,struct project *,char *,int) noex ;
extern int sysprojects_reset(SYSPROJECTS *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSPROJECTS_INCLUDE */


