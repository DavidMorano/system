/* sysrealname */
/* lang=C20 */

/* thread-safe enumeration of a system data-base */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSREALNAME_INCLUDE
#define	SYSREALNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<modload.h>
#include	<localmisc.h>
#include	<ipasswd.h>		/* <- money shot */


#define	SYSREALNAME		struct sysrealname_head
#define	SYSREALNAME_CUR		struct sysrealname_c
#define	SYSREALNAME_INFO	struct sysrealname_i
#define	SYSREALNAME_CA		struct sysrealname_calls
#define	SYSREALNAME_MAGIC	0x88776216
#define	SYSREALNAME_CURMAGIC	0x88776217
#define	SYSREALNAME_PR		"/usr/extra"
#define	SYSREALNAME_DBNAME	"/sysdb/realname"


struct sysrealname_c {
	uint		magic ;
	void		*scp ;
	const char	**sa ;
	int		sn ;
	int		fo ;		/* options */
} ;

struct sysrealname_i {
	time_t		writetime ;	/* time DB written */
	uint		writecount ;	/* write counter */
	uint		entries ;	/* total number of entries */
	uint		version ;
	uint		encoding ;
	uint		type ;
	uint		collisions ;
} ;

struct sysrealname_calls {
	int (*open)(void *,const char *) noex ;
	int (*info)(void *,IPASSWD_INFO *) noex ;
	int (*curbegin)(void *,IPASSWD_CUR *) noex ;
	int (*curend)(void *,IPASSWD_CUR *) noex ;
	int (*enumerate)(void *,IPASSWD_CUR *,char *,cchar **,char *,int) noex ;
	int (*fetcher)(void *,IPASSWD_CUR *,int,char *,cchar **,int) noex ;
	int (*audit)(void *) noex ;
	int (*close)(void *) noex ;
} ;

struct sysrealname_head {
	uint		magic ;
	MODLOAD		loader ;
	SYSREALNAME_CA	call ;
	void		*obj ;		/* object pointer */
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;

typedef struct sysrealname_head	sysrealname ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int sysrealname_open(SYSREALNAME *,const char *) noex ;
extern int sysrealname_curbegin(SYSREALNAME *,SYSREALNAME_CUR *) noex ;
extern int sysrealname_curend(SYSREALNAME *,SYSREALNAME_CUR *) noex ;
extern int sysrealname_look(SYSREALNAME *,SYSREALNAME_CUR *,int,
		cchar *,int) noex ;
extern int sysrealname_lookparts(SYSREALNAME *,SYSREALNAME_CUR *,int,
		cchar **,int) noex ;
extern int sysrealname_lookread(SYSREALNAME *,SYSREALNAME_CUR *,char *) noex ;
extern int sysrealname_enum(SYSREALNAME *,SYSREALNAME_CUR *,char *,
		cchar **,char *,int) noex ;
extern int sysrealname_audit(SYSREALNAME *) noex ;
extern int sysrealname_close(SYSREALNAME *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SYSREALNAME_INCLUDE */


