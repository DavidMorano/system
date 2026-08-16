/* calyears SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* CYS object implementation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */
#define	CF_TRANSHOL	0		/* translate holidays */
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
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/mman.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<mktmp.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<dayofmonth.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<fsdir.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<six.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<cyi.h>			/* LIBDAM */
#include	<cyimk.h>		/* LIBDAM */
#include	<calmgr.h>		/* LIBDAM */
#include	<calworder.h>		/* LIBDAM */

#include	"calyears.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	CYS		calyears
#define	CYS_Q		calyears_q
#define	CYS_C		calyears_cur
#define	CYS_O		calyears_obj
#define	CYS_E		calyears_ent
#define	CYS_DOMER	calyears_domer
#define	CALYEARS_DBSUF	"calendar"
#define	CALYEARS_DMODE	0777
#define	CALYEARS_DBDIR	"share/calendar"

#define	DOM		dayofmonth

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	IDXDNAME	".calyears"
#define	IDXSUF		"cyi"

#define	HOLBUFLEN	100

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

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


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
	calyears	*op ;
	cchar		*tudname ;
	cchar		*userhome ;
	char		*username ;
	cchar		**dns ;
	ids		id ;
	vecstr		dirs ;
	time_t		dt ;
	SI_FL		init, fl ;
	int		year ;
	int		isdst ;
	int		gmtoff ;	/* seconds west of GMT */
    } ; /* end struct (subinfo) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int calyears_ctor(calyears *op,Args ... args) noex {
    	CALYEARS	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->holp = new(nt) holidayer) != np) {
	        if ((op->domp = new(nt) vechand) != np) {
	            if ((op->calp = new(nt) vechand) != np) {
			rs = SR_OK ;
		    } /* end if (new-vechand) */
		    if (rs < 0) {
		        delete op->domp ;
		        op->domp = nullptr ;
		    } /* end if (error) */
	        } /* end if (new-vechand) */
		if (rs < 0) {
		    delete op->holp ;
		    op->holp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-holidayer) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calyears_ctor) */

local int calyears_dtor(calyears *op) noex {
	int		rs = SR_OK ;
	if (op->calp) ylikely {
	    delete op->calp ;
	    op->calp = nullptr ;
	}
	if (op->domp) ylikely {
	    delete op->domp ;
	    op->domp = nullptr ;
	}
	if (op->holp) ylikely {
	    delete op->holp ;
	    op->holp = nullptr ;
	}
	return rs ;
} /* end subroutine (calyears_dtor) */

template<typename ... Args>
local inline int calyears_magic(calyears *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == CALYEARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (calyears_magic) */

local int	calyears_argbeg(CYS *,cchar *) noex ;
local int	calyears_argend(CYS *) noex ;

local int	calyears_opensub(CYS *op,cchar **dns,cchar **cns) noex ;

local int	calyears_loadbuf(CYS *,char *,int,CYS_E *) noex ;
local int	calyears_calsdestroy(CYS *) noex ;

local int	calyears_domerfins(CYS *) noex ;
local int	calyears_domerbeg(CYS *,CYS_DOMER *,int) noex ;
local int	calyears_domerend(CYS *,CYS_DOMER *) noex ;
local int	calyears_domerget(CYS *,CYS_DOMER *,DOM **) noex ;

#ifdef	COMMENT
local int	calyears_checkupdate(CYS *,time_t) noex ;
local int	calyears_mksysvarsi(CYS *,cchar *) noex ;
#endif

local int	calyears_resultfins(CYS *,CYS_C *) noex ;
local int	calyears_lookmgr(CYS *,vecobj *,CALMGR *,CYS_Q *) noex ;
local int	calyears_mkresults(CYS *,vecobj *,CYS_C *) noex ;
local int	calyears_year(CYS *,time_t) noex ;
local int	calyears_mkday(CYS *,int,int,cchar *,int) noex ;
local int	calyars_domyear(CYS *,int,DOM **) noex ;

local int	calyears_gethash(CYS *,CYS_E *,uint *) noex ;
local int	calyears_getcm(CYS *,int,CALMGR **) noex ;
local int	calyears_samewords(CYS *,CYS_E *,CYS_E *) noex ;
local int	calyears_getcalbase(CYS *,CYS_E *,cchar **) noex ;

#if	CF_TRANSHOL
local int	calyears_transhol(CYS *,CYS_Q *,int,cchar *,int) noex ;
local int	calyears_dayname(CYS *,CYS_Q *,int,cchar *,int) noex ;
local int	calyears_holidayer(CYS *) noex ;
#endif /* CF_TRANSHOL */

local int	subinfo_start(SI *,CYS *,time_t) noex ;
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
local int	yearok(int) noex ;

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
cbool			f_debug		= CF_DEBUG ;


/* exported variables */

const calyears_obj	calyears_modinfo = {
	"calyears",
	szof(calyears),
	szof(calyears_cur)
} ; /* extern array */


/* exported subroutines */

local int calyears_opener(calyears *,cc *,cc **,cc **) noex ;

int calyears_open(CYS *op,cchar *pr,cchar **dns,cchar **cns) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = calyears_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->tmpdname == np) op->tmpdname = getenver(VARTMPDNAME) ;
	    if (op->tmpdname == np) op->tmpdname = TMPDNAME ;
	    if (pr[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
 		    rs = calyears_opener(op,pr,dns,cns) ;
		    c = rs ;
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		calyears_dtor(op) ;
	    } /* end if (error) */
	} /* end if (calyears_ctor) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_open) */

local int calyears_opener(calyears *op,cc *pr,cc **dns,cc **cns) noex {
    	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = calyears_argbeg(op,pr)) >= 0) ylikely {
	    cint	vn = 20 ;
	    cint	vo = vechandm.stationary ;
	    if ((rs = vechand_start(op->calp,vn,vo)) >= 0) {
		if ((rs = vechand_start(op->domp,1,0)) >= 0) {
		    custime	dt = getustime ;
		    if ((rs = calyears_year(op,dt)) >= 0) {
			if ((rs = calyears_opensub(op,dns,cns)) >= 0) {
	                   op->magval = CALYEARS_MAGIC ;
			}
		    } /* end if (ok) */
		    if (rs < 0) {
			calyears_domerfins(op) ;
			vechand_finish(op->domp) ;
		    } /* end if (error) */
		} /* end if (doms) */
	        if (rs < 0) {
	            calyears_calsdestroy(op) ;
	            vechand_finish(op->calp) ;
	        } /* end if (error) */
	    } /* end if (cals) */
	    if (rs < 0) {
	        calyears_argend(op) ;
	    } /* end if (error) */
	} /* end if (calyears_argbeg) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_opener) */

int calyears_close(CYS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = calyears_magic(op)) >= 0) ylikely {
	    if (op->open.hols && op->holp) {
	        op->open.hols = false ;
	        rs1 = holidayer_close(op->holp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = calyears_domerfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->domp) {
	        rs1 = vechand_finish(op->domp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = calyears_calsdestroy(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->calp) {
	        rs1 = vechand_finish(op->calp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = calyears_argend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = calyears_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->nentries = 0 ;
	    op->magval = 0 ;
	} /* end if (calyears_magic) */
	return rs ;
} /* end subroutine (calyears_close) */

int calyears_count(CYS *op) noex {
	int		rs ;
	if ((rs = calyears_magic(op)) >= 0) ylikely {
	    rs = op->nentries ;
	} /* end if (calyears_magic) */
	return rs ;
} /* end subroutine (calyears_count) */

int calyears_curbegin(CYS *op,CYS_C *curp) noex {
    	int		rs ;
	if ((rs = calyears_magic(op,curp)) >= 0) ylikely {
	    memclear(curp) ;
	    op->ncursors += 1 ;
	    curp->magval = CALYEARS_MAGIC ;
	} /* end if (calyears_magic) */
	return rs ;
} /* end subroutine (calyears_curbegin) */

int calyears_curend(CYS *op,CYS_C *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = calyears_magic(op,curp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == CALYEARS_MAGIC) {
		rs = SR_OK ;
	        if (curp->results) {
	            {
	                rs1 = calyears_resultfins(op,curp) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	            {
	                rs1 = lm_free(curp->results) ;
	                if (rs >= 0) rs = rs1 ;
	                curp->results = nullptr ;
	            }
	        } /* end if */
	        if (op->ncursors > 0) {
	            op->ncursors -= 1 ;
	        }
	        curp->i = 0 ;
	        curp->magval = 0 ;
	    } /* end if (cursor-valid) */
	} /* end if (calyears_magic) */
	return rs ;
} /* end subroutine (calyears_curend) */

/* args: 'qp' is in an input */
int calyears_curlook(CYS *op,CYS_C *curp,con CYS_Q *qp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = calyears_magic(op,curp,qp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == CALYEARS_MAGIC) ylikely {
		if ((rs = yearok(qp->y)) >= 0) {
		    if (curp->results) {
	    	        calyears_resultfins(op,curp) ;
	    	        lm_free(curp->results) ;
	    	        curp->results = nullptr ;
		    } /* end if (release) */
	            if (qp->y == 0) {
	                if ((rs = calyears_year(op,0)) >= 0) {
		            qp->y = op->year ;
	                }
	            } /* end if (zero-year) */

	if (rs >= 0) {
	    cint	sz = szof(CYS_E) ;
	    int 	vo = 0 ;
	    vo |= vecobjm.ordered ;
	    vo |= vecobjm.stationary ;
	    if (vecobj res ; (rs = res.start(sz,0,vo)) >= 0) {
	        vechand		*clp = op->calp ;
		void *vp ;
	        for (int i = 0 ; clp->get(clp,i,&vp) >= 0 ; i += 1) {
	            if (CALMGR *cmp = resumelife<CALMGR>(vp) ; cmp) {
	                rs = calyears_lookmgr(op,&res,cmp,qp) ;
	                c += rs ;
		    }
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            rs = calyears_mkresults(op,&res,curp) ;
	        } /* end if (ok) */
	        if ((rs < 0) || (c > 0)) {
	            for (int i = 0 ; res.get(i,&vp) >= 0 ; i += 1) {
	                if (CYS_Q *ep = resumelife<CYS_Q>(vp) ; ep) {
	                    calent_finish(ep) ;
		        }
	            } /* end for */
	        } /* end if (error) */
	        rs1 = res.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (res) */
	} /* end if (ok) */

	        } /* end if (yearok) */
	    } /* end if (cursor-magic) */
	} /* end if (calyears_magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_curlook) */

/* args: 'qp' is an output */
int calyears_curread(CYS *op,CYS_C *curp,mut CYS_Q *qp,
		char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if ((rs = calyears_magic(op,curp,qp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == CALYEARS_MAGIC) ylikely {
		rs = SR_OK ;
	        if (curp->results) {
	    	    cint	i = curp->i ;
	    	    if ((i >= 0) && (i < curp->nresults)) {
	        	CYS_E	*ep, *res = (CYS_E *) curp->results ;
			ep = (res + i) ;
	        	qp->y = ep->q.y ;
	        	qp->m = ep->q.m ;
	        	qp->d = ep->q.d ;
	        	if (rbuf) {
	            	    rs = calyears_loadbuf(op,rbuf,rlen,ep) ;
	            	    len = rs ;
	        	} /* end if */
	        	if (rs >= 0) {
	            	    curp->i = (i + 1) ;
			} /* end if (ok) */
	    	   } else {
	        	rs = SR_NOTFOUND ;
	    	   }
		} else {
	    	   rs = SR_NOTFOUND ;
		}
	    } /* end if (cursor-magic) */
	} /* end if (commandment_magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (calyears_curread) */

int calyears_check(CYS *op,time_t dt) noex {
	int		rs ;
	if ((rs = calyears_magic(op,curp,qp)) >= 0) ylikely {
#ifdef	COMMENT
	    rs = calyears_checkupdate(op,dt) ;
#endif
	} /* end if (commandment_magic) */
	return rs ;
} /* end subroutine (calyears_check) */

int calyears_already(CYS *op,vecobj *rlp,CYS_E *ep) noex {
	int		rs ;
	int		f = false ; /* return-value */
	if ((rs = calyears_magic(op,rlp,ep)) >= 0) ylikely {
	    uint	nhash ;
	    uint	ohash ;
	    if ((rs = calyears_gethash(op,ep,&nhash)) >= 0) {
		void *vp ;
	        for (int i = 0 rlp->get(i,&vp) >= 0 ; i += 1) {
	            if (CYS_E *oep = resumelife<CYS_E>(vp) ; oep) {
	                if ((rs = calyears_gethash(op,oep,&ohash)) >= 0) {
	    	            if (nhash == ohash) {
			        if ((rs = calyears_samewords(op,ep,oep)) > 0) {
				    f = true ;
	    	        	    if (f) break ;
			        }
			    } /* end if (same hash) */
		        } /* end if (calyears_gethash) */
	            } /* end if (non-null) */
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (calyears_gethash) */
	} /* end if (commandment_magic) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (calyears_already) */

int calyears_havestart(CYS *op,CYS_Q *qp,int y,cchar *lp,int ll) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		si = 0 ; /* this serves as the result flag */
	if ((rs = calyears_magic(op,lp)) >= 0) ylikely {
	    int		ch = MKCHAR(lp[0]) ;
	if (! CHAR_ISWHITE(ch)) {
	    if ((si = sibrk(lp,ll," \t")) >= 3) {
		if (isdigitlatin(ch)) {
		    cchar	*tp ;
	    	    if ((tp = strnchr(lp,ll,'/')) != nt) {
		    	int	cl ;
		    	cchar	*cp ;
			int tl = conv<int>(tp - lp) ;
	        	if ((rs = mkmonth(lp,tl)) >= 0) {
	        	    qp->m = conv<uchar>(rs & UCHAR_MAX) ;
	            	    cp = (tp + 1) ;
	            	    cl = conv<int>((lp + ll) - cp) ;
	            	    if ((tp = strnbrk(cp,cl," \t")) != np) {
			        cl = conv<int>(tp - cp) ;
			    }
	            	    if ((rs = calyears_mkday(op,y,qp->m,cp,cl)) >= 0) {
	            	        qp->d = conv<uchar>(rs & UCHAR_MAX) ;
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
	} /* end if (commandment_magic) */
	return (rs >= 0) ? si : rs ;
} /* end subroutine (calyears_havestart) */

int calyears_audit(CYS *op) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = calyears_magic(op)) >= 0) ylikely {
	    vechand *calp = op->calp ;
	    void *vp{} ;
	    for (int i = 0 ; calp->get(i,&vp) >= 0 ; i += 1) {
		if (calmgr *cmp = resumelife<calmgr>(vp) ; cmp) {
	            c += 1 ;
	            rs = calmgr_audit(cmp) ;
		} /* end if (non-null) */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (calyears_magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_audit) */


/* private subroutines */

local int calyears_argbeg(CYS *op,cchar *pr) noex {
	int		rs ;
	int		sz = 0 ;
	sz += (lenstr(pr) + 1) ;
	if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
	    op->a = bp ;
	    op->pr = bp ;
	    bp = (strwcpy(bp,pr,-1) + 1) ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (calyears_argbeg) */

local int calyears_argend(CYS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a) {
	   rs1 = lm_free(op->a) ;
	   if (rs >= 0) rs = rs1 ;
	   op->a = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (calyears_argend) */

local int calyears_opensub(CYS *op,cchar **dns,cchar **cns) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
        if (SI si ; (rs = subinfo_start(&si,op,0)) >= 0) {
            if ((rs = subinfo_calscreate(&si,dns,cns)) >= 0) {
                c = rs ;
                op->nentries = c ;
            }
            rs1 = subinfo_finish(&si) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (subinfo) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_opensub) */

local int calyears_resultfins(CYS *op,CYS_C *curp) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (curp->results) {
	        CYS_E	*ep = (CYS_E *) curp->results ;
	        for (int i = 0 ; i < curp->nresults ; i += 1) {
	            rs1 = calent_finish(ep+i) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end for */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calyears_resultfins) */

local int calyears_lookmgr(CYS *op,vecobj *rlp,CALMGR *calp,
		CYS_Q *qp) noex {
	int		rs = SR_BUGCHECK ;
	int		c = 0 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    {
	        rs = calmgr_lookup(calp,rlp,qp) ;
	        c = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_lookmgr) */

local int calyears_mkresults(CYS *op,vecobj *rlp,CYS_C *curp) noex {
	int		rs = SR_BUGCHECK ;
	int		c = 0 ; /* return-value */
	if (op && rlp && curp) ylikely {
	    if ((rs = rlp->sort(vrcmp)) >= 0) ylikely {
	        if ((rs = elp->count) > 0) {
	            cint	sz = (rs * szof(CYS_E)) ;
	            if (void *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
	                CYS_E *rp = resumelife<CYS_E>(bp) ;
		        void *vp ;
		        for (int i = 0 ; rlp->get(i,&vp) >= 0 ; i += 1) {
	                    if (CYS_E *ep = resumelife<CYS_E>(vp) ; ep) {
	                        rp[c++] = *ep ;	 /* copy! */
	                	rlp->del(i) ; /* entries are stationary */
		            }
	                } /* end for */
	                if (rs >= 0) {
	                    curp->results = rp ;
	                    curp->nresults = c ;
	                    curp->i = 0 ;
	                } else {
	                    lm_free(rp) ;
		        }
	            } /* end if (memory-acquire) */
		} /* end if (vecobj_count) */
	    } /* end if (vecobj_sort) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_mkresults) */

local int calyears_calsdestroy(CYS *op) noex {
    	vechand		*calp = op->calp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void *vp ;
	for (int i = 0 ; calp->get(i,&vp) >= 0 ; i += 1) {
	    if (CALMGR *cmp = resumelife<CALMGR>(vp) ; cmp) ylikely {
		{
		    rs1 = calmgr_finish(cmp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = lm_free(cmp) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (non-null) */
	} /* end for */
	return rs ;
} /* end subroutine (calyears_calsdestroy) */

local int calyears_year(CYS *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->year == 0) {
	    if (dt == 0) dt = getustime ;
	    if (TMTIME ts ; (rs = tmtime_timelocal(&ts,dt)) >= 0) ylikely {
	       op->year		= (ts.year + TMTIME_YEARBASE) ;
	       op->isdst	= ts.isdst ;
	       op->gmtoff	= ts.gmtoff ; /* seconds west of GMT */
	    } /* end if (tmtime_timelocal) */
	} /* end if (year needed) */
	return rs ;
} /* end subroutine (calyears_year) */

local int calyears_mkday(CYS *op,int y,int m,cchar *cp,int cl) noex {
	int		rs ;
	if (DOM *dmp ; ((rs = calyars_domyear(op,y,&dmp)) >= 0) ylikely {
	    rs = dayofmonth_mkday(dmp,m,cp,cl) ;
	}
	return rs ;
} /* end subroutine (calyears_mkday) */

/* HERE */
local int calyars_domyear(CYS *op,int y,DOM **rpp) noex {
	CYS_DOMER	*dop ;
	vechand		*dlp = op->domp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	void *vp ;
	for (int i = 0 ; (rs = dlp->get(i,&vp)) >= 0 ; i += 1) {
	    if (dop = resumelife<CYS_DOMER>(vp) ; dop) ylikely {
		if (dop->year == y) break ;
	    }
	} /* end for */
	if (rs >= 0) ylikely {
	    if (rpp) {
		DOM	*dmp ;
		if ((rs = calyears_domerget(op,dop,&dmp)) >= 0) {
	            *rpp = dmp ;
		}
	    }
	} else if (rs == rsn) {
	    cint	dsz = szof(CYS_DOMER) ;
	    if ((rs = lm_mall(dsz,&vp)) >= 0) {
		int	f_ent = true ;
	        dop = resumelife<CYS_DOMER>(vp) ;
	        if ((rs = calyears_domerbeg(op,dop,y)) >= 0) {
		    if ((rs = dlp->add(dop)) >= 0) {
			int	di = rs ;
			f_ent = false ;
	    	        if (rpp) {
			    DOM	*dmp ;
			    f_ent = true ;
			    if ((rs = calyears_domerget(op,dop,&dmp)) >= 0) {
				f_ent = false ;
	            	        *rpp = dmp ;
			    }
			}
			if (rs < 0) {
			    vechand_del(dlp,di) ;
			} /* end if (error) */
		    } /* end if (vechand_add) */
		    if (rs < 0) {
			calyears_domerend(op,dop) ;
		    } /* end if (error) */
		} /* end if (calyears_domerbeg) */
	        if ((rs < 0) && f_ent) {
		    lm_free(dop) ;
		} /* end if (error) */
	    } /* end if (m-a) */
	} /* end if (found or not) */
	return rs ;
} /* end subroutine (calyars_domyear) */

local int calyears_domerfins(CYS *op) noex {
	vechand		*dlp = op->domp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	void *vp ;
	for (int i = 0 ; dlp->get(i,&vp) >= 0 ; i += 1) {
	    if (CYS_DOMER *dep = resumelife<CYS_DOMER>(vp) ; dep) {
		c += 1 ;
		{
		    rs1 = calyears_domerend(op,dep) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = lm_free(dep) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (non-null) */
	} /* end for */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (calyears_domerfins) */

local int calyears_domerbeg(CYS *op,CYS_DOMER *dop,int y) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    if ((rs = dayofmonth_start(&dop->dom,y)) >= 0) {
	        dop->year = y ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end if (calyears_domerbeg) */

local int calyears_domerend(CYS *op,CYS_DOMER *dep) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    {
	        rs1 = dayofmonth_finish(&dep->dom) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    dep->year = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calyears_domerend) */

local int calyears_domerget(CYS *op,CYS_DOMER *dop,DOM **rpp) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (rpp) {
	        *rpp = &dop->dom ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calyars_domerget) */

local int calyears_gethash(CYS *op,CYS_E *ep,uint *rp) noex {
	int		rs ;
	if ((rs = calent_getci(ep)) >= 0) ylikely {
	   CALMGR	*cmp ;
	   cint	ci = rs ;
	   if ((rs = calyears_getcm(op,ci,&cmp)) >= 0) ylikely {
		rs = calmgr_gethash(cmp,ep,rp) ;
	   }
	}
	return rs ;
} /* end subroutine (calyears_gethash) */

/* get the CALMGR (pointer to) given a CALMGR index */
local int calyears_getcm(CYS *op,int ci,CALMGR **rpp) noex {
	vechand		*clp = op->calp ;
	int		rs ;
	{
	    rs = clp->get(ci,rpp) ;
	}
	return rs ;
} /* end subroutine (calyears_getcm) */

local int calyears_samewords(CYS *op,CYS_E *ep,CYS_E *oep) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	cchar		*md1 ;
	cchar		*md2 ;
	if ((rs = calyears_getcalbase(op,ep,&md1)) >= 0) {
	    if ((rs = calyears_getcalbase(op,oep,&md2)) >= 0) {
		CALWORDER	w1 ;
		CALWORDER	w2 ;
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
	        	rs1 = calworder_finish(&w2) ;
			if (rs >= 0) rs = rs1 ;
	    	    } /* end if (w2) */
	    	    rs1 = calworder_finish(&w1) ;
		   if (rs >= 0) rs = rs1 ;
		} /* end if (w1) */
	    } /* end if (calyears_getcalbase) */
	} /* end if (calyears_getcalbase) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (calyears_samewords) */

local int calyears_getcalbase(CYS *op,CYS_E *ep,cchar **rpp) noex {
	int		rs ;
	if ((rs = calent_getci(ep)) >= 0) {
	    CALMGR	*cmp ;
	    if ((rs = calyears_getcm(op,rs,&cmp)) >= 0) {
		rs = calmgr_getbase(cmp,rpp) ;
	    }
	}
	return rs ;
} /* end subroutine (calyears_getcalbase) */

local int calyears_loadbuf(CYS *op,char *rbuf,int rlen,CYS_E *ep) noex {
	int		rs ;
	if ((rs = calent_getci(ep)) >= 0) ylikely {
	    vechand	*ilp = op->calp ;
	    cint	cidx = rs ;
	    void *vp ;
	    if ((rs = ilp->get(cidx,&vp)) >= 0) {
	        if ((CALMGR *cmp = resumelife<CALMGR>(vp) ; cmp) {
	            rs = calmgr_loadbuf(cmp,rbuf,rlen,ep) ;
		}
	    } /* end if (vechand_get) */
	} /* end if (calet_getci) */
	return rs ;
} /* end subroutine (calyears_loadbuf) */

#if	CF_TRANSHOL
local int calyears_transhol(CYS *op,CYS_Q *qp,int y,
		cc *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		nal = 0 ;
	int		f_found = false ; /* return-value */
	bool		f_negative = false ;
	cchar		*nap = nullptr ;

	qp->m = 0 ;
	qp->d = 0 ;
	qp->y = ushort(y) ;

	if (cchar *tp ; (tp = strnbrk(sp,sl,"+-")) != np) {
	    nap = (tp + 1) ;
	    nal = (sl - intconv((tp + 1) - sp)) ;
	    sl = intconv(tp - sp) ;
	    f_negative = (tp[0] == '-') ;
	}
	if ((rs = calyears_dayname(op,qp,y,sp,sl)) > 0) {
	    f_found = true ;
	    if (nl > 0) {
	        if (int odays{} ; (rs = cfdeci(nap,nal,&odays)) >= 0) {
	            time_t	dt = getustime ;
	            if (f_negative) odays = (neg odays) ;
		    if (tmtime tm ; (rs = tmtime_timelocal(&tm,dt)) >= 0) {
	                tm.isdst = -1 ;
	                tm.gmtoff = op->gmtoff ;
	                tm.year = (y - TMTIME_YEARBASE) ;
	                tm.mon = qp->m ;
	                tm.mday = (qp->d + odays) ;
	                if ((rs = tmtime_adjtime(&tm,&t)) >= 0) {
	                    qp->m = (uchar) tm.mon ;
	                    qp->d = (uchar) tm.mday ;
			    qp->y = (ushort) (tm.year + TMTIME_YEARBASE) ;
	                }
		    } /* end if (tmtime_timelocal) */
	        } /* end if (odays) */
	    } /* end if (positive) */
	} /* end if (day-offset required) */

	return (rs >= 0) ? f_found : rs ;
} /* end subroutine (calyears_transhol) */
#endif /* CF_TRANSHOL */

#if	CF_TRANSHOL
local int calyears_dayname(CYS *op,CYS_Q *qp,int y,
		cchar *sp,int sl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if ((rs = calyears_holidayer(op)) > 0) {
	    holidayer		*holp = op->holp ;
	    holidayer_q		hc ;
	    cint		hlen = HOLBUFLEN ;
	    char		hbuf[HOLBUFLEN + 1] ;
	    holidayer_cur	hcur ;
	    if ((rs = holidayer_curbegin(holp,&hcur)) >= 0) {
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
} /* end subroutine (calyears_dayname) */

local int calyears_holidayer(CYS *op) noex {
	int		rs = SR_OK ;
	int		f = op->open.hols ;
	if (! op->init.hols) {
	    holidayer	*holp = op->holp ;
	    op->init.hols = true ;
	    if ((rs = holidayer_open(holp,op->pr)) >= 0) {
	        op->open.hols = true ;
		f = true ;
	    } else if (isNotHols(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (open database as necessary) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (calyears_holidayer) */
#endif /* CF_TRANSHOL */

#ifdef	COMMENT
local int calyears_checkupdate(CYS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		to = TO_CHECK ;
	int		f = false ;
	if (op->ncursors == 0) {
	    if (dt <= 0) dt = getustime ;
	    if ((dt - op->ti_lastcheck) >= to) {
	        op->ti_lastcheck = dt ;
	        if (ustat db ; (rs1 = u_stat(op->dbfname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > op->ti_db) ;
		    f = f || (sb.st_mtime > op->ti_map) ;
		    if (f) {
	                calyears_dbloadend(op) ;
	                if (SI si ; (rs = subinfo_start(&si,op,0)) >= 0) {
			    {
	                        rs = calyears_dbloadbegin(op,&si) ;
			    }
	                    rs1 = subinfo_finish(&si) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (subinfo) */
	            } /* end if (update) */
	        } /* end if (stat) */
	    } /* end if (time-out) */
	} /* end if (no cursors out) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (calyears_checkupdate) */
#endif /* COMMENT */

local int subinfo_start(SI *sip,CYS *op,time_t dt) noex {
	int		rs = SR_BUGCHECK ;
	if (dt == 0) dt = getustime ;
	if (sip && op) ylikely {
	    memclear(sip) ;
	    if (char *p ; (rs = lm_un(&p)) >= 0) ylikely {
		sip->username = p ;
	        sip->op = op ;
	        sip->dt = dt ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->fl.dirs) {
	    sip->fl.dirs = false ;
	    rs1 = vecstr_finish(&sip->dirs) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dns = nullptr ;
	}
	if (sip->tudname) {
	    rs1 = lm_free(sip->tudname) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->tudname = nullptr ;
	} /* end if (memory-release) */
	if (sip->userhome) {
	    rs1 = lm_free(sip->userhome) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->userhome = nullptr ;
	} /* end if (memory-release) */
	if (sip->fl.id) {
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->fl.id = false ;
	} /* end if (memory-release) */
	if (sip->username) {
	    rs1 = lm_free(sip->username) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->username = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (subinfo_finish) */

local int subinfo_calscreate(SI *sip,cchar **dns,cchar **cns) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (dns == nullptr) {
	    if ((rs = subinfo_mkdns(sip)) >= 0) {
	        dns = sip->dns ;
	    }
	}
	if ((rs >= 0) && (dns != nullptr)) {
	    for (int i = 0 ; dns[i] != nullptr ; i += 1) {
		cchar	*dn = dns[i] ;
	        if (dn[0] != '\0') {
	            rs = subinfo_calscreater(sip,dn,cns) ;
	            c += rs ;
		}
	        if (rs < 0) break ;
	    } /* end for (dns) */
	} /* end if (dns) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_calscreate) */

local int subinfo_calscreater(SI *sip,cchar *dn,cchar **cns) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	int		f_search = false ;
	cchar	**names = nullptr ;
	if (cns == nullptr) {
	    vecstr cals ;
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
} /* end subroutine (subinfo_calscreater) */

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
			vechand	*clp = op->calp ;
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
} /* end subroutine (subinfo_calcreate) */

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
} /* end subroutine (subinfo_mkdns) */

local int subinfo_havedir(SI *sip,cchar *dn) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (sip) ylikely {
	    if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
	        f = !!S_ISDIR(sb.st_mode) ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (subinfo_havedir) */

local int subinfo_ids(SI *sip) noex {
	int		rs = SR_OK ;
	if (! sip->fl.id) {
	    sip->fl.id = true ;
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
} /* end subroutine (subinfo_ids) */

local int subinfo_loadnames(SI *sip,vecstr *nlp,cchar *dirname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (sip) ylikely {
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
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_loadnames) */

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
} /* end subroutine (subinfo_username) */

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
} /* end subroutine (subinfo_tmpuserdir) */
#endif /* COMMENT */

#if	CF_CHECKDNAME
local int subinfo_checkdname(SI *sip,cchar *dname) noex {
	int		rs = SR_INVALID ;
	if (dname[0] == '/') {
	    if (ustat sb ; (rs = uc_stat(dname,&sb)) >= 0) {
		rs = SR_NOTDIR ;
	        if (S_ISDIR(sb.st_mode)) {
	            if ((rs = subinfo_ids(sip)) >= 0) {
	                rs = permids(&sip->id,&sb,W_OK) ;
		    }
	        } /* end if (directory) */
	    } /* end if (uc_stat) */
	} /* end if */
	return rs ;
} /* end subroutine (subinfo_checkdname) */
#endif /* CF_CHECKDNAME */

local int subinfo_regacc(SI *sip,cchar *fn,int am) noex {
	int		rs ;
	int		f = false ;
	if (ustat sb ; (rs = u_stat(fn,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode)) {
	        if ((rs = subinfo_ids(sip)) >= 0) {
	            if ((rs = permids(&sip->id,&sb,am)) >= 0) {
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
} /* end subroutine (subinfo_regacc) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
	    usernamelen = rs ;
	} /* end if */
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
} /* end subroutine (mkdname) */
#endif /* CF_MKDNAME */

local int mkmonth(cchar *cp,int cl) noex {
	int		rs ;
	int		v = 0 ; /* return-value */
	if ((rs = cfdeci(cp,cl,&v)) >= 0) ylikely {
	    v -= 1 ;
	} /* end if */
	return (rs >= 0) ? v : rs ;
} /* end subroutine (mkmonth) */

local int yearok(int y) noex {
    	int		rs = SR_OK ;
	bool		fbad = false ;
	fbad = fbad || ((y < 1970) && (y != 0)) ;
	fbad = fbad || (y >= 2038) ;
	if (fbad) {
	    rs = SR_DOM ;
	}
	return rs ;
} /* end subroutine (yearok) */

/* for use with 'vecobj_sort(3dam)' or similar */
local int vrcmp(cvoid *v1p,cvoid *v2p) noex {
	con CYS_E	**e1pp = (con CYS_E **) v1p ;
	con CYS_E	**e2pp = (con CYS_E **) v2p ;
	int		rc = 0 ;
	if (r1pp && e2pp) ylikely {
	    con CYS_E	*e1p = *e1pp ;
	    con CYS_E	*e2p = *e2pp ;
	    if (e1p || e2p) ylikely {
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
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (vrcmp) */

local bool isNotOrIllegalSeq(int rs) noex {
	return isOneOf(rsnotorils,rs) ;
} /* end subroutine (isNotOrIllegalSeq) */

local bool isNotHols(int rs) noex {
	return isOneOf(rsnothols,rs) ;
} /* end subroutine (isNotHols) */


