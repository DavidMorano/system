/* varbabies HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* KSH variable BABIES */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	VARBABIES_INCLUDE
#define	VARBABIES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecobj.h>


#define	VARBABIES	struct varbabies_head
#define	VARBABIES_CUR	struct varbabies_cursor
#define	VARBABIES_VAR	struct varbabies_variable


struct varbabies_head {
	vecobj		vars ;
	uint		magic ;
} ;

struct varbabies_cursor {
	int		i ;
} ;

struct varbabies_variable {
	cchar		*varname ;
	cchar		*soname ;
	int		refcount ;
} ;

typedef VARBABIES	varbabies ;
typedef VARBABIES_CUR	varbabies_cur ;
typedef VARBABIES_VAR	varbabies_var ;

EXTERNC_begin

extern int varbabies_set(varbabies *) noex ;
extern int varbabies_store(varbabies *,varbabies_var *) noex ;
extern int varbabies_fetch(varbabies *,varbabies_var *) noex ;
extern int varbabies_curbegin(varbabies *,varbabies_cur *) noex ;
extern int varbabies_curenum(varbabies *,varbabies_cur *,varbabies_var *) noex ;
extern int varbabies_curend(varbabies *,varbabies_cur *) noex ;
extern int varbabies_finish(varbabies *) noex ;

EXTERNC_end


#endif /* VARBABIES_INCLUDE */


