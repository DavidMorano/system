/* sysusershells HEADER */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSUSERSHELLS_INCLUDE
#define	SYSUSERSHELLS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>


#define	SYSUSERSHELLS		struct sysusershells_head
#define	SYSUSERSHELLS_MAGIC	0x88776229
#define	SYSUSERSHELLS_FNAME	"/sysdb/shells"


struct sysusershells_head {
	filemap		b ;
	uint		magic ;
} ;

typedef SYSUSERSHELLS	sysusershells ;

EXTERNC_begin

extern int sysusershells_open(SYSUSERSHELLS *,const char *) noex ;
extern int sysusershells_close(SYSUSERSHELLS *) noex ;
extern int sysusershells_readent(SYSUSERSHELLS *,char *,int) noex ;
extern int sysusershells_reset(SYSUSERSHELLS *) noex ;

EXTERNC_end


#endif /* SYSUSERSHELLS_INCLUDE */


