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
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getbufsize.h>
#include	<getusername.h>
#include	<estrings.h>
#include	<mktmp.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<sbuf.h>
#include	<char.h>
#include	<ids.h>
#include	<dayofmonth.h>
#include	<tmtime.hh>
#include	<fsdir.h>
#include	<mkpathx.h>
#include	<six.h>
#include	<ischarx.h>
#include	<cfdec.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"calyears.h"
#include	"calmgr.h"
#include	"calworder.h"
#include	"cyi.h"
#include	"cyimk.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	CALYEARS_DBSUF	"calendar"
#define	CALYEARS_DMODE	0777
#define	CALYEARS_DBDIR	"share/calendar"
#define	CALYEARS_DOMER	calyears_domer

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	DOM		dayofmonth

#define	IDXDNAME	".calyears"
#define	IDXSUF		"cyi"

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


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		usernamelen ;
	operator int () noex ;
    } ; /* end struct */
} /* end namespace */

struct calyears_domer {
	DOM		dom ;
	int		year ;
} ; /* end struct */

struct subinfo_flags {
	uint		id:1 ;
	uint		dom:1 ;
	uint		hols:1 ;
	uint		dirs:1 ;	/* VECSTR was initialed */
} ; /* end struct */

namespace {
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
    } ; /* end struct (subinfo) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int calyears_magic(calyears *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == CALYEARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (calyears_magic) */

local int	calyears_argbegin(CALYEARS *,cchar *) noex ;
local int	calyears_argend(CALYEARS *) noex ;

local int	calyears_opensub(CALYEARS *op,cchar **dns,cchar **cns) noex ;

local int	calyears_loadbuf(CALYEARS *,char *,int,CALENT *) noex ;
local int	calyears_calsdestroy(CALYEARS *) noex ;

local int	calyears_domerfins(CALYEARS *) noex ;
local int	calyears_domerbegin(CALYEARS *,CALYEARS_DOMER *,int) noex ;
local int	calyears_domerend(CALYEARS *,CALYEARS_DOMER *) noex ;
local int	calyears_domerget(CALYEARS *,CALYEARS_DOMER *,
			DOM **) noex ;

#ifdef	COMMENT
local int	calyears_checkupdate(CALYEARS *,time_t) noex ;
local int	calyears_mksysvarsi(CALYEARS *,cchar *) noex ;
#endif

local int	calyears_resultfins(CALYEARS *,CALYEARS_CUR *) noex ;
local int	calyears_lookmgr(CALYEARS *,vecobj *,CALMGR *,CALCITE *) noex ;
local int	calyears_mkresults(CALYEARS *,vecobj *,CALYEARS_CUR *) noex ;
local int	calyears_year(CALYEARS *,time_t) noex ;
local int	calyears_mkday(CALYEARS *,int,int,cchar *,int) noex ;
local int	calyars_domyear(CALYEARS *,int,DOM **) noex ;

local int	calyears_gethash(CALYEARS *,CALENT *,uint *) noex ;
local int	calyears_getcm(CALYEARS *,int,CALMGR **) noex ;
local int	calyears_samewords(CALYEARS *,CALENT *,CALENT *) noex ;
local int	calyears_getcalbase(CALYEARS *,CALENT *,cchar **) noex ;

#if	CF_TRANSHOL
local int	calyears_transhol(CALYEARS *,CALCITE *,int,cchar *,int) noex ;
local int	calyears_dayname(CALYEARS *,CALCITE *,int,cchar *,int) noex ;
local int	calyears_holidayer(CALYEARS *) noex ;
#endif /* CF_TRANSHOL */

local int	subinfo_start(SI *,CALYEARS *,time_t) noex ;
local int	subinfo_finish(SI *) noex ;
local int	subinfo_calscreate(SI *,cchar **,cchar **) noex ;
local int	subinfo_calscreater(SI *,cchar *,cchar **) noex ;
local int	subinfo_calcreate(SI *,cchar *,cchar *) noex ;
local int	subinfo_ids(SI *) noex ;
local int	subinfo_username(SI *) noex ;
local int	subinfo_mkdns(SI *) noex ;
local int	subinfo_havedir(SI *,cchar *) noex ;
local int	subinfo_loadnames(SI *,vecstr *,cchar *) noex ;
local int	subinfo_regacc(SI *,cchar *,int) noex ;

#if	CF_CHECKDNAME
local int	subinfo_checkdname(SI *,cchar *) noex ;
#endif

#ifdef	COMMENT
local int	subinfo_tmpuserdir(SI *) noex ;
#endif

local int	mkmonth(cchar *,int) noex ;

#if	CF_MKDNAME
local int	mkdname(cchar *,mode_t) noex ;
#endif

local int	vrcmp(cvoid *,cvoid *) noex ;

local bool	isNotOrIllegalSeq(int) noex ;
local bool	isNotHols(int) noex ;


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
} ; /* end enum */

enum tdays {
	tday_first,
	tday_second,
	tday_third,
	tday_fourth,
	tday_fifth,
	tday_last,
	tday_overlast
} ; /* end enum */

constexpr cint		rsnotorils[] = {
	SR_NOTFOUND,
	SR_ILSEQ,
	0
} ; /* end array */

constexpr cint		rsnothols[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ; /* end array */

static vars		var ;


/* exported variables */

extern const calyears_obj	calyears_modinfo = {
	"calyears",
	szof(calyears),
	szof(calyears_cur)
} ;


/* exported subroutines */

local int calyears_opener(calyears *,cc *,cc **,cc **) noex ;

int calyears_open(CALYEARS *op,cchar *pr,cchar **dns,cchar **cns) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && pr) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		static cint rsv = var ;
	        memclear(op) ;
	        if (op->tmpdname == np) op->tmpdname = getenv(VARTMPDNAME) ;
	        if (op->tmpdname == np) op->tmpdname = TMPDNAME ;
		if ((rs = rsv) >= 0) {
 		    rs = calyears_opener(op,pr,dns,cns) ;
		    c = rs ;
		} /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_open) */

local int calyears_opener(calyears *op,cc *pr,cc **dns,cc **) noex {
    	int		rs ;
	if ((rs = calyears_argbegin(op,pr)) >= 0) {
	    cint	vn = 20 ;
	    cint	vo = VECHAND_OSTATIONARY ;
	    if ((rs = vechand_start(&op->cals,vn,vo)) >= 0) {
		if ((rs = vechand_start(&op->doms,1,0)) >= 0) {
		    custime	dt = time(nullptr) ;
		    if ((rs = calyears_year(op,dt)) >= 0) {
			if ((rs = calyears_opensub(op,dns,cns)) >= 0) {
	                   op->magval = CALYEARS_MAGIC ;
			}
		    } /* end if (ok) */
		    if (rs < 0) {
			calyears_domerfins(op) ;
			vechand_finish(&op->doms) ;
		    } /* end if (error) */
		} /* end if (doms) */
	        if (rs < 0) {
	            calyears_calsdestroy(op) ;
	            vechand_finish(&op->cals) ;
	        } /* end if (error) */
	    } /* end if (cals) */
	    if (rs < 0) {
	        calyears_argend(op) ;
	    } /* end if (error) */
	} /* end if (calyears_argbegin) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_opener) */

int calyears_close(CALYEARS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (op->open.hols) {
	    op->open.hols = false ;
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
	op->magval = 0 ;
	return rs ;
}
/* end subroutine (calyears_close) */

int calyears_count(CALYEARS *op) noex {
	int		rs ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	rs = op->nentries ;

	return rs ;
}
/* end subroutine (calyears_count) */

int calyears_audit(CALYEARS *op) noex {
	CALMGR		*calp ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	for (i = 0 ; vechand_get(&op->cals,i,&calp) >= 0 ; i += 1) {
	    if (calp == nullptr) continue ;
	    c += 1 ;
	    rs = calmgr_audit(calp) ;
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_audit) */

int calyears_curbegin(CALYEARS *op,CALYEARS_CUR *curp) noex {

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == nullptr) return SR_FAULT ;

	memclear(curp) ;
	op->ncursors += 1 ;
	curp->magval = CALYEARS_MAGIC ;
	return SR_OK ;
}
/* end subroutine (calyears_curbegin) */

int calyears_curend(CALYEARS *op,CALYEARS_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == nullptr) return SR_FAULT ;

	if (curp->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;

	if (curp->results != nullptr) {
	    rs1 = calyears_resultfins(op,curp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = lm_free(curp->results) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->results = nullptr ;
	}

	if (op->ncursors > 0)
	    op->ncursors -= 1 ;

	curp->i = 0 ;
	curp->magval = 0 ;
	return rs ;
}
/* end subroutine (calyears_curend) */

int calyears_lookcite(CALYEARS *op,CALYEARS_CUR *curp,CALCITE *qp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (curp->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;

	if (qp->y >= 2038) return SR_DOM ;
	if ((qp->y < 1970) && (qp->y != 0)) return SR_DOM ;

	if (curp->results != nullptr) {
	    calyears_resultfins(op,curp) ;
	    lm_free(curp->results) ;
	    curp->results = nullptr ;
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
	            if (calp != nullptr) {
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
	                if (ep != nullptr) {
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
		char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		len = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (curp->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;

	if (curp->results != nullptr) {
	    cint	i = curp->i ;
	    if ((i >= 0) && (i < curp->nresults)) {
	        CALENT	*ep, *res = (CALENT *) curp->results ;
		ep = (res+i) ;
	        qp->y = ep->q.y ;
	        qp->m = ep->q.m ;
	        qp->d = ep->q.d ;
	        if (rbuf != nullptr) {
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

int calyears_check(CALYEARS *op,time_t dt) noex {
	int		rs = SR_OK ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
	if (op->magval != CALYEARS_MAGIC) return SR_NOTOPEN ;
#endif

#ifdef	COMMENT
	rs = calyears_checkupdate(op,dt) ;
#endif

	return rs ;
}
/* end subroutine (calyears_check) */

int calyears_already(CALYEARS *op,vecobj *rlp,CALENT *ep) noex {
	uint		nhash, ohash ;
	int		rs ;
	int		f = false ;

	if ((rs = calyears_gethash(op,ep,&nhash)) >= 0) {
	    CALENT	*oep ;
	    int	i ;
	    for (i = 0 ; vecobj_get(rlp,i,&oep) >= 0 ; i += 1) {
	        if (oep != nullptr) {
	            if ((rs = calyears_gethash(op,oep,&ohash)) >= 0) {
	    	        if (nhash == ohash) {
			    if ((rs = calyears_samewords(op,ep,oep)) > 0) {
				f = true ;
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

int calyears_havestart(CALYEARS *op,CALCITE *qp,int y,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		ch ;
	int		si = 0 ; /* this serves as the result flag */

	ch = MKCHAR(lp[0]) ;
	if (! CHAR_ISWHITE(ch)) {
	    if ((si = sibrk(lp,ll," \t")) >= 3) {
		if (isdigitlatin(ch)) {
		    cchar	*tp ;
	    	    if ((tp = strnchr(lp,ll,'/')) != nullptr) {
		    	int	cl ;
		    	cchar	*cp ;
	        	if ((rs = mkmonth(lp,(tp - lp))) >= 0) {
	        	    qp->m = (rs & UCHAR_MAX) ;
	            	    cp = (tp + 1) ;
	            	    cl = ((lp + ll) - cp) ;
	            	    if ((tp = strnbrk(cp,cl," \t")) != nullptr) {
			        cl = (tp - cp) ;
			    }
	            	    if ((rs = calyears_mkday(op,y,qp->m,cp,cl)) >= 0) {
	            	        qp->d = (rs & UCHAR_MAX) ;
			    } else if (isNotOrIllegalSeq(rs)) {
				rs = SR_OK ;
				si = 0 ; /* mark "no entry" */
			    }
	                } else {
	                    int	f = false ;
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

local int calyears_argbegin(CALYEARS *op,cchar *pr) noex {
	int		rs ;
	int		sz = 0 ;
	char		*bp ;
	sz += (lenstr(pr)+1) ;
	if ((rs = lm_mall(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1)+1) ;
	}
	return rs ;
}
/* end subroutine (calyears_argbegin) */

local int calyears_argend(CALYEARS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a != nullptr) {
	   rs1 = lm_free(op->a) ;
	   if (rs >= 0) rs = rs1 ;
	   op->a = nullptr ;
	}
	return rs ;
}
/* end subroutine (calyears_argend) */

local int calyears_opensub(CALYEARS *op,cchar **dns,cchar **cns) noex {
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

local int calyears_resultfins(CALYEARS *op,CALYEARS_CUR *curp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp->results != nullptr) {
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

local int calyears_lookmgr(CALYEARS *op,vecobj *rlp,CALMGR *calp,
		CALCITE *qp) noex {
	int		rs ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;
	rs = calmgr_lookup(calp,rlp,qp) ;
	c = rs ;

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_lookmgr) */

local int calyears_mkresults(CALYEARS *op,vecobj *rlp,CALYEARS_CUR *curp) noex {
	int		rs = SR_OK ;
	int		n ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;
	vecobj_sort(rlp,vrcmp) ; /* sort results in ascending order */

	if ((n = vecobj_count(rlp)) > 0) {
	    CALENT		*rp ;
	    CALENT		*ep ;
	    cint		sz = (n * szof(CALENT)) ;
	    if ((rs = lm_mall(sz,&rp)) >= 0) {
	        int	i ;
		for (i = 0 ; vecobj_get(rlp,i,&ep) >= 0 ; i += 1) {
	    	    if (ep != nullptr) {
	            rp[c++] = *ep ;	 /* copy! */
	            vecobj_del(rlp,i) ; /* entries are stationary */
		    }
	        } /* end for */
	        if (rs >= 0) {
	            curp->results = rp ;
	            curp->nresults = c ;
	            curp->i = 0 ;
	        } else
	            lm_free(rp) ;
	    } /* end if (m-a) */
	} /* end if (greater-than-zero) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_mkresults) */

local int calyears_calsdestroy(CALYEARS *op) noex {
	CALMGR		*calp ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; vechand_get(&op->cals,i,&calp) >= 0 ; i += 1) {
	    if (calp != nullptr) {
		rs1 = calmgr_finish(calp) ;
		if (rs >= 0) rs = rs1 ;
		rs1 = lm_free(calp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (calyears_calsdestroy) */

local int calyears_year(CALYEARS *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->year == 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if (TMTIME ts ; (rs = tmtime_timelocal(&ts,dt)) >= 0) {
	    op->year = (ts.year + TM_YEAR_BASE) ;
	    op->isdst = ts.isdst ;
	    op->gmtoff = ts.gmtoff ; /* seconds west of GMT */
	    } /* end if (tmtime_timelocal) */
	} /* end if (year needed) */
	return rs ;
}
/* end subroutine (calyears_year) */

local int calyears_mkday(CALYEARS *op,int y,int m,cchar *cp,int cl) noex {
	DOM	*dmp ;
	int		rs ;
	if ((rs = calyars_domyear(op,y,&dmp)) >= 0) {
	    rs = dayofmonth_mkday(dmp,m,cp,cl) ;
	}
	return rs ;
}
/* end subroutine (calyears_mkday) */

local int calyars_domyear(CALYEARS *op,int y,DOM **rpp) noex {
	CALYEARS_DOMER	*dop ;
	vechand		*dlp = &op->doms ;
	int		rs ;
	int		i ;
	for (i = 0 ; (rs = vechand_get(dlp,i,&dop)) >= 0 ; i += 1) {
	    if (dop != nullptr) {
		if (dop->year == y) break ;
	    }
	} /* end for */
	if (rs >= 0) {
	    if (rpp != nullptr) {
		DOM	*dmp ;
		if ((rs = calyears_domerget(op,dop,&dmp)) >= 0) {
	            *rpp = dmp ;
		}
	    }
	} else if (rs == SR_NOTFOUND) {
	    cint	dsz = szof(CALYEARS_DOMER) ;
	    if ((rs = lm_mall(dsz,&dop)) >= 0) {
		int	f_ent = true ;
	        if ((rs = calyears_domerbegin(op,dop,y)) >= 0) {
		    if ((rs = vechand_add(dlp,dop)) >= 0) {
			int	di = rs ;
			f_ent = false ;
	    	        if (rpp != nullptr) {
			    DOM	*dmp ;
			    f_ent = true ;
			    if ((rs = calyears_domerget(op,dop,&dmp)) >= 0) {
				f_ent = false ;
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
		    lm_free(dop) ;
	    } /* end if (m-a) */
	} /* end if (found or not) */
	return rs ;
}
/* end subroutine (calyars_domyear) */

local int calyears_domerfins(CALYEARS *op) noex {
	CALYEARS_DOMER	*dep ;
	vechand		*dlp = &op->doms ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		c = 0 ;
	for (i = 0 ; vechand_get(dlp,i,&dep) >= 0 ; i += 1) {
	    if (dep != nullptr) {
		c += 1 ;
		rs1 = calyears_domerend(op,dep) ;
		if (rs >= 0) rs = rs1 ;
		rs1 = lm_free(dep) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (calyears_domerfins) */

local int calyears_domerbegin(CALYEARS *op,CALYEARS_DOMER *dop,int y) noex {
	int		rs ;
	if (op == nullptr) return SR_FAULT ;
	if ((rs = dayofmonth_start(&dop->dom,y)) >= 0) {
	    dop->year = y ;
	}
	return rs ;
}
/* end if (calyears_domerbegin) */

local int calyears_domerend(CALYEARS *op,CALYEARS_DOMER *dep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op == nullptr) return SR_FAULT ;
	rs1 = dayofmonth_finish(&dep->dom) ;
	if (rs >= 0) rs = rs1 ;
	dep->year = 0 ;
	return rs ;
}
/* end subroutine (calyears_domerend) */

local int calyears_domerget(CALYEARS *op,CALYEARS_DOMER *dop,
		DOM **rpp) noex {
	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (rpp != nullptr) {
	    *rpp = &dop->dom ;
	}
	return rs ;
}
/* end subroutine (calyars_domerget) */

local int calyears_gethash(CALYEARS *op,CALENT *ep,uint *rp) noex {
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
local int calyears_getcm(CALYEARS *op,int ci,CALMGR **rpp) noex {
	vechand		*clp = &op->cals ;
	int		rs ;
	rs = vechand_get(clp,ci,rpp) ;
	return rs ;
}
/* end subroutine (calyears_getcm) */

local int calyears_samewords(CALYEARS *op,CALENT *ep,CALENT *oep) noex {
	int		rs ;
	int		f = false ;
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
	                	    f = true ;
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

local int calyears_getcalbase(CALYEARS *op,CALENT *ep,cchar **rpp) noex {
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

local int calyears_loadbuf(CALYEARS *op,char *rbuf,int rlen,CALENT *ep) noex {
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
local int calyears_transhol(CALYEARS *op,CALCITE *qp,int y,
		cc *sp,int sl) noex {
	int		rs ;
	int		nl ;
	int		f_negative = false ;
	int		f_found = false ;
	cchar	*tp ;
	cchar	*np ;

	qp->m = 0 ;
	qp->d = 0 ;
	qp->y = (ushort) y ;

	np = nullptr ;
	nl = 0 ;
	if ((tp = strnbrk(sp,sl,"+-")) != nullptr) {
	    np = (tp + 1) ;
	    nl = (sl - ((tp + 1) - sp)) ;
	    sl = (tp - sp) ;
	    f_negative = (tp[0] == '-') ;
	}

	if ((rs = calyears_dayname(op,qp,y,sp,sl)) > 0) {
	    f_found = true ;
	    if (nl > 0) {
	        if (int odays{} ; (rs = cfdeci(np,nl,&odays)) >= 0) {
		    TMTIME	tm ;
	            time_t	t = time(nullptr) ;

	            if (f_negative) odays = (- odays) ;

		    if ((rs = tmtime_timelocal(&tm,t)) >= 0) {
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
		    } /* end if (tmtime_timelocal) */
	        } /* end if (odays) */
	    } /* end if (positive) */
	} /* end if (day-offset required) */

	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (calyears_transhol) */
#endif /* CF_TRANSHOL */


#if	CF_TRANSHOL
local int calyears_dayname(CALYEARS *op,CALCITE *qp,int y,
		cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
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
	            f = true ;
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

local int calyears_holidayer(CALYEARS *op) noex {
	int		rs = SR_OK ;
	int		f = op->open.hols ;
	if (! op->init.hols) {
	    HOLIDAYER	*holp = &op->hols ;
	    op->init.hols = true ;
	    if ((rs = holidayer_open(holp,op->pr)) >= 0) {
	        op->open.hols = true ;
		f = true ;
	    } else if (isNotHols(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (open database as necessary) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (calyears_holidayer) */
#endif /* CF_TRANSHOL */

#ifdef	COMMENT
local int calyears_checkupdate(CALYEARS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		to = TO_CHECK ;
	int		f = false ;

	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= to) {
	        op->ti_lastcheck = dt ;
	        if (ustat db ; (rs1 = u_stat(op->dbfname,&sb)) >= 0) {
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

local int subinfo_start(SI *sip,CALYEARS *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (dt == 0) dt = time(nullptr) ;
	memclear(sip) ;
	sip->op = op ;
	sip->dt = dt ;
	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->fl.dirs) {
	    sip->fl.dirs = false ;
	    rs1 = vecstr_finish(&sip->dirs) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dns = nullptr ;
	}

	if (sip->tudname != nullptr) {
	    rs1 = lm_free(sip->tudname) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->tudname = nullptr ;
	}

	if (sip->userhome != nullptr) {
	    rs1 = lm_free(sip->userhome) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->userhome = nullptr ;
	}

	if (sip->fl.id) {
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->fl.id = false ;
	}

	return rs ;
}
/* end subroutine (subinfo_finish) */


local int subinfo_calscreate(SI *sip,cchar **dns,cchar **cns)
{
	int		rs = SR_OK ;
	int		c = 0 ;

	if (dns == nullptr) {
	    if ((rs = subinfo_mkdns(sip)) >= 0) {
	        dns = sip->dns ;
	    }
	}

	if ((rs >= 0) && (dns != nullptr)) {
	    int		i ;
	    for (i = 0 ; dns[i] != nullptr ; i += 1) {
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


local int subinfo_calscreater(SI *sip,cchar *dn,cchar *cns[])
{
	vecstr		cals ;
	int		rs = SR_OK ;
	int		c = 0 ;
	int		f_search = false ;
	cchar	**names = nullptr ;

	if (cns == nullptr) {
	    if ((rs = vecstr_start(&cals,1,0)) >= 0) {
	        f_search = true ;
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
		if (names != nullptr) {
	            for (int j = 0 ; names[j] != nullptr ; j += 1) {
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

local int subinfo_calcreate(SI *sip,cchar *dn,cchar *cn) noex {
	CALYEARS	*op = sip->op ;
	cint	nlen = MAXNAMELEN ;
	int		rs ;
	int		f = false ;
	cchar	*suf = CALYEARS_DBSUF ;
	char		nbuf[MAXNAMELEN + 1] ;

	if ((rs = snsds(nbuf,nlen,cn,suf)) >= 0) {
	    char	tbuf[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(tbuf,dn,nbuf)) >= 0) {
		if ((rs = subinfo_regacc(sip,tbuf,R_OK)) > 0) {
		    CALMGR	*calp ;
		    cint	size = szof(CALMGR) ;
		    f = true ;
	    	    if ((rs = lm_mall(size,&calp)) >= 0) {
			vechand	*clp = &op->cals ;
	        	if ((rs = vechand_add(clp,calp)) >= 0) {
	            	    cint	cidx = rs ;
	            	    if ((rs = calmgr_start(calp,op,cidx,dn,cn)) >= 0) {
	                	f = true ;
	            	    }
	            	    if (rs < 0)
	 			vechand_del(clp,cidx) ;
	        	} /* end if */
	        	if (rs < 0)
		            lm_free(calp) ;
	    	    } /* end if (m-a) */
		} /* end if (subinfo_regacc) */
	    } /* end if (mkpath) */
	} /* end if (snsds) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_calcreate) */

local int subinfo_mkdns(SI *sip) noex {
	CALYEARS	*op = sip->op ;
	int		rs = SR_OK ;
	int		tl ;
	int		c = 0 ;
	if ((rs = subinfo_username(sip)) >= 0) {
	    vecstr	*dlp = &sip->dirs ;
	    if ((rs = vecstr_start(dlp,1,0)) >= 0) {
	        cchar	*sharedname = CALYEARS_DBDIR ;
	        char	tbuf[MAXPATHLEN + 1] ;
		sip->fl.dirs = true ;
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
		    sip->fl.dirs = false ;
		} /* end if (error) */
	    } /* end if (vecstr) */
	} /* end if (username) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_mkdns) */

local int subinfo_havedir(SI *sip,cchar *dn) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (sip) {
	    if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
	        f = !!S_ISDIR(sb.st_mode) ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_havedir) */

local int subinfo_ids(SI *sip) noex {
	int		rs = SR_OK ;
	if (! sip->fl.id) {
	    sip->fl.id = true ;
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
}
/* end subroutine (subinfo_ids) */

local int subinfo_loadnames(SI *sip,vecstr *nlp,cchar *dirname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (sip == nullptr) return SR_FAULT ;
	if (fsdir dir ; (rs = fsdir_open(&dir,dirname)) >= 0) {
	    int		naml ;
	    cchar	*calsuf = CALYEARS_DBSUF ;
	    cchar	*namp ;
	    char	tbuf[MAXPATHLEN + 1] ;
	    for (fsdir_ent ds ; (rs = fsdir_read(&dir,&ds)) > 0 ; ) {
	        if (ds.name[0] != '.') {
	            cchar *tp ;
	            if ((tp = strrchr(ds.name,'.')) != nullptr) {
		        if (strcmp((tp+1),calsuf) == 0) {
	                    if ((rs = mkpath2(tbuf,dirname,ds.name)) >= 0) {
	    			if (ustat sb ; (rs = u_stat(tbuf,&sb)) >= 0) {
	            	            if (S_ISREG(sb.st_mode)) {
	                    	        namp = ds.name ;
	                    	        naml = intconv(tp - ds.name) ;
	                                c += 1 ;
	                                rs = vecstr_add(nlp,namp,naml) ;
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

local int subinfo_username(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->username[0] == '\0') {
	    if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) {
	        cint	pwlen = rs ;
	        ucentpw	pw ;
	        if ((rs = getpwusername(&pw,pwbuf,pwlen,-1)) >= 0) {
		    cint	unl = var.usernamelen ;
		    cchar	unp = pw.pw_name ;
		    cchar	dirp = pw_pw_dir ;
	            strwcpy(sip->username,unp,unl) ;
	            if (cchar *cp ; (rs = lm_strw(dirp,-1,&cp)) >= 0) {
	                sip->userhome = cp ;
		    } /* end if (memory-allocation) */
		} /* end if (getpwusername) */
		rs1 = lm_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a) */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (subinfo_username) */

#ifdef	COMMENT
local int subinfo_tmpuserdir(subinfo *sip) noex {
	cmode		dmode = 0775 ;
	int		rs ;
	if ((rs = subinfo_username(sip)) >= 0) {
	    if (sip->tudname == nullptr) {
	        cchar	*un = sip->username ;
	        char	tmpdname[MAXPATHLEN + 1] ;
	        if ((rs = mktmpuserdir(tmpdname,un,IDXDNAME,dmode)) >= 0) {
	            int		dl = rs ;
	            cchar	*dp ;
	            if ((rs = lm_strw(tmpdname,dl,&dp)) >= 0) {
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
local int subinfo_checkdname(SI *sip,cchar *dname) noex {
	int		rs = SR_OK ;
	if (dname[0] == '/') {
	    if (ustat sb ; (rs = uc_stat(dname,&sb)) >= 0) {
	        if (S_ISDIR(sb.st_mode)) {
	            if ((rs = subinfo_ids(sip)) >= 0) {
	                rs = permid(&sip->id,&sb,W_OK) ;
		    }
		} else {
	            rs = SR_NOTDIR ;
	        }
	    } /* end if (uc_stat) */
	} else {
	    rs = SR_INVALID ;
	}
	return rs ;
}
/* end subroutine (subinfo_checkdname) */
#endif /* CF_CHECKDNAME */

local int subinfo_regacc(SI *sip,cchar *fn,int am) noex {
	int		rs ;
	int		f = false ;
	if (ustat sb ; (rs = u_stat(fn,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode)) {
	        if ((rs = subinfo_ids(sip)) >= 0) {
	            if ((rs = permid(&sip->id,&sb,am)) >= 0) {
		        f = true ;
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

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(bufsize_un)) >= 0) {
	    usernamelen = rs ;
	}
	return rs ;
} /* end method (vars::operator) */

#if	CF_MKDNAME
local int mkdname(cchar *dname,mode_t dm) noex {
	cint		rsn = SR_NOENT ;
	int		rs ;
	if ((rs = checkdname(dname)) == rsn) {
	    rs = mkdirs(dname,dm) ;
	}
	return rs ;
}
/* end subroutine (mkdname) */
#endif /* CF_MKDNAME */

local int mkmonth(cchar *cp,int cl) noex {
	int		rs ;
	int		v = 0 ; /* return-value */
	if ((rs = cfdeci(cp,cl,&v)) >= 0) {
	    v -= 1 ;
	}
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (mkmonth) */

/* for use with 'vecobj_sort(3dam)' or similar */
local int vrcmp(cvoid *v1p,cvoid *v2p) noex {
	CALENT		*e1p, **e1pp = (CALENT **) v1p ;
	CALENT		*e2p, **e2pp = (CALENT **) v2p ;
	int		rc = 0 ;
	{
	    e1p = *e1pp ;
	    e2p = *e2pp ;
	    if (e1p || e2p) {
	        rs = +1 ;
	        if (e1p) {
	            rc = -1 ;
	            if (e2p) {
	                if ((rc = (e1p->q.m - e2p->q.m)) == 0) {
	                    rc = (e1p->q.d - e2p->q.d) ;
	                }
	            }
	        }
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (vrcmp) */

local bool isNotOrIllegalSeq(int rs) noex {
	return isOneOf(rsnotorils,rs) ;
}
/* end subroutine (isNotOrIllegalSeq) */

local bool isNotHols(int rs) noex {
	return isOneOf(rsnothols,rs) ;
}
/* end subroutine (isNotHols) */


