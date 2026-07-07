/* printdb HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* printer database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PRINTDB_INCLUDE
#define	PRINTDB_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<svcfile.h>		/* LIBUC */


#define	PRINTDB		struct printdb_head
#define	PRINTDB_DB	struct printdb_dbase
#define	PRINTDB_MAGIC	0x77446329


enum printdbs {
	printdb_local,
	printdb_system,
	printdb_overlast
} ; /* end enum */

struct printdb_dbase {
	svcfile		*sfp ;
	time_t		ti_find ;
	time_t		ti_check ;
	time_t		ti_open ;
	time_t		ti_mtime ;
	uint		f_open:1 ;
} ; /* end struct */

struct printdb_head {
	cchar		*pr ;		/* root program */
	cchar		*ur ;		/* root root */
	cchar		*uname ;	/* username */
	cchar		*fname ;
	char		*a ;		/* memory allocation */
	PRINTDB_DB	dbs[printdb_overlast] ;
	time_t		dt ;
	uint		magval ;
} ; /* end struct */

typedef	PRINTDB		printdb ;
typedef	PRINTDB_DB	printdb_db ;

EXTERNC_begin

extern int	printdb_open	(printdb *,cc *,cc *,cc *,cc *) noex ;
extern int	printdb_fetch	(printdb *,char *,int,cc *,cc *) noex ;
extern int	printdb_check	(printdb *,time_t) noex ;
extern int	printdb_close	(printdb *) noex ;

EXTERNC_end


#endif /* PRINTDB_INCLUDE */


