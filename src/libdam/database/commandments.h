/* commandments HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* COMMANDMENTS object implementation */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-03-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	COMMANDMENTS_INCLUDE
#define	COMMANDMENTS_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<cmi.h>			/* LIBU */


#define	COMMANDMENTS		struct commandments_head
#define	COMMANDMENTS_INFO	struct commandments_information
#define	COMMANDMENTS_CUR	struct commandments_cursor
#define	COMMANDMENTS_C		struct commandments_cursor
#define	COMMANDMENTS_ENT	struct commandments_entry
#define	COMMANDMENTS_E		struct commandments_entry
#define	COMMANDMENTS_FL		struct commandments_flags
#define	COMMANDMENTS_OBJ	struct commandments_object
#define	COMMANDMENTS_MAGIC	0x99447248
#define	COMMANDMENTS_DBNAME	"ten"


struct commandments_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct commandments_information {
	time_t		dbtime ;		/* db-time */
	time_t		citime ;		/* idx-time */
	uint		maxent ;
	uint		count ;
} ; /* end struct */

struct commandments_entry {
	uint		cn ;			/* commandment number */
} ; /* end struct */

struct commandments_cursor {
	cmi_cur		*vicurp ;
} ; /* end struct */

struct commandments_flags {
	uint		user:1 ;		/* user or system? */
	uint		idx:1 ;
	uint		ids:1 ;
} ; /* end struct */

struct commandments_head {
	cmi		*idxp ;
	void		*a ;
	cchar		*pr ;
	cchar		*dbname ;
	cchar		*uhome ;		/* user home dir */
	cchar 		*fname ;
	voidp		db_data ;		/* source DB */
	size_t		db_size ;		/* srouce DB size */
	time_t		ti_db ;			/* source DB m-time */
	time_t		ti_idx ;		/* index modification */
	time_t		ti_map ;		/* map */
	time_t		ti_lastcheck ;		/* last check of file */
	uid_t		uid ;
	uid_t		uid_pr ;
	gid_t		gid_pr ;
	COMMANDMENTS_FL	fl ;
	uint		magval ;
	int		ncursors ;
	int		nents ;
	int		maxent ;
} ; /* end struct */

typedef	COMMANDMENTS		commandments ;
typedef	COMMANDMENTS_FL		commandments_fl ;
typedef	COMMANDMENTS_INFO	commandments_info ;
typedef	COMMANDMENTS_CUR	commandments_cur ;
typedef	COMMANDMENTS_ENT	commandments_ent ;
typedef	COMMANDMENTS_OBJ	commandments_obj ;

EXTERNC_begin

extern int	commandments_open(commandments *,cchar *,cchar *) noex ;
extern int	commandments_audit(commandments *) noex ;
extern int	commandments_count(commandments *) noex ;
extern int	commandments_max(commandments *) noex ;
extern int	commandments_read(commandments *,char *,int,int) noex ;
extern int	commandments_get(commandments *,int,char *,int) noex ;
extern int	commandments_curbegin(commandments *,commandments_cur *) noex ;
extern int	commandments_curenum(commandments *,commandments_cur *,
			commandments_ent *,char *,int) noex ;
extern int	commandments_curend(commandments *,commandments_cur *) noex ;
extern int	commandments_close(commandments *) noex ;

EXTERNC_end

extern const commandments_obj	commandments_modinfo ;


#endif /* COMMANDMENTS_INCLUDE */


