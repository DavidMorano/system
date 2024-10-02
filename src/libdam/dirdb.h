/* dirdb HEADER */
/* lang=C20 */

/* handle directory list operations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIRDB_INCLUDE
#define	DIRDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/stat.h>
#include	<usystem.h>
#include	<vechand.h>
#include	<hdb.h>


#define	DIRDB_MAGIC	0x33119572
#define	DIRDB_NDEF	30
#define	DIRDB		struct dirdb_head
#define	DIRDB_ENT	struct dirdb_entry
#define	DIRDB_FID	struct dirdb_fider
#define	DIRDB_CUR	struct dirdb_cursor


struct dirdb_fider {
	ino_t		ino ;
	dev_t		dev ;
} ;

struct dirdb_entry {
	cchar		*name ;
	DIRDB_FID	fid ;
	int		count ;
} ;

struct dirdb_head {
	vechand		*dlp ;		/* directory-list-pointer */
	hdb		*dbp ;		/* data-base-pointer */
	uint		magic ;
	int		count ;
} ;

struct dirdb_cursor {
	int		i ;
} ;


typedef	DIRDB		dirdb ;
typedef	DIRDB_ENT	dirdb_ent ;
typedef	DIRDB_FID	dirdb_fid ;
typedef	DIRDB_CUR	dirdb_cur ;

EXTERNC_begin

extern int dirdb_start(dirdb *,int) noex ;
extern int dirdb_add(dirdb *,cchar *,int) noex ;
extern int dirdb_clean(dirdb *) noex ;
extern int dirdb_curbegin(dirdb *,dirdb_cur *) noex ;
extern int dirdb_curenum(dirdb *,dirdb_cur *,dirdb_ent **) noex ;
extern int dirdb_curend(dirdb *,dirdb_cur *) noex ;
extern int dirdb_finish(dirdb *) noex ;

EXTERNC_end


#endif /* DIRDB_INCLUDE */


