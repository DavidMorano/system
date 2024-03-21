/* sysvars HEADER */
/* lang=C20 */

/* interface to query the system-variable database */
/* version %I% last-modified %G% */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	SYSVARS_INCLUDE
#define	SYSVARS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"var.h"


#define	SYSVARS		struct sysvars_head
#define	SYSVARS_CUR	struct sysvars_cursor
#define	SYSVARS_OBJ	struct sysvars_object
#define	SYSVARS_FL	struct sysvars_flags
#define	SYSVARS_MAGIC	0x99889298
#define	SYSVARS_OPREFIX	(1<<0)		/* prefix match */


struct sysvars_object {
	char		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct sysvars_cursor {
	VAR_CUR		vcur ;
} ;

struct sysvars_flags {
	uint		var:1 ;		/* index DB is open */
} ;

struct sysvars_head {
	cvoid		*a ;		/* allocation */
	char		*pr ;
	char		*dbname ;	/* DB name (allocated) */
	VAR		vind ;		/* variable index */
	time_t		ti_db ;		/* DB mtime */
	SYSVARS_FL	f ;
	uint		magic ;
	int		ncursors ;
} ;

typedef SYSVARS		sysvars ;
typedef SYSVARS_CUR	sysvars_cur ;
typedef SYSVARS_OBJ	sysvars_obj ;
typedef SYSVARS_FL	sysvars_fl ;

EXTERNC_begin

extern int sysvars_open(sysvars *,char *,char *) noex ;
extern int sysvars_count(sysvars *) noex ;
extern int sysvars_curbegin(sysvars *,sysvars_cur *) noex ;
extern int sysvars_fetch(sysvars *, char *,int,sysvars_cur *,char *,int) noex ;
extern int sysvars_enum(sysvars *,sysvars_cur *,char *,int,char *,int) noex ;
extern int sysvars_curend(sysvars *,sysvars_cur *) noex ;
extern int sysvars_audit(sysvars *) noex ;
extern int sysvars_close(sysvars *) noex ;

EXTERNC_end


#endif /* SYSVARS_INCLUDE */

