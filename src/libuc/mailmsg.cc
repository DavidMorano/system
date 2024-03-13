/* mailmsg SUPPORT */
/* lang=C++20 */

/* message parsing object */
/* version %I% last-modified %G% */

#define	CF_PEDANTIC	0		/* extra precautions */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

	= 2004-10-13, David A­D­ Morano
	Removed VECSTR as an optional storage object.

	= 2017-09-06, David A­D­ Morano
	Removed some unused subroutine at the end.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
	to store them in allocated space. We then construct structures
	that contain elements that point to strings in the stored
	lines (previously read). We had (at least) two options for
	which object to use to store the lines: STRPACK or VECSTR.
	Either is useful and probably fairly efficient. The STRPACK
	is more space efficient but VECSTR is probably more speed
	efficient. I originally had both coded in and available
	with a compile-time switch, but I removed that option. We
	now only use STRPACK.  If you want VECSTR or something else,
	just replace calls to STRPACK with your new thing. There
	are only a few places where these are.

	This object is pretty kick-butt fast! We also use late
	processing and caching almost throughout, so we do not waste
	time on things the caller doesn't need.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<vechand.h>
#include	<vecobj.h>
#include	<mailmsgmatenv.h>
#include	<strpack.h>
#include	<strwcpy.h>
#include	<toxc.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"mailmsg.h"


/* local defines */

#ifndef	MAILMSG_HDR
#define	MAILMSG_HDR	struct mailmsg_hdr
#endif /* MAILMSG_HDR */
#ifndef	MAILMSG_ENV
#define	MAILMSG_ENV	struct mailmsg_env
#endif /* MAILMSG_ENV */

#define	MMH		MAILMSG_HDR
#define	MMHVAL		struct msghdrval
#define	MMHINST		struct msghdrinst
#define	MMHNAME		struct msghdrname

#define	DEFHEADERS	25

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#define	HDRNAMELEN	80

#define	ISHDRCONT(c)	(((c) == ' ') || ((c) == '\t'))
#define	ISEND(c)	(((c) == '\n') || ((c) == '\r'))


/* imported namespaces */


/* local typedefs */


/* external subroutines */

#if	defined(BSD) && (! defined(EXTERN_STRNCASECMP))
extern int	strncasecmp(cchar *,cchar *,int) noex ;
#endif

extern int	mailmsgmathdr(cchar *,int,int *) noex ;


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

static int mailmsg_procline(MAILMSG *,cchar *,int) noex ;

static int mailmsg_envbegin(MAILMSG *) noex ;
static int mailmsg_envadd(MAILMSG *,MAILMSGMATENV *) noex ;
static int mailmsg_envend(MAILMSG *) noex ;

static int mailmsg_hdrbegin(MAILMSG *) noex ;
static int mailmsg_hdrend(MAILMSG *) noex ;
static int mailmsg_hdraddnew(MAILMSG *,cchar *,int,cchar *,int) noex ;
static int mailmsg_hdraddcont(MAILMSG *,cchar *,int) noex ;
static int mailmsg_hdrmatch(MAILMSG *,MMHNAME **,cchar *,int) noex ;

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

int mailmsg_start(MAILMSG *op) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	memset(op,0,sizeof(MAILMSG)) ;

	if ((rs = strpack_start(&op->stores,2000)) >= 0) {
	    if ((rs = mailmsg_envbegin(op)) >= 0) {
		if ((rs = mailmsg_hdrbegin(op)) >= 0) {
		    op->magic = MAILMSG_MAGIC ;
		}
		if (rs < 0) {
		    mailmsg_envend(op) ;
		}
	    }
	    if (rs < 0) {
		strpack_finish(&op->stores) ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (mailmsg_start) */

int mailmsg_finish(MAILMSG *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	rs1 = mailmsg_hdrend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = mailmsg_envend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = strpack_finish(&op->stores) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (mailmsg_finish) */

int mailmsg_loadline(MAILMSG *op,cchar *lp,int ll) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	if (ll < 0)
	    ll = strlen(lp) ;

	while ((ll > 0) && ISEND(lp[ll-1])) {
	    ll -= 1 ;
	}

	if (ll > 0) {
	    int		sl ;
	    cchar	*sp ;
	    if ((rs = strpack_store(&op->stores,lp,ll,&sp)) >= 0) {
		sl = ll ;
	        rs = mailmsg_procline(op,sp,sl) ;
	    }
	}

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsg_loadline) */

int mailmsg_envcount(MAILMSG *op) noex {
	MAILMSG_ENV	*oep ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	oep = &op->envs ;
	rs = vecobj_count(&oep->insts) ;

	return rs ;
}
/* end subroutine (mailmsg_envcount) */

int mailmsg_envaddress(MAILMSG *op,int i,cchar **rpp) noex {
	MAILMSG_ENV	*oep = &op->envs ;
	int		rs ;
	int		el = 0 ;
	cchar		*rp = nullptr ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	void	*vp{} ;
	if ((rs = vecobj_get(&oep->insts,i,&vp)) >= 0) {
	    if (vp) {
		MAILMSGMATENV	*ep = (MAILMSGMATENV *) vp ;
		el = ep->a.el ;
		rp =  ep->a.ep ;
	    }
	}

	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}

	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mailmsg_envaddress) */

int mailmsg_envdate(MAILMSG *op,int i,cchar **rpp) noex {
	MAILMSG_ENV	*oep = &op->envs ;
	int		rs ;
	int		el = 0 ;
	void		*vp{} ;
	cchar		*rp = nullptr ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	if ((rs = vecobj_get(&oep->insts,i,&vp)) >= 0) {
	    if (vp) {
		MAILMSGMATENV	*ep = (MAILMSGMATENV *) vp ;
		el = ep->d.el ;
		rp = ep->d.ep ;
	    }
	}

	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}

	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mailmsg_envdate) */

int mailmsg_envremote(MAILMSG *op,int i,cchar **rpp) noex {
	MAILMSG_ENV	*oep = &op->envs ;
	int		rs ;
	int		el = 0 ;
	cchar		*rp = nullptr ;
	void		*vp{} ;
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	if ((rs = vecobj_get(&oep->insts,i,&vp)) >= 0) {
	    if (vp) {
		MAILMSGMATENV	*ep = (MAILMSGMATENV *) vp ;
		el = ep->r.el ;
		rp = ep->r.ep ;
	    }
	}

	if (rpp) {
	    *rpp = (rs >= 0) ? rp : nullptr ;
	}

	return (rs >= 0) ? el : rs ;
}
/* end subroutine (mailmsg_envremote) */

int mailmsg_hdrcount(MAILMSG *op,cchar *name) noex {
	MMHNAME		*hnp ;
	cint		hlen = HDRNAMELEN ;
	int		rs ;
	int		hl = -1 ;
	int		c = 0 ;
	cchar		*hp = name ;
	char		hbuf[HDRNAMELEN + 1] ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	if (hasuc(name,-1)) {
	    hl = strwcpylc(hbuf,name,hlen) - hbuf ;
	    hp = hbuf ;
	}

	if ((rs = mailmsg_hdrmatch(op,&hnp,hp,hl)) >= 0) {
	    if (hnp) {
	        rs = msghdrname_count(hnp) ;
	        c = rs ;
	    }
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailmsg_hdrcount) */

int mailmsg_hdrikey(MAILMSG *op,int hi,cchar **rpp) noex {
	MAILMSG_HDR	*ohp ;
	int		rs ;
	int		nl = 0 ;
	cchar		*namep = nullptr ;
	void		*vp{} ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	ohp = &op->hdrs ;
	if ((rs = vecobj_get(&ohp->names,hi,&vp)) >= 0) {
	    if (vp) {
		MMHNAME		*hnp = (MMHNAME *) vp ;
		nl = hnp->namelen ;
		namep = hnp->name ;
	    }
	}

	if (rpp) {
	    *rpp = (rs >= 0) ? namep : nullptr ;
	}

	return (rs >= 0) ? nl : rs ;
}
/* end subroutine (mailmsg_hdrikey) */

int mailmsg_hdriline(MAILMSG *op,cchar *name,int hi,int li,cchar **rpp) noex {
	MMHNAME		*hnp = nullptr ;
	cint		hlen = HDRNAMELEN ;
	int		rs ;
	int		hl = -1 ;
	int		vl = 0 ;
	cchar		*hp = name ;
	char		hbuf[HDRNAMELEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	if (hasuc(name,-1)) {
	    hl = strwcpylc(hbuf,name,hlen) - hbuf ;
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

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailmsg_hdriline) */

int mailmsg_hdrival(MAILMSG *op,cchar *name,int hi,cchar **rpp) noex {
	MMHNAME	*hnp = nullptr ;
	cint		hlen = HDRNAMELEN ;
	int		rs ;
	int		hl = -1 ;
	int		vl = 0 ;
	cchar		*hp = name ;
	char		hbuf[HDRNAMELEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	if (hasuc(name,-1)) {
	    hl = strwcpylc(hbuf,name,hlen) - hbuf ;
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

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailmsg_hdrival) */

int mailmsg_hdrval(MAILMSG *op,cchar *name,cchar **rpp) noex {
	MMHNAME	*hnp = nullptr ;
	cint		hlen = HDRNAMELEN ;
	int		rs ;
	int		hl = -1 ;
	int		vl = 0 ;
	cchar		*hp = name ;
	char		hbuf[HDRNAMELEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSG_MAGIC) return SR_NOTOPEN ;

	if (hasuc(name,-1)) {
	    hl = strwcpylc(hbuf,name,hlen) - hbuf ;
	    hp = hbuf ;
	}

	if ((rs = mailmsg_hdrmatch(op,&hnp,hp,hl)) >= 0) {
	    if (hnp != nullptr) {
	        rs = msghdrname_val(hnp,rpp) ;
	        vl = rs ;
	    }
	}

	if (rs < 0) {
	    if (rpp) *rpp = nullptr ;
	}

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailmsg_hdrval) */


/* local subroutines */

static int mailmsg_procline(MAILMSG *op,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		vl ;
	int		vi = 0 ;
	cchar		*vp ;

	if (op->msgstate == msgstate_env) {
	    MAILMSGMATENV	es ;

	    if ((rs = mailmsgmatenv(&es,lp,ll)) > 0) {
	        rs = mailmsg_envadd(op,&es) ;
	    } else if (rs == 0) {
	        op->msgstate = msgstate_hdr ;
	    }
		
	} /* end if */

	if ((rs >= 0) && (op->msgstate == msgstate_hdr)) {
	    if ((rs = mailmsgmathdr(lp,ll,&vi)) > 0) {
	        vp = (lp + vi) ;
	        vl = (ll - vi) ;
	        rs = mailmsg_hdraddnew(op,lp,rs,vp,vl) ;
	    } else if ((rs == 0) && (ll > 0) && ISHDRCONT(lp[0])) {
	        rs = mailmsg_hdraddcont(op,(lp+1),(ll-1)) ;
	    }
	} /* end if */

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (mailmsg_procline) */

static int mailmsg_envbegin(MAILMSG *op) noex {
	MAILMSG_ENV	*oep = &op->envs ;
	cint		size = sizeof(MAILMSGMATENV) ;
	int		rs ;

#if	CF_PEDANTIC
	memset(oep,0,sizeof(MAILMSG_ENV)) ;
#endif

	rs = vecobj_start(&oep->insts,size,4,0) ;

	return rs ;
}
/* end subroutine (mailmsg_envbegin) */

static int mailmsg_envend(MAILMSG *op) noex {
	MAILMSG_ENV	*oep = &op->envs ;
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = vecobj_finish(&oep->insts) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (mailmsg_envend) */

static int mailmsg_envadd(MAILMSG *op,MAILMSGMATENV *esp) noex {
	MAILMSG_ENV	*oep = &op->envs ;
	int		rs ;

	rs = vecobj_add(&oep->insts,esp) ;

	return rs ;
}
/* end subroutine (mailmsg_envadd) */

static int mailmsg_hdrbegin(MAILMSG *op) noex {
	MAILMSG_HDR	*ohp = &op->hdrs ;
	cint		size = sizeof(MMHNAME) ;
	int		rs ;

#if	CF_PEDANTIC
	memset(ohp,0,sizeof(MAILMSG_HDR)) ;
#endif

	ohp->lastname = -1 ;
	rs = vecobj_start(&ohp->names,size,10,0) ;

	return rs ;
}
/* end subroutine (mailmsg_hdrbegin) */

static int mailmsg_hdrend(MAILMSG *op) noex {
	MAILMSG_HDR	*ohp = &op->hdrs ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(&ohp->names,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		MMHNAME		*hnp = (MMHNAME *) vp ;
	        rs1 = msghdrname_finish(hnp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	{
	    rs1 = vecobj_finish(&ohp->names) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (mailmsg_hdrend) */

static int mailmsg_hdraddnew(MAILMSG *op,cc *hp,int hl,cc *vp,int vl) noex {
	MAILMSG_HDR	*ohp = &op->hdrs ;
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

	    if ((cl = sfshrink(vp,vl,&cp)) >= 0) {
	        MMHNAME	*hnp ;
	        if ((rs1 = mailmsg_hdrmatch(op,&hnp,hp,hl)) >= 0) {
	            ohp->lastname = rs1 ;
	            rs = msghdrname_addnew(hnp,cp,cl) ;
	        } else {
		    vecobj	*nlp = &ohp->names ;
	            void	*p ;
	            if ((rs = vecobj_addnew(nlp,&p)) >= 0) {
	                MMHNAME	*hnp = (MMHNAME *) p ;
		        cint	i = rs ;
	                if ((rs = msghdrname_start(hnp,hp,hl,cp,cl)) >= 0) {
	                    ohp->lastname = i ;
		        } else {
		            vecobj_del(nlp,i) ;
		        }
	            }
	        } /* end if */
	    } /* end if (shrink) */

	} else {
	    ohp->lastname = -1 ;
	}

	return rs ;
}
/* end subroutine (mailmsg_hdraddnew) */

static int mailmsg_hdraddcont(MAILMSG *op,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	if (vl > 0) {
	    MAILMSG_HDR	*ohp = &op->hdrs ;
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(vp,vl,&cp)) > 0) {
		cint	ln = ohp->lastname ;
		if (ln >= 0) {
		    void	*vp{} ;
		    if ((rs = vecobj_get(&ohp->names,ln,&vp)) >= 0) {
	    	        if (vp) {
		    	    MMHNAME	*hnp = (MMHNAME *) vp ;
	        	    rs = msghdrname_addcont(hnp,cp,cl) ;
	    	        }
		    }
		}
	    } /* end if (positive) */
	} /* end if (positive) */
	return rs ;
}
/* end subroutine (mailmsg_hdraddcont) */

static int mailmsg_hdrmatch(MAILMSG *op,MMHNAME **hnpp,
		cc *hp,int hl) noex {
	MAILMSG_HDR	*ohp = &op->hdrs ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		i{} ;		/* used afterwards */
	int		f = false ;
	void		*vp{} ;
	if (hl < 0) hl = strlen(hp) ;
	*hnpp = nullptr ;
	for (i = 0 ; (rs = vecobj_get(&ohp->names,i,&vp)) >= 0 ; i += 1) {
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

static int msghdrname_start(MMHNAME *hnp,cchar *hp,int hl,
		cchar *vp,int vl) noex {
	vecobj		*ilp = &hnp->insts ;
	int		rs ;
	int		size ;

#if	CF_PEDANTIC
	memset(hnp,0,sizeof(MMHNAME)) ;
#endif

	hnp->vp = nullptr ;
	hnp->vl = 0 ;
	hnp->f_alloc = false ;
	hnp->name = nullptr ;
	hnp->namelen = 0 ;
	hnp->lastinst = -1 ;

	size = sizeof(MMHINST) ;
	if ((rs = vecobj_start(ilp,size,2,0)) >= 0) {
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(hp,hl,&cp)) >= 0) {
	        hnp->name = cp ;
	        hnp->namelen = (rs-1) ;
	        rs = msghdrname_addnew(hnp,vp,vl) ;
		if (rs < 0) {
	    	    uc_free(hnp->name) ;
	    	    hnp->name = nullptr ;
	    	    hnp->namelen = 0 ;
		}
	    } /* end if (m-a) */
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
	void		*vp{} ;
	if ((hnp->vp != nullptr) && hnp->f_alloc) {
	    rs1 = uc_free(hnp->vp) ;
	    if (rs >= 0) rs = rs1 ;
	    hnp->vp = nullptr ;
	    hnp->vl = 0 ;
	    hnp->f_alloc = false ;
	}
	if (hnp->name != nullptr) {
	    rs1 = uc_free(hnp->name) ;
	    if (rs >= 0) rs = rs1 ;
	    hnp->name = nullptr ;
	    hnp->namelen = 0 ;
	}
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
	}
	hnp->vp = nullptr ;
	hnp->f_alloc = false ;
	return rs ;
}
/* end subroutine (msghdrname_finish) */

static int msghdrname_match(MMHNAME *hnp,cchar *hp,int hl) noex {
	int		rs = SR_OK ;
	int		f ;

	if (hp == nullptr) return SR_FAULT ;

	f = (hnp->namelen == hl) ;
	f = f && (hnp->name[0] == hp[0]) ;
	f = f && (strncmp(hnp->name,hp,hl) == 0) ;

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (msghdrname_match) */

static int msghdrname_addnew(MMHNAME *hnp,cchar *vp,int vl) noex {
	vecobj		*ilp = &hnp->insts ;
	int		rs ;
	int		f = false ;
	void		*p ;
	if ((rs = vecobj_addnew(ilp,&p)) >= 0) {
	    MMHINST	*instp = (MMHINST *) p ;
	    cint		i = rs ;
	    if ((rs = msghdrinst_start(instp,vp,vl)) >= 0) {
	        hnp->lastinst = i ;
		f = true ;
	    } else {
		vecobj_del(ilp,i) ;
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (msghdrname_addnew) */

static int msghdrname_addcont(MMHNAME *hnp,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	if (vl > 0) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(vp,vl,&cp)) > 0) {
	        cint	li = hnp->lastinst ;
		if (li >= 0) {
		    void	*vp{} ;
		    if ((rs = vecobj_get(&hnp->insts,li,&vp)) >= 0) {
		        if (vp) {
		    	    MMHINST	*hip = (MMHINST *) vp ;
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
	int		rs ;
	int		vl = 0 ;
	void		*vp{} ;
	if ((rs = vecobj_get(&hnp->insts,hi,&vp)) >= 0) {
	    if (vp) {
		MMHINST	*hip = (MMHINST *) vp ;
	        rs = msghdrinst_ival(hip,li,rpp) ;
	        vl = rs ;
	    }
	}
	return (rs >= 0) ? vl : rs ;
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
	    MMHINST	*hip ;
	    int		size = 1 ;
	    cchar	*hivp = nullptr ;
	    char	*bp ;
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(ilp,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
		    hip = (MMHINST *) vp ;
	            rs = msghdrinst_val(hip,&hivp) ;
	            size += (rs + 2) ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && ((rs = uc_malloc(size,&bp)) >= 0)) {
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
	            *bp = '\0' ;
	            hnp->vl = (bp - hnp->vp) ;
	    } /* end if (m-a) */
	    vl = hnp->vl ;
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
	cint		size = sizeof(MMHVAL) ;
	int		rs ;

#if	CF_PEDANTIC
	memset(hip,0,sizeof(MMHINST)) ;
#endif

	hip->vp = nullptr ;
	hip->vl = 0 ;
	if ((rs = vecobj_start(&hip->vals,size,4,0)) >= 0) {
	    if ((vl > 0) && (vp != nullptr)) {
	        rs = msghdrinst_add(hip,vp,vl) ;
	        if (rs < 0)
		    vecobj_finish(&hip->vals) ;
	    }
	}

	return rs ;
}
/* end subroutine (msghdrinst_start) */

static int msghdrinst_add(MMHINST *hip,cchar *vp,int vl) noex {
	int		rs = SR_OK ;
	if (vl > 0) {
	    vecobj	*vlp = &hip->vals ;
	    void	*p ;
	    if ((rs = vecobj_addnew(vlp,&p)) >= 0) {
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
	void		*vp{} ;
	if ((rs = vecobj_get(&hip->vals,li,&vp)) >= 0) {
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
	    int		size = 1 ;
	    char	*bp{} ;
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(&hip->vals,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
	    	    MMHVAL	*valp = (MMHVAL *) vp ;
	            size += (valp->vl + 1) ;
		}
	    } /* end for */
	    if ((rs = uc_malloc(size,&bp)) >= 0) {
		int	n = 0 ;
		hip->vp = bp ;
		hip->f_alloc = true ;
		for (int i = 0 ; vecobj_get(&hip->vals,i,&vp) >= 0 ; i += 1) {
		    if (vp) {
	    	        MMHVAL	*valp = (MMHVAL *) vp ;
		        if (valp->vl > 0) {
			    if (n++ > 0) *bp++ = ' ' ;
			    bp = strwcpy(bp,valp->vp,valp->vl) ;
			}
		    }
		} /* end for */
		*bp = '\0' ;
		hip->vl = (bp - hip->vp) ;
	    } /* end if (m-a) */
	    vl = hip->vl ;
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
	    rs1 = uc_free(hip->vp) ;
	    if (rs >= 0) rs = rs1 ;
	    hip->vp = nullptr ;
	    hip->vl = 0 ;
	    hip->f_alloc = false ;
	}
	{
	    rs1 = vecobj_finish(&hip->vals) ;
	    if (rs >= 0) rs = rs1 ;
	}
	hip->vp = nullptr ;
	hip->f_alloc = false ;
	return rs ;
}
/* end subroutine (msghdrinst_finish) */


