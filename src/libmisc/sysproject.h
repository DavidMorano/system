/* sysproject HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSPROJECT_INCLUDE
#define	SYSPROJECT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<project.h>		/* <- money shot */


#define	SYSPROJECT		struct sysproject_head
#define	SYSPROJECT_MAGIC	0x88776216
#define	SYSPROJECT_FNAME	"/sysdb/projects"


struct sysproject_head {
	filemap		b ;
	uint		magic ;
} ;

typedef SYSPROJECT	sysproject ;

EXTERNC_begin

extern int sysproject_open(sysproject *,const char *) noex ;
extern int sysproject_close(sysproject *) noex ;
extern int sysproject_readent(sysproject *,PROJECT *,char *,int) noex ;
extern int sysproject_reset(sysproject *) noex ;

EXTERNC_end


#endif /* SYSPROJECT_INCLUDE */


