/* modload HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* module-load management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MODLOAD_INCLUDE
#define	MODLOAD_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	MODLOAD			struct modload_head
#define	MODLOAD_MI		struct modload_mid
#define	MODLOAD_MAGIC		0x99447246
#define	MODLOAD_DEFENTS		(44 * 1000)
#define	MODLOAD_SYMSUF		"_modinfo"

enum modloados {
    modloado_libvar,
    modloado_prs,
    modloado_sdirs,
    modloado_avail,
    modloado_self,
    modloado_overlast
} ;

#ifdef	__cplusplus

struct modloadms {
    	static cint	libvar ;
    	static cint	prs ;
    	static cint	sdirs ;
    	static cint	avail ;
    	static cint	self ;
} ;

#endif /* __cplusplus */

/* options */
#define	MODLOAD_OLIBVAR		(1 << modloado_libvar)
#define	MODLOAD_OPRS		(1 << modloado_prs)
#define	MODLOAD_OSDIRS		(1 << modloado_sdirs)
#define	MODLOAD_OAVAIL		(1 << modloado_avail)
#define	MODLOAD_OSELF		(1 << modloado_self)


struct modload_mid {
	cchar		*name ;
	int		mv[1] ;
} ;

struct modload_head {
	void		*sop ;		/* shared-object (SO) pointer */
	cchar		*modname ;
	MODLOAD_MI	*midp ;
	uint		magic ;
} ;

typedef MODLOAD		modload ;
typedef MODLOAD_MI	modload_mi ;

EXTERNC_begin

extern int modload_open(modload *,cchar *,cchar *,cchar *,int,mainv) noex ;
extern int modload_getmv(modload *,int) noex ;
extern int modload_getmva(modload *,int *,int) noex ;
extern int modload_getsym(modload *,cchar *,cvoid **) noex ;
extern int modload_close(modload *) noex ;

EXTERNC_end


#endif /* MODLOAD_INCLUDE */


