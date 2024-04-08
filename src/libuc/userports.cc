/* userports SUPPORT */
/* lang=C++20 */

/* query the USERPOTS database for entries */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	userports

	Descriptiopn:
	This object reads the USERPORTS DB and provides for queries
	to it.

	Synopsis:

	int userports_query(UP *op,uid_t uid,cc *protoname,int port) noex

	Arguments:
	op		object pointer
	uid		UID to check under
	protoname	protocol name: if nullptr check all protocols
	port		port number to check

	Returns:
	>=0		query found
	<0		error, likely protocol not found (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<pwcache.h>
#include	<filemap.h>
#include	<field.h>
#include	<getax.h>
#include	<nulstr.h>
#include	<strn.h>
#include	<strdcpyx.h>
#include	<localmisc.h>

#include	"userports.h"


/* local defines */

#define	UP		userports

#undef	ENTRY
#define	ENTRY		struct entry_elem

#define	DEFENTS		20
#define	DEFSIZE		512

#define	MAXFSIZE	(4 * 1024 * 1024)
#define	MAXPWENT	20
#define	MAXPWTTL	(200*60*60)

#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	PROTONAME
#define	PROTONAME	"tcp"
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	getportnum(cchar *,cchar *) noex ;
}


/* external variables */


/* local structures */

struct entry_elem {
	uid_t		uid ;
	int		port ;
	int		protoidx ;
	int		portidx ;
} ;


/* forward references */

template<typename ... Args>
static int userports_ctor(userports *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->elp = new(nothrow) vecobj) != np) {
	        if ((op->plp = new(nothrow) vecpstr) != np) {
	            if ((op->olp = new(nothrow) vecpstr) != np) {
			rs = SR_OK ;
		    } /* end if (new-vecpstr) */
		    if (rs < 0) {
		        delete op->plp ;
		        op->plp = nullptr ;
		    }
		} /* end if (new-vecpstr) */
		if (rs < 0) {
		    delete op->elp ;
		    op->elp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (userports_ctor) */

static int userports_dtor(userports *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->olp) {
		delete op->olp ;
		op->olp = nullptr ;
	    }
	    if (op->plp) {
		delete op->plp ;
		op->plp = nullptr ;
	    }
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (userports_dtor) */

template<typename ... Args>
static int userports_magic(userports *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == USERPORTS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (userports_magic) */

static int userports_procfile(UP *) noex ;
static int userports_procline(UP *,pwcache *,cchar *,int) noex ;
static int userports_procent(UP *,uid_t,cchar *,int) noex ;
static int userports_procenter(UP *,uid_t,cchar *,cchar *) noex ;


/* local variables */

static constexpr cchar		fterms[] = {
	0x00, 0x02, 0x00, 0x00,
	0x09, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

static constexpr cpcchar	defprotos[] = {
	"tcp",
	"udp",
	"ddp",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int userports_open(UP *op,cchar *fname) noex {
	int		rs ;
	if ((fname == nullptr) || (fname[0] == '\0')) {
	    fname = USERPORTS_FNAME ;
	}
	if ((rs = userports_ctor(op)) >= 0) {
	cint		dents = DEFENTS ;
	cint		dsize = DEFSIZE ;
	cint		vo = 0 ; /* sorting is not needed (now) */
	cchar		*cp ;
	if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	    cint	sz = sizeof(ENTRY) ;
	    op->fname = cp ;
	    if ((rs = vecobj_start(op->elp,sz,dents,vo)) >= 0) {
	        if ((rs = vecpstr_start(op->plp,dents,dsize,0)) >= 0) {
	            if ((rs = vecpstr_start(op->olp,dents,dsize,0)) >= 0) {
	                if ((rs = userports_procfile(op)) >= 0) {
	                    op->magic = USERPORTS_MAGIC ;
	                } /* end if (procfile) */
		        if (rs < 0) {
		            vecpstr_finish(op->olp) ;
			}
	            } /* end if (ports) */
		    if (rs < 0) {
		        vecpstr_finish(op->plp) ;
		    }
	        } /* end if (protos) */
		if (rs < 0) {
		    vecobj_finish(op->elp) ;
		}
	    } /* end if (ents) */
	    if (rs < 0) {
		if (op->fname != nullptr) {
		    uc_free(op->fname) ;
		    op->fname = nullptr ;
		}
	    }
	} /* end if (memory-allocation) */
	    if (rs < 0) {
		userports_dtor(op) ;
	    }
	} /* end if (userports_ctor) */
	return rs ;
}
/* end subroutine (userports_open) */

int userports_close(UP *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = userports_magic(op)) >= 0) {
	    {
	        rs1 = vecpstr_finish(op->olp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecpstr_finish(op->plp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (userports_close) */

int userports_query(UP *op,uid_t uid,cc *protoname,int port) noex {
	ENTRY		*ep ;
	int		rs = SR_OK ;
	int		protoidx = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != USERPORTS_MAGIC) return SR_NOTOPEN ;

	if (uid < 0) return SR_INVALID ;
	if (port < 0) return SR_INVALID ;

	if ((protoname != nullptr) && (protoname[0] != '\0')) {
	    rs = vecpstr_already(op->plp,protoname,-1) ;
	    protoidx = rs ;
	}

	if (rs >= 0) {
	    int	i ;
	    int	f ;
	    for (i = 0 ; (rs = vecobj_get(op->elp,i,&ep)) >= 0 ; i += 1) {
	        if (ep) {
		    f = (uid == ep->uid) ;
		    f = f && (port == ep->port) ;
		    if (f && (protoidx > 0)) {
		        f = (protoidx == ep->protoidx) ;
		    }
		    if (f) break ;
	        }
	    } /* end for */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (userports_query) */

int userports_curbegin(UP *op,USERPORTS_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	curp->i = -1 ;
	return rs ;
}
/* end subroutine (userports_curbegin) */

int userports_curend(UP *op,USERPORTS_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	curp->i = -1 ;
	return rs ;
}
/* end subroutine (userports_curend) */

int userports_enum(UP *op,USERPORTS_CUR *curp,
		USERPORTS_ENT *entp) noex {
	ENTRY		*ep ;
	int		rs = SR_OK ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (entp == nullptr) return SR_FAULT ;

	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;

	while ((rs = vecobj_get(op->elp,i,&ep)) >= 0) {
	    if (ep != nullptr) break ;
	    i += 1 ;
	} /* end while */

	if (rs >= 0) {
	    cchar	*cp ;
	    entp->uid = ep->uid ;
	    if (rs >= 0) {
	        rs = vecpstr_get(op->plp,ep->protoidx,&cp) ;
	        entp->protocol = cp ;
	    }
	    if (rs >= 0) {
	        rs = vecpstr_get(op->olp,ep->portidx,&cp) ;
	        entp->portname = cp ;
	    }
	    curp->i = i ;
	} /* end if (found entry) */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (userports_enum) */

int userports_fetch(UP *op,USERPORTS_CUR *curp,uid_t uid,
		USERPORTS_ENT *entp) noex {
	ENTRY		*ep ;
	int		rs = SR_OK ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (entp == nullptr) return SR_FAULT ;

	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;

	while ((rs = vecobj_get(op->elp,i,&ep)) >= 0) {
	    if (ep != nullptr) {
		if (ep->uid == uid) break ;
	    }
	    i += 1 ;
	} /* end while */

	if (rs >= 0) {
	    cchar	*cp ;
	    entp->uid = ep->uid ;
	    if (rs >= 0) {
	        rs = vecpstr_get(op->plp,ep->protoidx,&cp) ;
	        entp->protocol = cp ;
	    }
	    if (rs >= 0) {
	        rs = vecpstr_get(op->olp,ep->portidx,&cp) ;
	        entp->portname = cp ;
	    }
	    curp->i = i ;
	} /* end if (found entry) */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (userports_fetch) */


/* private subroutines */

static int userports_procfile(UP *op) noex {
	pwcache		pwc ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = pwcache_start(&pwc,MAXPWENT,MAXPWTTL)) >= 0) {
	    USTAT	sb ;
	    filemap	fm, *fmp = &fm ;
	    csize	fsize = MAXFSIZE ;
	    int		ll ;
	    cchar	*lp ;
	    if ((rs = filemap_open(fmp,op->fname,fsize)) >= 0) {

	        rs = filemap_stat(fmp,&sb) ;
	        op->fi.mtime = sb.st_mtime ;
	        op->fi.dev = sb.st_dev ;
	        op->fi.ino = sb.st_ino ;

	        while (rs >= 0) {
	            rs = filemap_getln(fmp,&lp) ;
	            ll = rs ;
	            if (rs <= 0) break ;

		    if (lp[ll-1] == '\n') ll -= 1 ;

		    if (ll > 0) {
	                rs = userports_procline(op,&pwc,lp,ll) ;
	                if (rs > 0) c += rs ;
		    }

	        } /* end if (reading lines) */

	        filemap_close(fmp) ;
	    } /* end if (filemap) */

	    rs1 = pwcache_finish(&pwc) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pwcache) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (userports_procfile) */

static int userports_procline(UP *op,pwentpw *pwcp,cc *lp,int ll) noex {
	field		fsb ;
	cint	pwlen = getbufsize(getbufsize_pw) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*pwbuf ;

	if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	    int		fl ;
	    cchar	*fp ;

	    if ((fl = field_get(&fsb,fterms,&fp)) > 0) {
		ucentpw		pw ;
		char		un[USERNAMELEN+1] ;

	        strdcpy1w(un,USERNAMELEN,fp,fl) ;

	        if ((rs1 = pwcache_lookup(pwcp,&pw,pwbuf,pwlen,un)) >= 0) {
	            uid_t	uid = pw.pw_uid ;

		    while ((rs >= 0) && (fsb.term != '#')) {
	                fl = field_get(&fsb,fterms,&fp) ;
	                if (fl == 0) continue ;
			if (fl < 0) break ;

	                rs = userports_procent(op,uid,fp,fl) ;
	                if (rs > 0) c += rs ;

	            } /* end if (ports) */

	        } else if (rs != SR_NOTFOUND) {
	            rs = rs1 ;
		}

	    } /* end if */

	    field_finish(&fsb) ;
	} /* end if (field) */
	    uc_free(pwbuf) ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (userports_procline) */

static int userports_procent(UP *op,uid_t uid,cc *fp,int fl) noex {
	nulstr		portstr ;
	int		rs ;
	int		rs1 ;
	int		cl = 0 ;
	int		c = 0 ;
	cchar		*tp ;
	cchar		*cp = nullptr ;
	cchar		*ps ;
	if ((tp = strnchr(fp,fl,':')) != nullptr) {
	    cp = fp ;
	    cl = (tp-fp) ;
	    fl = ((fp+fl)-(tp+1)) ;
	    fp = (tp+1) ;
	}
	if ((rs = nulstr_start(&portstr,fp,fl,&ps)) >= 0) {
	    cchar	*pn ;
	    if ((cp != nullptr) && (cl > 0)) {
	        nulstr	protostr ;
	        if ((rs = nulstr_start(&protostr,cp,cl,&pn)) >= 0) {
		    {
	                rs = userports_procenter(op,uid,pn,ps) ;
			c += rs ;
		    }
	            rs1 = nulstr_finish(&protostr) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (nulstr) */
	    } else {
	        for (int i = 0 ; defprotos[i] != nullptr ; i += 1) {
		    {
	                pn = defprotos[i] ;
	                rs = userports_procenter(op,uid,pn,ps) ;
	                c += rs ;
		    }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if */
	    rs1 = nulstr_finish(&portstr) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (userports_procent) */

static int userports_procenter(UP *op,uid_t uid,cc *pn,cc *ps) noex {
	ENTRY		e{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;

	e.uid = uid ;

	if ((rs1 = getportnum(pn,ps)) >= 0) {
	    e.port = rs1 ;
	    f = true ;

	    if (rs >= 0) {
		vecpstr	*plp = op->plp ;
	        rs = vecpstr_adduniq(plp,pn,-1) ;
		if (rs == INT_MAX) rs = vecpstr_find(plp,pn) ;
	        e.protoidx = rs ;
	    }

	    if (rs >= 0) {
		vecpstr	*plp = op->olp ;
	        rs = vecpstr_adduniq(plp,ps,-1) ;
		if (rs == INT_MAX) rs = vecpstr_find(plp,ps) ;
	        e.portidx = rs ;
	    }

	    if (rs >= 0) {
	        rs = vecobj_add(op->elp,&e) ;
	    }

	} else if (rs1 != SR_NOTFOUND) {
	    rs = rs1 ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (userports_procenter) */


