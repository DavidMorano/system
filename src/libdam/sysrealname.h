/* sysrealname HEADER */
/* encoding=ISO8859-1 */
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
#include	<usystem.h>
#include	<modload.h>
#include	<ipasswd.h>		/* <- money shot */


#define	SYSREALNAME		struct sysrealname_head
#define	SYSREALNAME_FL		struct sysrealname_flags
#define	SYSREALNAME_CUR		struct sysrealname_cursor
#define	SYSREALNAME_INFO	struct sysrealname_information
#define	SYSREALNAME_CA		struct sysrealname_calls
#define	SYSREALNAME_MAGIC	0x88776216
#define	SYSREALNAME_CURMAGIC	0x88776217
#define	SYSREALNAME_PR		"/usr/extra"
#define	SYSREALNAME_DBNAME	"/sysdb/realname"


struct sysrealname_flags {
    	uint		modload:1 ;
} ;

struct sysrealname_cursor {
	void		*scp ;
	cchar		**sa ;
	uint		magic ;
	int		sn ;
	int		fo ;		/* options */
} ;

struct sysrealname_information {
	time_t		writetime ;	/* time DB written */
	uint		writecount ;	/* write counter */
	uint		entries ;	/* total number of entries */
	uint		version ;
	uint		encoding ;
	uint		type ;
	uint		collisions ;
} ;

struct sysrealname_head {
	modload		*mlp ;
	void		*callp ;
	void		*obj ;		/* object pointer */
	SYSREALNAME_FL	fl ;
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;

typedef SYSREALNAME	sysrealname ;

EXTERNC_begin

extern int sysrealname_open(sysrealname *,const char *) noex ;
extern int sysrealname_curbegin(sysrealname *,sysrealname_cur *) noex ;
extern int sysrealname_curend(sysrealname *,sysrealname_cur *) noex ;
extern int sysrealname_look(sysrealname *,sysrealname_cur *,int,
		cchar *,int) noex ;
extern int sysrealname_lookparts(sysrealname *,sysrealname_cur *,int,
		cchar **,int) noex ;
extern int sysrealname_lookread(sysrealname *,sysrealname_cur *,char *) noex ;
extern int sysrealname_enum(sysrealname *,sysrealname_cur *,char *,
		cchar **,char *,int) noex ;
extern int sysrealname_audit(sysrealname *) noex ;
extern int sysrealname_close(sysrealname *) noex ;

EXTERNC_end


#endif /* SYSREALNAME_INCLUDE */


