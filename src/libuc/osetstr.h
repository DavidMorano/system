/* osetstr HEADER */
/* lang=C20 */

/* ordered set of strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OSETSTR_INCLUDE
#define	OSETSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#define	OSETSTR_MAGIC	0x97351229
#define	OSETSTR		struct osetstr_head
#define	OSETSTR_CUR	struct osetstr_cursor


struct osetstr_cursor {
	void		*interp ;
} ;

struct osetstr_head {
	void		*setp ;
	uint		magic ;
} ;

typedef OSETSTR		osetstr ;
typedef OSETSTR_CUR	osetstr_cur ;

EXTERNC_begin

extern int osetstr_start(osetstr *,int) noex ;
extern int osetstr_already(osetstr *,cchar *,int) noex ;
extern int osetstr_add(osetstr *,cchar *,int) noex ;
extern int osetstr_del(osetstr *,cchar *,int) noex ;
extern int osetstr_delall(osetstr *) noex ;
extern int osetstr_count(osetstr *) noex ;
extern int osetstr_curbegin(osetstr *,osetstr_cur *) noex ;
extern int osetstr_enum(osetstr *,osetstr_cur *,cchar **) noex ;
extern int osetstr_curend(osetstr *,osetstr_cur *) noex ;
extern int osetstr_finish(osetstr *) noex ;

EXTERNC_end


#endif /* OSETSTR_INCLUDE */


