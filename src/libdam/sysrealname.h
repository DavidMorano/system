/* sysrealname HEADER */
/* charset=ISO8859-1 */
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
#include	<usystem.h>		/* LIBU */
#include	<modload.h>		/* LIBU */


#define	SYSREALNAME		struct sysrealname_head
#define	SYSREALNAME_FL		struct sysrealname_flags
#define	SYSREALNAME_CUR		struct sysrealname_cursor
#define	SYSREALNAME_INFO	struct sysrealname_information
#define	SYSREALNAME_MAGIC	0x88776216
#define	SYSREALNAME_CURMAGIC	0x88776217
#define	SYSREALNAME_PR		"/usr/extra"
#define	SYSREALNAME_DBNAME	"/sysdb/realname"


struct sysrealname_flags {
    	uint		modload:1 ;
} ; /* end struct */

struct sysrealname_cursor {
	void		*scp ;
	cchar		**sa ;
	uint		magval ;
	int		sn ;
	int		fo ;		/* options */
} ; /* end struct */

struct sysrealname_information {
	time_t		writetime ;	/* time DB written */
	uint		writecount ;	/* write counter */
	uint		entries ;	/* total number of entries */
	uint		version ;
	uint		encoding ;
	uint		type ;
	uint		collisions ;
} ; /* end struct */

struct sysrealname_head {
	modload		*mlp ;
	void		*callp ;
	void		*obj ;		/* object pointer */
	SYSREALNAME_FL	fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef SYSREALNAME		sysrealname ;
typedef	SYSREALNAME_FL		sysrealname_fl ;
typedef	SYSREALNAME_CUR		sysrealname_cur ;
typedef	SYSREALNAME_INFO	sysrealname_info ;

EXTERNC_begin

extern int sysrealname_open(sysrealname *,const char *) noex ;
extern int sysrealname_getinfo(sysrealname *,sysrealname_info *) noex ;
extern int sysrealname_curbegin(sysrealname *,sysrealname_cur *) noex ;
extern int sysrealname_curend(sysrealname *,sysrealname_cur *) noex ;
extern int sysrealname_curlook(sysrealname *,sysrealname_cur *,int,
		cchar *,int) noex ;
extern int sysrealname_curlookparts(sysrealname *,sysrealname_cur *,int,
		cchar **,int) noex ;
extern int sysrealname_curlookread(sysrealname *,sysrealname_cur *,
		char *) noex ;
extern int sysrealname_curenum(sysrealname *,sysrealname_cur *,char *,
		cchar **,char *,int) noex ;
extern int sysrealname_audit(sysrealname *) noex ;
extern int sysrealname_close(sysrealname *) noex ;

EXTERNC_end


#endif /* SYSREALNAME_INCLUDE */


