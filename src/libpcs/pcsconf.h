/* pcsconf HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* access the PCS configuration information */
/* version %I% last-modified %G% */


/* revision history:

	= 1992-03-10, David A­D­ Morano
	This module was originally written.

	= 1998-04-03, David A­D­ Morano
	This was modified for more general use.

	= 2008-10-07, David A­D­ Morano
        This was modified to allow for the main part to be a loadable
        module.

*/

/* Copyright © 1992,1998,2008 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSCONF_INCLUDE
#define	PCSCONF_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>
#include	<ptm.h>

#include	"pcsconfs.h"


#define	PCSCONF		struct pcsconf_head
#define	PCSCONF_FL	struct pcsconf_flags
#define	PCSCONF_CUR	struct pcsconf_cursor
#define	PCSCONF_CA	struct pcsconf_calls
#define	PCSCONF_MAGIC	0x97677246
#define	PCSCONF_USER	"pcs"


EXTERNC_begin

struct pcsconf_calls {
	int	(*start)(void *,cchar *,cchar **,cchar *) ;
	int	(*curbegin)(void *,void *) ;
	int	(*fetch)(void *,cchar *,int,void *,char *,int) ;
	int	(*enumerate)(void *,void *,char *,int,char *,int) ;
	int	(*curend)(void *,void *) ;
	int	(*audit)(void *) ;
	int	(*finish)(void *) ;
} ;

EXTERNC_end

struct pcsconf_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magic ;
} ;

struct pcsconf_flags {
	uint		defaults:1 ;
} ;

struct pcsconf_head {
	modload		loader ;
	ptm		mx ;
	void		*obj ;		/* object pointer */
	void		*cookmgr ;	/* cookie-manager */
	cchar		*pr ;		/* supplied program-root */
	cchar		**envv ;	/* supplied environment */
	cchar		*pcsusername ;	/* calculated */
	PCSCONF_CA	call ;
	uid_t		uid_pcs ;
	gid_t		gid_pcs ;
	PCSCONF_FL	fl ;
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;

typedef	PCSCONF		pcsconf ;
typedef	PCSCONF_FL	pcsconf_fl ;
typedef	PCSCONF_CUR	pcsconf_cur ;
typedef	PCSCONF_CA	pcsconf_ca ;

EXTERNC_begin

extern int pcsconf_start(pcsconf *,cchar *,cchar **,cchar *) noex ;
extern int pcsconf_curbegin(pcsconf *,pcsconf_cur *) noex ;
extern int pcsconf_fetch(pcsconf *,cchar *,int,pcsconf_cur *,char *,int) noex ;
extern int pcsconf_enum(pcsconf *,pcsconf_cur *,char *,int,char *,int) noex ;
extern int pcsconf_curend(pcsconf *,pcsconf_cur *) noex ;
extern int pcsconf_fetchone(pcsconf *,cchar *,int,char *,int) noex ;
extern int pcsconf_audit(pcsconf *) noex ;
extern int pcsconf_getpcsuid(pcsconf *) noex ;
extern int pcsconf_getpcsgid(pcsconf *) noex ;
extern int pcsconf_getpcsusername(pcsconf *,char *,int) noex ;
extern int pcsconf_getpr(pcsconf *,cchar **) noex ;
extern int pcsconf_getenvv(pcsconf *,cchar ***) noex ;
extern int pcsconf_finish(pcsconf *) noex ;

EXTERNC_end


#endif /* PCSCONF_INCLUDE */


