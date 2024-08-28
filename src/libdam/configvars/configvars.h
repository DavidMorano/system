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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<vecobj.h>
#include	<localmisc.h>		/* |MAXPATHLEN| */


#define	CONFIGVARS_MAGIC	0x04311633
#define	CONFIGVARS		struct configvars_head
#define	CONFIGVARS_ERR		struct configvars_errline
#define	CONFIGVARS_VAR		struct configvars_variable
#define	CONFIGVARS_FILE		struct configvars_filer
#define	CONFIGVARS_CUR		struct configvars_cursor
#define	CONFIGVARS_VFL		struct configvars_vflags
#define	CONFIGVARS_NFILES	(sizeof(int) * 8)

#define	CV		configvars
#define	CV_VAR		configvars_var
#define	CV_FILE		configvars_file
#define	CV_CUR		configvars_cur
#define	CV_ERR		configvars_err


struct paramfile_cursor {
	int		i ;
} ;

struct configvars_head {
	vecobj		*fesp ;		/* file entries */
	vecobj		*defp ;		/* collapsed defined variables */
	vecobj		*unvp ;		/* unset ENV variables */
	vecobj		*expp ;		/* collapsed environment variables */
	vecobj		*setp ;		/* "set" variables */
	vecobj		*varp ;		/* the user variables */
	time_t		checktime ;
	uint		magic ;		/* magic number */
} ;

struct configvars_filer {
	cchar		*filename ;
	vecobj		defines ;	/* defined variables */
	vecobj		unsets ;	/* unset ENV variables */
	vecobj		exports ;	/* environment variables */
	time_t		mtime ;
	int		fi ;
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
	int		klen ;
	int		vlen ;
} ;

typedef CONFIGVARS	configvars ;
typedef CONFIGVARS_VAR	configvars_var ;
typedef CONFIGVARS_FILE	configvars_file ;
typedef CONFIGVARS_CUR	configvars_cur ;
typedef CONFIGVARS_ERR	configvars_err ;

EXTERNC_begin

int configvars_open(configvars *,cchar *,vecobj *) noex ;
int configvars_addfile(configvars *,cchar *,vecobj *) noex ;
int configvars_check(configvars *,time_t) noex ;
int configvars_curbegin(configvars *,configvars_cur *) noex ;
int configvars_curend(configvars *,configvars_cur *) noex ;
int configvars_fetch(configvars *,cchar *,configvars_cur *,cchar **) noex ;
int configvars_close(configvars *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
static inline int configvars_magic(configvars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CONFIGVARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (configvars_magic) */

namespace configvars_obj {
    extern int configvars_parse(CV *,int,vecobj *) noex ;
    extern int configvars_finvars(CV *) noex ;
    extern int configvars_finfiles(CV *) noex ;
    extern int configvars_addvar(CV *,int,int,char *,int,char *,int) noex ;
}

namespace configvars_obj {
    extern int file_start(CV_FILE *,cchar *) noex ;
    extern int file_addvar(CV_FILE *,int,int,char *,int,char *,int) noex ;
    extern int file_finish(CV_FILE *) noex ;
    extern int file_finone(CV_FILE *,int) noex ;
}

namespace configvars_obj {
    extern int var_start(CV_VAR *,int,char *,int,char *,int) noex ;
    extern int var_finish(CV_VAR *) noex ;
}

namespace configvars_obj {
    extern void	badline(vecobj *,char *,int) noex ;
    extern void	freeit(char **) noex ;
}

#endif /* __cplusplus */


#endif /* CONFIGVARS_INCLUDE */


