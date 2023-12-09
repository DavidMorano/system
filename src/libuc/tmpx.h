/* tmpx INCLUDE */
/* lang=C20 */

/* object to facilitate UTMPX management */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	TMPX_INCLUDE
#define	TMPX_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<utmpx.h>
#include	<usystem.h>


/* object defines */

#define	TMPX		struct tmpx_head
#define	TMPX_CUR	struct tmpx_cursor
#define	TMPX_FL		struct tmpx_flags
#define	TMPX_MAGIC	1092387456
#define	TMPX_ENT	struct utmpx
#define	TMPX_ENTSIZE	sizeof(TMPX_ENT)

/* other defines */

#if	(defined(OSNAME_SunOS) && (OSNAME_SunOS > 0))
#define	TMPX_DEFUTMP	"/var/adm/utmpx"
#elif	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#define	TMPX_DEFUTMP	"/var/run/utmpx"
#elif	defined(OSNAME_Linux) && (OSNAME_Linux > 0)
#else
#define	TMPX_DEFUTMP	"/var/run/utmpx"
#endif

/* entry type values */

#define	TMPX_TEMPTY		0	/* entry is unused */
#define	TMPX_TRUNLEVEL		1
#define	TMPX_TBOOTTIME		2
#define	TMPX_TOLDTIME		3
#define	TMPX_TNEWTIME		4
#define	TMPX_TPROCINIT		5	/* process spawned by "init" */
#define	TMPX_TPROCLOGIN		6	/* a "getty" waiting for login */
#define	TMPX_TPROCUSER		7	/* a regular user process */
#define	TMPX_TPROCDEAD		8	/* dead process (moved to WTMPX) */
#define	TMPX_TACCOUNT		9	/* used in WTMPX only? */
#define	TMPX_TSIGNATURE		10	/* used in WTMPX only? */

/* entry lengths */

#define	TMPX_LID		4
#define	TMPX_LUSER		32
#define	TMPX_LLINE		32
#define	TMPX_LHOST		256

/* UTMPX stuff (in theory could be different from above) */

#ifndef	UTMPX_TEMPTY
#define	UTMPX_TEMPTY		0	/* entry is unused */
#define	UTMPX_TRUNLEVEL		1
#define	UTMPX_TBOOTTIME		2
#define	UTMPX_TOLDTIME		3
#define	UTMPX_TNEWTIME		4
#define	UTMPX_TPROCINIT		5	/* process spawned by "init" */
#define	UTMPX_TPROCLOGIN	6	/* a "getty" waiting for login */
#define	UTMPX_TPROCUSER		7	/* a regular user process */
#define	UTMPX_TPROCDEAD		8	/* used in WTMPX only? */
#define	UTMPX_TACCOUNT		9	/* used in WTMPX only? */
#define	UTMPX_TSIGNATURE	10	/* used in WTMPX only? */
#endif /* UTMPX_TEMPTY */

#ifndef	UTMPX_LID
#define	UTMPX_LID		4
#define	UTMPX_LUSER		32
#define	UTMPX_LLINE		32
#define	UTMPX_LHOST		256
#endif


struct tmpx_cursor {
	int		i ;
} ;

struct tmpx_flags {
	uint		writable:1 ;
} ;

struct tmpx_head {
	cchar		*fname ;	/* stored file name */
	caddr_t		mapdata ;	/* file mapping buffer */
	time_t		ti_open ;	/* open time (for FD caching) */
	time_t		ti_mod ;	/* last modification time */
	time_t		ti_check ;	/* last check time */
	size_t		mapsize ;
	size_t		fsize ;		/* file total size */
	TMPX_FL		f ;
	uint		mapoff ;	/* file mapping starting offset */
	uint		magic ;
	int		pagesize ;
	int		oflags ;	/* open flags */
	int		operms ;	/* open permissions */
	int		fd ;		/* file descriptor */
	int		ncursors ;
	int		mapei ;		/* index of top mapped entry */
	int		mapen ;		/* number of mapped entries */
} ;

typedef tmpx_head	tmpx ;
typedef tmpx_cursor	tmpx_cur ;
typedef TMPX_ENT	tmpx_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int tmpx_open(tmpx *,cchar *,int) noex ;
extern int tmpx_read(tmpx *,int,tmpx_ent *) noex ;
extern int tmpx_write(tmpx *,int,tmpx_ent *) noex ;
extern int tmpx_check(tmpx *,time_t) noex ;
extern int tmpx_curbegin(tmpx *,tmpx_cur *) noex ;
extern int tmpx_curend(tmpx *,tmpx_cur *) noex ;
extern int tmpx_enum(tmpx *,tmpx_cur *,tmpx_ent *) noex ;
extern int tmpx_fetchuser(tmpx *,tmpx_cur *,tmpx_ent *,cchar *) noex ;
extern int tmpx_fetchpid(tmpx *,tmpx_ent *,pid_t) noex ;
extern int tmpx_nusers(tmpx *) noex ;
extern int tmpx_close(tmpx *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* TMPX_INCLUDE */


