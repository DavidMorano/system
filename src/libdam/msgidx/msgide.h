/* msgide HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* message identification (MSG-ID) entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-03-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	MSGIDE_INCLUDE
#define	MSGIDE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<netdb.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


/* object defines */
#define	MSGIDE_ALL		struct msgide_all
#define	MSGIDE_UPDATE		struct msgide_update

/* entry field lengths */
#define	MSGIDE_LCOUNT		4
#define	MSGIDE_LUTIME		4	/* entry update */
#define	MSGIDE_LCTIME		4	/* entry creation */
#define	MSGIDE_LMTIME		4	/* message */
#define	MSGIDE_LHASH		4
#define	MSGIDE_LRECIP		32
#define	MSGIDE_LMSGID		(2 * MAXHOSTNAMELEN)
#define	MSGIDE_LFROM		(2 * MAXHOSTNAMELEN)

/* entry field offsets */
/* do this carefully! */
/* there is no good automatic way to do this in C language (sigh) */
/* the C language does not have all of the advantages of assembly language */
#define	MSGIDE_OCOUNT		0
#define	MSGIDE_OUTIME		(MSGIDE_OCOUNT + MSGIDE_LCOUNT)
#define	MSGIDE_OCTIME		(MSGIDE_OUTIME + MSGIDE_LUTIME)
#define	MSGIDE_OMTIME		(MSGIDE_OCTIME + MSGIDE_LCTIME)
#define	MSGIDE_OHASH		(MSGIDE_OMTIME + MSGIDE_LMTIME)
#define	MSGIDE_ORECIP		(MSGIDE_OHASH + MSGIDE_LHASH)
#define	MSGIDE_OMSGID		(MSGIDE_ORECIP + MSGIDE_LRECIP)
#define	MSGIDE_OFROM		(MSGIDE_OMSGID + MSGIDE_LMSGID)

#define	MSGIDE_SIZE		(MSGIDE_OFROM + MSGIDE_LFROM)


struct msgide_all {
	uint		count ;		/* count */
	uint		utime ;		/* time-stamp update */
	uint		ctime ;		/* time-stamp creation */
	uint		mtime ;		/* time-stamp message */
	uint		hash ;
	char		recipient[MSGIDE_LRECIP + 1] ;
	char		messageid[MSGIDE_LMSGID + 1] ;
	char		from[MSGIDE_LFROM + 1] ;
} ;

struct msgide_update {
	uint		count ;
	uint		utime ;
} ;

EXTERNC_begin

extern int msgide_all(struct msgide_all *,int,char *,int) noex ;
extern int msgide_update(struct msgide_update *,int,char *,int) noex ;

EXTERNC_end


#endif /* MSGIDE_INCLUDE */


