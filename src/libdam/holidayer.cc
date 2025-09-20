/* holidayer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* access for the HOLIDAYER database */
/* version %I% last-modified %G% */


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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<tzfile.h>		/* for TM_YEAR_BASE */
#include	<usystem.h>
#include	<estrings.h>
#include	<storebuf.h>
#include	<ids.h>
#include	<vecobj.h>
#include	<bfile.h>
#include	<tmtime.hh>
#include	<permx.h>
#include	<ctdec.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"holidayer.h"


/* local defines */

#define	HOLIDAYER_HOL	struct holidayer_hol
#define	HOLIDAYER_FPR	"holidays"

#define	HO		holidayer
#define	HO_CUR		holidayer_cur
#define	HO_CITE		holidayer_cite
#define	HO_H		HOLIDAYER_HOL

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

/* external subroutines */


/* external variables */


/* local structures */

struct holidayer_hol {
	holidays	hol ;
	uint		year ;
} ;

typedef holidayer_hol *	holp ;


/* local structures */


/* forward references */

static int holidayer_holbegin(HO *,HO_H *,int,cchar *) noex ;
static int holidayer_holend(HO *,HO_H *) noex ;
static int holidayer_holfins(HO *) noex ;
static int holidayer_holaudit(HO *,HO_H *) noex ;

static int holidayer_yearfind(HO *,uint,holidays **) noex ;
static int holidayer_yearfinder(HO *,int,HO_H **) noex ;
static int holidayer_yearfile(HO *,char *,uint) noex ;
static int holidayer_yearadd(HO *,HO_H *) noex ;
static int holidayer_dirok(HO *,cchar *) noex ;
static int holidayer_mkdir(HO *,char *,cchar *) noex ;
static int holidayer_mkfname(HO *,char *,cchar *,uint) noex ;
static int holidayer_holaudit(HO *,HO_H *) noex ;
static int holidayer_yearq(HO *,HO_CITE *) noex ;
static int holidayer_year(HO *,uint) noex ;
static int holidayer_yearmk(HO *) noex ;

static bool	isOurMode(mode_t) noex ;


/* local variables */

constexpr cpcchar		holdnames[] = {
	"etc/acct",
	"/etc/acct",
	nullptr
} ;


/* exported variables */

extern const holidayer_obj	holidayer_modinfo = {
	"holidayer",
	szof(holidayer),
	szof(holidayer_cur)
} ;


/* exported subroutines */

int holidayer_open(HO *op,cchar *pr) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memclear(op) ;
	if (cchar *cp{} ; (rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	    op->pr = cp ;
	    if ((rs = ids_load(&op->id)) >= 0) {
	        op->magic = HOLIDAYER_MAGIC ;
	    }
	}

	return rs ;
}
/* end subroutine (holidayer_open) */

int holidayer_close(HO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("holidayer_close: ent\n") ;
#endif

	if (op->fl.hols) {
	    rs1 = holidayer_holfins(op) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fl.hols = false ;
	    rs1 = vechand_finish(&op->hols) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = ids_release(&op->id) ;
	if (rs >= 0) rs = rs1 ;

	if (op->pr != nullptr) {
	    rs1 = uc_free(op->pr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pr = nullptr ;
	}

#if	CF_DEBUGS
	debugprintf("holidayer_close: ret rs=%d\n",rs) ;
#endif

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (holidayer_close) */

int holidayer_audit(HO *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;

	if (op->fl.hols) {
	    vechand	*hlp = &op->hols ;
	    if ((rs = vechand_audit(hlp)) >= 0) {
		void	*vp{} ;
	        for (int i = 0 ; vechand_get(hlp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	        	HOLIDAYER_HOL	*hep = holp(vp) ;
			c += 1 ;
	                rs = holidayer_holaudit(op,hep) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (vechand_audit) */
	} /* end if */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (holidayer_audit) */

int holidayer_curbegin(HO *op,HO_CUR *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;

	memclear(curp) ;
	curp->magic = HOLIDAYER_MAGIC ;

	op->ncursors += 1 ;
	return SR_OK ;
}
/* end subroutine (holidayer_curbegin) */

int holidayer_curend(HO *op,HO_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;

	if (op->ncursors > 0) {
	    if (curp->hop != nullptr) {
	        holidays	*hop = curp->hop ;
	        holidays_cur	*hcp = &curp->hcur ;
	        if ((rs = holidays_curend(hop,hcp)) >= 0) {
	            op->ncursors -= 1 ;
	            curp->hop = nullptr ;
	            curp->year = 0 ;
	            curp->magic = 0 ;
	        }
	    } /* end if (cursor was used) */
	} else {
	    rs = SR_PROTO ;
	}

	return rs ;
}
/* end subroutine (holidayer_curend) */

int holidayer_fetchcite(HO *op,HO_CITE *qp,
		HO_CUR *curp,char *rp,int rl) noex {
	int		rs ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("holidayer_fetchcite: ent %u:%u:%u\n",
	    qp->y,qp->m,qp->d) ;
#endif

	if ((rs = holidayer_yearq(op,qp)) >= 0) {
	    const uint	y = rs ;
	    if (curp->year == 0) {
	        if (curp->hop == nullptr) {
	            holidays		*holp ;
	            holidays_cur	*hcp = &curp->hcur ;
	            if ((rs = holidayer_yearfind(op,y,&holp)) >= 0) {
	                if ((rs = holidays_curbegin(holp,hcp)) >= 0) {
	                    curp->hop = holp ;
	                    curp->year = y ;
	                    rs = holidays_fetchcite(holp,qp,hcp,rp,rl) ;
	                    vl = rs ;
	                } /* end if (holidays_curbegin) */
	            } /* end if (holidayer_yearfind) */
	        } else
	            rs = SR_PROTO ;
	    } else if (curp->year == qp->y) {
	        if (curp->hop != nullptr) {
	            holidays		*holp = curp->hop ;
	            holidays_cur	*hcp = &curp->hcur ;
	            rs = holidays_fetchcite(holp,qp,hcp,rp,rl) ;
	            vl = rs ;
	        } else
	            rs = SR_PROTO ;
	    } else
	        rs = SR_PROTO ;
	} /* end if (holidayer_year) */

#if	CF_DEBUGS
	debugprintf("holidayer_fetchcite: ret rs=%d bl=%u\n",rs,vl) ;
#endif

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (holidayer_fetchcite) */

int holidayer_fetchname(HO *op,uint y,cc *kp,int kl,
		holidayer_cur *curp,holidayer_cite *qp,char *rp,int rl) noex {
	int		rs ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (op->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("holidayer_fetchname: ent y=%d k=>%r<\n",y,kp,kl) ;
	debugprintf("holidayer_fetchname: cur y=%u hop{%p}\n",
	    curp->year,curp->hop) ;
#endif

	if ((rs = holidayer_year(op,y)) >= 0) {
	    const uint	y = rs ;
	    if (curp->year == 0) {
	        if (curp->hop == nullptr) {
	            holidays		*holp ;
	            holidays_cur	*hcp = &curp->hcur ;
	            if ((rs = holidayer_yearfind(op,y,&holp)) >= 0) {
	                if ((rs = holidays_curbegin(holp,hcp)) >= 0) {
	                    curp->hop = holp ;
	                    curp->year = y ;
	                    rs = holidays_fetchname(holp,kp,kl,hcp,qp,rp,rl) ;
	                    vl = rs ;
	                } /* end if (holidays_curbegin) */
	            } /* end if (holidayer_yearfind) */
	        } else
	            rs = SR_PROTO ;
	    } else if (curp->year == y) {
	        if (curp->hop != nullptr) {
	            holidays		*holp = curp->hop ;
	            holidays_cur	*hcp = &curp->hcur ;
	            rs = holidays_fetchname(holp,kp,kl,hcp,qp,rp,rl) ;
	            vl = rs ;
	        } else
	            rs = SR_PROTO ;
	    } else
	        rs = SR_PROTO ;
	} /* end if (holidayer_year) */

#if	CF_DEBUGS
	debugprintf("holidayer_fetchname: ret rs=%d bl=%u\n",rs,vl) ;
#endif

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (holidayer_fetchname) */

int holidayer_enum(HO *op,HO_CUR *curp,HO_CITE *qp,
		char *vbuf,int vlen,uint y) noex {
	int		rs = SR_OK ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (op->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;
	if (curp->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;

	if (op->ncursors == 0) return SR_INVALID ;

	if (vbuf != nullptr) vbuf[0] = '\0' ;

#if	CF_DEBUGS
	debugprintf("holidayer_enum: ent y=%u\n",y) ;
#endif

	if ((rs = holidayer_year(op,y)) >= 0) {
	    const uint	y = rs ;
	    if (curp->year == 0) {
	        if (curp->hop == nullptr) {
	            holidays		*holp ;
	            holidays_cur	*hcp = &curp->hcur ;
	            if ((rs = holidayer_yearfind(op,y,&holp)) >= 0) {
	                if ((rs = holidays_curbegin(holp,hcp)) >= 0) {
	                    curp->hop = holp ;
	                    curp->year = y ;
	                    rs = holidays_enum(holp,hcp,qp,vbuf,vlen) ;
	                    vl = rs ;
	                } /* end if (holidays_curbegin) */
	            } /* end if (holidayer_yearfind) */
	        } else
	            rs = SR_PROTO ;
	    } else if (curp->year == y) {
	        if (curp->hop != nullptr) {
	            holidays		*holp = curp->hop ;
	            holidays_cur	*hcp = &curp->hcur ;
	            rs = holidays_enum(holp,hcp,qp,vbuf,vlen) ;
	            vl = rs ;
	        } else
	            rs = SR_PROTO ;
	    } else
	        rs = SR_PROTO ;
	} /* end if (holidayer_year) */

#if	CF_DEBUGS
	debugprintf("holidayer_enum: ret rs=%d vl=%u\n",rs,vl) ;
#endif

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (holidayer_enum) */

int holidayer_check(HO *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f_changed = false ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != HOLIDAYER_MAGIC) return SR_NOTOPEN ;

	if (dt == 0) dt = time(nullptr) ;

#ifdef	COMMENT
#else
	if (dt == 1) f_changed = true ;
#endif

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (holidayer_check) */


/* private subroutines */

static int holidayer_holfins(HO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.hols) {
	    vechand	*hlp = &op->hols ;
	    void	*vp{} ;
	    for (int i = 0 ; vechand_get(hlp,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
	            HOLIDAYER_HOL	*hep = holp(vp) ;
		    {
	                rs1 = holidayer_holend(op,hep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = uc_free(hep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        }
	    } /* end for */
	} /* end if (activated) */
	return rs ;
}
/* end subroutine (holidayer_holfins) */

static int holidayer_holbegin(HO *op,HO_H *hep,int y,cchar *fn) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = holidays_open(&hep->hol,op->pr,y,fn)) >= 0) {
	        hep->year = y ;
	    }
	}
	return rs ;
}
/* end subroutine (holidayer_holbegin) */

static int holidayer_holend(HO *op,HO_H *hep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op == nullptr) return SR_FAULT ;
#if	CF_DEBUGS
	debugprintf("holidayer_holend: ent y=%u\n",hep->year) ;
#endif
	rs1 = holidays_close(&hep->hol) ;
	if (rs >= 0) rs = rs1 ;
	hep->year = 0 ;
#if	CF_DEBUGS
	debugprintf("holidayer_holend: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (holidayer_holend) */

static int holidayer_yearfind(HO *op,uint y,holidays **rpp) noex {
	HOLIDAYER_HOL	*hep{} ;
	int		rs = SR_NOTFOUND ;
	if (op->fl.hols) {
	    vechand	*hlp = &op->hols ;
	    void	*vp{} ;
	    for (int i = 0 ; (rs = vechand_get(hlp,i,&vp)) >= 0 ; i += 1) {
	        if (vp) {
		    hep = holp(vp) ;
	            if (hep->year == y) break ;
	        }
	    } /* end for */
	} /* end if (hols) */
	if (rs == SR_NOTFOUND) {
	    if ((rs = holidayer_yearfinder(op,y,&hep)) >= 0) {
	        if (rpp != nullptr) *rpp = &hep->hol ;
	    }
	} else {
	    if (rpp != nullptr) *rpp = &hep->hol ;
	}
	return rs ;
}
/* end subroutine (holidayer_yearfind) */

static int holidayer_yearfinder(HO *op,int y,HO_H **rpp) noex {
	int		rs ;
	char		hfname[MAXPATHLEN+1] ;
	if ((rs = holidayer_yearfile(op,hfname,y)) >= 0) {
	    HOLIDAYER_HOL	*hep ;
	    cint		esize = szof(HOLIDAYER_HOL) ;
	    if ((rs = uc_malloc(esize,&hep)) >= 0) {
	        if ((rs = holidayer_holbegin(op,hep,y,hfname)) >= 0) {
	            if ((rs = holidayer_yearadd(op,hep)) >= 0) {
	                if (rpp != nullptr) *rpp = hep ;
	            }
	            if (rs < 0) {
	                holidayer_holend(op,hep) ;
		    }
	        }
	        if (rs < 0) {
	            uc_free(hep) ;
		}
	    } /* end if (m-a) */
	} /* end if (holidayer_yearfile) */
#if	CF_DEBUGS
	debugprintf("holidayer_yearfinder: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (holidayer_yearfinder) */

static int holidayer_yearfile(HO *op,char *hfname,uint y) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	for (int i = 0 ; holdnames[i] != nullptr ; i += 1) {
	    cchar	*dn = holdnames[i] ;
	    if ((rs = holidayer_dirok(op,dn)) > 0) {
	        if ((rs = holidayer_mkfname(op,hfname,dn,y)) > 0) {
	            ustat	sb ;
	            const int		pl = rs ;
	            if ((rs = uc_stat(hfname,&sb)) >= 0) {
	                if (isOurMode(sb.st_mode)) {
	                    const int	am = (R_OK) ;
	                    if ((rs = permid(&op->id,&sb,am)) >= 0) {
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
}
/* end subroutine (holidayer_yearfile) */

static int holidayer_yearadd(HO *op,HO_H *hep) noex {
	vechand		*hlp = &op->hols ;
	int		rs = SR_OK ;
	if (! op->fl.hols) {
	    const int	vo = VECHAND_OSTATIONARY ;
	    if ((rs = vechand_start(hlp,2,vo)) >= 0) {
	        op->fl.hols = true ;
	    }
	}
	if (rs >= 0) {
	    rs = vechand_add(hlp,hep) ;
	}
	return rs ;
}
/* end subroutine (holidayer_yearadd) */

static int holidayer_dirok(HO *op,cchar *dn) noex {
	int		rs ;
	int		f = false ;
	char		dbuf[MAXPATHLEN+1] ;
	if ((rs = holidayer_mkdir(op,dbuf,dn)) >= 0) {
	    USTAT	sb ;
	    if ((rs = uc_stat(dbuf,&sb)) >= 0) {
	        if (S_ISDIR(sb.st_mode)) {
	            cint	am = (R_OK|X_OK) ;
	            if ((rs = permid(&op->id,&sb,am)) >= 0) {
	                f = true ;
	            }
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (holidayer_mkdir) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (holidayer_dirok) */

static int holidayer_mkdir(HO *op,char *rbuf,cchar *dn) noex {
	cint		rlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;
	if (dn[0] != '/') {
	    cchar	*pr = op->pr ;
	    const int	prl = lenstr(op->pr) ;
	    if ((rs = storebuf_strw(rbuf,rlen,i,pr,prl)) >= 0) {
	        i += rs ;
	        if (pr[prl-1] != '/') {
	            rs = storebuf_chr(rbuf,rlen,i,'/') ;
	            i += rs ;
	        }
	    }
	}
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,dn,-1) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (holidayer_mkdir) */

static int holidayer_mkfname(HO *op,char *rbuf,cchar *dn,uint y) noex {
	cint		rlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;
	cchar		*pr = op->pr ;
	cchar		*prefix = HOLIDAYER_FPR ;

	if (dn[0] != '/') {
	    cint	prl = lenstr(pr) ;
	    if ((rs = storebuf_strw(rbuf,rlen,i,pr,prl)) >= 0) {
	        i += rs ;
	        if (pr[prl-1] != '/') {
	            rs = storebuf_chr(rbuf,rlen,i,'/') ;
	            i += rs ;
	        }
	    }
	}

	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,dn,-1) ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,prefix,-1) ;
	    i += rs ;
	}

	if ((rs >= 0) && (y > 0)) {
	    const int	dlen = DIGBUFLEN ;
	    char	dbuf[DIGBUFLEN+1] ;
	    if ((rs = ctdecpui(dbuf,dlen,4,y)) >= 0) {
	        rs = storebuf_strw(rbuf,rlen,i,dbuf,rs) ;
	        i += rs ;
	    }
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (holidayer_mkfname) */

static int holidayer_holaudit(HO *op,HO_H *hep) noex {
	int		rs ;
	if (op == nullptr) return SR_FAULT ;
	if (hep->year > 0) {
	    rs = holidays_audit(&hep->hol) ;
	} else {
	    rs = SR_BADFMT ;
	}
	return rs ;
}
/* end subroutine (holidayer_holaudit) */

static int holidayer_yearq(HO *op,HO_CITE *qp) noex {
	int		rs = SR_OK ;
	int		y = qp->y ;
	if (qp->y == 0) {
	    if (op->year == 0) {
	        rs = holidayer_yearmk(op) ;
	    }
	    if (rs >= 0) {
	        qp->y = op->year ;
	        y = op->year ;
	    }
	} else {
	    if (qp->y >= 2038) rs = SR_INVALID ;
	} /* end if (needed) */
	return (rs >= 0) ? y : rs ;
}
/* end subroutine (holidayer_yearq) */


static int holidayer_year(HO *op,uint ay) noex {
	int		rs = SR_OK ;
	int		y = int(ay) ;
	if (y == 0) {
	    if (op->year == 0) {
	        rs = holidayer_yearmk(op) ;
	    }
	    if (rs >= 0) {
	        y = op->year ;
	    }
	} else {
	    if (y >= 2038) rs = SR_INVALID ;
	}
	return (rs >= 0) ? y : rs ;
}
/* end subroutine (holidayer_year) */

static int holidayer_yearmk(HO *op) noex {
	int		rs = SR_OK ;
	int		y = 0 ;
	if (op->year == 0) {
	    tmtime	m ;
	    custime	t = time(nullptr) ;
	    if ((rs = tmtime_localtime(&m,t)) >= 0) {
	        y = (m.year + TM_YEAR_BASE) ;
	        op->year = y ;
	    }
	}
	return (rs >= 0) ? y : rs ;
}
/* end subroutine (holidayer_yearmk) */

static bool isOurMode(mode_t m) noex {
	bool	f = false ;
	f = f || S_ISREG(m) ;
	f = f || S_ISSOCK(m) ;
	f = f || S_ISCHR(m) ;
	f = f || S_ISFIFO(m) ;
	return f ;
}
/* end subroutine (isOurMode) */


