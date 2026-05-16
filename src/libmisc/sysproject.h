/* sysproject HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSPROJECT_INCLUDE
#define	SYSPROJECT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<filemap.h>


#define	SYSPROJECT		struct sysproject_head
#define	SYSPROJECT_MAGIC	0x88776216
#define	SYSPROJECT_FNAME	"/sysdb/project"


struct sysproject_head {
	filemap		*fmp ;
	uint		magval ;
} ; /* end struct */

typedef SYSPROJECT	sysproject ;

EXTERNC_begin

extern int sysproject_open	(sysproject *,cchar *) noex ;
extern int sysproject_close	(sysproject *) noex ;
extern int sysproject_readent	(sysproject *,PROJECT *,char *,int) noex ;
extern int sysproject_reset	(sysproject *) noex ;

EXTERNC_end


#endif /* SYSPROJECT_INCLUDE */


