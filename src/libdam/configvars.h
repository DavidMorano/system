/* configvars HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Configuration-Variables */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONFIGVARS_INCLUDE
#define	CONFIGVARS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* CSTD |time_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |MAXPATHLEN| */


#define	CONFIGVARS		struct configvars_head
#define	CONFIGVARS_VAR		struct configvars_variable
#define	CONFIGVARS_FILE		struct configvars_filer
#define	CONFIGVARS_CUR		struct configvars_cursor
#define	CONFIGVARS_VFL		struct configvars_vflags
#define	CONFIGVARS_MAGIC	0x04311633
#define	CONFIGVARS_NFILES	(szof(int) * 8)
#define	CONFIGVARS_WSETS	0
#define	CONFIGVARS_WVARS	1


struct configvars_cursor {
	int		i ;
} ; /* end struct */

struct configvars_head {
	vecobj		*fesp ;		/* file entries */
	vecobj		*varp ;		/* the user variables */
	vecobj		*defp ;		/* "define" variables */
	vecobj		*setp ;		/* "set" variables */
	vecobj		*expp ;		/* "export" environment variables */
	vecobj		*unvp ;		/* "unset" environment variables */
	time_t		ticheck ;
	uint		magval ;	/* magic number */
} ; /* end struct */

struct configvars_filer {
	ccharp		filename ;
	vecobj		defines ;	/* defined variables */
	vecobj		exports ;	/* environment variables */
	vecobj		unsets ;	/* unset ENV variables */
	time_t		mtime ;
	int		fi ;
} ; /* end struct */

struct configvars_vflags {
	uint		varsubed:1 ;	/* variable substituted */
	uint		expanded:1 ;	/* key expanded */
} ; /* end struct */

struct configvars_variable {
	char		*ma ;		/* memory allocation (for ARGV) */
	char		*key ;
	char		*value ;
	mainv		argv ;
	CONFIGVARS_VFL	fl ;
	int		argc ;
	int		fi ;		/* file index of source */
	int		fmask ;		/* file mask of dependencies */
	int		klen ;
	int		vlen ;
} ; /* end struct */

typedef CONFIGVARS	configvars ;
typedef CONFIGVARS_VAR	configvars_var ;
typedef CONFIGVARS_FILE	configvars_file ;
typedef CONFIGVARS_CUR	configvars_cur ;
typedef CONFIGVARS_VFL	configvars_vfl ;

EXTERNC_begin

int configvars_open(configvars *,cchar *,vecobj *) noex ;
int configvars_addfile(configvars *,cchar *,vecobj *) noex ;
int configvars_check(configvars *,time_t) noex ;
int configvars_curbegin(configvars *,configvars_cur *) noex ;
int configvars_curend(configvars *,configvars_cur *) noex ;
int configvars_fetch(configvars *,cchar *,configvars_cur *,cchar **) noex ;
int configvars_close(configvars *) noex ;

EXTERNC_end


#endif /* CONFIGVARS_INCLUDE */


