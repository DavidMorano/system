/* sysusernames */
/* lang=C20 */

/* system user-name enumeration */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSUSERNAMES_INCLUDE
#define	SYSUSERNAMES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<filemap.h>
#include	<localmisc.h>


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

extern int sysusernames_open(sysusernames *,cchar *) noex ;
extern int sysusernames_close(sysusernames *) noex ;
extern int sysusernames_readent(sysusernames *,char *,int) noex ;
extern int sysusernames_reset(sysusernames *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSUSERNAMES_INCLUDE */


