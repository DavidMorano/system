/* densitydb HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage reading or writing of a density file */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	DENSITYDB_INCLUDE
#define	DENSITYDB_INCLUDE


#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<time.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<endian.h>

#include	"densitydbe.h"


#define	DENSITYDB_MAGIC		0x10928474
#define	DENSITYDB		struct densitydb_head
#define	DENSITYDB_FL		struct densitydb_flags
#define	DENSITYDB_CUR		struct densitydb_cursor
#define	DENSITYDB_MAGSTR	"densitydb"
#define	DENSITYDB_MAGSIZ	16
/* other defines */
#define	DENSITYDB_FILEVERSION	0
#define	DENSITYDB_ENDIAN	ENDIAN	/* endian */
#define	DENSITYDB_ENT		densitydbe


struct densitydb_flags {
	uint		fileinit:1 ;		/* file initialized */
	uint		writable:1 ;
	uint		readlocked:1 ;
	uint		writelocked:1 ;
	uint		cursorlockbroken:1 ;	/* cursor lock broken */
	uint		cursoracc:1 ;		/* accessed while cursored? */
	uint		remote:1 ;		/* remote mounted file */
} ;

struct densitydb_head {
	char		*fname ;
	time_t		tiopen ;		/* file open time */
	time_t		tiaccess ;		/* file access time */
	time_t		timod ;			/* file modification time */
	void		*fhp ;
	void		*bop ;
	DENSITYDB_FL	fl ;
	uint		magic ;
	int		oflags ;
	int		maxent ;
	int		pagesz ;
	int		filesz ;
	int		fd ;
	int		cursors ;
	int		ebs ;		/* entry buffer size */
	mode_t		om ;
} ;

struct densitydb_cursor {
    	int		i ;
} ;

typedef	DENSITYDB		densitydb ;
typedef	DENSITYDB_FL		densitydb_fl ;
typedef	DENSITYDB_CUR		densitydb_cur ;
typedef	DENSITYDB_ENT		densitydb_ent ;

EXTERNC_begin

extern int densitydb_open(densitydb *,cchar *,int,int,int) noex ;
extern int densitydb_close(densitydb *) noex ;
extern int densitydb_check(densitydb *,time_t) noex ;
extern int densitydb_count(densitydb *) noex ;
extern int densitydb_curbegin(densitydb *,densitydb_cur *) noex ;
extern int densitydb_curend(densitydb *,densitydb_cur *) noex ;
extern int densitydb_curenum(densitydb *,densitydb_cur *,densitydb_ent *) noex ;
extern int densitydb_match(densitydb *,time_t,int,densitydb_ent *) noex ;
extern int densitydb_update(densitydb *,time_t,int,densitydb_ent *) noex ;
extern int densitydb_write(densitydb *,time_t,int,uint) noex ;

EXTERNC_end


#endif /* DENSITYDB_INCLUDE */


