/* getutmpent HEADER */
/* lang=C20 */

/* get UTMP entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	GETUTMPENT_INCLUDE
#define	GETUTMPENT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<time.h>
#include	<usystem.h>


#define	GETUTMPENT		struct getutmpent

/* UTMP entry types */
#ifndef	GETUTMPENT_TEMPTY
#define	GETUTMPENT_TEMPTY	0	/* entry is unused */
#define	GETUTMPENT_TRUNLEVEL	1
#define	GETUTMPENT_TBOOTTIME	2
#define	GETUTMPENT_TOLDTIME	3
#define	GETUTMPENT_TNEWTIME	4
#define	GETUTMPENT_TINITPROC	5	/* process spawned by "init" */
#define	GETUTMPENT_TLOGINPROC	6	/* a "getty" waiting for login */
#define	GETUTMPENT_TUSERPROC	7	/* a regular user process */
#define	GETUTMPENT_TDEADPROC	8	/* used in WTMP only? */
#define	GETUTMPENT_TACCOUNT	9	/* used in WTMP only? */
#define	GETUTMPENT_TSIGNATURE	10	/* used in WTMP only? */
#endif /* GETUTMPENT_TEMPTY */

/* UTMP field sizes */
#ifndef	GETUTMPENT_LID
#define	GETUTMPENT_LID		4
#define	GETUTMPENT_LUSER	32
#define	GETUTMPENT_LLINE	32
#define	GETUTMPENT_LHOST	256
#endif


struct getutmpent {
	time_t		date ;
	pid_t		sid ;
	int		session ;
	char		id[GETUTMPENT_LID + 1] ;
	char		line[GETUTMPENT_LLINE + 1] ;
	char		user[GETUTMPENT_LUSER + 1] ;
	char		host[GETUTMPENT_LHOST + 1] ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int getutmpent(GETUTMPENT *,pid_t) noex ;

extern int getutmpname(char *,int,pid_t) noex ;
extern int getutmphost(char *,int,pid_t) noex ;
extern int getutmpline(char *,int,pid_t) noex ;
extern int getutmpid(char *,int,pid_t) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETUTMPENT_INCLUDE */


