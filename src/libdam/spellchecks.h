/* spellchecks HEADER */
/* lang=C20 */

/* SPELLCHECKS object implementation */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	SPELLCHECKS_INCLUDE
#define	SPELLCHECKS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vechand.h>
#include	<bits.h>


#define	SPELLCHECKS_MAGIC	0x99447246
#define	SPELLCHECKS_CONFNAME	"conf"
#define	SPELLCHECKS_SEARCHNAME	"spellchecks"
#define	SPELLCHECKS		struct spellchecks_head
#define	SPELLCHECKS_OBJ		struct spellchecks_object
#define	SPELLCHECKS_CUR		struct spellchecks_cursor
#define	SPELLCHECKS_FL		struct spellchecks_flags


struct spellchecks_obj {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct spellchecks_cursor {
	void		*results ;
	uint		magic ;
	uint		nresults ;
	int		i ;
} ;

struct spellchecks_flags {
	uint		vind:1 ;		/* index is loaded */
} ;

struct spellchecks_head {
	cchar		*pr ;
	cchar		*tmpdname ;
	SPELLCHECKS_FL	f ;
	vechand		lists ;			/* spell-lists */
	uint		magic ;
	int		nlists ;
	int		ncursors ;
} ;

typedef	SPELLCHECKS	spellchecks ;
typedef	SPELLCHECKS_OBJ	spellchecks_obj ;
typedef	SPELLCHECKS_CUR	spellchecks_cur ;
typedef	SPELLCHECKS_FL	spellchecks_fl ;

EXTERNC_begin

extern int spellchecks_start(spellchecks *,cchar *,cchar *) noex ;
extern int spellchecks_count(spellchecks *) noex ;
extern int spellchecks_look(spellchecks *,cchar *,int) noex ;
extern int spellchecks_looks(spellchecks *,BITS *,cchar *,int) noex ;
extern int spellchecks_curbegin(spellchecks *,spellchecks_cur *) noex ;
extern int spellchecks_enum(spellchecks *,spellchecks_cur *,char *,int) noex ;
extern int spellchecks_curend(spellchecks *,spellchecks_cur *) noex ;
extern int spellchecks_check(spellchecks *,time_t) noex ;
extern int spellchecks_audit(spellchecks *) noex ;
extern int spellchecks_finish(spellchecks *) noex ;

EXTERNC_end


#endif /* SPELLCHECKS_INCLUDE */



