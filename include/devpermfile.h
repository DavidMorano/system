/* devpermfile HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* object to handle parameter files */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-15, David A­D­ Morano
	This code was started for Levo related work.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEVPERMFILE_INCLUDE
#define	DEVPERMFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	DEVPERMFILE_MAGIC	0x12349887
#define	DEVPERMFILE		struct devpermfile_head
#define	DEVPERMFILE_ENT		struct devpermfile_e
#define	DEVPERMFILE_CUR		struct devpermfile_c
#define	DEVPERMFILE_FL		struct devpermfile_flags

#define	DEVPERMFILE_ELEN (sizeof(struct devpermfile_e)+(2*(MAXPATHLEN+1)))


struct devpermfile_flags {
	uint		dummy:1 ;
} ;

struct devpermfile_head {
	uint		magic ;
	const char	*fname ;
	VECOBJ		keys ;
	VECOBJ		entries ;	/* parameter entries */
	DEVPERMFILE_FL	f ;
	time_t		ti_check ;	/* time last checked */
	time_t		ti_mod ;
	int		fsize ;
	int		intcheck ;	/* check interval (seconds) */
	int		intchange ;	/* file-change interval (seconds) */
	int		ccount ;	/* cursor count */
} ;

struct devpermfile_e {
	const char	*console ;
	const char	*dev ;
	mode_t		devmode ;
	int		devlen ;
} ;

struct devpermfile_c {
	int		i ;
} ;

#ifdef	COMMENT

typedef struct devpermfile_head	devpermfile ;
typedef struct devpermfile_e	devpermfile_ent ;
typedef struct devpermfile_c	devpermfile_cur ;

#endif /* COMMENT */

EXTERNC_begin

extern int devpermfile_open(DEVPERMFILE *,const char *) ;
extern int devpermfile_curbegin(DEVPERMFILE *,DEVPERMFILE_CUR *) ;
extern int devpermfile_curend(DEVPERMFILE *,DEVPERMFILE_CUR *) ;
extern int devpermfile_fetch(DEVPERMFILE *,const char *,DEVPERMFILE_CUR *,
		DEVPERMFILE_ENT *,char *,int) ;
extern int devpermfile_enum(DEVPERMFILE *,DEVPERMFILE_CUR *,
		DEVPERMFILE_ENT *,char *,int) ;
extern int devpermfile_checkint(DEVPERMFILE *,int) ;
extern int devpermfile_check(DEVPERMFILE *,time_t) ;
extern int devpermfile_close(DEVPERMFILE *) ;

EXTERNC_end


#endif /* DEVPERMFILE_INCLUDE */


