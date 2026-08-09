/* bibleverse HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* BibleVerse object-load management */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEVERSE_INCLUDE
#define	BIBLEVERSE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBUC */
#include	<bibleverses.h>		/* LIBDAM */
#include	<biblecite.h>		/* LIBDAM */


#define	BIBLEVERSE		struct bibleverse_head
#define	BIBLEVERSE_FL		struct bibleverse_flags
#define	BIBLEVERSE_Q		struct bibleverse_query
#define	BIBLEVERSE_CITE		struct bibleverse_query
#define	BIBLEVERSE_CUR		struct bibleverse_cursor
#define	BIBLEVERSE_INFO		struct bibleverse_information
#define	BIBLEVERSE_CA		struct bibleverse_calls
#define	BIBLEVERSE_MAGIC	0x99447246


struct bibleverse_flags {
        uint		modload:1 ;
} ; /* end struct */

struct bibleverse_information {
	time_t		dbtime ;	/* db-time */
	time_t		vitime ;	/* vi-time */
	uint		maxbook ;
	uint		maxchap ;
	uint		nverses ;
	uint		nzverses ;
} ; /* end struct */

struct bibleverse_query {
	uchar		b, c, v ;
} ; /* end struct */

struct bibleverse_cursor {
	void		*scp ;
	uint		magval ;
} ; /* end struct */

struct bibleverse_head {
	modload		*mlp ;
	void		*callp ;
	void		*obj ;		/* object pointer */
	BIBLEVERSE_FL	fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef	BIBLEVERSE		bibleverse ;
typedef	BIBLEVERSE_Q		bibleverse_q ;
typedef	BIBLEVERSE_CITE		bibleverse_cite ;
typedef	BIBLEVERSE_CUR		bibleverse_cur ;
typedef	BIBLEVERSE_INFO		bibleverse_info ;
typedef	BIBLEVERSE_CA		bibleverse_ca ;

EXTERNC_begin

extern int	bibleverse_open(bibleverse *,cchar *,cchar *) noex ;
extern int	bibleverse_count(bibleverse *) noex ;
extern int	bibleverse_read(bibleverse *,char *,int,
			con bibleverse_q *) noex ;
extern int	bibleverse_get(bibleverse *,con bibleverse_q *,
			char *,int) noex ;
extern int	bibleverse_curbegin(bibleverse *,bibleverse_cur *) noex ;
extern int	bibleverse_curenum(bibleverse *,bibleverse_cur *,
			bibleverse_q *,char *,int) noex ;
extern int	bibleverse_curend(bibleverse *,bibleverse_cur *) noex ;
extern int	bibleverse_audit(bibleverse *) noex ;
extern int	bibleverse_getinfo(bibleverse *,bibleverse_info *) noex ;
extern int	bibleverse_chapters(bibleverse *,int,uchar *,int) noex ;
extern int	bibleverse_close(bibleverse *) noex ;

EXTERNC_end


#endif /* BIBLEVERSE_INCLUDE */


