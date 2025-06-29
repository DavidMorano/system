/* utmpaccent HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UTMPACCENT management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UTMPACCENT_INCLUDE
#define	UTMPACCENT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utmpx.h>		/* <- POSIX� (the money shot) */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	UTMPACCENT		struct utmpaccent_s
#define	CUTMPACCENT		const struct utmpaccent_s

/* entry types */
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
 /* entry field lengths */
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
} ; /* end struct (utmpaccent_s) */

typedef UTMPACCENT	utmpaccent ;
typedef CUTMPACCENT	cutmpaccent ;

EXTERNC_begin

extern int utmpaccent_load(utmpaccent *,char *,int,CUTMPX *) noex ;
extern int utmpaccent_size(cutmpaccent *) noex ;

EXTERNC_end


#endif /* UTMPACCENT_INCLUDE */


