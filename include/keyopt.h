/* keyopt HEADER */
/* charset=ISO8859-1 */
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


#include	<envstandards.h>	/* ordered first to configure */
#include	<usystem.h>


#define	KEYOPT_MAGIC	0x84634270
#define	KEYOPT		struct keyopt_head
#define	KEYOPT_NA	struct keyopt_name
#define	KEYOPT_VAL	struct keyopt_value
#define	KEYOPT_CUR	struct keyopt_cursor


struct keyopt_value {
	KEYOPT_VAL	*next ;
	cchar		*value ;
} ;

struct keyopt_name {
	cchar		*name ;		/* the key */
	KEYOPT_NA	*next ;
	KEYOPT_VAL	*head ;		/* first */
	KEYOPT_VAL	*tail ;		/* last */
	KEYOPT_VAL	*current ;	/* used for interation only */
	int		count ;		/* count of values */
} ;

struct keyopt_cursor {
	KEYOPT_NA	*keyp ;
	KEYOPT_VAL	*valuep ;
} ;

struct keyopt_head {
	KEYOPT_NA	*head ;
	KEYOPT_NA	*tail ;
	uint		magic ;
	int		count ;		/* count of keys */
} ;

typedef KEYOPT		keyopt ;
typedef	KEYOPT_NA	keyopt_na ;
typedef KEYOPT_CUR	keyopt_cur ;
typedef	KEYOPT_VAL	keyopt_val ;

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
extern int keyopt_lastvalue(keyopt *,cchar *,cchar **) noex ;

#ifdef	COMMENT
extern int keyopt_findvalue(keyopt *,cchar *,cchar *,int,keyopt_val **) noex ;
#endif /* COMMENT */

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
static inline int keyopt_magic(keyopt *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == KEYOPT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (keyopt_magic) */

#endif /* __cplusplus */


#endif /* KEYOPT_INCLUDE */


