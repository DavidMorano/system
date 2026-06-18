/* biblepara SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEPARA object-load management */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEPARA_INCLUDE
#define	BIBLEPARA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<modload.h>		/* LIBUC */
#include	<bibleparas.h>		/* LIBDAM */


#define	BIBLEPARA		struct biblepara_head
#define	BIBLEPARA_FL		struct biblepara_flags
#define	BIBLEPARA_CITE		struct biblepara_query
#define	BIBLEPARA_Q		struct biblepara_query
#define	BIBLEPARA_CUR		struct biblepara_cursor
#define	BIBLEPARA_INFO		struct biblepara_information
#define	BIBLEPARA_MAGIC		0x99447246


struct biblepara_flags {
        uint		modload:1 ;
} ; /* end struct */

struct biblepara_information {
	time_t		dbtime ;	/* db-time */
	time_t		vitime ;	/* vi-time */
	uint		maxbook ;
	uint		maxchap ;
	uint		nverses ;
	uint		nzverses ;
} ; /* end struct */

struct biblepara_query {
	uchar		b, c, v ;
} ; /* end struct */

struct biblepara_cursor {
	void		*scp ;
	uint		magval ;
} ; /* end struct */

struct biblepara_head {
	modload		*mlp ;
	void		*callp ;
	void		*obj ;		/* object pointer */
	BIBLEPARA_FL	fl ;
	uint		magval ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ; /* end struct */

typedef	BIBLEPARA		biblepara ;
typedef	BIBLEPARA_FL		biblepara_fl ;
typedef	BIBLEPARA_CITE		biblepara_cute ;
typedef	BIBLEPARA_Q		biblepara_q ;
typedef	BIBLEPARA_CUR		biblepara_cur ;
typedef	BIBLEPARA_INFO		biblepara_info ;

EXTERNC_begin

extern int	biblepara_open(biblepara *,cchar *,cchar *) noex ;
extern int	biblepara_count(biblepara *) noex ;
extern int	biblepara_ispara(biblepara *,con biblepara_q *) noex ;
extern int	biblepara_curbegin(biblepara *,biblepara_cur *) noex ;
extern int	biblepara_curend(biblepara *,biblepara_cur *) noex ;
extern int	biblepara_curenum(biblepara *,biblepara_cur *,
			biblepara_q *) noex ;
extern int	biblepara_audit(biblepara *) noex ;
extern int	biblepara_getinfo(biblepara *,biblepara_info *) noex ;
extern int	biblepara_close(biblepara *) noex ;

EXTERNC_end


#endif /* BIBLEPARA_INCLUDE */


