/* pcsconfs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface to query the PCS configuration-variable database */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-07, David A­D­ Morano
	This was created to allow for the main part of the PCSCONF
	facility to be a loadable module.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSCONFS_INCLUDE
#define	PCSCONFS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	"var.h"


#define	PCSCONFS		struct pcsconfs_head
#define	PCSCONFS_CUR		struct pcsconfs_cursor
#define	PCSCONFS_OBJ		struct pcsconfs_object
#define	PCSCONFS_MAGIC		0x99889298
#define	PCSCONFS_CURMAGIC	0x99889299


struct pcsconfs_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct pcsconfs_cursor {
	uint		magic ;
	var_cur		vcur ;
} ;

struct pcsconfs_flags {
	uint		prdb:1 ;		/* CONF is global */
	uint		db:1 ;			/* DB is open */
} ;

struct pcsconfs_head {
	cchar		**envv ;
	cchar		*pr ;			/* program-root */
	cchar		*cfname ;		/* DB database name */
	cchar		*a ;			/* memory allocation */
	var		db ;
	time_t		ti_conf ;		/* DB mtime */
	PCSCONFS_FL	fl ;
	uint		magic ;
	int		ncursors ;
} ;

typedef	PCSCONFS	pcsconfs ;
typedef	PCSCONFS_CUR	pcsconfs_cur ;
typedef	PCSCONFS_OBJ	pcsconfs_obj ;

EXTERNC_begin

extern int pcsconfs_start(pcsconfs *,cchar *,cchar **,cchar *) noex ;
extern int pcsconfs_curbegin(pcsconfs *,pcsconfs_cur *) noex ;
extern int pcsconfs_fetch(pcsconfs *, cchar *,int,pcsconfs_cur *,
				char *,int) noex ;
extern int pcsconfs_enum(pcsconfs *,pcsconfs_cur *,char *,int,char *,int) noex ;
extern int pcsconfs_curend(pcsconfs *,pcsconfs_cur *) noex ;
extern int pcsconfs_audit(pcsconfs *) noex ;
extern int pcsconfs_finish(pcsconfs *) noex ;

EXTERNC_end


#endif /* PCSCONFS_INCLUDE */



