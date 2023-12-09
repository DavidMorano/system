/* envs */
/* lang=C20 */

/* environment-variable list container */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ENVS_INCLUDE
#define	ENVS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<hdb.h>
#include	<vecstr.h>
#include	<localmisc.h>


#define	ENVS		struct envs_head
#define	ENVS_CUR	struct envs_cursor
#define	ENVS_MAGIC	0x44376247


struct envs_cursor {
	hdb_cur		cur ;
	int		i ;
} ;

struct envs_head {
	uint		magic ;
	hdb		vars ;
} ;

typedef ENVS		envs ;
typedef ENVS_CUR	envs_cur ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int envs_start(envs *,int) noex ;
extern int envs_store(envs *,cchar *,int,cchar *,int) noex ;
extern int envs_present(envs *,cchar *,int) noex ;
extern int envs_substr(envs *,cchar *,int,cchar *,int) noex ;
extern int envs_curbegin(envs *,envs_cur *) noex ;
extern int envs_curend(envs *,envs_cur *) noex ;
extern int envs_enumkey(envs *,envs_cur *,cchar **) noex ;
extern int envs_enum(envs *,envs_cur *,cchar **,cchar **) noex ;
extern int envs_fetch(envs *,cchar *,int,envs_cur *,cchar **) noex ;
extern int envs_delname(envs *,cchar *,int) noex ;
extern int envs_count(envs *) noex ;
extern int envs_finish(envs *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* ENVS_INCLUDE */


