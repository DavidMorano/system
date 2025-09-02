/* kshvar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* KSH variable framework */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	KSHVAR_INCLUDE
#define	KSHVAR_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	KSHVAR		struct kshvar_head


struct kshvar_head {
	vecobj		vars ;
	uint		magic ;
} ;

struct kshvar_c {
	int		i ;
} ;

struct kshvar_var {
	cchar		*varname ;
	cchar		*soname ;
	int		refcount ;
} ;

EXTERNC_begin

extern int	kshvar_start(KSHVAR *) noex ;
extern int	kshvar_store(KSHVAR *,KSHVAR_VAR *) noex ;
extern int	kshvar_fetch(KSHVAR *,KSHVAR_VAR *) noex ;
extern int	kshvar_enum(KSHVAR *,KSHVAR_CUR *,KSHVAR_VAR *) noex ;
extern int	kshvar_finish(KSHVAR *) noex ;

EXTERNC_end


#endif /* KSHVAR_INCLUDE */


