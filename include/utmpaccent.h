/* utmpaccent HEADER */
/* lang=C20 */

/* UTMPACCENT management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UTMPACCENT_INCLUDE
#define	UTMPACCENT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utmpx.h>		/* <- POSIX� (the money shot) */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>


#ifndef	UTMPFENT
#define	UTMPFENT		struct utmpx
#endif

#ifndef	CUTMPFENT
#define	CUTMPFENT		const struct utmpx
#endif

#define	UTMPACCENT		struct utmpaccent_s
#define	CUTMPACCENT		const struct utmpaccent_s

#define	UTMPACCENT_BUFLEN	332

#ifndef	UTMPACCENT_TEMPTY
#define	UTMPACCENT_TEMPTY	0	/* entry is unused */
#define	UTMPACCENT_TRUNLEVEL	1
#define	UTMPACCENT_TBOOTTIME	2
#define	UTMPACCENT_TOLDTIME	3
#define	UTMPACCENT_TNEWTIME	4
#define	UTMPACCENT_TPROCINIT	5	/* process spawned by "init" */
#define	UTMPACCENT_TPROCLOGIN	6	/* a "getty" waiting for login */
#define	UTMPACCENT_TPROCUSER	7	/* a regular user process */
#define	UTMPACCENT_TPROCDEAD	8	/* used in WTMPX only? */
#define	UTMPACCENT_TACCOUNT	9	/* used in WTMPX only? */
#define	UTMPACCENT_TSIGNATURE	10	/* used in WTMPX only? */
#endif /* UTMPACCENT_TEMPTY */

#ifndef	UTMPACCENT_LID
#define	UTMPACCENT_LID		4
#define	UTMPACCENT_LUSER	32
#define	UTMPACCENT_LLINE	32
#define	UTMPACCENT_LHOST	256
#endif


struct utmpaccent_s {
	cchar		*user ;
	cchar		*line ;
	cchar		*host ;
	time_t		ctime ;
	pid_t		sid ;
	uint		session ;
	ushort		type ;
	ushort		syslen ;
	short		e_exit ;
	short		e_term ;
	char		id[UTMPACCENT_LID+1] ;
} ; /* end struct (utmpaccent) */

typedef UTMPACCENT	utmpaccent ;


#ifdef	__cplusplus
extern "C" {
#endif

extern int utmpaccent_load(UTMPACCENT *,char *,int,CUTMPFENT *) noex ;
extern int utmpaccent_size(CUTMPACCENT *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UTMPACCENT_INCLUDE */


