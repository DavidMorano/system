/* ktag HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	KTAG_INCLUDE
#define	KTAG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<txtindexmk.h>
#include	<eigendb.h>


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
	KTAG_PARAMS	*kap ;
	TXTINDEXMK_KEY	*tkeys ;	/* storage for TXTMKINDEXMK_ADDTAGS */
	cchar		*fname ;
	vecobj		keys ;
	vecstr		store ;
	ulong		recoff ;
	ulong		reclen ;
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


