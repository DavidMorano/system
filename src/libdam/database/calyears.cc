/* calyears SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* CALYEARS object implementation */
/* version %I% last-modified %G% */

#define	CF_SAFE		1		/* normal safety */
#define	CF_TRANSHOL	1		/* translate holidays */
#define	CF_MKDNAME	0		/* |mkdname()| */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	calyears

	Description:
	This module manages access to the various CALENDAR databases
	either in the distribution or specified by the caller.

	Implementation notes:

	= parsing a calendar file

	There are several valid forms for the date (month-day) part of
	a calendar entry.  These are:
		mm/dd		most common
		name[±ii]	name plus-or-minus increment in days

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<estrings.h>
#include	<mktmp.h>
#include	<vecobj.h>
#include	<sbuf.h>
#include	<char.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<dayofmonth.h>
#include	<tmtime.hh>
#include	<getusername.h>
#include	<fsdir.h>
#include	<localmisc.h>
#include	<mkpathx.h>
#include	<six.h>
#include	<ischarx.h>
#include	<cfdec.h>

#include	"calyears.h"
#include	"calmgr.h"
#include	"calworder.h"
#include	"cyi.h"
#include	"cyimk.h"


/* local defines */

#define	CALYEARS_DBSUF	"calendar"
#define	CALYEARS_DMODE	0777
#define	CALYEARS_DBDIR	"share/calendar"
#define	CALYEARS_DOMER	calyears_domer

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	IDXDNAME	".calyears"
#define	IDXSUF		"cyi"

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#define	HOLBUFLEN	100

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#ifndef	VCNAME
#define	VCNAME		"var"
#endif

#undef	NLINES
#define	NLINES		20

#define	CEBUFLEN	(NLINES * 3 * szof(int))

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)
#define	TO_CHECK	4

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct calyears_domer {
	DAYOFMONTH	dom ;
	int		year ;
} ;

struct subinfo_flags {
	uint		id:1 ;
	uint		dom:1 ;
	uint		hols:1 ;
	uint		dirs:1 ;	/* VECSTR was initialed */
} ;

struct subinfo {
	idx		id ;
	vecstr		dirs ;
	CALYEARS	*op ;
	cchar		*tudname ;
	cchar		*userhome ;
	cchar		**dns ;
	time_t		dt ;
	SI_FL		init, fl ;
	int		year ;
	int		isdst ;
	int		gmtoff ;	/* seconds west of GMT */
	char		username[USERNAMELEN + 1] ;
} ;


/* forward references */

static int	calyears_argbegin(CALYEARS *,cchar *) ;
static int	calyears_argend(CALYEARS *) ;

static int	calyears_opensub(CALYEARS *op,cchar **dns,cchar **cns) ;

static int	calyears_loadbuf(CALYEARS *,char *,int,CALENT *) ;
static int	calyears_calsdestroy(CALYEARS *) ;

static int	calyears_domerfins(CALYEARS *) ;
static int	calyears_domerbegin(CALYEARS *,CALYEARS_DOMER *,int) ;
static int	calyears_domerend(CALYEARS *,CALYEARS_DOMER *) ;
static int	calyears_domerget(CALYEARS *,CALYEARS_DOMER *,DAYOFMONTH **) ;

#ifdef	COMMENT
static int	calyears_checkupdate(CALYEARS *,time_t) ;
static int	calyears_mksysvarsi(CALYEARS *,cchar *) ;
#endif

static int	calyears_resultfins(CALYEARS *,CALYEARS_CUR *) ;
static int	calyears_lookmgr(CALYEARS *,vecobj *,CALMGR *,CALCITE *) ;
static int	calyears_mkresults(CALYEARS *,vecobj *,CALYEARS_CUR *) ;
static int	calyears_year(CALYEARS *,time_t) ;
static int	calyears_mkday(CALYEARS *,int,int,cchar *,int) ;
static int	calyars_domyear(CALYEARS *,int,DAYOFMONTH **) ;

static int	calyears_gethash(CALYEARS *,CALENT *,uint *) ;
static int	calyears_getcm(CALYEARS *,int,CALMGR **) ;
static int	calyears_samewords(CALYEARS *,CALENT *,CALENT *) ;
static int	calyears_getcalbase(CALYEARS *,CALENT *,cchar **) ;

#if	CF_TRANSHOL
static int	calyears_transhol(CALYEARS *,CALCITE *,int,cchar *,int) ;
static int	calyears_dayname(CALYEARS *,CALCITE *,int,cchar *,int) ;
static int	calyears_holidayer(CALYEARS *) ;
#endif /* CF_TRANSHOL */

static int	subinfo_start(SI *,CALYEARS *,time_t) ;
static int	subinfo_finish(SI *) ;
static int	subinfo_calscreate(SI *,cchar **,cchar **) ;
static int	subinfo_calscreater(SI *,cchar *,cchar **) ;
static int	subinfo_calcreate(SI *,cchar *,cchar *) ;
static int	subinfo_ids(SI *) ;
static int	subinfo_username(SI *) ;
static int	subinfo_mkdns(SI *) ;
static int	subinfo_havedir(SI *,cchar *) ;
static int	subinfo_loadnames(SI *,vecstr *,cchar *) ;
static int	subinfo_regacc(SI *,cchar *,int) ;

#if	CF_CHECKDNAME
static int	subinfo_checkdname(SI *,cchar *) noex ;
#endif

#ifdef	COMMENT
static int	subinfo_tmpuserdir(SI *) noex ;
#endif

static int	mkmonth(cchar *,int) noex ;

#if	CF_MKDNAME
static int	mkdname(cchar *,mode_t) noex ;
#endif

static int	vrcmp(cvoid *,cvoid *) noex ;

static int	isNotOrIllegalSeq(int) noex ;
static int	isNotHols(int) noex ;


/* local variables */

enum wdays {
	wday_sunday,
	wday_monday,
	wday_tuesday,
	wday_wednesday,
	wday_thursday,
	wday_friday,
	wday_saturday,
	wday_overlast
} ;

enum tdays {
	tday_first,
	tday_second,
	tday_third,
	tday_fourth,
	tday_fifth,
	tday_last,
	tday_overlast
} ;

constexpr cint	rsnotorils[] = {
	SR_NOTFOUND,
	SR_ILSEQ,
	0
} ;

constexpr cint	rsnothols[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ;


/* exported variables */

extern const calyears_obj	calyears_modinfo = {
	"calyears",
	szof(calyears),
	szof(calyears_cur)
} ;


/* exported subroutines */

int calyears_open(CALYEARS *op,cchar *pr,cchar **dns,cchar **cns) noex {
	int		rs ;
	int		c = 0 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif
	if (pr == NULL) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if (op->tmpdname == NULL) op->tmpdname = getenv(VARTMPDNAME) ;
	if (op->tmpdname == NULL) op->tmpdname = TMPDNAME ;

	if ((rs = calyears_argbegin(op,pr)) >= 0) {
	    cint	vo = VECHAND_OSTATIONARY ;
	    if ((rs = vechand_start(&op->cals,20,vo)) >= 0) {
		if ((rs = vechand_start(&op->doms,1,0)) >= 0) {
		    const time_t	dt = time(NULL) ;
		    if ((rs = calyears_year(op,dt)) >= 0) {
			if ((rs = calyears_opensub(op,dns,cns)) >= 0) {
	                   op->magic = CALYEARS_MAGIC ;
			}
		    }
		    if (rs < 0) {
			calyears_domerfins(op) ;
			vechand_finish(&op->doms) ;
		    }
		} /* end if (doms) */
	            if (rs < 0) {
	                calyears_calsdestroy(op) ;
	                vechand_finish(&op->cals) ;
	            }
	    } /* end if (cals) */
	    if (rs < 0)
	        calyears_argend(op) ;
	} /* end if (calyears_argbegin) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_open) */


/* free up the entire vector string data structure object */
int calyears_close(CALYEARS *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (op->open.hols) {
	    op->open.hols = FALSE ;
	    rs1 = holidayer_close(&op->hols) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = calyears_domerfins(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(&op->doms) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = calyears_calsdestroy(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(&op->cals) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = calyears_argend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->nentries = 0 ;
	op->magic = 0 ;
	return rs ;
}
/* end subroutine (calyears_close) */


int calyears_count(CALYEARS *op)
{
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	rs = op->nentries ;

	return rs ;
}
/* end subroutine (calyears_count) */


int calyears_audit(CALYEARS *op)
{
	CALMGR		*calp ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	for (i = 0 ; vechand_get(&op->cals,i,&calp) >= 0 ; i += 1) {
	    if (calp == NULL) continue ;
	    c += 1 ;
	    rs = calmgr_audit(calp) ;
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_audit) */


int calyears_curbegin(CALYEARS *op,CALYEARS_CUR *curp)
{

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == NULL) return SR_FAULT ;

	memclear(curp) ;
	op->ncursors += 1 ;
	curp->magic = CALYEARS_MAGIC ;
	return SR_OK ;
}
/* end subroutine (calyears_curbegin) */


int calyears_curend(CALYEARS *op,CALYEARS_CUR *curp)
{
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == NULL) return SR_FAULT ;

	if (curp->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;

	if (curp->results != NULL) {
	    rs1 = calyears_resultfins(op,curp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(curp->results) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->results = NULL ;
	}

	if (op->ncursors > 0)
	    op->ncursors -= 1 ;

	curp->i = 0 ;
	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (calyears_curend) */


int calyears_lookcite(CALYEARS *op,CALYEARS_CUR *curp,CALCITE *qp)
{
	int		rs = SR_OK ;
	int		c = 0 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == NULL) return SR_FAULT ;
	if (qp == NULL) return SR_FAULT ;

	if (curp->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;

	if (qp->y >= 2038) return SR_DOM ;
	if ((qp->y < 1970) && (qp->y != 0)) return SR_DOM ;

	if (curp->results != NULL) {
	    calyears_resultfins(op,curp) ;
	    uc_free(curp->results) ;
	    curp->results = NULL ;
	}

	if (qp->y == 0) {
	    if ((rs = calyears_year(op,0)) >= 0) {
		qp->y = op->year ;
	    }
	}

	if (rs >= 0) {
	    vecobj	res ;
	    cint	sz = szof(CALENT) ;
	    int 	vo = 0 ;
	    vo |= VECOBJ_OORDERED ;
	    vo |= VECOBJ_OSTATIONARY ;
	    if ((rs = vecobj_start(&res,sz,0,vo)) >= 0) {
	        CALMGR		*calp ;
	        vechand		*clp = &op->cals ;
	        int		i ;
	        for (i = 0 ; vechand_get(clp,i,&calp) >= 0 ; i += 1) {
	            if (calp != NULL) {
	                rs = calyears_lookmgr(op,&res,calp,qp) ;
	                c += rs ;
		    }
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            rs = calyears_mkresults(op,&res,curp) ;
	        }
	        if ((rs < 0) || (c > 0)) {
	            CALENT	*ep ;
	            for (i = 0 ; vecobj_get(&res,i,&ep) >= 0 ; i += 1) {
	                if (ep != NULL) {
	                    calent_finish(ep) ;
		        }
	            } /* end for */
	        } /* end if (error) */
	        vecobj_finish(&res) ;
	    } /* end if (res) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_lookcite) */


int calyears_read(CALYEARS *op,CALYEARS_CUR *curp,CALYEARS_CITE *qp,
		char *rbuf,int rlen)
{
	int		rs = SR_OK ;
	int		len = 0 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == NULL) return SR_FAULT ;
	if (qp == NULL) return SR_FAULT ;

	if (curp->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;

	if (curp->results != NULL) {
	    cint	i = curp->i ;
	    if ((i >= 0) && (i < curp->nresults)) {
	        CALENT	*ep, *res = (CALENT *) curp->results ;
		ep = (res+i) ;
	        qp->y = ep->q.y ;
	        qp->m = ep->q.m ;
	        qp->d = ep->q.d ;
	        if (rbuf != NULL) {
	            rs = calyears_loadbuf(op,rbuf,rlen,ep) ;
	            len = rs ;
	        } /* end if */
	        if (rs >= 0)
	            curp->i = (i + 1) ;
	    } else
	        rs = SR_NOTFOUND ;
	} else
	    rs = SR_NOTFOUND ;

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (calyears_read) */


/* ARGSUSED */
int calyears_check(CALYEARS *op,time_t dt)
{
	int		rs = SR_OK ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;

	if (op->magic != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

#ifdef	COMMENT
	rs = calyears_checkupdate(op,dt) ;
#endif

	return rs ;
}
/* end subroutine (calyears_check) */


int calyears_already(CALYEARS *op,vecobj *rlp,CALENT *ep)
{
	uint		nhash, ohash ;
	int		rs ;
	int		f = FALSE ;

	if ((rs = calyears_gethash(op,ep,&nhash)) >= 0) {
	    CALENT	*oep ;
	    int	i ;
	    for (i = 0 ; vecobj_get(rlp,i,&oep) >= 0 ; i += 1) {
	        if (oep != NULL) {
	            if ((rs = calyears_gethash(op,oep,&ohash)) >= 0) {
	    	        if (nhash == ohash) {
			    if ((rs = calyears_samewords(op,ep,oep)) > 0) {
				f = TRUE ;
	    	        	if (f) break ;
			    }
			} /* end if (same hash) */
		    } /* end if (calyears_gethash) */
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (calyears_gethash) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calyears_already) */


int calyears_havestart(CALYEARS *op,CALCITE *qp,int y,cchar *lp,int ll)
{
	int		rs = SR_OK ;
	int		ch ;
	int		si = 0 ; /* this serves as the result flag */

	ch = MKCHAR(lp[0]) ;
	if (! CHAR_ISWHITE(ch)) {
	    if ((si = sibrk(lp,ll," \t")) >= 3) {
		if (isdigitlatin(ch)) {
		    cchar	*tp ;
	    	    if ((tp = strnchr(lp,ll,'/')) != NULL) {
		    	int	cl ;
		    	cchar	*cp ;
	        	if ((rs = mkmonth(lp,(tp - lp))) >= 0) {
	        	    qp->m = (rs & UCHAR_MAX) ;
	            	    cp = (tp + 1) ;
	            	    cl = ((lp + ll) - cp) ;
	            	    if ((tp = strnbrk(cp,cl," \t")) != NULL) {
			        cl = (tp - cp) ;
			    }
	            	    if ((rs = calyears_mkday(op,y,qp->m,cp,cl)) >= 0) {
	            	        qp->d = (rs & UCHAR_MAX) ;
			    } else if (isNotOrIllegalSeq(rs)) {
				rs = SR_OK ;
				si = 0 ; /* mark "no entry" */
			    }
	                } else {
	                    int	f = FALSE ;
	            	    f = f || (rs == SR_INVALID) ;
	            	    if (f) rs = SR_ILSEQ ;
	        	} /* end if */
	           } else
	        	rs = SR_ILSEQ ;
		} else if (isalphalatin(ch)) {
#if	CF_TRANSHOL
	    	    if ((rs = calyears_transhol(op,qp,y,lp,si)) == 0) {
		        si = 0 ;
		    }
#else
	    	    rs = SR_OK ;
		    si = 0 ;	/* this says NO */
#endif /* CF_TRANSHOL */
		} /* end if */
		if (rs >= 0) {
	    	    si += siskipwhite((lp+si),(ll-si)) ;
		}
	    } else
	        rs = SR_ILSEQ ;
	} /* end if (not-white) */

	return (rs >= 0) ? si : rs ;
}
/* end subroutine (calyears_havestart) */


/* private subroutines */


static int calyears_argbegin(CALYEARS *op,cchar *pr)
{
	int		rs ;
	int		sz = 0 ;
	char		*bp ;
	sz += (lenstr(pr)+1) ;
	if ((rs = uc_malloc(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	}
	return rs ;
}
/* end subroutine (calyears_argbegin) */


static int calyears_argend(CALYEARS *op)
{
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a != NULL) {
	   rs1 = uc_free(op->a) ;
	   if (rs >= 0) rs = rs1 ;
	   op->a = NULL ;
	}
	return rs ;
}
/* end subroutine (calyears_argend) */


static int calyears_opensub(CALYEARS *op,cchar *dns[],cchar *cns[])
{
	SI		si, *sip = &si ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	            if ((rs = subinfo_start(sip,op,0)) >= 0) {

	                if ((rs = subinfo_calscreate(sip,dns,cns)) >= 0) {
	                    c = rs ;
	                    op->nentries = c ;
	                }

	                rs1 = subinfo_finish(sip) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_opensub) */


static int calyears_resultfins(CALYEARS *op,CALYEARS_CUR *curp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;
	if (curp->results != NULL) {
	    CALENT	*ep = (CALENT *) curp->results ;
	    int		i ;
	    for (i = 0 ; i < curp->nresults ; i += 1) {
	        rs1 = calent_finish(ep+i) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end for */
	} /* end if (non-null) */

	return rs ;
}
/* end subroutine (calyears_resultfins) */


static int calyears_lookmgr(CALYEARS *op,vecobj *rlp,CALMGR *calp,
		CALCITE *qp)
{
	int		rs ;
	int		c = 0 ;

	if (op == NULL) return SR_FAULT ;
	rs = calmgr_lookup(calp,rlp,qp) ;
	c = rs ;

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_lookmgr) */


static int calyears_mkresults(CALYEARS *op,vecobj *rlp,CALYEARS_CUR *curp)
{
	int		rs = SR_OK ;
	int		n ;
	int		c = 0 ;

	if (op == NULL) return SR_FAULT ;
	vecobj_sort(rlp,vrcmp) ; /* sort results in ascending order */

	if ((n = vecobj_count(rlp)) > 0) {
	    CALENT		*rp ;
	    CALENT		*ep ;
	    cint		sz = (n * szof(CALENT)) ;
	    if ((rs = uc_malloc(sz,&rp)) >= 0) {
	        int	i ;
		for (i = 0 ; vecobj_get(rlp,i,&ep) >= 0 ; i += 1) {
	    	    if (ep != NULL) {
	            rp[c++] = *ep ;	 /* copy! */
	            vecobj_del(rlp,i) ; /* entries are stationary */
		    }
	        } /* end for */
	        if (rs >= 0) {
	            curp->results = rp ;
	            curp->nresults = c ;
	            curp->i = 0 ;
	        } else
	            uc_free(rp) ;
	    } /* end if (m-a) */
	} /* end if (greater-than-zero) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_mkresults) */


static int calyears_calsdestroy(CALYEARS *op)
{
	CALMGR		*calp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; vechand_get(&op->cals,i,&calp) >= 0 ; i += 1) {
	    if (calp != NULL) {
		rs1 = calmgr_finish(calp) ;
		if (rs >= 0) rs = rs1 ;
		rs1 = uc_free(calp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (calyears_calsdestroy) */


static int calyears_year(CALYEARS *op,time_t dt)
{
	int		rs = SR_OK ;

	if (op->year == 0) {
	    TMTIME	tm ;
	    if (dt == 0) dt = time(NULL) ;
	    rs = tmtime_localtime(&tm,dt) ;
	    op->year = (tm.year + TM_YEAR_BASE) ;
	    op->isdst = tm.isdst ;
	    op->gmtoff = tm.gmtoff ; /* seconds west of GMT */
	}

	return rs ;
}
/* end subroutine (calyears_year) */

static int calyears_mkday(CALYEARS *op,int y,int m,cchar *cp,int cl) noex {
	DAYOFMONTH	*dmp ;
	int		rs ;
	if ((rs = calyars_domyear(op,y,&dmp)) >= 0) {
	    rs = dayofmonth_mkday(dmp,m,cp,cl) ;
	}
	return rs ;
}
/* end subroutine (calyears_mkday) */

static int calyars_domyear(CALYEARS *op,int y,DAYOFMONTH **rpp) noex {
	CALYEARS_DOMER	*dop ;
	vechand		*dlp = &op->doms ;
	int		rs ;
	int		i ;
	for (i = 0 ; (rs = vechand_get(dlp,i,&dop)) >= 0 ; i += 1) {
	    if (dop != NULL) {
		if (dop->year == y) break ;
	    }
	} /* end for */
	if (rs >= 0) {
	    if (rpp != NULL) {
		DAYOFMONTH	*dmp ;
		if ((rs = calyears_domerget(op,dop,&dmp)) >= 0) {
	            *rpp = dmp ;
		}
	    }
	} else if (rs == SR_NOTFOUND) {
	    cint	dsz = szof(CALYEARS_DOMER) ;
	    if ((rs = uc_malloc(dsz,&dop)) >= 0) {
		int	f_ent = TRUE ;
	        if ((rs = calyears_domerbegin(op,dop,y)) >= 0) {
		    if ((rs = vechand_add(dlp,dop)) >= 0) {
			int	di = rs ;
			f_ent = FALSE ;
	    	        if (rpp != NULL) {
			    DAYOFMONTH	*dmp ;
			    f_ent = TRUE ;
			    if ((rs = calyears_domerget(op,dop,&dmp)) >= 0) {
				f_ent = FALSE ;
	            	        *rpp = dmp ;
			    }
			}
			if (rs < 0)
			    vechand_del(dlp,di) ;
		    } /* end if (vechand_add) */
		    if (rs < 0)
			calyears_domerend(op,dop) ;
		} /* end if (calyears_domerbegin) */
	        if ((rs < 0) && f_ent)
		    uc_free(dop) ;
	    } /* end if (m-a) */
	} /* end if (found or not) */
	return rs ;
}
/* end subroutine (calyars_domyear) */

static int calyears_domerfins(CALYEARS *op) noex {
	CALYEARS_DOMER	*dep ;
	vechand		*dlp = &op->doms ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		c = 0 ;
	for (i = 0 ; vechand_get(dlp,i,&dep) >= 0 ; i += 1) {
	    if (dep != NULL) {
		c += 1 ;
		rs1 = calyears_domerend(op,dep) ;
		if (rs >= 0) rs = rs1 ;
		rs1 = uc_free(dep) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_domerfins) */

static int calyears_domerbegin(CALYEARS *op,CALYEARS_DOMER *dop,int y) noex {
	int		rs ;
	if (op == NULL) return SR_FAULT ;
	if ((rs = dayofmonth_start(&dop->dom,y)) >= 0) {
	    dop->year = y ;
	}
	return rs ;
}
/* end if (calyears_domerbegin) */

static int calyears_domerend(CALYEARS *op,CALYEARS_DOMER *dep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op == NULL) return SR_FAULT ;
	rs1 = dayofmonth_finish(&dep->dom) ;
	if (rs >= 0) rs = rs1 ;
	dep->year = 0 ;
	return rs ;
}
/* end subroutine (calyears_domerend) */

static int calyears_domerget(CALYEARS *op,CALYEARS_DOMER *dop,
		DAYOFMONTH **rpp) noex {
	int		rs = SR_OK ;
	if (op == NULL) return SR_FAULT ;
	if (rpp != NULL) {
	    *rpp = &dop->dom ;
	}
	return rs ;
}
/* end subroutine (calyars_domerget) */

static int calyears_gethash(CALYEARS *op,CALENT *ep,uint *rp) noex {
	int		rs ;
	if ((rs = calent_getci(ep)) >= 0) {
	   CALMGR	*cmp ;
	   cint	ci = rs ;
	   if ((rs = calyears_getcm(op,ci,&cmp)) >= 0) {
		rs = calmgr_gethash(cmp,ep,rp) ;
	   }
	}
	return rs ;
}
/* end subroutine (calyears_gethash) */

/* get the CALMGR (pointer to) given a CALMGR index */
static int calyears_getcm(CALYEARS *op,int ci,CALMGR **rpp) noex {
	vechand		*clp = &op->cals ;
	int		rs ;
	rs = vechand_get(clp,ci,rpp) ;
	return rs ;
}
/* end subroutine (calyears_getcm) */

static int calyears_samewords(CALYEARS *op,CALENT *ep,CALENT *oep) noex {
	int		rs ;
	int		f = FALSE ;
	cchar		*md1 ;
	cchar		*md2 ;
	if ((rs = calyears_getcalbase(op,ep,&md1)) >= 0) {
	    if ((rs = calyears_getcalbase(op,oep,&md2)) >= 0) {
		CALWORDER	w1, w2 ;
	        if ((rs = calworder_start(&w1,md1,ep)) >= 0) {
	    	    if ((rs = calworder_start(&w2,md2,oep)) >= 0) {
			int	c1l, c2l ;
			cchar	*c1p, *c2p ;
	        	while ((rs >= 0) && (! f)) {

	            	    c1l = calworder_get(&w1,&c1p) ;

	            	    c2l = calworder_get(&w2,&c2p) ;

	            	    if (c1l != c2l) break ;

	            	    if ((c1l == 0) && (c2l == 0)) {
	                	    f = TRUE ;
	                	    break ;
	            	    }

	            	    if (c1p[0] != c2p[0]) break ;

	            	    if (strncmp(c1p,c2p,c1l) != 0) break ;

	        	} /* end while */
	        	calworder_finish(&w2) ;
	    	    } /* end if (w2) */
	    	    calworder_finish(&w1) ;
		} /* end if (w1) */
	    } /* end if (calyears_getcalbase) */
	} /* end if (calyears_getcalbase) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calyears_samewords) */

static int calyears_getcalbase(CALYEARS *op,CALENT *ep,cchar **rpp) noex {
	int		rs ;
	if ((rs = calent_getci(ep)) >= 0) {
	    CALMGR	*cmp ;
	    if ((rs = calyears_getcm(op,rs,&cmp)) >= 0) {
		rs = calmgr_getbase(cmp,rpp) ;
	    }
	}
	return rs ;
}
/* end subroutine (calyears_getcalbase) */

static int calyears_loadbuf(CALYEARS *op,char *rbuf,int rlen,CALENT *ep) noex {
	int		rs ;

	if ((rs = calent_getci(ep)) >= 0) {
	    CALMGR	*calp ;
	    vechand	*ilp = &op->cals ;
	    cint	cidx = rs ;
	    if ((rs = vechand_get(ilp,cidx,&calp)) >= 0) {
	        rs = calmgr_loadbuf(calp,rbuf,rlen,ep) ;
	    }
	}

	return rs ;
}
/* end subroutine (calyears_loadbuf) */

#if	CF_TRANSHOL
static int calyears_transhol(CALYEARS *op,CALCITE *qp,int y,
		cc *sp,int sl) noex {
	int		rs ;
	int		nl ;
	int		f_negative = FALSE ;
	int		f_found = FALSE ;
	cchar	*tp ;
	cchar	*np ;

	qp->m = 0 ;
	qp->d = 0 ;
	qp->y = (ushort) y ;

	np = NULL ;
	nl = 0 ;
	if ((tp = strnbrk(sp,sl,"+-")) != NULL) {
	    np = (tp + 1) ;
	    nl = (sl - ((tp + 1) - sp)) ;
	    sl = (tp - sp) ;
	    f_negative = (tp[0] == '-') ;
	}

	if ((rs = calyears_dayname(op,qp,y,sp,sl)) > 0) {
	    f_found = TRUE ;
	    if (nl > 0) {
	        if (int odays{} ; (rs = cfdeci(np,nl,&odays)) >= 0) {
		    TMTIME	tm ;
	            time_t	t = time(NULL) ;

	            if (f_negative) odays = (- odays) ;

		    if ((rs = tmtime_localtime(&tm,t)) >= 0) {
	                tm.isdst = -1 ;
	                tm.gmtoff = op->gmtoff ;
	                tm.year = (y - TM_YEAR_BASE) ;
	                tm.mon = qp->m ;
	                tm.mday = (qp->d + odays) ;
	                if ((rs = tmtime_adjtime(&tm,&t)) >= 0) {
	                    qp->m = (uchar) tm.mon ;
	                    qp->d = (uchar) tm.mday ;
			    qp->y = (ushort) (tm.year+TM_YEAR_BASE) ;
	                }
		    } /* end if (tmtime_localtime) */
	        } /* end if (odays) */
	    } /* end if (positive) */
	} /* end if (day-offset required) */

	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (calyears_transhol) */
#endif /* CF_TRANSHOL */


#if	CF_TRANSHOL
static int calyears_dayname(CALYEARS *op,CALCITE *qp,int y,cchar *sp,int sl)
{
	int		rs ;
	int		rs1 ;
	int		f = FALSE ;
	if ((rs = calyears_holidayer(op)) > 0) {
	    HOLIDAYER		*holp = &op->hols ;
	    HOLIDAYER_CUR	hcur ;
	    HOLIDAYER_CITE	hc ;
	    cint		hlen = HOLBUFLEN ;
	    char		hbuf[HOLBUFLEN + 1] ;

	    if ((rs = holidayer_curbegin(holp,&hcur)) >= 0) {
		cint	rsn = SR_NOTFOUND ;

	        rs = holidayer_fetchname(holp,y,sp,sl,&hcur,&hc,hbuf,hlen) ;
	        if (rs >= 0) {
	            f = TRUE ;
		    qp->y = (ushort) y ;
	            qp->m = hc.m ;
	            qp->d = hc.d ;
	        } else if (rs == rsn) {
		    rs = SR_OK ;
		}

	        rs1 = holidayer_curend(holp,&hcur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (holidayer-cur) */
	} /* end if (calyears_holidayer) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calyears_dayname) */

static int calyears_holidayer(CALYEARS *op)
{
	int		rs = SR_OK ;
	int		f = op->open.hols ;
	if (! op->init.hols) {
	    HOLIDAYER	*holp = &op->hols ;
	    op->init.hols = TRUE ;
	    if ((rs = holidayer_open(holp,op->pr)) >= 0) {
	        op->open.hols = TRUE ;
		f = TRUE ;
	    } else if (isNotHols(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (open database as necessary) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calyears_holidayer) */
#endif /* CF_TRANSHOL */


#ifdef	COMMENT
static int calyears_checkupdate(CALYEARS *op,time_t dt)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		to = TO_CHECK ;
	int		f = FALSE ;

	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(NULL) ;
	    if ((dt - op->ti_lastcheck) >= to) {
	        op->ti_lastcheck = dt ;
	        if (USTAT db ; (rs1 = u_stat(op->dbfname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > op->ti_db) ;
		    f = f || (sb.st_mtime > op->ti_map) ;
		    if (f) {
	                SI	si ;

	                calyears_dbloadend(op) ;

	                if ((rs = subinfo_start(&si,op,0)) >= 0) {

	                    rs = calyears_dbloadbegin(op,&si) ;

	                    rs1 = subinfo_finish(&si) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (subinfo) */

	            } /* end if (update) */
	        } /* end if (stat) */
	    } /* end if (time-out) */
	} /* end if (no cursors out) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calyears_checkupdate) */
#endif /* COMMENT */

static int subinfo_start(SI *sip,CALYEARS *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (dt == 0) dt = time(NULL) ;
	memclear(sip) ;
	sip->op = op ;
	sip->dt = dt ;
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->fl.dirs) {
	    sip->fl.dirs = FALSE ;
	    rs1 = vecstr_finish(&sip->dirs) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dns = NULL ;
	}

	if (sip->tudname != NULL) {
	    rs1 = uc_free(sip->tudname) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->tudname = NULL ;
	}

	if (sip->userhome != NULL) {
	    rs1 = uc_free(sip->userhome) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->userhome = NULL ;
	}

	if (sip->fl.id) {
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->fl.id = FALSE ;
	}

	return rs ;
}
/* end subroutine (subinfo_finish) */


static int subinfo_calscreate(SI *sip,cchar **dns,cchar **cns)
{
	int		rs = SR_OK ;
	int		c = 0 ;

	if (dns == NULL) {
	    if ((rs = subinfo_mkdns(sip)) >= 0) {
	        dns = sip->dns ;
	    }
	}

	if ((rs >= 0) && (dns != NULL)) {
	    int		i ;
	    for (i = 0 ; dns[i] != NULL ; i += 1) {
		cchar	*dn = dns[i] ;
	        if (dn[0] != '\0') {
	            rs = subinfo_calscreater(sip,dn,cns) ;
	            c += rs ;
		}
	        if (rs < 0) break ;
	    } /* end for (dns) */
	} /* end if (dns) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_calscreate) */


static int subinfo_calscreater(SI *sip,cchar *dn,cchar *cns[])
{
	vecstr		cals ;
	int		rs = SR_OK ;
	int		c = 0 ;
	int		f_search = FALSE ;
	cchar	**names = NULL ;

	if (cns == NULL) {
	    if ((rs = vecstr_start(&cals,1,0)) >= 0) {
	        f_search = TRUE ;
	        if ((rs = subinfo_loadnames(sip,&cals,dn)) > 0) {
		    cchar	**npp ;
	            if ((rs = vecstr_getvec(&cals,&npp)) >= 0) {
	                names = npp ;
		    }
	        } /* end if (subinfo_loadnames) */
	    } /* end if (vecstr_start) */
	} else {
	    names = cns ;
	}

	if (rs >= 0) {
	    if ((rs = subinfo_ids(sip)) >= 0) {
		if (names != NULL) {
	    	    int		j ;
	            for (j = 0 ; names[j] != NULL ; j += 1) {
	                if (names[j][0] != '\0') {
	                    rs = subinfo_calcreate(sip,dn,names[j]) ;
	                    c += rs ;
			}
	                if (rs < 0) break ;
	            } /* end for (names) */
	        } /* end if (subinfo_ids) */
	    } /* end if (subinfo_ids) */
	} /* end if (ok) */

	if (f_search) {
	    vecstr_finish(&cals) ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_calscreater) */


static int subinfo_calcreate(SI *sip,cchar *dn,cchar *cn)
{
	CALYEARS	*op = sip->op ;
	cint	nlen = MAXNAMELEN ;
	int		rs ;
	int		f = FALSE ;
	cchar	*suf = CALYEARS_DBSUF ;
	char		nbuf[MAXNAMELEN + 1] ;

	if ((rs = snsds(nbuf,nlen,cn,suf)) >= 0) {
	    char	tbuf[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(tbuf,dn,nbuf)) >= 0) {
		if ((rs = subinfo_regacc(sip,tbuf,R_OK)) > 0) {
		    CALMGR	*calp ;
		    cint	size = szof(CALMGR) ;
		    f = TRUE ;
	    	    if ((rs = uc_malloc(size,&calp)) >= 0) {
			vechand	*clp = &op->cals ;
	        	if ((rs = vechand_add(clp,calp)) >= 0) {
	            	    cint	cidx = rs ;
	            	    if ((rs = calmgr_start(calp,op,cidx,dn,cn)) >= 0) {
	                	f = TRUE ;
	            	    }
	            	    if (rs < 0)
	 			vechand_del(clp,cidx) ;
	        	} /* end if */
	        	if (rs < 0)
		            uc_free(calp) ;
	    	    } /* end if (m-a) */
		} /* end if (subinfo_regacc) */
	    } /* end if (mkpath) */
	} /* end if (snsds) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_calcreate) */


static int subinfo_mkdns(SI *sip)
{
	CALYEARS	*op = sip->op ;
	int		rs = SR_OK ;
	int		tl ;
	int		c = 0 ;

	if ((rs = subinfo_username(sip)) >= 0) {
	    vecstr	*dlp = &sip->dirs ;
	    if ((rs = vecstr_start(dlp,1,0)) >= 0) {
	        cchar	*sharedname = CALYEARS_DBDIR ;
	        char	tbuf[MAXPATHLEN + 1] ;
		sip->fl.dirs = TRUE ;

/* user-home area */

		if (rs >= 0) {
	            cchar	*un = sip->username ;
	            if ((rs = mkpath2(tbuf,un,sharedname)) >= 0) {
	                tl = rs ;
	                if ((rs = subinfo_havedir(sip,tbuf)) > 0) {
	                    c += 1 ;
	                    rs = vecstr_add(dlp,tbuf,tl) ;
	                }
	            }
		} /* end if (ok) */

/* system area */

	        if (rs >= 0) {
	            if ((rs = mkpath2(tbuf,op->pr,sharedname)) >= 0) {
	                tl = rs ;
	                if ((rs = subinfo_havedir(sip,tbuf)) > 0) {
	                    c += 1 ;
	                    rs = vecstr_add(dlp,tbuf,tl) ;
	                }
	            }
	        } /* end if (ok) */

/* finish */

	        if (rs >= 0) {
	            cchar	**dap ;
	            if ((rs = vecstr_getvec(dlp,&dap)) >= 0) {
	                sip->dns = (cchar **) dap ;
		    }
	        }

		if (rs < 0) {
		    vecstr_finish(dlp) ;
		    sip->fl.dirs = FALSE ;
		}
	    } /* end if (vecstr) */
	} /* end if (username) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_mkdns) */

static int subinfo_havedir(SI *sip,cchar *dn) noex {
	int		rs ;
	int		f = FALSE ;

	if (sip == NULL) return SR_FAULT ;
	if (USTAT sb ; (rs = u_stat(dn,&sb)) >= 0) {
	    f = S_ISDIR(sb.st_mode) ? 1 : 0 ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_havedir) */

static int subinfo_ids(SI *sip) noex {
	int		rs = SR_OK ;
	if (! sip->fl.id) {
	    sip->fl.id = TRUE ;
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
}
/* end subroutine (subinfo_ids) */

static int subinfo_loadnames(SI *sip,vecstr *nlp,cchar *dirname) noex {
	FSDIR		dir ;
	FSDIR_ENT	ds ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if (sip == NULL) return SR_FAULT ;
	if ((rs = fsdir_open(&dir,dirname)) >= 0) {
	    ustat	sb ;
	    int			nl ;
	    cchar		*calsuf = CALYEARS_DBSUF ;
	    cchar		*tp ;
	    cchar		*np ;
	    char		tbuf[MAXPATHLEN + 1] ;

	    while ((rs = fsdir_read(&dir,&ds)) > 0) {
	        if (ds.name[0] != '.') {

	            if ((tp = strrchr(ds.name,'.')) != NULL) {
		        if (strcmp((tp+1),calsuf) == 0) {
	                    if ((rs = mkpath2(tbuf,dirname,ds.name)) >= 0) {
	                        if ((rs = u_stat(tbuf,&sb)) >= 0) {
	            	            if (S_ISREG(sb.st_mode)) {
	                    	        np = ds.name ;
	                    	        nl = (tp - ds.name) ;
	                                c += 1 ;
	                                rs = vecstr_add(nlp,np,nl) ;
	            	            } /* end if (regular file) */
			        } else if (isNotPresent(rs)) {
			            rs = SR_OK ;
	                        } /* end if (correct file extension) */
	                    } /* end if (mkpath) */
			} /* end if (our suffix) */
	            } /* end if (candidate) */
	        } /* end if (not invisible) */
	        if (rs < 0) break ;
	    } /* end while */

	    rs1 = fsdir_close(&dir) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fsdir) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_loadnames) */

static int subinfo_username(SI *sip) noex {
	int		rs = SR_OK ;

	if (sip->username[0] == '\0') {
	    struct passwd	pw ;
	    cint		pwlen = getbufsize(getbufsize_pw) ;
	    char		*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        if ((rs = getpwusername(&pw,pwbuf,pwlen,-1)) >= 0) {
	            cchar	*cp ;
	            strwcpy(sip->username,pw.pw_name,USERNAMELEN) ;
	            if ((rs = uc_mallocstrw(pw.pw_dir,-1,&cp)) >= 0) {
	                sip->userhome = cp ;
		    }
		}
		uc_free(pwbuf) ;
	    } /* end if (m-a) */
	} /* end if (needed) */

	return rs ;
}
/* end subroutine (subinfo_username) */

#ifdef	COMMENT
static int subinfo_tmpuserdir(subinfo *sip) noex {
	cmode		dmode = 0775 ;
	int		rs ;

	if ((rs = subinfo_username(sip)) >= 0) {
	    if (sip->tudname == NULL) {
	        cchar	*un = sip->username ;
	        char	tmpdname[MAXPATHLEN + 1] ;
	        if ((rs = mktmpuserdir(tmpdname,un,IDXDNAME,dmode)) >= 0) {
	            int		dl = rs ;
	            cchar	*dp ;
	            if ((rs = uc_mallocstrw(tmpdname,dl,&dp)) >= 0) {
	                sip->tudname = dp ;
	            }
	        }
	    } /* end if */
	} /* end if (username) */

	return rs ;
}
/* end subroutine (subinfo_tmpuserdir) */
#endif /* COMMENT */

#if	CF_CHECKDNAME
static int subinfo_checkdname(SI *sip,cchar *dname) noex {
	int		rs = SR_OK ;

	if (dname[0] == '/') {
	    ustat	sb ;
	    if ((rs = uc_stat(dname,&sb)) >= 0) {
	        if (S_ISDIR(sb.st_mode)) {
	            if ((rs = subinfo_ids(sip)) >= 0) {
	                rs = permid(&sip->id,&sb,W_OK) ;
		    }
		} else {
	            rs = SR_NOTDIR ;
	        }
	    } /* end if (uc_stat) */
	} else
	    rs = SR_INVALID ;

	return rs ;
}
/* end subroutine (subinfo_checkdname) */
#endif /* CF_CHECKDNAME */

static int subinfo_regacc(SI *sip,cchar *fn,int am) noex {
	int		rs ;
	int		f = FALSE ;
	if (USTAT sb ; (rs = u_stat(fn,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode)) {
	        if ((rs = subinfo_ids(sip)) >= 0) {
	            if ((rs = permid(&sip->id,&sb,am)) >= 0) {
		        f = TRUE ;
	            } else if (isNotAccess(rs)) {
		        rs = SR_OK ;
	            }
		} /* end if (subinfo_ids) */
	    } /* end if (is-reg) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_regacc) */

#if	CF_MKDNAME
static int mkdname(cchar *dname,mode_t dm) noex {
	cint		rsn = SR_NOENT ;
	int		rs ;
	if ((rs = checkdname(dname)) == rsn) {
	    rs = mkdirs(dname,dm) ;
	}
	return rs ;
}
/* end subroutine (mkdname) */
#endif /* CF_MKDNAME */

static int mkmonth(cchar *cp,int cl) noex {
	int		rs ;
	int		v ;

	rs = cfdeci(cp,cl,&v) ;
	v -= 1 ;

	return (rs >= 0) ? v : rs ;
}
/* end subroutine (mkmonth) */

/* for use with 'vecobj_sort(3dam)' or similar */
static int vrcmp(cvoid *v1p,cvoid *v2p) noex {
	CALENT		*e1p, **e1pp = (CALENT **) v1p ;
	CALENT		*e2p, **e2pp = (CALENT **) v2p ;
	int		rc = 0 ;
	if (*e1pp != NULL) {
	    if (*e2pp != NULL) {
	        e1p = *e1pp ;
	        e2p = *e2pp ;
	        if ((rc = (e1p->q.m - e2p->q.m)) == 0) {
	            rc = (e1p->q.d - e2p->q.d) ;
	        }
	    } else {
	        rc = -1 ;
	    }
	} else {
	    rc = 1 ;
	}
	return rc ;
}
/* end subroutine (vrcmp) */

static int isNotOrIllegalSeq(int rs) noex {
	return isOneOf(rsnotorils,rs) ;
}
/* end subroutine (isNotOrIllegalSeq) */

static int isNotHols(int rs) noex {
	return isOneOf(rsnothols,rs) ;
}
/* end subroutine (isNotHols) */


