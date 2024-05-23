/* systems HEADER */
/* lang=C20 */

/* get machine dialing information from UUCP "Systems" DB */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSTEMS_INCLUDE
#define	SYSTEMS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<vecobj.h>


#define	SYSTEMS_CUR	struct systems_c
#define	SYSTEMS_ENT	struct systems_ent
#define	SYSTEMS		struct systems_head
#define	SYSTEMS_MAGIC	31415926


struct systems_c {
	int		i ;
} ;

struct systems_ent {
	cchar		*sysname ;
	cchar		*dialername ;
	cchar		*dialerargs ;
	int		fi ;
	int		sysnamelen ;
	int		dialernamelen ;
	int		dialerargslen ;
} ;

struct systems_head {
	vecobj		files ;
	vecobj		entries ;
	time_t		checktime ;
	uint		magic ;
} ;

EXTERNC_begin

extern int	systems_open(SYSTEMS *,cchar *) noex ;
extern int	systems_fileadd(SYSTEMS *,cchar *) noex ;
extern int	systems_filedel(SYSTEMS *,cchar *) noex ;
extern int	systems_check(SYSTEMS *,time_t) noex ;
extern int	systems_curbegin(SYSTEMS *,SYSTEMS_CUR *) noex ;
extern int	systems_curend(SYSTEMS *,SYSTEMS_CUR *) noex ;
extern int	systems_enum(SYSTEMS *,SYSTEMS_CUR *,SYSTEMS_ENT **) noex ;
extern int	systems_fetch(SYSTEMS *,cchar *,SYSTEMS_CUR *,
			SYSTEMS_ENT **) noex ;
extern int	systems_close(SYSTEMS *) noex ;

EXTERNC_end


#endif /* SYSTEMS_INCLUDE */


