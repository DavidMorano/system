/* terment INCLUDE */
/* lang=C20 */

/* methods for the TERMENT object */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This code provides the methods for the TERMENT object. The
	TERMENT object, while residing in a file, forms a single
	record in a file-database of records. This is similar to a
	raw UTMPX object when it is written out to the 'utmpx' file.
	This object is used by the TERMENQ object to represent a
	record in the TERMENQ database.

*******************************************************************************/

#ifndef	TERMENT_INCLUDE
#define	TERMENT_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"terment.h"


#define	TERMENT		struct terment

/* entry type values */

#define	TERMENT_TEMPTY		0	/* entry is unused */
#define	TERMENT_TLOGINPROC	1	/* a "getty" waiting for login */
#define	TERMENT_TUSERPROC	2	/* a regular user process */
#define	TERMENT_TDEADPROC	3	/* dead process (moved to WTERMENT) */

/* entry lengths */

#define	TERMENT_LID		4
#define	TERMENT_LLINE		32
#define	TERMENT_LTERMTYPE	32
#define	TERMENT_LANSWER		116


struct terment {
	pid_t		sid ;		/* session ID */
	uchar		type ;		/* type of entry (see above) */
	uchar		termcode ;	/* ANSI terminal code */
	uchar		lines ;
	uchar		cols ;
	char		id[TERMENT_LID] ;	/* UTMP ID */
	char		line[TERMENT_LLINE] ;
	char		termtype[TERMENT_LTERMTYPE] ;
	char		answer[TERMENT_LANSWER] ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int terment_start(TERMENT *) noex ;
extern int terment_settype(TERMENT *,int) noex ;
extern int terment_setsid(TERMENT *,pid_t) noex ;
extern int terment_setcode(TERMENT *,int) noex ;
extern int terment_setlines(TERMENT *,int) noex ;
extern int terment_setcols(TERMENT *,int) noex ;
extern int terment_setid(TERMENT *,cchar *,int) noex ;
extern int terment_setline(TERMENT *,cchar *,int) noex ;
extern int terment_settermtype(TERMENT *,cchar *,int) noex ;
extern int terment_setanswer(TERMENT *,cchar *,int) noex ;

extern int terment_gettype(TERMENT *) noex ;
extern int terment_getsid(TERMENT *) noex ;
extern int terment_getcode(TERMENT *) noex ;
extern int terment_getlines(TERMENT *) noex ;
extern int terment_getcols(TERMENT *) noex ;
extern int terment_getid(TERMENT *,cchar **) noex ;
extern int terment_getline(TERMENT *,cchar **) noex ;
extern int terment_gettermtype(TERMENT *,cchar **) noex ;
extern int terment_getanswer(TERMENT *,cchar **) noex ;
extern int terment_finish(TERMENT *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* TERMENT_INCLUDE */


