/* keysymer HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* keysym name-value database */
/* version %I% last-modified %G% */


/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	KEYSYMER_INCLUDE
#define	KEYSYMER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mapstrint.h>		/* LIBUC */


#define	KEYSYMER		struct keysymer_head
#define	KEYSYMER_FL		struct keysymer_flags
#define	KEYSYMER_KE		struct keysymer_entry
#define	KEYSYMER_CUR		struct keysymer_cursor
#define	KEYSYMER_NAMELEN	60
#define	KEYSYMER_MAGIC		0x24282138


struct keysymer_flags {
	uint		dummy:1 ;	/* dummy */
} ; /* end struct */

struct keysymer_entry {
	int		keynum ;
	char		keyname[KEYSYMER_NAMELEN+1] ;
} ; /* end struct */

struct keysymer_cursor {
	MAPSTRINT_CUR	c ;
} ; /* end struct */

struct keysymer_head {
	mapstrint	*mlp ;		/* map-list-pointer */
	KEYSYMER_FL	fl ;
	uint		magval ;
} ; /* end struct */

typedef	KEYSYMER	keysymer ;
typedef	KEYSYMER_FL	keysymer_fl ;
typedef	KEYSYMER_KE	keysymer_ke ;
typedef	KEYSYMER_CUR	keysymer_cur ;

EXTERNC_begin

extern int keysymer_open(keysymer *,cchar *) noex ;
extern int keysymer_count(keysymer *) noex ;
extern int keysymer_lookup(keysymer *,cchar *,int) noex ;
extern int keysymer_curbegin(keysymer *,keysymer_cur *) noex ;
extern int keysymer_curend(keysymer *,keysymer_cur *) noex ;
extern int keysymer_curenum(keysymer *,keysymer_cur *,keysymer_ke *) noex ;
extern int keysymer_close(keysymer *) noex ;

EXTERNC_end


#endif /* KEYSYMER_INCLUDE */


