/* systems HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get machine dialing information from UUCP "Systems" DB */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSTEMS_INCLUDE
#define	SYSTEMS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */


#define	SYSTEMS		struct systems_head
#define	SYSTEMS_CUR	struct systems_cursor
#define	SYSTEMS_ENT	struct systems_entry
#define	SYSTEMS_MAGIC	31415926


struct systems_cursor {
	int		i ;
} ; /* end struct */

struct systems_entry {
	cchar		*sysname ;
	cchar		*dialername ;
	cchar		*dialerargs ;
	int		fi ;
	int		sysnamelen ;
	int		dialernamelen ;
	int		dialerargslen ;
} ; /* end struct */

struct systems_head {
	vecobj		*flp ;		/* file-list-pointer */
	vecobj		*elp ;		/* entry-list-pointer */
	time_t		checktime ;
	uint		magval ;
} ; /* end struct */

typedef	SYSTEMS		systems ;
typedef	SYSTEMS_CUR	systems_cur ;
typedef	SYSTEMS_ENT	systems_ent ;

EXTERNC_begin

extern int systems_open		(systems *,cchar *) noex ;
extern int systems_fileadd	(systems *,cchar *) noex ;
extern int systems_filedel	(systems *,cchar *) noex ;
extern int systems_check	(systems *,time_t) noex ;
extern int systems_curbegin	(systems *,systems_cur *) noex ;
extern int systems_curend	(systems *,systems_cur *) noex ;
extern int systems_curenum	(systems *,systems_cur *,systems_ent **) noex ;
extern int systems_fetch	(systems *,cchar *,
		systems_cur *,systems_ent **) noex ;
extern int systems_close	(systems *) noex ;

EXTERNC_end


#endif /* SYSTEMS_INCLUDE */


