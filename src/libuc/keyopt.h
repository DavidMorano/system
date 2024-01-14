/* keyopt HEADER */
/* lang=C20 */

/* paramater option manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	KEYOPT_INCLUDE
#define	KEYOPT_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>


#define	KEYOPT_MAGIC	0x84634270
#define	KEYOPT		struct keyopt_head
#define	KEYOPT_NAME	struct keyopt_name
#define	KEYOPT_VALUE	struct keyopt_value
#define	KEYOPT_CUR	struct keyopt_cursor


struct keyopt_value {
	KEYOPT_VALUE	*next ;
	cchar		*value ;
} ;

struct keyopt_name {
	cchar		*name ;		/* the key */
	KEYOPT_NAME	*next ;
	KEYOPT_VALUE	*head ;		/* first */
	KEYOPT_VALUE	*tail ;		/* last */
	KEYOPT_VALUE	*current ;	/* used for interation only */
	int		count ;		/* count of values */
} ;

struct keyopt_cursor {
	KEYOPT_NAME	*keyp ;
	KEYOPT_VALUE	*valuep ;
} ;

struct keyopt_head {
	KEYOPT_NAME	*head ;
	KEYOPT_NAME	*tail ;
	uint		magic ;
	int		count ;		/* count of keys */
} ;

typedef KEYOPT		keyopt ;
typedef KEYOPT_CUR	keyopt_cur ;

EXTERNC_begin

extern int keyopt_start(keyopt *) noex ;
extern int keyopt_loads(keyopt *,cchar *,int) noex ;
extern int keyopt_load(keyopt *,cchar *,int) noex ;
extern int keyopt_loadvalue(keyopt *,cchar *,cchar *,int) noex ;
extern int keyopt_count(keyopt *) noex ;
extern int keyopt_curbegin(keyopt *,keyopt_cur *) noex ;
extern int keyopt_curend(keyopt *,keyopt_cur *) noex ;
extern int keyopt_enumkeys(keyopt *,keyopt_cur *,cchar **) noex ;
extern int keyopt_fetch(keyopt *,cchar *,keyopt_cur *,cchar **) noex ;
extern int keyopt_enumvalues(keyopt *,cchar *,keyopt_cur *,cchar **) noex ;
extern int keyopt_finish(keyopt *) noex ;

#ifdef	COMMENT
extern int keyopt_findvalue(keyopt *,cchar *,cchar *,int,KEYOPT_VALUE **) noex ;
#endif /* COMMENT */

EXTERNC_end


#endif /* KEYOPT_INCLUDE */


