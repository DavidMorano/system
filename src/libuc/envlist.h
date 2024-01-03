/* envlist HEADER */
/* lang=C20 */

/* environment container */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ENVLIST_INCLUDE
#define	ENVLIST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<hdb.h>
#include	<localmisc.h>


#define	ENVLIST			struct envlist_head
#define	ENVLIST_CHUNKSIZE	256


struct envlist_head {
	HDB		list ;
	void		*store ;
} ;

typedef ENVLIST		envlist ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int envlist_start(ENVLIST *,int) noex ;
extern int envlist_addkeyval(ENVLIST *,cchar *,cchar *,int) noex ;
extern int envlist_add(ENVLIST *,cchar *,int) noex ;
extern int envlist_present(ENVLIST *,cchar *,int,cchar **) noex ;
extern int envlist_count(ENVLIST *) noex ;
extern int envlist_finish(ENVLIST *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* ENVLIST_INCLUDE */


