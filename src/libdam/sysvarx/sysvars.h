/* sysvars HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* interface to query the system-variable database */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSVARS_INCLUDE
#define	SYSVARS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<var.h>


#define	SYSVARS		struct sysvars_head
#define	SYSVARS_FL	struct sysvars_flags
#define	SYSVARS_CUR	struct sysvars_cursor
#define	SYSVARS_OBJ	struct sysvars_object
#define	SYSVARS_MAGIC	0x99889298
#define	SYSVARS_OPREFIX	(1<<0)		/* prefix match */


struct sysvars_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct sysvars_cursor {
	var_cur		*vcurp ;
} ;

struct sysvars_flags {
	uint		var:1 ;		/* index DB is open */
} ;

struct sysvars_head {
	cvoid		*a ;		/* memory-allocation */
	char		*pr ;
	char		*dbname ;	/* DB name (allocated) */
	var		*vindp ;	/* variable-index-pointer */
	time_t		ti_db ;		/* DB mtime */
	SYSVARS_FL	fl ;
	uint		magic ;
	int		ncursors ;
} ;

typedef SYSVARS		sysvars ;
typedef SYSVARS_FL	sysvars_fl ;
typedef SYSVARS_CUR	sysvars_cur ;
typedef SYSVARS_OBJ	sysvars_obj ;

EXTERNC_begin

extern int sysvars_open(sysvars *,char *,char *) noex ;
extern int sysvars_count(sysvars *) noex ;
extern int sysvars_curbegin(sysvars *,sysvars_cur *) noex ;
extern int sysvars_fetch(sysvars *, char *,int,sysvars_cur *,char *,int) noex ;
extern int sysvars_curenum(sysvars *,sysvars_cur *,char *,int,char *,int) noex ;
extern int sysvars_curend(sysvars *,sysvars_cur *) noex ;
extern int sysvars_audit(sysvars *) noex ;
extern int sysvars_close(sysvars *) noex ;

EXTERNC_end


#endif /* SYSVARS_INCLUDE */


