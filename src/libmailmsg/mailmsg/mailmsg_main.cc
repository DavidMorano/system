/* mailmsg_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* message parsing object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

	= 2004-10-13, David A­D­ Morano
	Removed VECSTR as an optional storage object.

	= 2017-09-06, David A­D­ Morano
	Removed some unused subroutine at the end.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	mailmsg

	Description:
	This module operates on mail messages. It is expected that
	a file be opened and fed to this object one line at a time.
	The order of processing should proceed roughly as follows
	by the user:

	call	mailmsg_start()

	load lines of message:
		while (read_line) {
		    mailmsg_loadline()
		}

	call any of:
		mailmsg_envaddress()
		mailmsg_hdrval()
		mailmsg_hdrival()
		...()

	finally call:
		mailmsg_finish()

	Notes:

	Our basic algoirthm is to read all message-header lines and
	to store them in allocated space.  We then construct structures
	that contain elements that point to strings in the stored
	lines (previously read).  We had (at least) two options for
	which object to use to store the lines: STRPACK or VECSTR.
	Either is useful and probably fairly efficient.  The STRPACK
	is more space efficient but VECSTR is probably more speed
	efficient.  I originally had both coded in and available
	with a compile-time switch, but I removed that option.  We
	now only use STRPACK.  If you want VECSTR or something else,
	just replace calls to STRPACK with your new thing.  There
	are only a few places where these are.

	This object is pretty kick-butt fast! We also use late
	processing and caching almost throughout, so we do not waste
	time on things the caller doesn't need.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<strpack.h>
#include	<vecobj.h>
#include	<vechand.h>
#include	<mailmsgmatenv.h>
#include	<mailmsgmathdr.h>
#include	<strwcpy.h>
#include	<toxc.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"mailmsg.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	MMHVAL		msghdrval
#define	MMHINST		msghdrinst
#define	MMHNAME		msghdrname

#define	DEFHEADERS	25

#define	HDRNAMELEN	80

#define	ISHDRCONT(c)	(((c) == ' ') || ((c) == '\t'))
#define	ISEND(c)	(((c) == '\n') || ((c) == '\r'))

#define	DEFSTRLEN	1000


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct msghdrval {
	cchar		*vp ;
	int		vl ;
} ;

struct msghdrinst {
	cchar		*vp ;
	vecobj		vals ;
	int		vl ;
	int		f_alloc ;
} ;

struct msghdrname {
	cchar		*name ;
	cchar		*vp ;
	vecobj		insts ;
	int		vl ;
	int		f_alloc ;
	int		namelen ;
	int		lastinst ;	/* index of last INST */
} ;

enum msgstates {
	msgstate_env,
	msgstate_hdr,
	msgstate_overlast
} ;


/* forward references */

template<typename ... Args>
static int mailmsg_ctor(mailmsg *op,Args ... args) noex {
    	MAILMSG		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->slp = new(nothrow) strpack) != np) {
	        if ((op->elp = new(nothrow) vecobj) != np) {
	            if ((op->hlp = new(nothrow) vecobj) != np) {
			rs = SR_OK ;
		    } /* end if (new-vecobj) */
		    if (rs < 0) {
		        delete op->elp ;
		        op->elp = nullptr ;
		    }
		} /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->slp ;
		    op->slp = nullptr ;
		}
	    } /* end if (new-strpack) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsg_ctor) */

static int mailmsg_dtor(mailmsg *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->hlp) {
		delete op->hlp ;
		op->hlp = nullptr ;
	    }
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	    if (op->slp) {
		delete op->slp ;
		op->slp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsg_dtor) */

static int mailmsg_procline(mailmsg *,cchar *,int) noex ;

static int mailmsg_envbegin(mailmsg *) noex ;
static int mailmsg_envadd(mailmsg *,mmenvdat *) noex ;
static int mailmsg_envend(mailmsg *) noex ;

static int mailmsg_hdrbegin(mailmsg *) noex ;
static int mailmsg_hdrend(mailmsg *) noex ;
static int mailmsg_hdraddnew(mailmsg *,cchar *,int,cchar *,int) noex ;
static int mailmsg_hdraddcont(mailmsg *,cchar *,int) noex ;
static int mailmsg_hdrmatch(mailmsg *,MMHNAME **,cchar *,int) noex ;

static int msghdrname_start(MMHNAME *,cchar *,int,cchar *,int) noex ;
static int msghdrname_match(MMHNAME *,cchar *,int) noex ;
static int msghdrname_addnew(MMHNAME *,cchar *,int) noex ;
static int msghdrname_addcont(MMHNAME *,cchar *,int) noex ;
static int msghdrname_iline(MMHNAME *,int,int,cchar **) noex ;
static int msghdrname_ival(MMHNAME *,int,cchar **) noex ;
static int msghdrname_val(MMHNAME *,cchar **) noex ;
static int msghdrname_count(MMHNAME *) noex ;
static int msghdrname_finish(MMHNAME *) noex ;

static int msghdrinst_start(MMHINST *,cchar *,int) noex ;
static int msghdrinst_add(MMHINST *,cchar *,int) noex ;
static int msghdrinst_ival(MMHINST *,int,cchar **) noex ;
static int msghdrinst_val(MMHINST *,cchar **) noex ;
static int msghdrinst_finish(MMHINST *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsg_start(mailmsg *op) noex {
	int		rs ;
	if ((rs = mailmsg_ctor(op)) >= 0) {
	    cint	deflen = DEFSTRLEN ;
	    if ((rs = strpack_start(op->slp,deflen)) >= 0) {
	        if ((rs = mailmsg_envbegin(op)) >= 0) {
		    if ((rs = mailmsg_hdrbegin(op)) >= 0) {
		        op->magic = MAILMSG_MAGIC ;
		    }
		    if (rs < 0) {
		        mailmsg_envend(op) ;
		    }
	        }
	        if (rs < 0) {
		    strpack_finish(op->slp) ;
	        }
	    } /* end if */
	    if (rs < 0) {
		mailmsg_dtor(op) ;
	    }
	} /* end if (mailmsg_ctor) */
	return rs ;
}
/* end subroutine (mailmsg_start) */

int mailmsg_finish(mailmsg *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailmsg_magic(op)) >= 0) {
	    {
	        rs1 = mailmsg_hdrend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mailmsg_envend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->slp) {
	        rs1 = strpack_finish(op->slp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = mailmsg_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsg_finish) */

int mailmsg_loadline(mailmsg *op,cchar *lp,int ll) noex {
	int		rs ;
	if ((rs = mailmsg_magic(op,lp)) >= 0) {
	    if (ll < 0) ll = lenstr(lp) ;
	    while ((ll > 0) && ISEND(lp[ll-1])) {
	        ll -= 1 ;
	    }
	    if (ll > 0) {
	        int	sl ;
	        cchar	*sp ;
	        if ((rs = strpack_store(op->slp,lp,ll,&sp)) >= 0) {
		    sl = ll ;
	            rs = mailmsg_procline(op,sp,sl) ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsg_loadline) */

int mailmsg_envcount(mailmsg *op) noex {
	int		rs ;
	if ((rs = mailmsg_magic(op)) >= 0) {
	    rs = vecobj_count(op->elp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsg_envcount) */

int mailmsg_envaddress(mailmsg *op,int i,cchar **rpp) noex {
	int		rs ;
	int		el = 0 ;
	if ((rs = mailmsg_magic(op)) >= 0) {
	    cchar	*rp = nullptr ;
	    void	*vp{} ;
	    if ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        if (vp) {
		    mmenvdat	*ep = (mmenvdat *) vp ;
		    el = ep->a.el ;
		    rp =  ep->a.ep ;
	        }
	    }
	    if (rpp) {
	        *rpp = (rs >= 0) ? rp : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mailmsg_envaddress) */

int mailmsg_envdate(mailmsg *op,int i,cchar **rpp) noex {
	int		rs ;
	int		el = 0 ;
	if ((rs = mailmsg_magic(op)) >= 0) {
	    cchar	*rp = nullptr ;
	    void	*vp{} ;
	    if ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        if (vp) {
		    mmenvdat	*ep = (mmenvdat *) vp ;
		    el = ep->d.el ;
		    rp = ep->d.ep ;
	        }
	    }
	    if (rpp) {
	        *rpp = (rs >= 0) ? rp : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mailmsg_envdate) */

int mailmsg_envremote(mailmsg *op,int i,cchar **rpp) noex {
	int		rs ;
	int		el = 0 ;
	if ((rs = mailmsg_magic(op)) >= 0) {
	    cchar	*rp = nullptr ;
	    void	*vp{} ;
	    if ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        if (vp) {
		    mmenvdat	*ep = (mmenvdat *) vp ;
		    el = ep->r.el ;
		    rp = ep->r.ep ;
	        }
	    }
	    if (rpp) {
	        *rpp = (rs >= 0) ? rp : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mailmsg_envremote) */

int mailmsg_hdrcount(mailmsg *op,cchar *name) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = mailmsg_magic(op,name)) >= 0) {
	    MMHNAME	*hnp{} ;
	    cint	hlen = HDRNAMELEN ;
	    int		hl = -1 ;
	    cchar	*hp = name ;
	    char	hbuf[HDRNAMELEN + 1] ;
	    if (hasuc(name,-1)) {
	        hl = intconv(strwcpylc(hbuf,name,hlen) - hbuf) ;
	        hp = hbuf ;
	    }
	    if ((rs = mailmsg_hdrmatch(op,&hnp,hp,hl)) >= 0) {
	        if (hnp) {
	            rs = msghdrname_count(hnp) ;
	            c = rs ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailmsg_hdrcount) */

int mailmsg_hdrikey(mailmsg *op,int hi,cchar **rpp) noex {
	int		rs ;
	int		nl = 0 ;
	if ((rs = mailmsg_magic(op)) >= 0) {
	    cchar	*namep = nullptr ;
	    void	*vp{} ;
	    if ((rs = vecobj_get(op->hlp,hi,&vp)) >= 0) {
	        if (vp) {
		    MMHNAME	*hnp = (MMHNAME *) vp ;
		    nl = hnp->namelen ;
		    namep = hnp->name ;
	        }
	    }
	    if (rpp) {
	        *rpp = (rs >= 0) ? namep : nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? nl : rs ;
}
/* end subroutine (mailmsg_hdrikey) */

int mailmsg_hdriline(mailmsg *op,cchar *name,int hi,int li,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = mailmsg_magic(op,name)) >= 0) {
	    MMHNAME	*hnp = nullptr ;
	    cint	hlen = HDRNAMELEN ;
	    int		hl = -1 ;
	    cchar	*hp = name ;
	    char	hbuf[HDRNAMELEN + 1] ;
	    if (hasuc(name,-1)) {
	        hl = intconv(strwcpylc(hbuf,name,hlen) - hbuf) ;
	        hp = hbuf ;
	    }
	    if ((rs = mailmsg_hdrmatch(op,&hnp,hp,hl)) >= 0) {
	        if (hnp) {
	            rs = msghdrname_iline(hnp,hi,li,rpp) ;
	            vl = rs ;
	        }
	    }
	    if (rs < 0) {
	        if (rpp) *rpp = nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailmsg_hdriline) */

int mailmsg_hdrival(mailmsg *op,cchar *name,int hi,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = mailmsg_magic(op,name)) >= 0) {
	    MMHNAME	*hnp = nullptr ;
	    cint	hlen = HDRNAMELEN ;
	    int		hl = -1 ;
	    cchar	*hp = name ;
	    char	hbuf[HDRNAMELEN + 1] ;
	    if (hasuc(name,-1)) {
	        hl = intconv(strwcpylc(hbuf,name,hlen) - hbuf) ;
	        hp = hbuf ;
	    }
	    if ((rs = mailmsg_hdrmatch(op,&hnp,hp,hl)) >= 0) {
	        if (hnp) {
	            rs = msghdrname_ival(hnp,hi,rpp) ;
	            vl = rs ;
	        }
	    }
	    if (rs < 0) {
	        if (rpp) *rpp = nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailmsg_hdrival) */

int mailmsg_hdrval(mailmsg *op,cchar *name,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = mailmsg_magic(op,name)) >= 0) {
	    MMHNAME	*hnp = nullptr ;
	    cint	hlen = HDRNAMELEN ;
	    int		hl = -1 ;
	    cchar	*hp = name ;
	    char	hbuf[HDRNAMELEN + 1] ;
	    if (hasuc(name,-1)) {
	        hl = intconv(strwcpylc(hbuf,name,hlen) - hbuf) ;
	        hp = hbuf ;
	    }
	    if ((rs = mailmsg_hdrmatch(op,&hnp,hp,hl)) >= 0) {
	        if (hnp) {
	            rs = msghdrname_val(hnp,rpp) ;
	            vl = rs ;
	        }
	    }
	    if (rs < 0) {
	        if (rpp) *rpp = nullptr ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailmsg_hdrval) */


/* local subroutines */

static int mailmsg_procline(mailmsg *op,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	if (op->msgstate == msgstate_env) {
	    if (mmenvdat es ; (rs = mailmsgmatenv(&es,lp,ll)) > 0) {
	        rs = mailmsg_envadd(op,&es) ;
	    } else if (rs == 0) {
	        op->msgstate = msgstate_hdr ;
	    }
	} /* end if */
	if ((rs >= 0) && (op->msgstate == msgstate_hdr)) {
	    if (int vi ; (rs = mailmsgmathdr(lp,ll,&vi)) > 0) {
	        cchar	*vp = (lp + vi) ;
	        cint	vl = (ll - vi) ;
	        rs = mailmsg_hdraddnew(op,lp,rs,vp,vl) ;
	    } else if ((rs == 0) && (ll > 0) && ISHDRCONT(lp[0])) {
	        rs = mailmsg_hdraddcont(op,(lp+1),(ll-1)) ;
	    }
	} /* end if */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsg_procline) */

static int mailmsg_envbegin(mailmsg *op) noex {
	cint		esz = szof(mmenvdat) ;
	return vecobj_start(op->elp,esz,4,0) ;
}
/* end subroutine (mailmsg_envbegin) */

static int mailmsg_envend(mailmsg *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->elp) {
	    rs1 = vecobj_finish(op->elp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (mailmsg_envend) */

static int mailmsg_envadd(mailmsg *op,mmenvdat *esp) noex {
	int		rs = SR_FAULT ;
	if (op && esp) {
	    rs = vecobj_add(op->elp,esp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsg_envadd) */

static int mailmsg_hdrbegin(mailmsg *op) noex {
	cint		hsz = szof(MMHNAME) ;
	op->lastname = -1 ;
	return vecobj_start(op->hlp,hsz,10,0) ;
}
/* end subroutine (mailmsg_hdrbegin) */

static int mailmsg_hdrend(mailmsg *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op->hlp,i,&vp) >= 0 ; i += 1) {
	    MMHNAME	*hnp = (MMHNAME *) vp ;
	    if (vp) {
	        rs1 = msghdrname_finish(hnp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	{
	    rs1 = vecobj_finish(op->hlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (mailmsg_hdrend) */

static int mailmsg_hdraddnew(mailmsg *op,cc *hp,int hl,cc *valp,int vall) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (hl != 0) {
	    cint	hlen = HDRNAMELEN ;
	    int		cl ;
	    cchar	*cp ;
	    char	hbuf[HDRNAMELEN + 1] ;
	    if (hasuc(hp,hl)) {
	        if ((hl < 0) || (hl > hlen)) hl = hlen ;
	        strwcpylc(hbuf,hp,hl) ;
	        hp = hbuf ;
	    }
	    if ((cl = sfshrink(valp,vall,&cp)) >= 0) {
	        MMHNAME	*hnp ;
	        if ((rs1 = mailmsg_hdrmatch(op,&hnp,hp,hl)) >= 0) {
	            op->lastname = rs1 ;
	            rs = msghdrname_addnew(hnp,cp,cl) ;
	        } else {
		    vecobj	*nlp = op->hlp ;
	            if (void *p ; (rs = vecobj_addnew(nlp,&p)) >= 0) {
		        cint	i = rs ;
	                hnp = (MMHNAME *) p ;
	                if ((rs = msghdrname_start(hnp,hp,hl,cp,cl)) >= 0) {
	                    op->lastname = i ;
		        } else {
		            vecobj_del(nlp,i) ;
		        }
	            }
	        } /* end if */
	    } /* end if (shrink) */
	} else {
	    op->lastname = -1 ;
	}
	return rs ;
}
/* end subroutine (mailmsg_hdraddnew) */

static int mailmsg_hdraddcont(mailmsg *op,cchar *valp,int vall) noex {
	int		rs = SR_OK ;
	if (vall > 0) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(valp,vall,&cp)) > 0) {
		cint	ln = op->lastname ;
		if (ln >= 0) {
		    if (void *vp ; (rs = vecobj_get(op->hlp,ln,&vp)) >= 0) {
		    	MMHNAME		*hnp = (MMHNAME *) vp ;
	    	        if (vp) {
	        	    rs = msghdrname_addcont(hnp,cp,cl) ;
	    	        }
		    }
		}
	    } /* end if (positive) */
	} /* end if (positive) */
	return rs ;
}
/* end subroutine (mailmsg_hdraddcont) */

static int mailmsg_hdrmatch(mailmsg *op,MMHNAME **hnpp,cc *hp,int hl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		i ; /* used-afterwards */
	int		f = false ;
	if (hl < 0) hl = lenstr(hp) ;
	*hnpp = nullptr ;
	void	*vp{} ;
	for (i = 0 ; (rs = vecobj_get(op->hlp,i,&vp)) >= 0 ; i += 1) {
	    *hnpp = (MMHNAME *) vp ;
	    if (*hnpp != nullptr) {
	        if ((rs = msghdrname_match(*hnpp,hp,hl)) > 0) {
		    f = true ;
		} else if (rs == rsn) {
		    rs = SR_OK ;
		}
	    }
	    if (f) break ;
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mailmsg_hdrmatch) */

static int msghdrname_start(MMHNAME *hnp,cc *hp,int hl,cc *valp,int vall) noex {
	vecobj		*ilp = &hnp->insts ;
	int		rs ;
	int		isz = szof(MMHINST) ;
	memclear(hnp) ;
	hnp->vp = nullptr ;
	hnp->vl = 0 ;
	hnp->f_alloc = false ;
	hnp->name = nullptr ;
	hnp->namelen = 0 ;
	hnp->lastinst = -1 ;
	if ((rs = vecobj_start(ilp,isz,2,0)) >= 0) {
	    if (cchar *cp ; (rs = libmem.strw(hp,hl,&cp)) >= 0) {
	        hnp->name = cp ;
	        hnp->namelen = (rs-1) ;
	        rs = msghdrname_addnew(hnp,valp,vall) ;
		if (rs < 0) {
		    void *vp = voidp(hnp->name) ;
	    	    libmem.free(vp) ;
	    	    hnp->name = nullptr ;
	    	    hnp->namelen = 0 ;
		} /* end if (error) */
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
		vecobj_finish(ilp) ;
	    }
	} /* end if (vecobj_start) */
	return rs ;
}
/* end subroutine (msghdrname_start) */

static int msghdrname_finish(MMHNAME *hnp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((hnp->vp != nullptr) && hnp->f_alloc) {
	    void *vp = voidp(hnp->vp) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    hnp->vp = nullptr ;
	    hnp->vl = 0 ;
	    hnp->f_alloc = false ;
	}
	if (hnp->name != nullptr) {
	    void *vp = voidp(hnp->name) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    hnp->name = nullptr ;
	    hnp->namelen = 0 ;
	}
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(&hnp->insts,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		MMHINST		*hip = (MMHINST *) vp ;
	        rs1 = msghdrinst_finish(hip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	{
	    hnp->lastinst = -1 ;
	    rs1 = vecobj_finish(&hnp->insts) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end block */
	hnp->vp = nullptr ;
	hnp->f_alloc = false ;
	return rs ;
}
/* end subroutine (msghdrname_finish) */

static int msghdrname_match(MMHNAME *hnp,cchar *hp,int hl) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (hnp && hp) {
	    rs = SR_OK ;
	    f = (hnp->namelen == hl) ;
	    f = f && (hnp->name[0] == hp[0]) ;
	    f = f && (strncmp(hnp->name,hp,hl) == 0) ;
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (msghdrname_match) */

static int msghdrname_addnew(MMHNAME *hnp,cchar *vp,int vl) noex {
	vecobj		*ilp = &hnp->insts ;
	int		rs ;
	int		f = false ;
	if (void *p ; (rs = vecobj_addnew(ilp,&p)) >= 0) {
	    MMHINST	*instp = (MMHINST *) p ;
	    cint	i = rs ;
	    if ((rs = msghdrinst_start(instp,vp,vl)) >= 0) {
	        hnp->lastinst = i ;
		f = true ;
	    } else {
		ilp->del(i) ;
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (msghdrname_addnew) */

static int msghdrname_addcont(MMHNAME *hnp,cchar *valp,int vall) noex {
	int		rs = SR_OK ;
	if (vall > 0) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(valp,vall,&cp)) > 0) {
	        cint	li = hnp->lastinst ;
		if (li >= 0) {
		    vecobj	*ilp = &hnp->insts ;
		    if (void *vp ; (rs = ilp->get(li,&vp)) >= 0) {
		    	MMHINST	*hip = (MMHINST *) vp ;
		        if (vp) {
	    		    rs = msghdrinst_add(hip,cp,cl) ;
			}
		    }
		}
	    } /* end if (positive) */
	} /* end if (positive) */
	return rs ;
}
/* end subroutine (msghdrname_addcont) */

static int msghdrname_iline(MMHNAME *hnp,int hi,int li,cchar **rpp) noex {
    	vecobj		*ilp = &hnp->insts ;
	int		rs ;
	int		len = 0 ;
	if (void *vp ; (rs = ilp->get(hi,&vp)) >= 0) {
	    MMHINST	*hip = (MMHINST *) vp ;
	    if (vp) {
	        rs = msghdrinst_ival(hip,li,rpp) ;
	        len = rs ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (msghdrname_iline) */

static int msghdrname_ival(MMHNAME *hnp,int hi,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	void		*vp{} ;
	if ((rs = vecobj_get(&hnp->insts,hi,&vp)) >= 0) {
	    if (vp) {
	        MMHINST		*hip = (MMHINST *) vp ;
	        rs = msghdrinst_val(hip,rpp) ;
	        vl = rs ;
	    }
	}
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (msghdrname_ival) */

static int msghdrname_val(MMHNAME *hnp,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		vl = hnp->vl ;
	if (hnp->vp == nullptr) {
	    vecobj	*ilp = &hnp->insts ;
	    MMHINST	*hip{} ;
	    int		sz = 1 ;
	    cchar	*hivp = nullptr ;
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(ilp,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
		    hip = (MMHINST *) vp ;
	            rs = msghdrinst_val(hip,&hivp) ;
	            sz += (rs + 2) ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	    if (rs >= 0) {
	        if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
	            int		n = 0 ;
		    hnp->vp = bp ;
	            hnp->f_alloc = true ;
	            for (int i = 0 ; vecobj_get(ilp,i,&vp) >= 0 ; i += 1) {
	                if (vp) {
		    	    hip = (MMHINST *) vp ;
	                    if ((rs = msghdrinst_val(hip,&hivp)) > 0) {
	                        if (n++ > 0) {
	                            *bp++ = ',' ;
	                            *bp++ = ' ' ;
	                        }
	                        bp = strwcpy(bp,hivp,rs) ;
			    } /* end if (positive) */
	                } /* end if (non-null) */
			if (rs < 0) break ;
	            } /* end for */
		    if (rs >= 0) {
	                *bp = '\0' ;
	                hnp->vl = intconv(bp - hnp->vp) ;
		    }
		    if (rs < 0) {
			libmem.free(bp) ;
			hnp->vp = nullptr ;
		    } /* end if (error) */
	        } /* end if (memory-allocation) */
	        vl = hnp->vl ;
	    } /* end if (ok) */
	} /* end if (needed) */
	if (rpp) {
	    *rpp = (rs >= 0) ? hnp->vp : nullptr ;
	}
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (msghdrname_val) */

static int msghdrname_count(MMHNAME *hnp) noex {
	return vecobj_count(&hnp->insts) ;
}
/* end subroutine (msghdrname_count) */

static int msghdrinst_start(MMHINST *hip,cchar *vp,int vl) noex {
	cint		vsz = szof(MMHVAL) ;
	int		rs ;
	memclear(hip) ;
	hip->vp = nullptr ;
	hip->vl = 0 ;
	if ((rs = vecobj_start(&hip->vals,vsz,4,0)) >= 0) {
	    if ((vl > 0) && vp) {
	        rs = msghdrinst_add(hip,vp,vl) ;
	        if (rs < 0) {
		    vecobj_finish(&hip->vals) ;
		}
	    }
	} /* enf if (vecobj_start) */
	return rs ;
}
/* end subroutine (msghdrinst_start) */

static int msghdrinst_add(MMHINST *hip,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	if (vl > 0) {
	    vecobj	*vlp = &hip->vals ;
	    if (void *p ; (rs = vecobj_addnew(vlp,&p)) >= 0) {
	        MMHVAL	*valp = (MMHVAL *) p ;
		valp->vp = vp ;
		valp->vl = vl ;
	    }
	}
	return rs ;
}
/* end subroutine (msghdrinst_add) */

static int msghdrinst_ival(MMHINST *hip,int li,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	cchar		*rp = nullptr ;
	if (void *vp ; (rs = vecobj_get(&hip->vals,li,&vp)) >= 0) {
	    MMHVAL *valp = (MMHVAL *) vp ;
	    vl = valp->vl ;
	    rp = valp->vp ;
	}
	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (msghdrinst_ival) */

static int msghdrinst_val(MMHINST *hip,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		vl = hip->vl ;
	if (hip->vp == nullptr) {
	    vecobj	*vlp = &hip->vals ;
	    int		sz = 1 ;
	    void	*vp{} ;
	    for (int i = 0 ; vlp->get(i,&vp) >= 0 ; i += 1) {
	        if (vp) {
	    	    MMHVAL	*valp = (MMHVAL *) vp ;
	            sz += (valp->vl + 1) ;
		}
	    } /* end for */
	    if (rs >= 0) {
	        if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
		    int	n = 0 ;
		    hip->vp = bp ;
		    hip->f_alloc = true ;
		    for (int i = 0 ; vlp->get(i,&vp) >= 0 ; i += 1) {
		        if (vp) {
	    	            MMHVAL	*valp = (MMHVAL *) vp ;
		            if (valp->vl > 0) {
			        if (n++ > 0) *bp++ = ' ' ;
			        bp = strwcpy(bp,valp->vp,valp->vl) ;
			    }
		        }
		    } /* end for */
		    *bp = '\0' ;
		    hip->vl = intconv(bp - hip->vp) ;
		    if (rs < 0) {
			libmem.free(bp) ;
			hip->vp = nullptr ;
		    } /* end if (error) */
	        } /* end if (memory-allocation) */
	        vl = hip->vl ;
	    } /* end if (ok) */
	} /* end if (needed) */
	if (rpp) {
	    *rpp = (rs >= 0) ? hip->vp : nullptr ;
	}
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (msghdrinst_val) */

static int msghdrinst_finish(MMHINST *hip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((hip->vp != nullptr) && hip->f_alloc) {
	    void *vp = voidp(hip->vp) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    hip->vp = nullptr ;
	    hip->vl = 0 ;
	    hip->f_alloc = false ;
	} /* end if */
	{
	    rs1 = vecobj_finish(&hip->vals) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end block */
	hip->vp = nullptr ;
	hip->f_alloc = false ;
	return rs ;
}
/* end subroutine (msghdrinst_finish) */


