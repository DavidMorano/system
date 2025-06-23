/* devpermfile HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

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
#define	DEVPERMFILE_FL		struct devpermfile_flags
#define	DEVPERMFILE_ENT		struct devpermfile_entry
#define	DEVPERMFILE_CUR		struct devpermfile_cursor

#define	DEVPERMFILE_ELEN (sizeof(struct devpermfile_e)+(2*(MAXPATHLEN+1)))


struct devpermfile_flags {
	uint		dummy:1 ;
} ;

struct devpermfile_head {
	cchar		*fname ;
	vecobj		keys ;
	vecobj		entries ;	/* parameter entries */
	DEVPERMFILE_FL	f ;
	time_t		ti_check ;	/* time last checked */
	time_t		ti_mod ;
	uint		magic ;
	int		fsize ;
	int		intcheck ;	/* check interval (seconds) */
	int		intchange ;	/* file-change interval (seconds) */
	int		ccount ;	/* cursor count */
} ;

struct devpermfile_entry {
	cchar		*console ;
	cchar		*dev ;
	mode_t		devmode ;
	int		devlen ;
} ;

struct devpermfile_cursor {
	int		i ;
} ;

typedef	DEVPERMFILE		devpermfile ;
typedef	DEVPERMFILE_FL		devpermfile_fl ;
typedef	DEVPERMFILE_ENT		devpermfile_ent ;
typedef	DEVPERMFILE_CUR		devpermfile_cur ;

EXTERNC_begin

extern int devpermfile_open(devpermfile *,cchar *) noex ;
extern int devpermfile_curbegin(devpermfile *,devpermfile_cur *) noex ;
extern int devpermfile_curend(devpermfile *,devpermfile_cur *) noex ;
extern int devpermfile_fetch(devpermfile *,cchar *,devpermfile_cur *,
		devpermfile_ent *,char *,int) noex ;
extern int devpermfile_enum(devpermfile *,devpermfile_cur *,
		devpermfile_ent *,char *,int) noex ;
extern int devpermfile_checkint(devpermfile *,int) noex ;
extern int devpermfile_check(devpermfile *,time_t) noex ;
extern int devpermfile_close(devpermfile *) noex ;

EXTERNC_end


#endif /* DEVPERMFILE_INCLUDE */


