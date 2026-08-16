/* holidayer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* access for the HOLIDAYER database */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_RESTYEAR	0		/* restriction on specified year */

/* revision history:

	= 2016-06-02, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	holidayer

	Description:
	This object module provides an interface to the HOLIDAYS
	(see |holidays(4)|) database.  The "database" consists of
	all |holidays(4)| database files found on the system (in
	the places that are searched).  This was an El Cheapo sort
	of implementation since we just depend upon the older
	HOLIDAYS object to access individual |holidays(4)| database
	files.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<bfile.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"holidayer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	HO		holidayer
#define	HO_CUR		holidayer_cur
#define	HO_CITE		holidayer_cite
#define	HO_H		holidayer_hol
#define	HO_FPR		"holidays"
#define	HO_MAG		HOLIDAYER_MAGIC

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_RESTYEAR
#define	CF_RESTYEAR	0		/* restriction on specified year */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct holidayer_hol {
	holidays	hol ;
	uint		year ;
} ; /* end struct */

typedef holidayer_hol *	holptr ;


/* local structures */


/* forward references */

template<typename ... Args>
local inline int holidayer_ctor(holidayer *op,Args ... args) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_OK ;
	    op->pr = nullptr ;
	    op->idp = nullptr ;
	    op->hlp = nullptr ;
	    op->fl = {} ;
	    if (op->idp = new(nt) ids ; op->idp) {
		if (op->hlp = new(nt) vechand ; op->hlp) {
		    rs = SR_OK ;
	        } /* end if (new-vechand) */
		if (rs < 0) {
		    delete op->idp ;
		    op->idp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-ids) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (holidayer_ctor) */

local int holidayer_dtor(holidayer *op) noex {
	int		rs = SR_OK ;
	if (op->hlp) ylikely {
	    delete op->hlp ;
	    op->hlp = nullptr ;
	}
	if (op->idp) ylikely {
	    delete op->idp ;
	    op->idp = nullptr ;
	}
	return rs ;
} /* end subroutine (holidayer_dtor) */

template<typename ... Args>
local inline int holidayer_magic(holidayer *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == HO_MAG) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (holidayer_magic) */

local int holidayer_liststart	(HO *) noex ;
local int holidayer_listfinish	(HO *) noex ;

local int holidayer_holbegin	(HO *,HO_H *,int,cchar *) noex ;
local int holidayer_holend	(HO *,HO_H *) noex ;
local int holidayer_holfins	(HO *) noex ;
local int holidayer_holaudit	(HO *,HO_H *) noex ;

local int holidayer_yearfind	(HO *,uint,holidays **) noex ;
local int holidayer_yearfinder	(HO *,int,HO_H **) noex ;
local int holidayer_yearfile	(HO *,char *,uint) noex ;
local int holidayer_yearadd	(HO *,HO_H *) noex ;
local int holidayer_dirok	(HO *,cchar *) noex ;
local int holidayer_mkdir	(HO *,char *,cchar *) noex ;
local int holidayer_mkfname	(HO *,char *,cchar *,uint) noex ;
local int holidayer_holaudit	(HO *,HO_H *) noex ;
local int holidayer_yearq	(HO *,HO_CITE *) noex ;
local int holidayer_year	(HO *,uint) noex ;
local int holidayer_yearmk	(HO *) noex ;

local inline int restyear(int) noex ;

local bool	isOurMode(mode_t) noex ;


/* local variables */

constexpr cpcchar	holdnames[] = {
	"etc/acct",
	"/etc/acct",
	nullptr
} ; /* end array */

static bufsizevar	maxpathlen(bufsize_mp) ;
cbool			f_debug		= CF_DEBUG ;
cbool			f_restyear	= CF_RESTYEAR ;


/* exported variables */

const holidayer_obj	holidayer_modinfo = {
	"holidayer",
	szof(holidayer),
	szof(HO_CUR)
} ; /* end initialization */


/* exported subroutines */

int holidayer_open(HO *op,cchar *pr) noex {
	int		rs ;
	if ((rs = holidayer_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
		if ((rs = maxpathlen) >= 0) ylikely {
	            if (cchar *cp ; (rs = mem.strw(pr,-1,&cp)) >= 0) ylikely {
			ids *idp = op->idp ;
	                op->pr = cp ;
	                if ((rs = idp->load) >= 0) ylikely {
	                    op->magval = HO_MAG ;
	                } /* end if (ids_load) */
			if (rs < 0) {
			    voidp vp = voidp(op->pr) ;
			    mem.free(vp) ;
			    op->pr = nullptr ;
			} /* end if (error) */
	            } /* end if (memory-acquire) */
		} /* end if (maxpathlen) */
	    } /* end if (valid) */
	    if (rs < 0) {
		holidayer_dtor(op) ;
	    } /* end if (error) */
	} /* end if (lastlogfile_ctor) */
	return rs ;
} /* end subroutine (holidayer_open) */

int holidayer_close(HO *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = holidayer_magic(op)) >= 0) ylikely {
	    if (op->fl.hols) {
	        if (vechand *hlp = op->hlp ; hlp) {
	            {
	                rs1 = holidayer_holfins(op) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	            {
			rs1 = holidayer_listfinish(op) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end if (non-null) */
	    } /* end if */
	    if (op->idp) {
	        rs1 = ids_release(op->idp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->pr) {
	        void *vp = voidp(op->pr) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = holidayer_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (holidayer_magic) */
	return rs ;
} /* end subroutine (holidayer_close) */

int holidayer_audit(HO *op) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = holidayer_magic(op)) >= 0) ylikely {
	    if (op->fl.hols) {
	        vechand	*hlp = op->hlp ;
	        if ((rs = hlp->audit) >= 0) ylikely {
		    void	*vp{} ;
	            for (int i = 0 ; hlp->get(i,&vp) >= 0 ; i += 1) {
	      	        HO_H	*hep = holptr(vp) ;
	                if (vp) {
			    c += 1 ;
	                    rs = holidayer_holaudit(op,hep) ;
	                }
	                if (rs < 0) break ;
	            } /* end for */
	        } /* end if (vechand_audit) */
	    } /* end if */
	} /* end if (holidayer_magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (holidayer_audit) */

int holidayer_curbegin(HO *op,HO_CUR *curp) noex {
    	cnothrow	nt{} ;
    	int		rs ;
	if ((rs = holidayer_magic(op,curp)) >= 0) ylikely {
	    memclear(curp) ;
	    rs = SR_NOMEM ;
	    if (holidays_cur *hcurp = new(nt) holidays_cur ; hcurp) ylikely {
		rs = SR_OK ;
	        curp->magval = HO_MAG ;
	        op->ncursors += 1 ;
	    } /* end if (new-holidays_cur) */
	} /* end if (holidayer_magic) */
	return rs ;
} /* end subroutine (holidayer_curbegin) */

int holidayer_curend(HO *op,HO_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = holidayer_magic(op,curp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == HO_MAG) ylikely {
		rs = SR_BUGCHECK ;
		if (holidays_cur *hcp = curp->hcurp ; hcp) ylikely {
	            if (op->ncursors > 0) ylikely {
	                if (curp->hop) ylikely {
	                    holidays	*hop = curp->hop ;
		            rs = SR_OK ;
	                    if ((rs1 = holidays_curend(hop,hcp)) >= 0) ylikely {
	                        op->ncursors -= 1 ;
	                        curp->hop = nullptr ;
	                        curp->year = 0 ;
	                        curp->magval = 0 ;
	                    } /* end if */
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (cursor was used) */
	            } else {
	                rs = SR_PROTO ;
	            }
		    {
			delete hcp ;
			curp->hcurp = nullptr ;
		    } /* end block (memory-release) */
		    curp->magval = 0 ;
	        } /* end if (non-null) */
	    } /* end if (cursor-magic) */
	} /* end if (holidayer_magic) */
	return rs ;
} /* end subroutine (holidayer_curend) */

local int holidayer_fetchcites(HO *op,HO_CITE *qp,
		HO_CUR *curp,char *rp,int rl) noex {
    	int		rs ;
	int		vl = 0 ; /* return-value */
        if ((rs = holidayer_yearq(op,qp)) >= 0) ylikely {
            const uint  y = rs ;
            if (curp->year == 0) {
                if (curp->hop == nullptr) {
                    holidays            *holp ;
                    holidays_cur        *hcp = curp->hcurp ;
                    if ((rs = holidayer_yearfind(op,y,&holp)) >= 0) {
                        if ((rs = holidays_curbegin(holp,hcp)) >= 0) {
                            curp->hop = holp ;
                            curp->year = y ;
                            rs = holidays_fetchcite(holp,qp,hcp,rp,rl) ;
                            vl = rs ;
                        } /* end if (holidays_curbegin) */
                    } /* end if (holidayer_yearfind) */
                } else {
                    rs = SR_PROTO ;
                }
            } else if (curp->year == qp->y) {
                if (curp->hop != nullptr) {
                    holidays            *holp = curp->hop ;
                    holidays_cur        *hcp = curp->hcurp ;
                    rs = holidays_fetchcite(holp,qp,hcp,rp,rl) ;
                    vl = rs ;
                } else {
                    rs = SR_PROTO ;
                }
            } else {
                rs = SR_PROTO ;
            }
        } /* end if (holidayer_year) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidayer_fetchcites) */

int holidayer_fetchcite(HO *op,HO_CITE *qp,
		HO_CUR *curp,char *rp,int rl) noex {
	int		rs ;
	int		vl = 0 ; /* return-value */
	if ((rs = holidayer_magic(op,qp,curp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == HO_MAG) ylikely {
		rs = holidayer_fetchcites(op,qp,curp,rp,rl) ;
		vl = rs ;
	    } /* end if (cursor-magic) */
	} /* end if (holidayer_magic) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidayer_fetchcite) */

local int holidayer_fetchnames(HO *op,uint y,cc *kp,int kl,
		HO_CUR *curp,HO_CITE *qp,char *rp,int rl) noex {
    	int		rs ;
	int		vl = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
        if ((rs = holidayer_year(op,y)) >= 0) ylikely {
            const uint  my = rs ;
            if (curp->year == 0) ylikely {
                if (curp->hop == nullptr) {
                    holidays            *holp ;
                    holidays_cur        *hcp = curp->hcurp ;
                    if ((rs = holidayer_yearfind(op,y,&holp)) >= 0) {
                        if ((rs = holidays_curbegin(holp,hcp)) >= 0) {
                            cauto hof = holidays_fetchname ;
                            curp->hop = holp ;
                            curp->year = my ;
                            rs = hof(holp,kp,kl,hcp,qp,rp,rl) ;
                            vl = rs ;
                        } /* end if (holidays_curbegin) */
                    } /* end if (holidayer_yearfind) */
                } else {
                    rs = SR_PROTO ;
                }
            } else if (curp->year == y) {
                if (curp->hop != nullptr) {
                    holidays            *holp = curp->hop ;
                    holidays_cur        *hcp = curp->hcurp ;
                    rs = holidays_fetchname(holp,kp,kl,hcp,qp,rp,rl) ;
                    vl = rs ;
                } else {
                    rs = SR_PROTO ;
                }
            } else {
                rs = SR_PROTO ;
            }
        } /* end if (holidayer_year) */
	DEBUGPRINTF("ret rs=%d bl=%u\n",rs,vl) ;
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidayer_fetchnames) */

int holidayer_fetchname(HO *op,uint y,cc *kp,int kl,
		HO_CUR *curp,HO_CITE *qp,char *rp,int rl) noex {
	int		rs ;
	int		vl = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if ((rs = holidayer_magic(op,kp,curp,qp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (op->magval == HO_MAG) ylikely {
	        DEBUGPRINTF("ent y=%d k=>%r<\n",y,kp,kl) ;
	        DEBUGPRINTF("cur y=%u hop{%p}\n",curp->year,curp->hop) ;
		rs = holidayer_fetchnames(op,y,kp,kl,curp,qp,rp,rl) ;
		vl = rs ;
	    } /* end if (cursor-magic) */
	} /* end if (holidayer_magic) */
	DEBUGPRINTF("ret rs=%d bl=%u\n",rs,vl) ;
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidayer_fetchname) */

local int holidayer_curenums(HO *op,HO_CUR *curp,HO_CITE *qp,
		char *vbuf,int vlen,uint y) noex {
    	int		rs ;
	int		vl = 0 ; /* return-value */
	if ((rs = holidayer_year(op,y)) >= 0) ylikely {
	    const uint	my = rs ;
	    if (curp->year == 0) {
	        if (curp->hop == nullptr) {
	            holidays		*holp ;
	            holidays_cur	*hcp = curp->hcurp ;
	            if ((rs = holidayer_yearfind(op,y,&holp)) >= 0) {
	                if ((rs = holidays_curbegin(holp,hcp)) >= 0) {
	                    curp->hop = holp ;
	                    curp->year = my ;
	                    rs = holidays_curenum(holp,hcp,qp,vbuf,vlen) ;
	                    vl = rs ;
	                } /* end if (holidays_curbegin) */
	            } /* end if (holidayer_yearfind) */
	        } else {
	            rs = SR_PROTO ;
		}
	    } else if (curp->year == y) {
	        if (curp->hop != nullptr) {
	            holidays		*holp = curp->hop ;
	            holidays_cur	*hcp = curp->hcurp ;
	            rs = holidays_curenum(holp,hcp,qp,vbuf,vlen) ;
	            vl = rs ;
	        } else {
	            rs = SR_PROTO ;
		}
	    } else {
	        rs = SR_PROTO ;
	    }
	} /* end if (holidayer_year) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidayer_curenums) */

int holidayer_curenum(HO *op,HO_CUR *curp,HO_CITE *qp,
		char *vbuf,int vlen,uint y) noex {
	int		rs ;
	int		vl = 0 ; /* return-value */
	if ((rs = holidayer_magic(op,curp,qp)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (curp->magval == HO_MAG) ylikely {
	        rs = SR_INVALID ;
	        if (op->ncursors > 0) ylikely {
		    if (vbuf) vbuf[0] = '\0' ;
		    rs = holidayer_curenums(op,curp,qp,vbuf,vlen,y) ;
		    vl = rs ;
	        }
	    } /* end if (cursor-magic) */
	} /* end if (holidayer_magic) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (holidayer_curenum) */

int holidayer_check(HO *op,time_t dt) noex {
	int		rs ;
	int		f_changed = false ; /* return-value */
	if ((rs = holidayer_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if (dt == 1) f_changed = true ;
	} /* end if (holidayer_magic) */
	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (holidayer_check) */


/* private subroutines */

local int holidayer_liststart(HO *op) noex {
    	int		rs = SR_OK ;
	if (! op->fl.hols) ylikely {
	    rs = SR_BUGCHECK ;
	    if (vechand *hlp = op->hlp ; hlp) ylikely {
	        cint	vn = 2 ;
	        cint	vo = vechandm.stationary ;
	        if ((rs = hlp->start(vn,vo)) >= 0) ylikely {
	            op->fl.hols = true ;
	        }
	    } /* end if (bug-check) */
	} /* end if (needed to be opened) */
	return rs ;
} /* end subroutine (holidayer_liststart) */

local int holidayer_listfinish(HO *op) noex {
    	int		rs = SR_OK ;
	if (op->fl.hols) ylikely {
	    rs = SR_BUGCHECK ;
	    if (vechand *hlp = op->hlp ; hlp) ylikely {
	        if ((rs = hlp->finish) >= 0) ylikely {
	            op->fl.hols = true ;
	        }
	    } /* end if (bug-check) */
	} /* end if (needed to be opened) */
	return rs ;
} /* end subroutine (holidayer_listfinish) */

local int holidayer_holfins(HO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.hols) ylikely {
	    vechand	*hlp = op->hlp ;
	    void	*vp{} ;
	    for (int i = 0 ; hlp->get(i,&vp) >= 0 ; i += 1) {
	        if (HO_H *hep = holptr(vp) ; hep) {
		    {
	                rs1 = holidayer_holend(op,hep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = mem.free(hep) ;
	                if (rs >= 0) rs = rs1 ;
		    } /* end block (memory-release) */
	        }
	    } /* end for */
	} /* end if (activated) */
	return rs ;
} /* end subroutine (holidayer_holfins) */

local int holidayer_holbegin(HO *op,HO_H *hep,int y,cchar *fn) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    if ((rs = holidays_open(&hep->hol,op->pr,y,fn)) >= 0) ylikely {
	        hep->year = y ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (holidayer_holbegin) */

local int holidayer_holend(HO *op,HO_H *hep) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op) ylikely {
	    rs1 = holidays_close(&hep->hol) ;
	    if (rs >= 0) rs = rs1 ;
	    hep->year = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (holidayer_holend) */

local int holidayer_yearfind(HO *op,uint y,holidays **rpp) noex {
	HO_H		*hep{} ;
	int		rs = SR_NOTFOUND ;
	if (op->fl.hols) ylikely {
	    vechand	*hlp = op->hlp ;
	    void	*vp{} ;
	    for (int i = 0 ; (rs = hlp->get(i,&vp)) >= 0 ; i += 1) {
	        if (vp) {
		    hep = holptr(vp) ;
	            if (hep->year == y) break ;
	        }
	    } /* end for */
	} /* end if (hols) */
	if (rs == SR_NOTFOUND) {
	    if ((rs = holidayer_yearfinder(op,y,&hep)) >= 0) {
	        if (rpp) *rpp = &hep->hol ;
	    }
	} else {
	    if (rpp) *rpp = &hep->hol ;
	}
	return rs ;
} /* end subroutine (holidayer_yearfind) */

local int holidayer_yearfinder(HO *op,int y,HO_H **rpp) noex {
	int		rs ;
	int		rs1 ;
	if (char *hfname ; (rs = mem.mp(&hfname)) >= 0) ylikely {
	    if ((rs = holidayer_yearfile(op,hfname,y)) >= 0) ylikely {
	        cint	esz = szof(HO_H) ;
	        if (void *vp ; (rs = mem.mall(esz,&vp)) >= 0) ylikely {
	            HO_H *hep = resumelife<HO_H>(vp) ;
	            if ((rs = holidayer_holbegin(op,hep,y,hfname)) >= 0) {
	                if ((rs = holidayer_yearadd(op,hep)) >= 0) {
	                    if (rpp != nullptr) *rpp = hep ;
	                }
	                if (rs < 0) {
	                    holidayer_holend(op,hep) ;
		        } /* end if (error) */
	            }
	            if (rs < 0) {
	                mem.free(hep) ;
		    } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (holidayer_yearfile) */
	    rs1 = mem.free(hfname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (holidayer_yearfinder) */

local int holidayer_yearfile(HO *op,char *hfname,uint y) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	for (int i = 0 ; holdnames[i] ; i += 1) {
	    cchar	*dn = holdnames[i] ;
	    if ((rs = holidayer_dirok(op,dn)) > 0) {
	        if ((rs = holidayer_mkfname(op,hfname,dn,y)) > 0) {
	            cint	pl = rs ;
	            if (ustat sb ; (rs = uc_stat(hfname,&sb)) >= 0) {
	                if (isOurMode(sb.st_mode)) {
	                    cint	am = (R_OK) ;
	                    if ((rs = permids(op->idp,&sb,am)) >= 0) {
	                        len = pl ;
	                    } else if (isNotAccess(rs)) {
	                        rs = SR_OK ;
	                    }
	                }
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            }
	        } /* end if (holidayer_mkfname) */
	    } /* end if (holidayer_dirok) */
	    if (rs > 0) break ;
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (holidayer_yearfile) */

local int holidayer_yearadd(HO *op,HO_H *hep) noex {
	int		rs ;
	if ((rs = holidayer_liststart(op)) >= 0) ylikely {
	    vechand	*hlp = op->hlp ;
	    rs = vechand_add(hlp,hep) ;
	}
	return rs ;
} /* end subroutine (holidayer_yearadd) */

local int holidayer_dirok(HO *op,cchar *dn) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if (char *dbuf ; (rs = mem.mp(&dbuf)) >= 0) ylikely {
	    if ((rs = holidayer_mkdir(op,dbuf,dn)) >= 0) ylikely {
	        if (ustat sb ; (rs = uc_stat(dbuf,&sb)) >= 0) ylikely {
	            if (S_ISDIR(sb.st_mode)) {
	                cint	am = (R_OK|X_OK) ;
	                if ((rs = permids(op->idp,&sb,am)) >= 0) {
	                    f = true ;
	                }
	            } /* end if (is-dir) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (holidayer_mkdir) */
	    rs1 = mem.free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (holidayer_dirok) */

local int holidayer_mkdir(HO *op,char *rbuf,cchar *dn) noex {
	cint		rlen = maxpathlen ;
	int		rs = SR_OK ;
	int		i = 0 ;
	if (dn[0] != '/') {
	    cchar	*pr = op->pr ;
	    cint	prl = lenstr(op->pr) ;
	    if ((rs = storebuf_strw(rbuf,rlen,i,pr,prl)) >= 0) {
	        i += rs ;
	        if (pr[prl-1] != '/') {
	            rs = storebuf_chr(rbuf,rlen,i,'/') ;
	            i += rs ;
	        }
	    }
	} /* end if (leading slash character) */
	if (rs >= 0) ylikely {
	    rs = storebuf_strw(rbuf,rlen,i,dn,-1) ;
	    i += rs ;
	} /* end if (ok) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (holidayer_mkdir) */

local int holidayer_mkfname(HO *op,char *rbuf,cchar *dn,uint y) noex {
	cint		rlen = maxpathlen ;
	int		rs = SR_OK ;
	int		i = 0 ;
	cchar		*pr = op->pr ;
	cchar		*prefix = HO_FPR ;
	if (dn[0] != '/') {
	    cint	prl = lenstr(pr) ;
	    if ((rs = storebuf_strw(rbuf,rlen,i,pr,prl)) >= 0) {
	        i += rs ;
	        if (pr[prl-1] != '/') {
	            rs = storebuf_chr(rbuf,rlen,i,'/') ;
	            i += rs ;
	        }
	    }
	} /* end if */
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,dn,-1) ;
	    i += rs ;
	} /* end if */
	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	} /* end if */
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,prefix,-1) ;
	    i += rs ;
	} /* end if */
	if ((rs >= 0) && (y > 0)) {
	    cint	dlen = DIGBUFLEN ;
	    char	dbuf[DIGBUFLEN+1] ;
	    if ((rs = ctdecpui(dbuf,dlen,4,y)) >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,dbuf,rs) ;
	        i += rs ;
	    }
	} /* end if */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (holidayer_mkfname) */

local int holidayer_holaudit(HO *op,HO_H *hep) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (hep->year > 0) {
	        rs = holidays_audit(&hep->hol) ;
	    } else {
	        rs = SR_BADFMT ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (holidayer_holaudit) */

local int holidayer_yearq(HO *op,HO_CITE *qp) noex {
	int		rs = SR_OK ;
	int		y = qp->y ;
	if (qp->y == 0) {
	    if (op->year == 0) {
	        rs = holidayer_yearmk(op) ;
	    }
	    if (rs >= 0) {
	        qp->y = ushort(op->year) ;
	        y = op->year ;
	    } /* end if (ok) */
	} else {
	    rs = restyear(qp->y) ;
	} /* end if (needed) */
	return (rs >= 0) ? y : rs ;
} /* end subroutine (holidayer_yearq) */

local int holidayer_year(HO *op,uint ay) noex {
	int		rs = SR_OK ;
	int		y = int(ay) ;
	if (y == 0) {
	    if (op->year == 0) {
	        rs = holidayer_yearmk(op) ;
	    }
	    if (rs >= 0) {
	        y = op->year ;
	    } /* end if (ok) */
	} else {
	    rs = restyear(y) ;
	} /* end if */
	return (rs >= 0) ? y : rs ;
} /* end subroutine (holidayer_year) */

local int holidayer_yearmk(HO *op) noex {
	int		rs = SR_OK ;
	int		y = 0 ; /* return-value */
	if (op->year == 0) {
	    custime	dt = time(nullptr) ;
	    if (tmtime m ; (rs = tmtime_timelocal(&m,dt)) >= 0) {
	        y = (m.year + TMTIME_YEARBASE) ;
	        op->year = y ;
	    }
	} /* end if (zero) */
	return (rs >= 0) ? y : rs ;
} /* end subroutine (holidayer_yearmk) */

local inline int restyear(int y) noex {
    	int		rs = SR_OK ;
	if_constexpr (f_restyear) {
	    if (y >= 2038) {
	        rs = SR_INVALID ;
	    }
	} /* end if_constexpr (f_restyear) */
	return rs ;
} /* end subroutine (restyear) */

local bool isOurMode(mode_t m) noex {
	bool	f = false ;
	f = f || S_ISREG(m) ;
	f = f || S_ISSOCK(m) ;
	f = f || S_ISCHR(m) ;
	f = f || S_ISFIFO(m) ;
	return f ;
} /* end subroutine (isOurMode) */


