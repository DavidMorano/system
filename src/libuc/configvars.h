/* configvars HEADER */
/* lang=C++20 */

/* Configuration-Variables */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONFIGVARS_INCLUDE
#define	CONFIGVARS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<time.h>
#include	<vecitem.h>
#include	<localmisc.h>


#define	CONFIGVARS_MAGIC	0x04311633
#define	CONFIGVARS		struct configvars_head
#define	CONFIGVARS_ERR		struct configvars_errline
#define	CONFIGVARS_VAR		struct configvars_variable
#define	CONFIGVARS_FILE		struct configvars_filer
#define	CONFIGVARS_CUR		struct configvars_cursor
#define	CONFIGVARS_VFL		struct configvars_vflags

#define	CONFIGVARS_NFILES	(sizeof(int) * 8)


struct paramfile_cursor {
	int		i ;
} ;

struct configvars_head {
	vecitem		fes ;		/* file entries */
	vecitem		defines ;	/* collapsed defined variables */
	vecitem		unsets ;	/* unset ENV variables */
	vecitem		exports ;	/* collapsed environment variables */
	vecitem		sets ;		/* "set" variables */
	vecitem		vars ;		/* the user's variables */
	time_t		checktime ;
	uint		magic ;		/* magic number */
} ;

struct configvars_filer {
	cchar		*filename ;
	vecitem		defines ;	/* defined variables */
	vecitem		unsets ;	/* unset ENV variables */
	vecitem		exports ;	/* environment variables */
	time_t		mtime ;
} ;

struct configvars_errline {
	int		line ;
	char		filename[MAXPATHLEN + 1] ;
} ;

struct configvars_vflags {
	uint		varsubed:1 ;	/* variable substituted */
	uint		expanded:1 ;	/* key expanded */
} ;

struct configvars_variable {
	char		*ma ;		/* memory allocation (for ARGV) */
	char		*key ;
	char		*value ;
	mainv		argv ;
	CONFIGVARS_VFL	f ;
	int		argc ;
	int		fi ;		/* file index of source */
	int		fmask ;		/* file mask of dependencies */
	int		klen, vlen ;
} ;

typedef CONFIGVARS	configvars ;
typedef CONFIGVRS_VAR	configvars_var ;
typedef CONFIGVARS_FILE	configvars_file

#ifdef	__cplusplus
extern "C" {
#endif

int configvars_open(configvars *,cchar *,vecitem *) noex ;
int configvars_addfile(configvars *,cchar *,vecitem *) noex ;
int configvars_check(configvars *,time_t) noex ;
int configvars_curbegin(configvars *,configvars_cur *) noex ;
int configvars_curend(configvars *,configvars_cur *) noex ;
int configvars_fetch(configvars *,cchar *,configvars_cur *,cchar **) noex ;
int configvars_close(configvars *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* CONFIGVARS_INCLUDE */


