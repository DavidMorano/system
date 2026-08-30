/* kshvar SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* KSH variable framework */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	KSHVAR_INCLUDE
#define	KSHVAR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */


#define	KSHVAR		struct kshvar_head
#define	KSHVAR_MAGIC	0x49736218


struct kshvar_head {
	vecobj		vars ;
	uint		magval ;
} ; /* end struct */

struct kshvar_c {
	int		i ;
} ; /* end struct */

struct kshvar_var {
	cchar		*varname ;
	cchar		*soname ;
	int		refcount ;
} ; /* end struct */

EXTERNC_begin

extern int	kshvar_start(KSHVAR *) noex ;
extern int	kshvar_store(KSHVAR *,KSHVAR_VAR *) noex ;
extern int	kshvar_fetch(KSHVAR *,KSHVAR_VAR *) noex ;
extern int	kshvar_curbegin(KSHVAR *,KSHVAR_CUR *) noex ;
extern int	kshvar_curenum(KSHVAR *,KSHVAR_CUR *,KSHVAR_VAR *) noex ;
extern int	kshvar_curend(KSHVAR *,KSHVAR_CUR *) noex ;
extern int	kshvar_finish(KSHVAR *) noex ;

EXTERNC_end


#endif /* KSHVAR_INCLUDE */


