/* varbabies HEADER */
/* lang=C20 */

/* KSH variable BABIES */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	VARBABIES_INCLUDE
#define	VARBABIES_INCLUDE


#include	<envstandards.h>	/* first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<vecobj.h>
#include	<localmisc.h>


#define	VARBABIES	struct varbabies_head


struct varbabies_head {
	vecobj		vars ;
	uint		magic ;
} ;

struct varbabies_c {
	int		i ;
} ;

struct varbabies_var {
	cchar		*varname ;
	cchar		*soname ;
	int		refcount ;
} ;

EXTERNC_begin

extern int varbabies_set(VARBABIES *) noex ;
extern int varbabies_store(VARBABIES *,VARBABIES_VAR *) noex ;
extern int varbabies_fetch(VARBABIES *,VARBABIES_VAR *) noex ;
extern int varbabies_enum(VARBABIES *,VARBABIES_CUR *,VARBABIES_VAR *) noex ;
extern int varbabies_finish(VARBABIES *) noex ;

EXTERNC_end


#endif /* VARBABIES_INCLUDE */




