/* sysusernames */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSUSERNAMES_INCLUDE
#define	SYSUSERNAMES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<filemap.h>
#include	<localmisc.h>
#include	<pwd.h>			/* <- money shot */


#define	SYSUSERNAMES		struct sysusernames_head
#define	SYSUSERNAMES_MAGIC	0x88776218
#define	SYSUSERNAMES_FNAME	"/sysdb/usernames"


struct sysusernames_head {
	uint		magic ;
	filemap		b ;
} ;

typedef struct sysusernames_head	sysusernames ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int sysusernames_open(SYSUSERNAMES *,const char *) noex ;
extern int sysusernames_close(SYSUSERNAMES *) noex ;
extern int sysusernames_readent(SYSUSERNAMES *,char *,int) noex ;
extern int sysusernames_reset(SYSUSERNAMES *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSUSERNAMES_INCLUDE */


