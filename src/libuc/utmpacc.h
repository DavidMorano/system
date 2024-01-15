/* utmpacc HEADER */
/* lang=C20 */

/* UTMPACC management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UTMPACC_INCLUDE
#define	UTMPACC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utmpx.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>
#include	<utmpaccent.h>		/* <- the money shot */


#define	UTMPACC_ENT	UTMPACCENT
#define	UTMPACC_SB	struct utmpacc_statistics
#define	UTMPACC_CUR	struct utmpacc_cursor

#define	UTMPACC_BUFLEN	332		/* large enough for all fields */
#define	UTMPACC_MAX	20
#define	UTMPACC_TTL	(20*60)

#if	(defined(OSNAME_SunOS) && (OSNAME_SunOS > 0))
#define	UTMPACC_DEFUTMP	"/var/adm/utmpx"
#elif	(defined(OSNAME_Darwin) && (OSNAME_Darwin > 0))
#define	UTMPACC_DEFUTMP	"/var/run/utmpx"
#else
#define	UTMPACC_DEFUTMP	"/var/run/utmpx"
#endif

#ifndef	UTMPACC_TEMPTY
#define	UTMPACC_TEMPTY		0	/* entry is unused */
#define	UTMPACC_TRUNLEVEL	1
#define	UTMPACC_TBOOTTIME	2
#define	UTMPACC_TOLDTIME	3
#define	UTMPACC_TNEWTIME	4
#define	UTMPACC_TPROCINIT	5	/* process spawned by "init" */
#define	UTMPACC_TPROCLOGIN	6	/* a "getty" waiting for login */
#define	UTMPACC_TPROCUSER	7	/* a regular user process */
#define	UTMPACC_TPROCDEAD	8	/* used in WTMPX only? */
#define	UTMPACC_TACCOUNT	9	/* used in WTMPX only? */
#define	UTMPACC_TSIGNATURE	10	/* used in WTMPX only? */
#endif /* UTMPACC_TEMPTY */

#ifndef	UTMPACC_LID
#define	UTMPACC_LID		4
#define	UTMPACC_LUSER		32
#define	UTMPACC_LLINE		32
#define	UTMPACC_LHOST		256
#endif


enum utxproctypes {
	utxproctype_user,
	utxproctype_login,
	utxproctype_init,
	utxproctype_dead,
	utxproctype_overlast
} ;

struct utmpacc_statistics {
	uint		maxusers ;
	uint		maxents ;
} ;

struct utmpacc_cursor {
	void		*icursor ;
} ;

typedef	UTMPACC_SB	utmpacc_sb ;
typedef	UTMPACC_CUR	utmpacc_cur ;
typedef	UTMPACC_ENT	utmpacc_ent ;

EXTERNC_begin

extern int utmpacc_init() noex ;
extern int utmpacc_boottime(time_t *) noex ;
extern int utmpacc_runlevel() noex ;
extern int utmpacc_users(int) noex ;
extern int utmpacc_entsid(utmpacc_ent *,char *,int,pid_t) noex ;
extern int utmpacc_entline(utmpacc_ent *,char *,int,cchar *,int) noex ;
extern int utmpacc_stats(utmpacc_sb *) noex ;
extern int utmpacc_extract(int) noex ;
extern int utmpacc_fini() noex ;

extern int utmpacc_curbegin(utmpacc_cur *) noex ;
extern int utmpacc_curenum(utmpacc_cur *,utmpacc_ent *,char *,int) noex ;
extern int utmpacc_curend(utmpacc_cur *) noex ;

EXTERNC_end


#endif /* UTMPACC_INCLUDE */


