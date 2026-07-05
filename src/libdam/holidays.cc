/* holidays SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* access for the holidays database */
/* version %I% last-modified %G% */

#define	CF_FIRSTHASH	0		/* perform FIRSTHASH */

/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	holidays

	Description:
	This object module provides an interface to the holidays (see
	|holidays(4)|) database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<tzfile.h>		/* POSIX TM_YEAR_BASE */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<strtab.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<nleadstr.h>		/* LIBUC */
#include	<hash.h>		/* LIBUC |hash_{x}(3uc)| */
#include	<hashindex.h>		/* LIBUC */
#include	<nextpowtwo.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |DIGBUFLEN| */
#include	<bfile.h>		/* LIBB */

#include	"holidays.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	HO		holidays
#define	HO_MAGIC	HOLIDAYS_MAGIC
#define	HO_DEFRECS	20
#define	HO_HOLSUF	"holidays"
#define	HO_MAXRECS	(USHORT_MAX - 2)
#define	HO_NSKIP	4
#define	HO_OBJ		holidays_obj
#define	HO_CITE		holidays_cite
#define	HO_CUR		holidays_cur

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	40
#endif

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	SI		subinfo
#define	SI_REC		subinfo_rec


/* external subroutines */


/* external variables */


/* local structures */

struct varentry {
	uint		khash ;
	uint		ri ;
	uint		ki ;
	uint		hi ;
} ; /* end struct */

enum itentries {
	itentry_ri,
	itentry_info,
	itentry_nhi,
	itentry_overlast
} ; /* end enum */


/* local structures */

struct subinfo_rec {
	uint		cite ;		/* m:d */
	uint		ki ;		/* key-string index */
	uint		vi ;		/* val-string index */
} ; /* end struct */

struct subinfo {
	holidays	*op ;
	vecobj		recs ;
	strtab		kstrs ;
	strtab		vstrs ;
	bfile		hfile ;
	int		fsz ;
} ; /* end struct */


/* forward references */

local int	holidays_dbfind(HO *,ids *,char *) noex ;
local int	holidays_dbfinder(HO *,ids *,char *,cchar *) noex ;

local int	subinfo_start(SI *,HO *) noex ;
local int	subinfo_finish(SI *) noex ;
local int	subinfo_procfile(SI *) noex ;
local int	subinfo_procyear(SI *,cchar *,int) noex ;
local int	subinfo_procline(SI *,cchar *,int) noex ;
local int	subinfo_proclineval(SI *,uint,cchar *,int) noex ;
local int	subinfo_mkdata(SI *) noex ;
local int	subinfo_mkrt(SI *) noex ;
local int	subinfo_mkst(SI *) noex ;
local int	subinfo_mkind(SI *,cchar *,int (*)[3],int) noex ;

local int	getyear(time_t) noex ;
local int	getcite(uint *,cchar *,int) noex ;
local int	mkcite(uint *,int,int) noex ;

local int	indinsert(uint (*rt)[3],int (*it)[3],int,varentry *) noex ;

local bool	ismatkey(cchar *,cchar *,int) noex ;

local int	vcmprec(cvoid **,cvoid **) noex ;
local int	cmprec(cvoid *,cvoid *) noex ;


/* local variables */

constexpr cpcchar	holdnames[] = {
	"etc/acct",
	"etc",
	"/etc/acct",
	nullptr
} ; /* end array */


/* exported variables */

const holidays_obj	holidays_modinfo = {
	"holidays",
	szof(holidays),
	szof(HO_CUR)
} ; /* end array */


/* exported subroutines */

int holidays_open(HO *op,cchar *pr,int year,cchar *fname) noex {
	time_t		dt = time(nullptr) ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fl = -1 ;
	int		c = 0 ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	if (year <= 0)
	    year = getyear(dt) ;

	if (year < 1970)
	    return SR_INVALID ;

	memclear(op) ;
	op->year = year ;
	op->pr = pr ;

	if ((fname == nullptr) || (fname[0] == '\0')) {
	    if (ids id ; (ids_load(&id)) >= 0) {
	        rs = holidays_dbfind(op,&id,tmpfname) ;
	        fl = rs ;
	        if ((rs >= 0) && (fl > 0)) {
	            fname = tmpfname ;
		}
	        ids_release(&id) ;
	    } /* end if (ids) */

	} /* end if */

	if (rs >= 0) {
	    if (cchar *cp ; (rs = lm_strw(fname,fl,&cp)) >= 0) {
	        SI	si ;
	        op->fname = cp ;
	        if ((rs = subinfo_start(&si,op)) >= 0) {
	            if ((rs = subinfo_procfile(&si)) >= 0) {
	                c = rs ;
	                if ((rs = subinfo_mkdata(&si)) >= 0) {
	                    op->ti_check = dt ;
	                    op->magval = HO_MAGIC ;
	                }
	            }
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	        if (rs < 0) {
	            void *vp = voidp(op->fname) ;
	            lm_free(vp) ;
	            op->fname = nullptr ;
	        } /* end if (error) */
	    } /* end if (m-a) */
	} /* end if (ok) */
	if (rs < 0) {
	    holidays_close(op) ;
	} /* end if (error) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (holidays_open) */

int holidays_close(HO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	if (op->vst) {
	    rs1 = lm_free(op->vst) ;
	    if (rs >= 0) rs = rs1 ;
	    op->vst = nullptr ;
	} /* end if (memory-release) */
	if (op->kit) {
	    rs1 = lm_free(op->kit) ;
	    if (rs >= 0) rs = rs1 ;
	    op->kit = nullptr ;
	}
	if (op->kst) {
	    rs1 = lm_free(op->kst) ;
	    if (rs >= 0) rs = rs1 ;
	    op->kst = nullptr ;
	}
	if (op->rt) {
	    rs1 = lm_free(op->rt) ;
	    if (rs >= 0) rs = rs1 ;
	    op->rt = nullptr ;
	}
	if (op->fname) {
	    void *vp = voidp(op->fname) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}
	op->magval = 0 ;
	return rs ;
} /* end subroutine (holidays_close) */

int holidays_count(HO *op) noex {
	int		rs = SR_OK ;
	int		c ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	c = (op->rtlen - 1) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (holidays_count) */

int holidays_audit(HO *op) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	return rs ;
}
/* end subroutine (holidays_audit) */

int HO_CURbegin(HO *op,holidays_cur *curp) noex {
    	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	curp->chash = 0 ;
	op->ncursors += 1 ;

	return rs ;
}
/* end subroutine (HO_CURbegin) */

int HO_CURend(HO *op,holidays_cur *curp) noex {
    	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	if (op->ncursors > 0) {
	    op->ncursors -= 1 ;
	}

	return rs ;
} /* end subroutine (HO_CURend) */

int holidays_fetchcite(HO *op,HO_CITE *qp,HO_CUR *curp,
		char *vbuf,int vlen) noex {
	HO_CUR		dcur ;
	uint		(*rt)[3] ;
	uint		(*rpp)[3] ;
	uint		scite ;
	cint	esz = (3 * szof(uint)) ;
	int		rs = SR_OK ;
	int		ri, vi ;
	int		rtlen ;
	int		vl = 0 ;
	cchar	*vst ;
	cchar	*vp ;

	if (op == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	if (curp == nullptr) {
	    curp = &dcur ;
	    curp->i = 0 ;
	}

	if (vbuf != nullptr)
	    vbuf[0] = '\0' ;

	vst = op->vst ;

	rt = op->rt ;
	rtlen = op->rtlen ;

	scite = 0 ;
	scite |= (qp->m << 8) ;
	scite |= (qp->d << 0) ;

	if (curp->i <= 0) {

	    uint	(*srt)[3] = (rt + 1) ;
	    int		srtlen = (rtlen - 1) ;
	    rpp = (uint (*)[3]) bsearch(&scite,srt,srtlen,esz,cmprec) ;
	    if (rpp == nullptr)
	        rs = SR_NOTFOUND ;

	    if (rs >= 0) {
	        ri = intconv(rpp - rt) ;
	        while (ri > 0) {
	            int	pri ;
	            pri = (ri - 1) ;
	            if (scite != rt[pri][0])
	                break ;
	            ri = pri ;
	        } /* end while */
	    } /* end if (ok) */

	} else {

	    ri = (curp->i + 1) ;
	    if ((ri >= rtlen) || (scite != rt[ri][0]))
	        rs = SR_NOTFOUND ;

	} /* end if */

/* if successful, retrieve value */

	if (rs >= 0) {

	    vi = rt[ri][2] ;
	    vp = (vst + vi) ;
	    if (vbuf != nullptr) {
	        rs = sncpy1(vbuf,vlen,vp) ;
	        vl = rs ;
	    } else
	        vl = lenstr(vp) ;

	    if (qp != nullptr) {
	        uint	cite = rt[ri][0] ;
	        qp->m = ((cite >> 8) & UCHAR_MAX) ;
	        qp->d = ((cite >> 0) & UCHAR_MAX) ;
	    }

	    if (rs >= 0)
	        curp->i = ri ;

	} /* end if (got one) */

	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidays_fetchcite) */

int holidays_fetchname(HO *op,cc *kp,int kl,HO_CUR *curp,HO_CITE *qp,
		char *vbuf,int vlen) noex {
	HO_CUR	dcur ;
	uint		khash, nhash, chash ;
	uint		(*rt)[3] ;
	cint	nskip = HO_NSKIP ;
	int		rs = SR_OK ;
	int		ri, ki, vi, hi ;
	int		c ;
	int		(*it)[3] ;
	int		itlen ;
	int		vl = 0 ;
	int		f_mat = FALSE ;
	cchar	*kst, *vst ;
	cchar	*vp ;
	cchar	*cp ;
	char		keybuf[KEYBUFLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	if (curp == nullptr) {
	    curp = &dcur ;
	    curp->i = 0 ;
	}

	if (kl < 0)
	    kl = lenstr(kp) ;

	if (hasuc(kp,kl)) {
	    if (kl > KEYBUFLEN) kl = KEYBUFLEN ;
	    strwcpylc(keybuf,kp,kl) ;
	    kp = keybuf ;
	}

	if (vbuf != nullptr)
	    vbuf[0] = '\0' ;

	kst = op->kst ;
	vst = op->vst ;

	rt = op->rt ;
	it = op->kit ;
	itlen = op->itlen ;

	if (curp->i <= 0) {

/* unhappy or not, the index-table uses same-hash-linking! */

	    khash = hash_elf(kp,kl) ;

	    nhash = khash ;
	    chash = (khash & INT_MAX) ;
	    curp->chash = chash ;	/* store "check" hash */

	    hi = hashindex(khash,itlen) ;

	    c = 0 ;
	    while ((ri = it[hi][itentry_ri]) > 0) {

	        f_mat = ((it[hi][itentry_info] & INT_MAX) == chash) ;
	        if (f_mat) {
	            ki = rt[ri][1] ;
	            cp = (kst + ki) ;
	            f_mat = (cp[0] == kp[0]) && ismatkey(cp,kp,kl) ;
	        }

	        if (f_mat)
	            break ;

	        if ((it[hi][itentry_info] & (~ INT_MAX)) == 0)
	            break ;

	        if (c >= (itlen + nskip))
	            break ;

	        nhash = hash_again(nhash,c++,nskip) ;

	        hi = hashindex(nhash,itlen) ;

	    } /* end while */

	    if ((rs >= 0) && (! f_mat))
	        rs = SR_NOTFOUND ;

	} else {

	    chash = curp->chash ;
	    hi = curp->i ;

	    if (hi < itlen) {

	        ri = it[hi][itentry_ri] ;

	        if (ri > 0) {

	            hi = it[hi][itentry_nhi] ;

	            if (hi != 0) {

	                ri = it[hi][itentry_ri] ;
	                f_mat = ((it[hi][itentry_info] & INT_MAX) == chash) ;
	                if ((ri > 0) && f_mat) {
	                    ki = rt[ri][1] ;
	                    f_mat = ismatkey((kst + ki),kp,kl) ;
	                }

	                if (! f_mat) {
	                    rs = SR_NOTFOUND ;
			}

	            } else {
	                rs = SR_NOTFOUND ;
		    }

	        } else {
	            rs = SR_NOTFOUND ;
		}

	    } else {
	        rs = SR_NOTFOUND ;
	    }

	} /* end if (preparation) */

/* if successful, retrieve value */

	if (rs >= 0) {

	    vi = rt[ri][2] ;
	    vp = (vst + vi) ;
	    if (vbuf != nullptr) {
	        rs = sncpy1(vbuf,vlen,vp) ;
	        vl = rs ;
	    } else
	        vl = lenstr(vp) ;

	    if (qp != nullptr) {
	        uint	cite = rt[ri][0] ;
	        qp->m = ((cite >> 8) & UCHAR_MAX) ;
	        qp->d = ((cite >> 0) & UCHAR_MAX) ;
	    }

	    if (rs >= 0) {
	        curp->i = hi ;
	    }

	} /* end if (got one) */

	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidays_fetchname) */

int holidays_enum(HO *op,HO_CUR *curp,HO_CITE *qp,char *vbuf,int vlen) noex {
	uint		(*rt)[3] ;
	int		rs = SR_OK ;
	int		ri, vi ;
	int		vl = 0 ;
	cchar	*vst ;
	cchar	*vp ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	if (op->ncursors == 0) return SR_INVALID ;

	if (vbuf != nullptr) {
	    vbuf[0] = '\0' ;
	}

	ri = (curp->i < 1) ? 1 : (curp->i + 1) ;

/* ok, we're good to go */

	if (ri < op->rtlen) {
	    vst = op->vst ;
	    rt = op->rt ;
	    vi = rt[ri][2] ;
	    if (vi < op->vslen) {

	        vp = (vst + vi) ;
	        if (vbuf != nullptr) {
	            rs = sncpy1(vbuf,vlen,vp) ;
	            vl = rs ;
	        } else
	            vl = lenstr(vp) ;

	        if (qp != nullptr) {
	            uint	cite = rt[ri][0] ;
	            qp->m = ((cite >> 8) & UCHAR_MAX) ;
	            qp->d = ((cite >> 0) & UCHAR_MAX) ;
	        }

	        if (rs >= 0)
	            curp->i = ri ;

	    } else
	        rs = SR_BADFMT ;
	} else 
	    rs = SR_NOTFOUND ;

	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidays_enum) */

int holidays_check(HO *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f_changed = FALSE ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != HO_MAGIC) return SR_NOTOPEN ;

	if (dt == 0) dt = time(nullptr) ;

#ifdef	COMMENT
#else
	if (dt == 1) f_changed = TRUE ;
#endif

	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (holidays_check) */


/* private subroutines */

local int holidays_dbfind(HO *op,ids *idp,char *tmpfname) noex {
	int		rs ;
	int		fl = 0 ;
	cchar	*fsuf = HO_HOLSUF ;
	char		digbuf[DIGBUFLEN + 1] ;
	char		cname[MAXNAMELEN + 1] ;

	tmpfname[0] = '\0' ;
	if ((rs = ctdeci(digbuf,DIGBUFLEN,op->year)) >= 0) {
	    if ((rs = sncpy2(cname,MAXNAMELEN,fsuf,digbuf)) >= 0) {
	        rs = holidays_dbfinder(op,idp,tmpfname,cname) ;
	        fl = rs ;
	        if ((rs >= 0) && (fl == 0)) {
	            rs = holidays_dbfinder(op,idp,tmpfname,fsuf) ;
	            fl = rs ;
	        }
	    }
	} /* end if */

	return (rs >= 0) ? fl : rs ;
} /* end subroutine (holidays_dbfind) */

local int holidays_dbfinder(HO *op,ids *idp,char *tmpfname,
		cchar *cname) noex {
	ustat	sb ;
	int		rs = SR_OK ;
	int		rs1 = SR_NOENT ;
	int		i ;
	int		fl = 0 ;
	cchar	*hdn ;

	for (i = 0 ; holdnames[i] != nullptr ; i += 1) {

	    hdn = holdnames[i] ;
	    if (hdn[0] != '/') {
	        rs = mkpath3(tmpfname,op->pr,hdn,cname) ;
	    } else {
	        rs = mkpath2(tmpfname,hdn,cname) ;
	    }
	    fl = rs ;

	    if (rs >= 0) {
	        if ((rs1 = u_stat(tmpfname,&sb)) >= 0) {
	            rs1 = permid(idp,&sb,R_OK) ;
		}
	    }

	    if (rs1 >= 0)
	        break ;

	} /* end for */

	if (rs1 < 0)
	    fl = 0 ;

	return (rs >= 0) ? fl : rs ;
} /* end subroutine (holidays_dbfinder) */

local int subinfo_start(SI *sip,HO *op) noex {
	int		rs = SR_OK ;
	memclear(sip) ;
	sip->op = op ;
	if ((rs = bopen(&sip->hfile,op->fname,"r",0666)) >= 0) {
	    cint	sz = szof(SI_REC) ;
	    cint	n = HO_DEFRECS ;
	    if ((rs = vecobj_start(&sip->recs,sz,n,0)) >= 0) {
	        ustat	sb ;
	        bcontrol(&sip->hfile,BC_STAT,&sb) ;
	        sip->fsz = intconv(sb.st_size) ;
	        if ((rs = strtab_start(&sip->kstrs,(sip->fsz/3))) >= 0) {
	            rs = strtab_start(&sip->vstrs,sip->fsz) ;
	            if (rs < 0) {
	                strtab_finish(&sip->kstrs) ;
		    } /* end if (error) */
	        }
	        if (rs < 0) {
	            vecobj_finish(&sip->recs) ;
		} /* end if (error) */
	    } /* end if (vecobj_start) */
	    if (rs < 0) {
	        bclose(&sip->hfile) ;
	    } /* end if (error) */
	} /* end if (file-open) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	rs1 = strtab_finish(&sip->vstrs) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = strtab_finish(&sip->kstrs) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = vecobj_finish(&sip->recs) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = bclose(&sip->hfile) ;
	if (rs >= 0) rs = rs1 ;
	}
	sip->op = nullptr ;
	return rs ;
} /* end subroutine (subinfo_finish) */

local int subinfo_procfile(SI *sip) noex {
	HO		*op = sip->op ;
	cint	llen = LINEBUFLEN ;
	cint	maxrecs = HO_MAXRECS ;
	int		rs = SR_OK ;
	int		len ;
	int		c = 0 ;
	int		f_start = TRUE ;
	char		lbuf[LINEBUFLEN + 1] ;

	while ((rs = breadln(&sip->hfile,lbuf,llen)) > 0) {
	    len = rs ;

	    if (lbuf[len-1] == '\n') len -= 1 ;
	    if ((len == 0) || (lbuf[0] == '*')) continue ;

	    while (CHAR_ISWHITE(lbuf[len-1])) len -= 1 ;
	    if (len == 0) continue ;

	    if (f_start) {
	        int	year ;
	        f_start = FALSE ;
	        rs = subinfo_procyear(sip,lbuf,len) ;
	        year = rs ;

	        if ((rs >= 0) && (year != op->year))
	            rs = SR_NOMSG ;
	    } else {
	        rs = subinfo_procline(sip,lbuf,len) ;
	        c += rs ;

	    }

	    if (c >= maxrecs) break ;
	    if (rs < 0) break ;
	} /* end if (reading) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_procfile) */

local int subinfo_procyear(SI *sip,cchar lbuf[],int llen) noex {
	int		rs = SR_ILSEQ ;
	int		sl ;
	int		year = SR_ILSEQ ;
	cchar	*sp ;

	if (sip == nullptr) return SR_FAULT ;

	sp = lbuf ;
	sl = llen ;
	cchar *cp ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	    rs = cfdeci(cp,cl,&year) ;
	}

	return (rs >= 0) ? year : rs ;
} /* end subroutine (subinfo_procyear) */

local int subinfo_procline(SI *sip,cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		sl = llen ;
	int		cl ;
	int		c = 0 ;
	cchar	*sp = lbuf ;
	cchar	*cp ;

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    uint	cite ;
	    if (getcite(&cite,cp,cl) >= 0) { /* ignore errors */
	        sl -= intconv((cp + cl) - sp) ;
	        sp = (cp + cl) ;
	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
	            rs = subinfo_proclineval(sip,cite,cp,cl) ;
	            c = rs ;
	        }
	    } /* end if (getcite) */
	} /* end if (got next field) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_procline) */

local int subinfo_proclineval(SI *sip,uint cite,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		cl ;
	int		c = 0 ;
	cchar	*cp ;

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    char	keybuf[KEYBUFLEN + 1] ;
	    if (hasuc(cp,cl)) {
	        if (cl > KEYBUFLEN) cl = KEYBUFLEN ;
	        strwcpylc(keybuf,cp,cl) ;
	        cp = keybuf ;
	    }
	    if ((rs = strtab_add(&sip->kstrs,cp,cl)) >= 0) {
	        int	ki = rs ;
	        if ((rs = strtab_add(&sip->vstrs,sp,sl)) >= 0) {
	            SI_REC	r ;
	            int		vi = rs ;
	            c += 1 ;
	            r.cite = cite ;
	            r.ki = ki ;
	            r.vi = vi ;
	            rs = vecobj_add(&sip->recs,&r) ;
	        } /* end if (strtab_add) */
	    } /* end if (strtab_add) */
	} /* end if (nextfield) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_proclineval) */

local int subinfo_mkdata(SI *sip) noex {
	HO		*op = sip->op ;
	int		rs ;
	if ((rs = subinfo_mkrt(sip)) >= 0) {
	    rs = subinfo_mkst(sip) ;
	    if ((rs < 0) && (op->rt != nullptr)) {
	        lm_free(op->rt) ;
	        op->rt = nullptr ;
	    } /* end if (error) */
	}
	return rs ;
} /* end subroutine (subinfo_mkdata) */

local int subinfo_mkrt(SI *sip) noex {
	HO		*op = sip->op ;
	uint		(*rt)[3] ;
	int		rs = SR_OK ;
	int		sz ;
	int		n ;
	int		c = 0 ;

	n = vecobj_count(&sip->recs) ;

	if (n > 1) {
	    vecobj_sort(&sip->recs,vcmprec) ;
	}

	sz = (n + 2) * 3 * szof(uint) ;
	if ((rs = lm_mall(sz,&rt)) >= 0) {
	    rt[c][0] = 0 ;
	    rt[c][1] = 0 ;
	    rt[c][2] = 0 ;
	    c += 1 ;
	    void *vp ;
	    for (int i = 0 ; vecobj_get(&sip->recs,i,&vp) >= 0 ; i += 1) {
		SI_REC *rp = (SI_REC *) vp ;
	        if (vp) {
	            rt[c][0] = rp->cite ;
	            rt[c][1] = rp->ki ;
	            rt[c][2] = rp->vi ;
	            c += 1 ;
		}
	    } /* end for */
	    rt[c][0] = 0 ;
	    rt[c][1] = 0 ;
	    rt[c][2] = 0 ;
	    op->rt = rt ;
	    op->rtlen = c ;
	} /* end if (memory-acquire) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_mkrt) */

local int subinfo_mkst(SI *sip) noex {
	HO		*op = sip->op ;
	int		rs ;
	if ((rs = strtab_strsize(&sip->kstrs)) >= 0) {
	    cint	ksz = rs ;
	    if (char	*kst ; (rs = lm_mall(ksz,&kst)) >= 0) {
		int	kisz ;
		if ((rs = strtab_strmk(&sip->kstrs,kst,ksz)) >= 0) {
		    int		(*kit)[3] ;
		    op->itlen = nextpowtwo(op->rtlen) ;
		    kisz = (op->itlen + 1) * 3 * szof(int) ;
		    if ((rs = lm_call(1,kisz,&kit)) >= 0) {
			if ((rs = subinfo_mkind(sip,kst,kit,op->itlen)) >= 0) {
			    if ((rs = strtab_strsize(&sip->vstrs)) >= 0) {
				cint	vs = rs ;
				if (char *vst ; (rs = lm_mall(vs,&vst)) >= 0) {
				    strtab	*vsp = &sip->vstrs ;
				    if ((rs = strtab_strmk(vsp,vst,vs)) >= 0) {
					op->kst = kst ;
					op->vst = vst ;
					op->kit = kit ;
					op->kslen = ksz ;
					op->vslen = vs ;
				    } /* end if */
				} /* end if (m-a) */
			    } /* end if */
			} /* end if */
			if (rs < 0) {
			    lm_free(kit) ;
			} /* end if (error) */
		    } /* end if (m-a) */
		} /* end if (strtab_strmk) */
	        if (rs < 0) {
		    lm_free(kst) ;
		} /* end if (error) */
	    } /* end if (m-a) */
	} /* end if */
	return rs ;
} /* end subroutine (subinfo_mkst) */

/* make an index table of the record table */
local int subinfo_mkind(SI *sip,cchar kst[],int (*it)[3],int il) noex {
	HO		*op = sip->op ;
	varentry	ve ;
	uint		khash ;
	uint		(*rt)[3] ;
	int		rs = SR_OK ;
	[[maybe_unused]] int		rs1 ;
	int		ri, ki, hi ;
	int		rtl ;
	int		sc = 0 ;
	cchar	*kp ;

	rt = op->rt ;
	rtl = op->rtlen ;

#if	CF_FIRSTHASH
	{
	    vecobj		ves ;
	    cint		sz = szof(varentry) ;
	    int			vo = VECOBJ_OCOMPACT ;
	    if ((rs = vecobj_start(&ves,sz,rtl,vo)) >= 0) {
	        int	i ;

	        for (ri = 1 ; ri < rtl ; ri += 1) {

	            ki = rt[ri][1] ;
	            kp = kst + ki ;
	            khash = hash_elf(kp,-1) ;

	            hi = hashindex(khash,il) ;

	            if (it[hi][0] == 0) {
	                it[hi][0] = ri ;
	                it[hi][1] = (khash & INT_MAX) ;
	                it[hi][2] = 0 ;
	                sc += 1 ;
	            } else {
	                ve.ri = ri ;
	                ve.ki = ki ;
	                ve.khash = chash ;
	                ve.hi = hi ;
	                rs = vecobj_add(&ves,&ve) ;
	            }

	            if (rs < 0) break ;
	        } /* end for */

	        if (rs >= 0) {
	            varentry	*vep ;
	            for (i = 0 ; vecobj_get(&ves,i,&vep) >= 0 ; i += 1) {
	                sc += indinsert(rt,it,il,vep) ;
	            } /* end for */
	        }

	        vecobj_finish(&ves) ;
	    } /* end if (vecobj) */

	} /* end bloock */
#else /* CF_FIRSTHASH */

	for (ri = 1 ; ri < rtl ; ri += 1) {
	    ki = rt[ri][1] ;
	    kp = kst + ki ;

	    khash = hash_elf(kp,-1) ;

	    hi = hashindex(khash,il) ;

	    ve.ri = ri ;
	    ve.ki = ki ;
	    ve.khash = khash ;
	    ve.hi = hi ;
	    sc += indinsert(rt,it,il,&ve) ;

	} /* end for */

#endif /* CF_FIRSTHASH */

	it[il][0] = -1 ;
	it[il][1] = 0 ;
	it[il][2] = 0 ;

	if (sc < 0) {
	    sc = 0 ;
	}

	return (rs >= 0) ? sc : rs ;
} /* end subroutine (subinfo_mkind) */

local int getyear(time_t dt) noex {
	TMTIME		tm ;
	int		rs ;
	int		year ;

	rs = tmtime_timegm(&tm,dt) ;
	year = (tm.year + TM_YEAR_BASE) ;

	return (rs >= 0) ? year : rs ;
} /* end subroutine (getyear) */

local int getcite(uint *citep,cchar *cp,int cl) noex {
	int		rs = SR_ILSEQ ;
	if (cchar *tp ; (tp = strnchr(cp,cl,'/')) != nullptr) {
	    int tl = intconv(tp - cp) ;
	    if (int m ; (rs = cfdeci(cp,tl,&m)) >= 0) {
	        cl -= intconv((tp + 1) - cp) ;
	        cp = (tp + 1) ;
	        if (int d ; (rs = cfdeci(cp,cl,&d)) >= 0) {
	            rs = mkcite(citep,m,d) ;
	        } /* end if (cfdeci) */
	    } /* end if (cfdeci) */
	} /* end if (strnchr) */

	return rs ;
} /* end subroutine (getcite) */

local int mkcite(uint *citep,int m,int d) noex {
	int		rs = SR_DOM ;
	if ((m >= 0) && (m <= 12)) {
	    if ((d >= 0) && (d <= 31)) {
	        int c = 0 ;
	        m -= 1 ; /* compliance w/ UNIX® facilities */
	        c |= (m << 8) ;
	        c |= d ;
	        *citep = c ;
	        rs = SR_OK ;
	    } /* end if (months) */
	} /* end if (days) */
	return rs ;
} /* end subroutine (mkcite) */

local int indinsert(uint (*rt)[3],int (*it)[3],int il,varentry *vep) noex {
	uint		nhash, chash ;
	uint		ri, ki ;
	uint		lhi, nhi, hi ;
	int		c = 0 ;

	hi = vep->hi ;
	nhash = vep->khash ;
	chash = (nhash & INT_MAX) ;

	forever {

	    if (it[hi][0] == 0)
	        break ;

	    ri = it[hi][0] ;
	    ki = rt[ri][1] ;
	    if (ki == vep->ki)
	        break ;

	    it[hi][1] |= (~ INT_MAX) ;
	    nhash = hash_again(nhash,c++,HO_NSKIP) ;

	    hi = hashindex(nhash,il) ;

	} /* end while */

	if (it[hi][0] > 0) {

	    lhi = hi ;
	    while ((nhi = it[lhi][2]) > 0) {
	        lhi = nhi ;
	    }
	    hi = hashindex((lhi + 1),il) ;

	    while (it[hi][0] > 0) {
	        hi = hashindex((hi + 1),il) ;
	    }
	    it[lhi][2] = hi ;

	} /* end if (same-key continuation) */

	it[hi][0] = vep->ri ;
	it[hi][1] = chash ;
	it[hi][2] = 0 ;

	return c ;
} /* end subroutine (indinsert) */

local bool ismatkey(cchar key[],cchar kp[],int kl) noex {
	bool	f = (key[0] == kp[0]) ;
	if (f) {
	    cint	m = nleadstr(key,kp,kl) ;
	    f = (m == kl) && (key[m] == '\0') ;
	}
	return f ;
} /* end subroutine (ismatkey) */

local int vcmprec(cvoid **v1pp,cvoid **v2pp) noex {
	uint		**i1pp = (uint **) v1pp ;
	uint		**i2pp = (uint **) v2pp ;
	int		rc = 0 ;
	if ((i1pp != nullptr) || (i2pp != nullptr)) {
	    if (i1pp != nullptr) {
	        if (i2pp != nullptr) {
	            uint	*i1p = *i1pp ;
	            uint	*i2p = *i2pp ;
	            rc = (*i1p - *i2p) ;
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = +1 ;
	    }
	}
	return rc ;
} /* end subroutine (vcmprec) */

local int cmprec(cvoid *v1p,cvoid *v2p) noex {
	uint		*i1p = (uint *) v1p ;
	uint		*i2p = (uint *) v2p ;
	int		rc = 0 ;
	if ((i1p != nullptr) || (i2p != nullptr)) {
	    if (i1p != nullptr) {
	        if (i2p != nullptr) {
	      	    rc = (*i1p - *i2p) ;
	    	} else
	            rc = -1 ;
	    } else
	        rc = +1 ;
	}
	return rc ;
} /* end subroutine (cmprec) */


