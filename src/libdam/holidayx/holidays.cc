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
#include	<intcmp.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<strtab.h>		/* LIBUC */
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

#define	HOS		holidays
#define	HOS_MAGIC	HOLIDAYS_MAGIC
#define	HOS_DEFRECS	20
#define	HOS_HOLSUF	"holidays"
#define	HOS_MAXRECS	(USHORT_MAX - 2)
#define	HOS_NSKIP	4
#define	HOS_OBJ		holidays_obj
#define	HOS_CITE	holidays_cite
#define	HOS_CUR		holidays_cur

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

#define	SI		subinfo
#define	SI_REC		subinfo_rec

#ifndef	CF_FIRSTHASH
#define	CF_FIRSTHASH	0		/* perform FIRSTHASH */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


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

template<typename ... Args>
local inline int holidays_ctor(holidays *op,Args ... args) noex {
    	HOLIDAYS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (holidays_ctor) */

local int holidays_dtor(holidays *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (holidays_dtor) */

template<typename ... Args>
local inline int holidays_magic(holidays *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == HOS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (holidays_magic) */

local int	holidays_dbfind(HOS *,ids *,char *) noex ;
local int	holidays_dbfinder(HOS *,ids *,char *,cchar *) noex ;

local int	subinfo_start(SI *,HOS *) noex ;
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

cbool			f_firsthash	= CF_FIRSTHASH ;


/* exported variables */

const holidays_obj	holidays_modinfo = {
	"holidays",
	szof(holidays),
	szof(HOS_CUR)
} ; /* end array */


/* exported subroutines */

local int holidays_opens(HOS *op,cchar *pr,int year,cchar *fname) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	(void) pr ;
	(void) year ;
        if (cchar *cp ; (rs = mem.strw(fname,-1,&cp)) >= 0) {
            op->fname = cp ;
            if (SI si ; (rs = subinfo_start(&si,op)) >= 0) {
                if ((rs = subinfo_procfile(&si)) >= 0) {
                    c = rs ;
                    if ((rs = subinfo_mkdata(&si)) >= 0) {
                        op->ti_check = time(nullptr) ;
                        op->magval = HOS_MAGIC ;
                    }
                }
                rs1 = subinfo_finish(&si) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (subinfo) */
            if (rs < 0) {
                void *vp = voidp(op->fname) ;
                mem.free(vp) ;
                op->fname = nullptr ;
            } /* end if (error) */
        } /* end if (memory-acquire) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (holidays_opens) */

int holidays_open(HOS *op,cchar *pr,int year,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if ((rs = holidays_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		custime	dt = time(nullptr) ;
		rs = SR_INVALID ;
	        if (year <= 0) year = getyear(dt) ;
	        if (year >= 1970) {
		    int		fl = -1 ;
		    op->year	= year ;
		    op->pr	= pr ;
		    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
		        if ((fname == nullptr) || (fname[0] == '\0')) {
	    	            if (ids id ; (rs = id.load) >= 0) {
	        	        rs = holidays_dbfind(op,&id,tbuf) ;
	        	        fl = rs ;
	        	        if ((rs >= 0) && (fl > 0)) {
	            	            fname = tbuf ;
			        }
	        	        rs1 = id.release ;
			        if (rs >= 0) rs = rs1 ;
	    	            } /* end if (ids) */
		        } /* end if */
		        if (rs >= 0) {
	    	            rs = holidays_opens(op,pr,year,fname) ;
	    	            c = rs ;
		        } /* end if (ok) */
			rs1 = mem.free(tbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (valid) */
	    } /* end if (valid) */
	    if (rs < 0) {
		holidays_dtor(op) ;
	    } /* end if (error) */
	} /* end if (holidays_ctor) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (holidays_open) */

int holidays_close(HOS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = holidays_magic(op)) >= 0) {
	    if (op->vst) {
	        rs1 = mem.free(op->vst) ;
	        if (rs >= 0) rs = rs1 ;
	        op->vst = nullptr ;
	    } /* end if (memory-release) */
	    if (op->kit) {
	        rs1 = mem.free(op->kit) ;
	        if (rs >= 0) rs = rs1 ;
	        op->kit = nullptr ;
	    } /* end if (memory-release) */
	    if (op->kst) {
	        rs1 = mem.free(op->kst) ;
	        if (rs >= 0) rs = rs1 ;
	        op->kst = nullptr ;
	    } /* end if (memory-release) */
	    if (op->rt) {
	        rs1 = mem.free(op->rt) ;
	        if (rs >= 0) rs = rs1 ;
	        op->rt = nullptr ;
	    } /* end if (memory-release) */
	    if (op->fname) {
	        void *vp = voidp(op->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = holidays_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (holidays_magic) */
	return rs ;
} /* end subroutine (holidays_close) */

int holidays_count(HOS *op) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = holidays_magic(op)) >= 0) {
	    c = (op->rtlen - 1) ;
	} /* end if (holidays_magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (holidays_count) */

int holidays_audit(HOS *op) noex {
	int		rs ;
	if ((rs = holidays_magic(op)) >= 0) {
	    rs = 1 ;
	} /* end if (holidays_magic) */
	return rs ;
} /* end subroutine (holidays_audit) */

int holidays_curbegin(HOS *op,holidays_cur *curp) noex {
    	int		rs ;
	if ((rs = holidays_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    curp->chash = 0 ;
	    op->ncursors += 1 ;
	} /* end if (holidays_magic) */
	return rs ;
} /* end subroutine (holidays_curbegin) */

int holidays_curend(HOS *op,holidays_cur *curp) noex {
    	int		rs ;
	if ((rs = holidays_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (holidays_magic) */
	return rs ;
} /* end subroutine (holidays_curend) */

int holidays_fetchcite(HOS *op,HOS_CITE *qp,HOS_CUR *curp,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		vl = 0 ; /* return-value */
	if ((rs = holidays_magic(op,qp)) >= 0) {
	    HOS_CUR	dcur ;
	    uint	(*rt)[3] ;
	    uint	(*rpp)[3] ;
	    uint	scite ;
	    cint	esz = (3 * szof(uint)) ;
	    int		ri, vi ;
	    int		rtlen ;
	    cchar	*vst ;
	    cchar	*vp ;

	    if (curp == nullptr) {
	        curp = &dcur ;
	        curp->i = 0 ;
	    }
    
	    if (vbuf) {
	        vbuf[0] = '\0' ;
	    }
    
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
	        if (rpp == nullptr) {
	            rs = SR_NOTFOUND ;
	        }
    
	        if (rs >= 0) {
	            ri = intconv(rpp - rt) ;
	            while (ri > 0) {
	                int	pri = (ri - 1) ;
	                if (scite != rt[pri][0]) break ;
	                ri = pri ;
	            } /* end while */
	        } /* end if (ok) */
    
	    } else {
	        ri = (curp->i + 1) ;
	        if ((ri >= rtlen) || (scite != rt[ri][0])) {
	            rs = SR_NOTFOUND ;
		}
	    } /* end if */
	    /* if successful, retrieve value */
	    if (rs >= 0) {
	        vi = rt[ri][2] ;
	        vp = (vst + vi) ;
	        if (vbuf != nullptr) {
	            rs = sncpy1(vbuf,vlen,vp) ;
	            vl = rs ;
	        } else {
	            vl = lenstr(vp) ;
	        }
	        if (qp) {
	            uint	cite = rt[ri][0] ;
	            qp->m = ((cite >> 8) & UCHAR_MAX) ;
	            qp->d = ((cite >> 0) & UCHAR_MAX) ;
	        } /* end if (query) */
	        if (rs >= 0) {
	            curp->i = ri ;
	        } /* end if (ok) */
	    } /* end if (ok: got one) */
    
	} /* end if (holidays_magic) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidays_fetchcite) */

int holidays_fetchname(HOS *op,cc *kp,int kl,HOS_CUR *curp,HOS_CITE *qp,
		char *vbuf,int vlen) noex {
	int		rs = SR_OK ;
	int		vl = 0 ; /* return-value */
	if ((rs = holidays_magic(op,kp)) >= 0) {
	    HOS_CUR	dcur ;
	    uint	khash, nhash ;
	    uint	chash ;
	    uint	(*rt)[3] ;
	    cint	nskip = HOS_NSKIP ;
	    int		ri, ki, vi, hi ;
	    int		c = 0 ;
	    int		(*it)[3] ;
	    int		itlen ;
	    int		f_mat = false ;
	    cchar	*kst, *vst ;
	    cchar	*vp ;
	    cchar	*cp ;
	    char	keybuf[KEYBUFLEN + 1] ;
    
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
    
	    if (vbuf) {
	        vbuf[0] = '\0' ;
	    }
    
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
		    cuint hav = (it[hi][itentry_info] & INT_MAX) ;
	            f_mat = (hav == chash) ;
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
	        if ((rs >= 0) && (! f_mat)) {
	            rs = SR_NOTFOUND ;
		}
	    } else {
    
	        chash = curp->chash ;
	        hi = curp->i ;
    
	        if (hi < itlen) {
	            ri = it[hi][itentry_ri] ;
	            if (ri > 0) {
	                hi = it[hi][itentry_nhi] ;
	                if (hi != 0) {
	                    ri = it[hi][itentry_ri] ;
			    cuint hav = (it[hi][itentry_info] & INT_MAX) ;
	                    f_mat = (hav == chash) ;
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
	        } else {
	            vl = lenstr(vp) ;
	        }
	        if (qp != nullptr) {
	            uint	cite = rt[ri][0] ;
	            qp->m = ((cite >> 8) & UCHAR_MAX) ;
	            qp->d = ((cite >> 0) & UCHAR_MAX) ;
	        }
	        if (rs >= 0) {
	            curp->i = hi ;
	        }
	    } /* end if (got one) */
    
	} /* end if (holidays_magic) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidays_fetchname) */

int holidays_curenum(HOS *op,HOS_CUR *curp,HOS_CITE *qp,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		vl = 0 ; /* return-value */
	if ((rs = holidays_magic(op,curp)) >= 0) {
	    uint	(*rt)[3] ;
	    int		ri = (curp->i < 1) ? 1 : (curp->i + 1) ;
	    int		vi ; /* used-below */
	    if (vbuf) {
	        vbuf[0] = '\0' ;
	    }
	    /* ok, we are good to go */
	    if (ri < op->rtlen) {
	        cchar *vst = op->vst ;
	        rt = op->rt ;
	        vi = rt[ri][2] ;
	        if (vi < op->vslen) {
	            cchar *vp = (vst + vi) ;
	            if (vbuf) {
	                rs = sncpy1(vbuf,vlen,vp) ;
	                vl = rs ;
	            } else {
	                vl = lenstr(vp) ;
		    }
	            if (qp) {
	                uint	cite = rt[ri][0] ;
	                qp->m = ((cite >> 8) & UCHAR_MAX) ;
	                qp->d = ((cite >> 0) & UCHAR_MAX) ;
	            } /* end if (query) */
	            if (rs >= 0) {
	                curp->i = ri ;
		    } /* end if (ok) */
	        } else {
	            rs = SR_BADFMT ;
	        }
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (holidays_magic) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidays_curenum) */

int holidays_check(HOS *op,time_t dt) noex {
	int		rs ;
	int		f_changed = false ; /* return-value */
	if ((rs = holidays_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if (dt == 1) f_changed = true ; /* dummy check (never happens) */
	} /* end if (holidays_magic) */
	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (holidays_check) */


/* private subroutines */

local int holidays_dbfind(HOS *op,ids *idp,char *tmpfname) noex {
    	cint		diglen = DIGBUFLEN ;
	int		rs ;
	int		rs1 ;
	int		fl = 0 ; /* return-value */
	char		digbuf[DIGBUFLEN + 1] ;
	if (char *cnbuf ; (rs = mem.mn(&cnbuf)) >= 0) {
	    cchar	*fsuf = HOS_HOLSUF ;
	    cint	cnlen = rs ;
	    tmpfname[0] = '\0' ;
	    if ((rs = ctdeci(digbuf,diglen,op->year)) >= 0) {
	        if ((rs = sncpy2(cnbuf,cnlen,fsuf,digbuf)) >= 0) {
	            rs = holidays_dbfinder(op,idp,tmpfname,cnbuf) ;
	            fl = rs ;
	            if ((rs >= 0) && (fl == 0)) {
	                rs = holidays_dbfinder(op,idp,tmpfname,fsuf) ;
	                fl = rs ;
	            }
	        } /* end if (sncpy) */
	    } /* end if */
	    rs1 = mem.free(cnbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fl : rs ;
} /* end subroutine (holidays_dbfind) */

local int holidays_dbfinder(HOS *op,ids *idp,char *tmpfname,
		cchar *cname) noex {
	int		rs = SR_OK ;
	int		rs1 = SR_NOENT ;
	int		fl = 0 ; /* return-value */
	cchar	*hdn ;
	for (int i = 0 ; holdnames[i] != nullptr ; i += 1) {
	    hdn = holdnames[i] ;
	    if (hdn[0] != '/') {
	        rs = mkpath3(tmpfname,op->pr,hdn,cname) ;
	    } else {
	        rs = mkpath2(tmpfname,hdn,cname) ;
	    }
	    fl = rs ;
	    if (rs >= 0) {
		ustat	sb ;
	        if ((rs1 = u_stat(tmpfname,&sb)) >= 0) {
	            rs1 = permids(idp,&sb,R_OK) ;
		}
	    } /* end if (error) */
	    if (rs1 >= 0) break ;
	} /* end for */
	if (rs1 < 0) {
	    fl = 0 ;
	} /* end if (error) */
	return (rs >= 0) ? fl : rs ;
} /* end subroutine (holidays_dbfinder) */

local int subinfo_start(SI *sip,HOS *op) noex {
	int		rs ;
	memclear(sip) ;
	sip->op = op ;
	if ((rs = bopen(&sip->hfile,op->fname,"r",0666)) >= 0) {
	    cint	sz = szof(SI_REC) ;
	    cint	vn = HOS_DEFRECS ;
	    if ((rs = vecobj_start(&sip->recs,sz,vn,0)) >= 0) {
		bfile *bfp = &sip->hfile ;
	        if (ustat sb ; (rs = bfp->control(BC_STAT,&sb)) >= 0) {
	            strtab *ksp = &sip->kstrs ;
	            sip->fsz = intconv(sb.st_size) ;
	            if ((rs = strtab_start(ksp,(sip->fsz/3))) >= 0) {
	                rs = strtab_start(&sip->vstrs,sip->fsz) ;
	                if (rs < 0) {
	                    strtab_finish(&sip->kstrs) ;
		        } /* end if (error) */
	            } /* end if (strtab_start) */
		} /* end if (bcontrol) */
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
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
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
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_finish) */

local int subinfo_procfile(SI *sip) noex {
	HOS		*op = sip->op ;
	cint		maxrecs = HOS_MAXRECS ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	int		f_start = true ;
	if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) {
	    cint llen = rs ;
	    while ((rs = breadln(&sip->hfile,lbuf,llen)) > 0) {
	        int len = rs ;
	        if (lbuf[len-1] == '\n') len -= 1 ;
	        if ((len == 0) || (lbuf[0] == '*')) continue ;
	        while (CHAR_ISWHITE(lbuf[len-1])) len -= 1 ;
	        if (len == 0) continue ;
	        if (f_start) {
	            int	year ;
	            f_start = false ;
	            rs = subinfo_procyear(sip,lbuf,len) ;
	            year = rs ;
	            if ((rs >= 0) && (year != op->year)) {
	                rs = SR_NOMSG ;
		    }
	        } else {
	            rs = subinfo_procline(sip,lbuf,len) ;
	            c += rs ;
	        }
	        if (c >= maxrecs) break ;
	        if (rs < 0) break ;
	    } /* end if (reading) */
	    rs1 = mem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_procfile) */

local int subinfo_procyear(SI *sip,cchar *lbuf,int llen) noex {
	int		rs = SR_BUGCHECK ;
	int		year = SR_ILSEQ ; /* return-value */
	if (sip) {
	    rs = SR_ILSEQ ;
	    cchar *cp ;
	    if (int cl ; (cl = sfnext(lbuf,llen,&cp)) > 0) {
	        rs = cfdeci(cp,cl,&year) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? year : rs ;
} /* end subroutine (subinfo_procyear) */

local int subinfo_procline(SI *sip,cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		sl = llen ;
	int		c = 0 ; /* return-value */
	cchar	*sp = lbuf ;
	cchar	*cp ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
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
	int		c = 0 ; /* return-value */
	cchar	*cp ;
	if (int cl ; (cl = sfnext(sp,sl,&cp)) > 0) {
	    char	keybuf[KEYBUFLEN + 1] ;
	    if (hasuc(cp,cl)) {
	        if (cl > KEYBUFLEN) cl = KEYBUFLEN ;
	        strwcpylc(keybuf,cp,cl) ;
	        cp = keybuf ;
	    } /* end if (was upper case) */
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
	HOS		*op = sip->op ;
	int		rs ;
	if ((rs = subinfo_mkrt(sip)) >= 0) {
	    rs = subinfo_mkst(sip) ;
	    if ((rs < 0) && (op->rt != nullptr)) {
	        mem.free(op->rt) ;
	        op->rt = nullptr ;
	    } /* end if (error) */
	}
	return rs ;
} /* end subroutine (subinfo_mkdata) */

local int subinfo_mkrt(SI *sip) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	if ((rs = vecobj_count(&sip->recs)) >= 0) {
	    cint n = rs ;
	    if (n > 1) {
	        rs = vecobj_sort(&sip->recs,vcmprec) ;
	    }
	    if (rs >= 0) {
	        HOS	*op = sip->op ;
	        uint	(*rt)[3] ;
	        int	sz = (n + 2) * 3 * szof(uint) ;
	        if ((rs = mem.mall(sz,&rt)) >= 0) {
		    vecobj *rlp = &sip->recs ;
	            rt[c][0] = 0 ;
	            rt[c][1] = 0 ;
	            rt[c][2] = 0 ;
	            c += 1 ;
	            void *vp ;
	            for (int i = 0 ; rlp->get(i,&vp) >= 0 ; i += 1) {
		        if (SI_REC *rp = resumelife<SI_REC>(vp) ; rp) {
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
	    } /* end if (ok) */
	} /* end if (vecobj_count) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (subinfo_mkrt) */

local int subinfo_mkst(SI *sip) noex {
	HOS		*op = sip->op ;
	int		rs ;
	if ((rs = strtab_strsize(&sip->kstrs)) >= 0) {
	    cint	ksz = rs ;
	    if (char	*kst ; (rs = mem.mall(ksz,&kst)) >= 0) {
		int	kisz ;
		if ((rs = strtab_strmk(&sip->kstrs,kst,ksz)) >= 0) {
		    int		(*kit)[3] ;
		    op->itlen = nextpowtwo(op->rtlen) ;
		    kisz = (op->itlen + 1) * 3 * szof(int) ;
		    if ((rs = mem.call(1,kisz,&kit)) >= 0) {
			if ((rs = subinfo_mkind(sip,kst,kit,op->itlen)) >= 0) {
			    if ((rs = strtab_strsize(&sip->vstrs)) >= 0) {
				cint	vs = rs ;
				if (char *vst ; (rs = mem.mall(vs,&vst)) >= 0) {
				    strtab	*vsp = &sip->vstrs ;
				    if ((rs = strtab_strmk(vsp,vst,vs)) >= 0) {
					op->kst = kst ;
					op->vst = vst ;
					op->kit = kit ;
					op->kslen = ksz ;
					op->vslen = vs ;
				    } /* end if */
				} /* end if (memory-acquire) */
			    } /* end if */
			} /* end if */
			if (rs < 0) {
			    mem.free(kit) ;
			} /* end if (error) */
		    } /* end if (memory-acquire) */
		} /* end if (strtab_strmk) */
	        if (rs < 0) {
		    mem.free(kst) ;
		} /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if */
	return rs ;
} /* end subroutine (subinfo_mkst) */

/* make an index table of the record table */
local int subinfo_mkind(SI *sip,cchar *kst,int (*it)[3],int il) noex {
	HOS		*op = sip->op ;
	varentry	ve ;
	uint		khash ;
	uint		(*rt)[3] ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ri, ki, hi ;
	int		rtl ;
	int		sc = 0 ;
	cchar	*kp ;

	rt = op->rt ;
	rtl = op->rtlen ;

#if	CF_FIRSTHASH
	{
	    cint	sz = szof(varentry) ;
	    int		vo = vecobjm.compact ;
	    vecobj	ves ;
	    if ((rs = vecobj_start(&ves,sz,rtl,vo)) >= 0) {
	        int	i ;
	        for (int ri = 1 ; ri < rtl ; ri += 1) {

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
	            } /* end if */

	            if (rs < 0) break ;
	        } /* end for */

	        if (rs >= 0) {
		    vecobj *velp = *ves ;
		    void *vp ;
	            for (int i = 0 ; velp->get(i,&vp) >= 0 ; i += 1) {
	                if (varentry *vep = resumelife<varentry>(vp) ; vp) {
	                    sc += indinsert(rt,it,il,vep) ;
			}
	            } /* end for */
	        } /* end if (ok) */
	        rs1 = vecobj_finish(&ves) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecobj) */
	} /* end block */
#else /* CF_FIRSTHASH */
	(void) rs1 ;
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
	int		rs ;
	int		year = 0 ; /* return-value */
	if (tmtime tm ; (rs = tmtime_timegm(&tm,dt)) >= 0) {
	    year = (tm.year + TM_YEAR_BASE) ;
	}
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
	uint		nhash = vep->khash ;
	uint		chash ;
	uint		ri ;
	uint		ki ;
	uint		lhi ;
	uint		nhi ;
	uint		hi = vep->hi ;
	int		c = 0 ;
	chash = (nhash & INT_MAX) ;
	forever {
	    if (it[hi][0] == 0) break ;
	    ri = it[hi][0] ;
	    ki = rt[ri][1] ;
	    if (ki == vep->ki) break ;
	    it[hi][1] |= (compl INT_MAX) ;
	    nhash = hash_again(nhash,c++,HOS_NSKIP) ;
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

local bool ismatkey(cchar *key,cchar *kp,int kl) noex {
	bool	f = (key[0] == kp[0]) ;
	if (f) {
	    cint	m = nleadstr(key,kp,kl) ;
	    f = (m == kl) && (key[m] == '\0') ;
	}
	return f ;
} /* end subroutine (ismatkey) */

/* for |vecobj_sort(3uc)| */
local int vcmprec(cvoid **v1pp,cvoid **v2pp) noex {
	con uint	**i1pp = (con uint **) v1pp ;
	con uint	**i2pp = (con uint **) v2pp ;
	int		rc = 0 ;
	{
	    con uint	*i1p = cuintp(*i1pp) ;
	    con uint	*i2p = cuintp(*i2pp) ;
	    if (i1p || i2p) {
	        if (i1p) {
	            if (i2p) {
		        rc = intcmp(*i1p,*i2p) ;
	            } else {
	                rc = -1 ;
		    }
	        } else {
	            rc = +1 ;
	        }
	    } /* end if */
	} /* end block */
	return rc ;
} /* end subroutine (vcmprec) */

/* for |bsearch(3c)| */
local int cmprec(cvoid *v1p,cvoid *v2p) noex {
	con uint	*i1p = cuintp(v1p) ;
	con uint	*i2p = cuintp(v2p) ;
	int		rc = 0 ;
	if (i1p || i2p) {
	    if (i1p) {
	        if (i2p) {
		    rc = intcmp(*i1p,*i2p) ;
	    	} else {
	            rc = -1 ;
		}
	    } else {
	        rc = +1 ;
	    }
	}
	return rc ;
} /* end subroutine (cmprec) */


