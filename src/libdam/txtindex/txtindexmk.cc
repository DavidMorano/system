/* txtindexmk SUPPORT */
/* lang=C++20 */

/* interface to the TIMS loadable object */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	txtindexmk

	Description:
	This module implements an interface (a trivial one) that
	provides access to the TIM object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<getnodename.h>
#include	<mkpr.h>
#include	<estrings.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"txtindexmk.h"
#include	"txtindexmks.h"


/* local defines */

#define	TIM_DEFENTS	(44 * 1000)
#define	TIM_MODBNAME	"txtindexmks"
#define	TIM_OBJNAME	"txtindexmks"
#define	TIM		txtindexmk
#define	TIM_PA		txtindexmk_pa
#define	TIM_TAG		txtindexmk_tag
#define	TIM_KEY		txtindexmk_key
#define	TIM_ENTS	txtindexmk_ents

#define	VARPRNAME	"LOCAL"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


#undef	TIS_TAG
#define	TIS_TAG		txtindexmks_tag

#undef	TIS_KEY
#define	TIS_KEY		txtindexmks_key


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	txtindexmk_objloadbegin(TIM *,cchar *,cchar *) noex ;
static int	txtindexmk_objloadend(TIM *) noex ;
static int	txtindexmk_loadcalls(TIM *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* global variables */


/* local variables */

enum subs {
	sub_open,
	sub_addeigens,
	sub_addtags,
	sub_close,
	sub_noop,
	sub_abort,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"addeigens",
	"addtags",
	"close",
	"noop",
	"abort",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int txtindexmk_open(TIM *op,TIM_PA *pp,cchar *db,int of,mode_t om)
{
	int		rs ;
	cchar	*objname = TIM_OBJNAME ;
	char		dbuf[MAXHOSTNAMELEN+1] ;

	if (op == nullptr) return SR_FAULT ;
	if (db == nullptr) return SR_FAULT ;

	if (db[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(TIM)) ;

	if ((rs = getnodedomain(nullptr,dbuf)) >= 0) {
	    const int	plen = MAXPATHLEN ;
	    cchar	*pn = VARPRNAME ;
	    char	pbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpr(pbuf,plen,pn,dbuf)) >= 0) {
		cchar	*pr = pbuf ;
		if ((rs = txtindexmk_objloadbegin(op,pr,objname)) >= 0) {
	    	    if ((rs = (*op->call.open)(op->obj,pp,db,of,om)) >= 0) {
			op->magic = TIM_MAGIC ;
	    	    }
	    	    if (rs < 0)
			txtindexmk_objloadend(op) ;
		} /* end if (txtindexmk_objloadbegin) */
	    } /* end if (mkpr) */
	} /* end if (getnodedomain) */

	return rs ;
}
/* end subroutine (txtindexmk_open) */


/* free up the entire vector string data structure object */
int txtindexmk_close(TIM *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TIM_MAGIC) return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = txtindexmk_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (txtindexmk_close) */


int txtindexmk_addeigens(TIM *op,TIM_KEY keys[],int nkeys)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TIM_MAGIC) return SR_NOTOPEN ;

	rs = (*op->call.addeigens)(op->obj,keys,nkeys) ;

	return rs ;
}
/* end subroutine (txtindexmk_addeigens) */


int txtindexmk_addtags(TIM *op,TIM_TAG tags[],int ntags)
{
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TIM_MAGIC) return SR_NOTOPEN ;

	rs = (*op->call.addtags)(op->obj,tags,ntags) ;

	return rs ;
}
/* end subroutine (txtindexmk_addtags) */


int txtindexmk_noop(TIM *op)
{
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != TIM_MAGIC) return SR_NOTOPEN ;

	if (op->call.noop != nullptr) {
	    rs = (*op->call.noop)(op->obj) ;
	}

	return rs ;
}
/* end subroutine (txtindexmk_noop) */


/* private subroutines */


/* find and load the DB-access object */
static int txtindexmk_objloadbegin(TIM *op,cchar *pr,cchar *objname)
{
	MODLOAD		*lp = &op->loader ;
	VECSTR		syms ;
	const int	n = nelem(subs) ;
	const int	vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;

	if ((rs = vecstr_start(&syms,n,vo)) >= 0) {
	    const int	nlen = SYMNAMELEN ;
	    int		i ;
	    int		f_modload = false ;
	    char	nbuf[SYMNAMELEN + 1] ;

	    for (i = 0 ; (i < n) && (subs[i] != nullptr) ; i += 1) {
	        if (isrequired(i)) {
	            if ((rs = sncpy3(nbuf,nlen,objname,"_",subs[i])) >= 0) {
			rs = vecstr_add(&syms,nbuf,rs) ;
		    }
		}
		if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
		cchar	**sv ;
	        if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
	            cchar	*mn = TIM_MODBNAME ;
	            const int	mo = (MODLOAD_OLIBVAR | MODLOAD_OSDIRS) ;
	            if ((rs = modload_open(lp,pr,mn,objname,mo,sv)) >= 0) {
	    		int	mv[2] ;
			f_modload = true ;
	    		if ((rs = modload_getmva(lp,mv,2)) >= 0) {
			    void	*p ;
			    op->objsize = mv[0] ;
		            op->cursize = mv[1] ;
		            if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
		                op->obj = p ;
		                rs = txtindexmk_loadcalls(op,objname) ;
		                if (rs < 0) {
			            uc_free(op->obj) ;
			            op->obj = nullptr ;
		                }
		            } /* end if (memory-allocation) */
	                } /* end if (modload_getmva) */
	                if (rs < 0) {
			    f_modload = false ;
		            modload_close(lp) ;
			}
	            } /* end if (modload_open) */
		} /* end if (vecstr_getvec) */
	    } /* end if (ok) */

	    rs1 = vecstr_finish(&syms) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_modload) {
	        modload_close(lp) ;
	    }
	} /* end if (vecstr-syms) */

	return rs ;
}
/* end subroutine (txtindexmk_objloadbegin) */


static int txtindexmk_objloadend(TIM *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->obj != nullptr) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}

	rs1 = modload_close(&op->loader) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (txtindexmk_objloadend) */


static int txtindexmk_loadcalls(TIM *op,cchar objname[])
{
	MODLOAD		*lp = &op->loader ;
	const int	nlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	char		nbuf[SYMNAMELEN + 1] ;
	const void	*snp ;

	for (i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(nbuf,nlen,objname,"_",subs[i])) >= 0) {
	         if ((rs = modload_getsym(lp,nbuf,&snp)) == SR_NOTFOUND) {
		     snp = nullptr ;
		     if (! isrequired(i)) rs = SR_OK ;
		}
	    }

	    if (rs < 0) break ;

	    if (snp != nullptr) {

	        c += 1 ;
		switch (i) {

		case sub_open:
		    op->call.open = (int (*)(void *,TIMS_PA *,
			cchar *,int,int)) snp ;
		    break ;

		case sub_addeigens:
		    op->call.addeigens = 
			(int (*)(void *,TIS_KEY *,int)) snp ;
		    break ;

		case sub_addtags:
		    op->call.addtags = 
			(int (*)(void *,TIS_TAG *,int)) snp ;
		    break ;

		case sub_noop:
		    op->call.noop = (int (*)(void *)) snp ;
		    break ;

		case sub_abort:
		    op->call.abort = (int (*)(void *)) snp ;
		    break ;

		case sub_close:
		    op->call.close = (int (*)(void *)) snp ;
		    break ;

		} /* end switch */

	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (txtindexmk_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_addeigens:
	case sub_addtags:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


