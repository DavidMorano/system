/* ktag HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* tag accummulator object */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	KTAG_INCLUDE
#define	KTAG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<txtindexmk.h>
#include	<eigendb.h>


#define	KTAG_MAGIC	0x70811238
#define	KTAG		struct ktag_head
#define	KTAG_PA		struct ktag_params
#define	KTAG_KEY	TXTINDEXMK_KEY
#define	KTAG_TAG	TXTINDEXMK_TAG


struct ktag_params {
	eigendb		*edbp ;
	uchar		*wterms ;
	int		minwlen ;
	int		f_eigen ;
} ;

struct ktag_head {
	KTAG_PA		*kap ;
	KTAG_KEY	*tkeys ;	/* storage for TXTMKINDEXMK_ADDTAGS */
	cchar		*fname ;
	vecobj		*klp ;		/* key-list-pointer */
	vecstr		*slp ;		/* string-list-pointer */
	ulong		recoff ;
	ulong		reclen ;
	uint		magic ;
	int		f_store ;
} ;

typedef	KTAG		ktag ;
typedef	KTAG_PA		ktag_pa ;
typedef	KTAG_KEY	ktag_key ;
typedef	KTAG_TAG	ktag_tag ;

EXTERNC_begin

extern int	ktag_start(ktag *,ktag_pa *,uint,cchar *,int) noex ;
extern int	ktag_add(ktag *,cchar *,int) noex ;
extern int	ktag_procline(ktag *,cchar *,int) noex ;
extern int	ktag_procword(ktag *,cchar *,int) noex ;
extern int	ktag_mktag(ktag *,uint,ktag_tag *) noex ;
extern int	ktag_storelc(ktag *,cchar **,cchar *,int) noex ;
extern int	ktag_finish(ktag *) noex ;

EXTERNC_end


#endif /* KTAG_INCLUDE */


