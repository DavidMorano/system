/* zdb HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* time-zone database management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ZDB_INCLUDE
#define	ZDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


#define	ZDB		struct zdb_head


struct zdb_head {
	cchar		*name ;		/* static-data (not allocated) */
	short		off ;		/* minutes west of GMT */
	short		isdst ;
} ;

typedef ZDB		zdb ;

EXTERNC_begin

extern int	zdb_nameoff(zdb *,cchar *,int,int) noex ;
extern int	zdb_name(zdb *,cchar *,int) noex ;
extern int	zdb_off(zdb *,int) noex ;
extern int	zdb_offisdst(zdb *,int,int) noex ;
extern int	zdb_count(zdb *) noex ;

EXTERNC_end


#endif /* ZDB_INCLUDE */


