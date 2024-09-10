/* utmpent HEADER */
/* lang=C20 */

/* get user information from UTMP database */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	UTMPENT_INCLUDE
#define	UTMPENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<utmpx.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


/* object defines */
#ifndef	UTMPENT
#define	UTMPENT		struct futmpx
#endif

/* entry type values */
#define	UTMPENT_TEMPTY		0	/* entry is unused */
#define	UTMPENT_TRUNLEVEL	1
#define	UTMPENT_TBOOTTIME	2
#define	UTMPENT_TOLDTIME	3
#define	UTMPENT_TNEWTIME	4
#define	UTMPENT_TINITPROC	5	/* process spawned by "init" */
#define	UTMPENT_TLOGINPROC	6	/* a "getty" waiting for login */
#define	UTMPENT_TUSERPROC	7	/* a regular user process */
#define	UTMPENT_TDEADPROC	8	/* dead process (moved to WUTMPENT) */
#define	UTMPENT_TACCOUNT	9	/* used in WUTMPENT only? */
#define	UTMPENT_TSIGNATURE	10	/* used in WUTMPENT only? */

/* entry lengths */
#define	UTMPENT_LID		4
#define	UTMPENT_LUSER		32
#define	UTMPENT_LLINE		32
#define	UTMPENT_LHOST		256

typedef	UTMPENT		utmpent

EXTERNC_begin

extern int utmpent_start(utmpent *) noex ;
extern int utmpent_settype(utmpent *,int) noex ;
extern int utmpent_setpid(utmpent *,pid_t) noex ;
extern int utmpent_setsession(utmpent *,int) noex ;
extern int utmpent_setlines(utmpent *,int) noex ;
extern int utmpent_setcols(utmpent *,int) noex ;
extern int utmpent_setid(utmpent *,cchar *,int) noex ;
extern int utmpent_setline(utmpent *,cchar *,int) noex ;
extern int utmpent_setuser(utmpent *,cchar *,int) noex ;
extern int utmpent_sethost(utmpent *,cchar *,int) noex ;
extern int utmpent_gettype(utmpent *) noex ;
extern int utmpent_getpid(utmpent *) noex ;
extern int utmpent_getsession(utmpent *) noex ;
extern int utmpent_getlines(utmpent *) noex ;
extern int utmpent_getcols(utmpent *) noex ;
extern int utmpent_getid(utmpent *,cchar **) noex ;
extern int utmpent_getline(utmpent *,cchar **) noex ;
extern int utmpent_getuser(utmpent *,cchar **) noex ;
extern int utmpent_gethost(utmpent *,cchar **) noex ;
extern int utmpent_finish(utmpent *) noex ;

EXTERNC_end


#endif /* UTMPENT_INCLUDE */


