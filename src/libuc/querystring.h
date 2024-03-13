/* querystring HEADER */
/* lang=C99 */

/* Query-String manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-09-25, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#ifndef	QUERYSTRING_INCLUDE
#define	QUERYSTRING_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<strpack.h>


#define	QUERYSTRING	struct querystring_head
#define	QUERYSTRING_FL	struct querystring_flags
#define	QUERYSTRING_CUR	struct querystring_cur


struct querystring_cur {
	int		i ;
} ;

struct querystring_flags {
	uint		packer:1 ;
} ;

struct querystring_head {
	QUERYSTRING_FL	open ;
	strpack		packer ;
	cchar		*(*kv)[2] ;
	int		n ;
} ;

typedef QUERYSTRING	querystring ;
typedef QUERYSTRING_FL	querystring_fl ;
typedef QUERYSTRING_CUR	querystring_cur ;

EXTERNC_begin

extern int querystring_start(querystring *,cchar *,int) noex ;
extern int querystring_count(querystring *) noex ;
extern int querystring_already(querystring *,cchar *,int) noex ;
extern int querystring_curbegin(querystring *,querystring_cur *) noex ;
extern int querystring_curend(querystring *,querystring_cur *) noex ;
extern int querystring_fetch(querystring *,cchar *,int,querystring_cur *,
		cchar **) noex ;
extern int querystring_enum(querystring *,querystring_cur *,cchar **,
		cchar **) noex ;
extern int querystring_get(querystring *,int,cchar **,cchar **) noex ;
extern int querystring_finish(querystring *) noex ;

EXTERNC_end


#endif /* QUERYSTRING_INCLUDE */


