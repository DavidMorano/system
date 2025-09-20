/* termenq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* manage reading or writing of the TERMENQ database */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */


#ifndef	TERMENQ_INCLUDE
#define	TERMENQ_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<terment.h>


/* object defines */
#define	TERMENQ		struct termenq_head
#define	TERMENQ_FL	struct termenq_flags
#define	TERMENQ_ENT	terment
#define	TERMENQ_CUR	struct termenq_cursor
#define	TERMENQ_MAGIC	0x10923874


struct termenq_cursor {
	int		i ;
} ;

struct termenq_flags {
	uint		writable:1 ;
} ;

struct termenq_head {
	cchar		*fname ;	/* stored file name */
	caddr_t		mapdata ;	/* file mapping buffer */
	time_t		ti_open ;	/* open time (for FD caching) */
	time_t		ti_mod ;	/* last modification time */
	time_t		ti_check ;	/* last check time */
	size_t		mapsize ;
	size_t		fsize ;		/* file total size */
	TERMENQ_FL	fl ;
	uint		mapoff ;	/* file mapping starting offset */
	uint		magic ;
	int		pagesz ;
	int		oflags ;	/* open flags */
	int		operms ;	/* open permissions */
	int		fd ;		/* file descriptor */
	int		ncursors ;
	int		mapei ;		/* index of top mapped entry */
	int		mapen ;		/* number of mapped entries */
	int		en ;		/* convenience store */
} ;

typedef	TERMENQ		termenq ;
typedef	TERMENQ_FL	termenq_fl ;
typedef	TERMENQ_ENT	termenq_ent ;
typedef	TERMENQ_CUR	termenq_cur ;

EXTERNC_begin

extern int termenq_open(termenq *,cchar *,int) noex ;
extern int termenq_read(termenq *,int,termenq_ent *) noex ;
extern int termenq_write(termenq *,int,termenq_ent *) noex ;
extern int termenq_check(termenq *,time_t) noex ;
extern int termenq_curbegin(termenq *,termenq_cur *) noex ;
extern int termenq_curend(termenq *,termenq_cur *) noex ;
extern int termenq_curenum(termenq *,termenq_cur *,termenq_ent *) noex ;
extern int termenq_curfetchln(termenq *,termenq_cur *,termenq_ent *,
		cchar *) noex ;
extern int termenq_fetchsid(termenq *,termenq_ent *,pid_t) noex ;
extern int termenq_nactive(termenq *) noex ;
extern int termenq_close(termenq *) noex ;

EXTERNC_end


#endif /* TERMENQ_INCLUDE */


