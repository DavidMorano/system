/* artlist HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* article list handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that came before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

#ifndef	ARTLIST_INCLUDE
#define	ARTLIST_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/timeb.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vechand.h>
#include	<dater.h>


#define	ARTLIST			struct artlist_head
#define	ARTLIST_ENT		struct artlist_entry
#define	ARTLIST_MAGIC		0x83465875
#define	ARTLIST_CURMAGIC	0x83465876


struct artlist_head {
	dater		*datep ;
	vechand		*artp ;
	TIMEB		now ;
	uint		magic ;
} ; /* end struct */

struct artlist_entry {
	cchar		*ngdir ;	/* newsgroup directory */
	cchar		*name ;		/* filename (overlaps w/ 'ngdir') */
	cchar		*subject ;	/* header SUBJECT */
	cchar		*replyto ;	/* header REPLYTO */
	cchar		*from ;		/* header FEOM */
	cchar		*newsgroups ;	/* header NEWSGROUPS */
	cchar		*messageid ;	/* header MESSAGEID */
	cchar		*articleid ;	/* header ARTICLEID */
	time_t		mtime ;		/* modification time (always has one) */
	time_t		atime ;		/* arrival time (envelope if any) */
	time_t		ptime ;		/* posting time (if any) */
	time_t		ctime ;		/* compose time (if any) */
	uint		magic ;
	int		lines ;		/* lines in article body (if known) */
	int		clen ;		/* article body (if known) */
} ; /* end struct */

typedef ARTLIST		artlist ;
typedef ARTLIST_ENT	artlist_ent ;

EXTERNC_begin

extern int	artlist_start(artlist *,TIMEB *,cchar *) noex ;
extern int	artlist_finish(artlist *) noex ;
extern int	artlist_add(artlist *,cchar *,cchar *) noex ;
extern int	artlist_sort(artlist *,int,int) noex ;
extern int	artlist_get(artlist *,int,cchar **,cchar **,time_t *) noex ;
extern int	artlist_getentry(artlist *,int,artlist_ent **) noex ;

EXTERNC_end


#endif /* ARTLIST_INCLUDE */


