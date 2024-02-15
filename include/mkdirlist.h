/* mkdirlist SUPPORT */
/* lang=C++20 */

/* create a list of the newsgroup directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	MKDIRLIST_INCLUDE
#define	MKDIRLIST_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<usystem.h>
#include	<vechand.h>


#define	MKDIRLIST_MAGIC		0x8987372A
#define	MKDIRLIST		struct mkdirlist_head
#define	MKDIRLIST_ENT		struct mkdirlist_ent
#define	MKDIRLIST_OFL		struct mkdirlist_eflags


struct mkdirlist_head {
	vechand		*dlp ;		/* Directory-List-Pointer */
	uint		magic ;
} ;

struct mkdirlist_eflags {
	uint		excl:3 ;	/* ??? */
	uint		new:1 ;		/* new newsgroup */
	uint		options:1 ;	/* directory has options file */
	uint		subscribe:1 ;	/* subscribed in user file? */
	uint		seen:1 ;	/* already written out to user file */
	uint		show:1 ;	/* display to user */
	uint		link:1 ;	/* has been linked to another */
} ;

struct mkdirlist_ent {
	cchar		*name ;
	MKDIRLIST_ENT	*link ;
	ino_t		ino ;
	dev_t		dev ;
	time_t		mtime ;
	time_t		utime ;
	int		nlen ;
	int		narticles ;
	int		order ;
	MKDIRLIST_OFL	f ;
	mode_t		mode ;
} ;

typedef MKDIRLIST	mkdirlist ;
typedef MKDIRLIST_ENT	mkdirlist_ent ;

EXTERNC_begin

extern int mkdirlist_start(MKDIRLIST *,cchar *,cchar *) noex ;
extern int mkdirlist_link(MKDIRLIST *) noex ;
extern int mkdirlist_defshow(MKDIRLIST *) noex ;
extern int mkdirlist_sort(MKDIRLIST *) noex ;
extern int mkdirlist_get(MKDIRLIST *,int,MKDIRLIST_ENT **) noex ;
extern int mkdirlist_ung(MKDIRLIST *,cchar *,time_t,int,int) noex ;
extern int mkdirlist_showdef(MKDIRLIST *) noex ;
extern int mkdirlist_show(MKDIRLIST *,cchar *,int) noex ;
extern int mkdirlist_audit(MKDIRLIST *) noex ;
extern int mkdirlist_finish(MKDIRLIST *) noex ;

EXTERNC_end


#endif /* MKDIRLIST_INCLUDE */


