/* modload HEADER */
/* charset=ISO8859-1 */
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
#define	MODLOAD_SYMSUF		"modinfo"

enum modloados {
    modloado_libvar,
    modloado_libprs,
    modloado_libsdirs,
    modloado_avail,
    modloado_self,
    modloado_overlast
} ; /* end enum (modloados) */

#ifdef	__cplusplus

struct modloadms {
    	static cint	libvar ;
    	static cint	libprs ;
    	static cint	libsdirs ;
    	static cint	avail ;
    	static cint	self ;
} ; /* end structenum (modloadms) */

#endif /* __cplusplus */

/* options */
#define	MODLOAD_OLIBVAR		(1 << modloado_libvar)
#define	MODLOAD_OLIBPRS		(1 << modloado_libprs)
#define	MODLOAD_OLIBSDIRS	(1 << modloado_libsdirs)
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

#ifdef	__cplusplus
enum modloadmems {
	modloadmem_close,
	modloadmem_overlast
} ;
struct modload ;
struct modload_co {
	modload		*op = nullptr ;
	int		w = -1 ;
	void operator () (modload *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (modload_co) */
struct modload : modload_head {
	modload_co	close ;
	modload() noex {
	    close(this,modloadmem_close) ;
	} ;
	modload(const modload &) = delete ;
	modload &operator = (const modload &) = delete ;
	int open(cchar *,cchar *,cchar *,int,mainv) noex ;
	int getmv(int) noex ;
	int getmva(int *,int) noex ;
	int getsym(cchar *,cvoid **) noex ;
	void dtor() noex ;
	destruct modload() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (modload) */
#else	/* __cplusplus */
typedef MODLOAD		modload ;
#endif /* __cplusplus */

typedef MODLOAD_MI	modload_mi ;

EXTERNC_begin

extern int modload_open(modload *,cchar *,cchar *,cchar *,int,mainv) noex ;
extern int modload_getmv(modload *,int) noex ;
extern int modload_getmva(modload *,int *,int) noex ;
extern int modload_getsym(modload *,cchar *,cvoid **) noex ;
extern int modload_close(modload *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const modloadms	modloadm ;
#endif /* __cplusplus */


#endif /* MODLOAD_INCLUDE */


