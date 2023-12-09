/* KEYOPT */


/* revision history:

	= 1998-09-01, David A�D� Morano

	This code module was originally written.


*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	KEYOPT_INCLUDE
#define	KEYOPT_INCLUDE	1


#include	<envstandards.h>
#include	<sys/types.h>
#include	<usystem.h>


#define	KEYOPT_MAGIC	0x84634270
#define	KEYOPT		struct keyopt_head
#define	KEYOPT_NAME	struct keyopt_n
#define	KEYOPT_VALUE	struct keyopt_v
#define	KEYOPT_CUR	struct keyopt_c


struct keyopt_v {
	struct keyopt_v		*next ;
	const char		*value ;
} ;

struct keyopt_n {
	const char		*name ;		/* the key */
	struct keyopt_n		*next ;
	struct keyopt_v 	*head ;		/* first */
	struct keyopt_v 	*tail ;		/* last */
	struct keyopt_v		*current ;	/* used for interation only */
	int			count ;		/* count of values */
} ;

struct keyopt_c {
	struct keyopt_n		*keyp ;
	struct keyopt_v		*valuep ;
} ;

struct keyopt_head {
	uint			magic ;
	struct keyopt_n		*head ;
	struct keyopt_n		*tail ;
	int			count ;		/* count of keys */
} ;


#if	(! defined(KEYOPT_MASTER)) || (KEYOPT_MASTER == 0)

#ifdef	__cplusplus
extern "C" {
#endif

extern int keyopt_start(KEYOPT *) ;
extern int keyopt_loads(KEYOPT *,const char *,int) ;
extern int keyopt_load(KEYOPT *,const char *,int) ;
extern int keyopt_loadvalue(KEYOPT *,const char *,const char *,int) ;
extern int keyopt_count(KEYOPT *) ;
extern int keyopt_curbegin(KEYOPT *,KEYOPT_CUR *) ;
extern int keyopt_curend(KEYOPT *,KEYOPT_CUR *) ;
extern int keyopt_enumkeys(KEYOPT *,KEYOPT_CUR *,const char **) ;
extern int keyopt_fetch(KEYOPT *,const char *,KEYOPT_CUR *,const char **) ;
extern int keyopt_enumvalues(KEYOPT *,const char *,KEYOPT_CUR *,
		const char **) ;
extern int keyopt_finish(KEYOPT *) ;

#ifdef	COMMENT
extern int keyopt_findvalue(KEYOPT *,const char *,
			const char *,int,KEYOPT_VALUE **) ;
#endif /* COMMENT */

#ifdef	__cplusplus
}
#endif

#endif /* KEYOPT_MASTER */

#endif /* KEYOPT_INCLUDE */


