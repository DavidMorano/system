/* article SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage an ARTICLE object */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_SAFE		1		/* safety */

/* revision history:

	= 1995-05-01, David A�D� Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	article

	Description:
	This little object manages some particulars about an
	bulletin-board article.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"article.h"


/* local defines */


/* external subroutines */

extern int	sfshrink(cchar *,int,cchar **) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


int article_start(ARTICLE *op)
{
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	memset(op,0,sizeof(ARTICLE)) ;
	op->clen = -1 ;
	op->clines = -1 ;

	if ((rs = ng_start(&op->ngs)) >= 0) {
	    op->fl.ngs = TRUE ;
	    if ((rs = retpath_start(&op->path)) >= 0) {
	        op->fl.path = TRUE ;
		if ((rs = vechand_start(&op->envdates,1,0)) >= 0) {
	            op->fl.envdates = TRUE ;
		}
		if (rs < 0) {
	            op->fl.path = TRUE ;
	    	    retpath_finish(&op->path) ;
		}
	    }
	    if (rs < 0) {
	        op->fl.ngs = FALSE ;
		ng_finish(&op->ngs) ;
	    }
	} /* end if (ngs) */
 
	return rs ;
}
/* end subroutine (article_start) */


int article_finish(ARTICLE *op)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

#if	CF_DEBUGS
	debugprintf("article_finish: ent\n") ;
#endif

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

#if	CF_DEBUGS
	debugprintf("article_finish: f_msgdate=%u\n",op->fl.msgdate) ;
#endif

	if (op->fl.msgdate) {
	    op->fl.msgdate = FALSE ;
	    rs1 = dater_finish(&op->msgdate) ;
	    if (rs >= 0) rs = rs1 ;
	}

#if	CF_DEBUGS
	debugprintf("article_finish: f_envdates=%u\n",op->fl.envdates) ;
#endif

	if (op->fl.envdates) {
	    DATER	*dp ;
	    int		i ;
	    op->fl.envdates = FALSE ;
	    for (i = 0 ; vechand_get(&op->envdates,i,&dp) >= 0 ; i += 1) {
		rs1 = dater_finish(dp) ;
		if (rs >= 0) rs = rs1 ;
		rs1 = uc_free(dp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end for */
	    rs1 = vechand_finish(&op->envdates) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (envdates) */

#if	CF_DEBUGS
	debugprintf("article_finish: f_path=%u\n",op->fl.path) ;
#endif
	if (op->fl.path) {
	    op->fl.path = FALSE ;
	    rs1 = retpath_finish(&op->path) ;
	    if (rs >= 0) rs = rs1 ;
	}

#if	CF_DEBUGS
	debugprintf("article_finish: f_ngs=%u\n",op->fl.ngs) ;
#endif

	if (op->fl.ngs) {
	    op->fl.ngs = FALSE ;
	    rs1 = ng_finish(&op->ngs) ;
	    if (rs >= 0) rs = rs1 ;
	}

#if	CF_DEBUGS
	debugprintf("article_finish: addrs\n") ;
#endif

	for (i = 0 ; i < articleaddr_overlast ; i += 1) {
	    if (op->af[i]) {
	        op->af[i] = FALSE ;
	        rs1 = ema_finish(&op->addr[i]) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

#if	CF_DEBUGS
	debugprintf("article_finish: strs\n") ;
#endif

	for (i = 0 ; i < articlestr_overlast ; i += 1) {
	    if (op->strs[i] != NULL) {
	        rs1 = uc_free(op->strs[i]) ;
	        if (rs >= 0) rs = rs1 ;
	        op->strs[i] = NULL ;
	    }
	} /* end for */

#if	CF_DEBUGS
	debugprintf("article_finish: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (article_finish) */


int article_addpath(ARTICLE *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (! op->fl.path) {
	    op->fl.path = TRUE ;
	    rs = retpath_start(&op->path) ;
	}

	if (rs >= 0) {
	    rs = retpath_parse(&op->path,sp,sl) ;
	}

	return rs ;
}
/* end subroutine (article_addpath) */


int article_addng(ARTICLE *op,cchar *sp,int sl)
{
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (! op->fl.ngs) {
	    op->fl.ngs = TRUE ;
	    rs = ng_start(&op->ngs) ;
	}

	if (rs >= 0) {
	    rs = ng_addparse(&op->ngs,sp,sl) ;
	}

	return rs ;
}
/* end subroutine (article_addng) */


int article_addenvdate(ARTICLE *op,DATER *d2p)
{
	DATER		*dp ;
	const int	msize = sizeof(DATER) ;
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (d2p == NULL) return SR_FAULT ;

	if ((rs = uc_malloc(msize,&dp)) >= 0) {
	    if ((rs = dater_startcopy(dp,d2p)) >= 0) {
		rs = vechand_add(&op->envdates,dp) ;
		if (rs < 0)
		    dater_finish(dp) ;
	    }
	    if (rs < 0)
		uc_free(dp) ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (article_addenvdate) */


int article_addmsgdate(ARTICLE *op,DATER *dp)
{
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;
	if (dp == NULL) return SR_FAULT ;

	if (! op->fl.msgdate) {
	    op->fl.msgdate = TRUE ;
	    rs = dater_start(&op->msgdate,NULL,NULL,0) ;
	}

	if (rs >= 0) {
	    rs = dater_setcopy(&op->msgdate,dp) ;
	}

	return rs ;
}
/* end subroutine (article_addmsgdate) */


int article_addaddr(ARTICLE *op,int type,cchar *sp,int sl)
{
	const int	n = articleaddr_overlast ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if ((type < 0) || (type >= n)) return SR_INVALID ;

	if (! op->af[type]) {
	    op->af[type] = TRUE ;
	    rs = ema_start(&op->addr[type]) ;
	}

	if (rs >= 0) {
	    rs = ema_parse(&op->addr[type],sp,sl) ;
	}

	return rs ;
}
/* end subroutine (article_addaddr) */


int article_addstr(ARTICLE *op,int type,cchar *sp,int sl)
{
	const int	n = articlestr_overlast ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if ((type < 0) || (type >= n)) return SR_INVALID ;

	if (op->strs[type] != NULL) {
	    rs = uc_free(op->strs[type]) ;
	    op->strs[type] = NULL ;
	}

	if (rs >= 0) {
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(sp,sl,&cp)) > 0) {
		op->strs[type] = cp ;
		rs = (rs-1) ;
	    }
	}

	return rs ;
}
/* end subroutine (article_addstr) */


/* extract newsgroup names from the "newsgroups" header string */
int article_addparse(ARTICLE *op,cchar *sp,int sl)
{
	EMA		aid ;
	EMA_ENT		*ep ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	if (sl < 0) sl = strlen(sp) ;

#if	CF_DEBUGS
	debugprintf("article_addparse: ent\n") ;
	debugprintf("article_addparse: > %r\n",sp,sl) ;
#endif

	if ((rs = ema_start(&aid)) >= 0) {
	    if ((rs = ema_parse(&aid,sp,sl)) > 0) {
		int	i ;
		int	cl ;
		cchar	*cp ;

#if	CF_DEBUGS
	        debugprintf("article_addparse: got some ema\n") ;
#endif

	        for (i = 0 ; ema_get(&aid,i,&ep) >= 0 ; i += 1) {
	            if (ep != NULL) {

#if	CF_DEBUGS
	            debugprintf("article_addparse: ema entry\n") ;
#endif

	            if ((ep->fl.error) || (ep->al <= 0)) continue ;

	            if ((cl = sfshrink(ep->ap,ep->al,&cp)) > 0) {
			n += 1 ;
	                rs = ng_add(&op->ngs,cp,cl,NULL) ;
		    }

		    }
	            if (rs < 0) break ;
	        } /* end for */

	    } /* end if (parse) */
	    rs1 = ema_finish(&aid) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ema) */

#if	CF_DEBUGS
	debugprintf("article_addparse: ret rs=%d n=%u\n",rs,n) ;
#endif

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (article_addparse) */


int article_ao(ARTICLE *op,uint aoff,uint alen)
{

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	op->aoff = aoff ;
	op->alen = alen ;
	return SR_OK ;
}
/* end subroutine (article_ao) */


int article_countenvdate(ARTICLE *op)
{
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	rs = vechand_count(&op->envdates) ;
	return rs ;
}
/* end subroutine (article_countenvdate) */


int article_getenvdate(ARTICLE *op,int i,DATER **epp)
{
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	rs = vechand_get(&op->envdates,i,epp) ;
	return rs ;
}
/* end subroutine (article_getenvdate) */


int article_getstr(ARTICLE *op,int type,cchar **rpp)
{
	const int	n = articlestr_overlast ;
	int		rs = SR_OK ;
	cchar	*sp ;

	if (op == NULL) return SR_FAULT ;

	if ((type < 0) || (type >= n)) return SR_INVALID ;

	sp = op->strs[type] ;
	if (sp == NULL) rs = SR_NOTFOUND ;

	if (rs >= 0) rs = strlen(sp) ;

	if (rpp != NULL) {
	    *rpp = (rs >= 0) ? sp : NULL ;
	}

	return rs ;
}
/* end subroutine (article_getstr) */


int article_getaddrema(ARTICLE *op,int type,EMA **rpp)
{
	const int	n = articleaddr_overlast ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if ((type < 0) || (type >= n)) return SR_INVALID ;

	if (! op->af[type]) {
	    op->af[type] = TRUE ;
	    rs = ema_start(&op->addr[type]) ;
	}

	if (rpp != NULL) {
	    *rpp = (rs >= 0) ? (op->addr + type) : NULL ;
	}

	return rs ;
}
/* end subroutine (article_getaddrema) */


