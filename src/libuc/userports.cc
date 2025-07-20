/* userports SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* query the USERPORTS database for entries */
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
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<netdb.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<getportnum.h>
#include	<mallocxx.h>
#include	<pwcache.h>
#include	<filemap.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<nulstr.h>
#include	<strn.h>
#include	<strdcpyx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"userports.h"

import libutil ;

/* local defines */

#define	UP		userports
#define	UP_ENT		userports_ent
#define	UP_CUR		userports_cur

#define	ENTRY		entry_elem

#define	DEFENTS		20
#define	DEFSIZE		512

#define	MAXFSIZE	(4 * 1024 * 1024)
#define	MAXPWENT	20
#define	MAXPWTTL	(200*60*60)

#ifndef	SVCNAMELEN
#define	SVCNAMELEN	32
#endif

#ifndef	PROTONAME
#define	PROTONAME	"tcp"
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct entry_elem {
	uid_t		uid ;
	int		port ;
	int		protoidx ;
	int		portidx ;
} ;

typedef	ENTRY		entry ;
typedef	ENTRY *		entryp ;

namespace {
    struct vars {
	int		usernamelen ;
	operator int () noex ;
    } ;
}


/* forward references */

template<typename ... Args>
static int userports_ctor(userports *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->elp = new(nothrow) vecobj) != np) ylikely {
	        if ((op->plp = new(nothrow) vecpstr) != np) ylikely {
	            if ((op->olp = new(nothrow) vecpstr) != np) ylikely {
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
	if (op) ylikely {
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
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == USERPORTS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (userports_magic) */

static int userports_opener(UP *,cchar *) noex ;
static int userports_procfile(UP *) noex ;
static int userports_procline(UP *,pwcache *,cchar *,int) noex ;
static int userports_procent(UP *,uid_t,cchar *,int) noex ;
static int userports_procenter(UP *,uid_t,cchar *,cchar *) noex ;


/* local variables */

constexpr fieldterminit		ft("\t #,") ;

constexpr cpcchar		defprotos[] = {
	"tcp",
	"udp",
	"ddp",
	nullptr
} ;

static vars			var ;


/* exported variables */


/* exported subroutines */

int userports_open(UP *op,cchar *fname) noex {
	int		rs ;
	if ((fname == nullptr) || (fname[0] == '\0')) {
	    fname = USERPORTS_FNAME ;
	}
	if ((rs = userports_ctor(op)) >= 0) ylikely {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) ylikely {
		rs = userports_opener(op,fname) ;
	    }
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
	if ((rs = userports_magic(op)) >= 0) ylikely {
	    if (op->olp) {
	        rs1 = vecpstr_finish(op->olp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->plp) {
	        rs1 = vecpstr_finish(op->plp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->elp) {
	        rs1 = vecobj_finish(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    {
		rs1 = userports_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (userports_close) */

int userports_query(UP *op,uid_t uid,cc *protoname,int port) noex {
	int		rs ;
	if ((rs = userports_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (port >= 0) ylikely {
	        int	protoidx = 0 ;
		rs = SR_OK ;
	        if ((protoname != nullptr) && (protoname[0] != '\0')) {
	            rs = vecpstr_already(op->plp,protoname,-1) ;
	            protoidx = rs ;
	        }
	        if (rs >= 0) {
		    vecobj	*elp = op->elp ;
		    auto	vg = vecobj_get ;
		    void	*vp{} ;
	            for (int i = 0 ; (rs = vg(elp,i,&vp)) >= 0 ; i += 1) {
	                if (vp) {
	        	    entry	*ep = entryp(vp) ;
		            bool	f = true ;
		            f = f && (uid == ep->uid) ;
		            f = f && (port == ep->port) ;
		            if (f && (protoidx > 0)) {
		                f = (protoidx == ep->protoidx) ;
		            }
		            if (f) break ;
	                } /* end if (non-null) */
	            } /* end for */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (userports_query) */

int userports_curbegin(UP *op,UP_CUR *curp) noex {
	int		rs ;
	if ((rs = userports_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	}
	return rs ;
}
/* end subroutine (userports_curbegin) */

int userports_curend(UP *op,UP_CUR *curp) noex {
	int		rs ;
	if ((rs = userports_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	}
	return rs ;
}
/* end subroutine (userports_curend) */

int userports_curenum(UP *op,UP_CUR *curp,UP_ENT *entp) noex {
	int		rs = SR_OK ;
	int		i ; /* used-afterwards */
	if ((rs = userports_magic(op,curp,entp)) >= 0) ylikely {
	    void	*vp{} ;
	    i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    while ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        if (vp) break ;
	        i += 1 ;
	    } /* end while */
	    if ((rs >= 0) && vp) {
	        entry	*ep = entryp(vp) ;
	        cchar	*cp{} ;
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
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (userports_curenum) */

int userports_fetch(UP *op,UP_CUR *curp,uid_t uid,UP_ENT *entp) noex {
	int		rs ;
	int		i = 0 ; /* used-afterwards */
	if ((rs = userports_magic(op,curp,entp)) >= 0) ylikely {
	    void	*vp{} ;
	    i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    while ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        if (vp) {
		    entry	*ep = entryp(vp) ;
		    if (ep->uid == uid) break ;
	        }
	        i += 1 ;
	    } /* end while */
	    if ((rs >= 0) && vp) {
	        entry	*ep = entryp(vp) ;
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
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (userports_fetch) */


/* private subroutines */

static int userports_opener(UP *op,cchar *fname) noex {
	cint		dents = DEFENTS ;
	cint		dsize = DEFSIZE ;
	cint		vo = 0 ; /* sorting is not needed (now) */
	int		rs ;
	if (cchar *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) ylikely {
	    cint	sz = szof(entry) ;
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
	return rs ;
}
/* end subroutine (userports_opener) */

static int userports_procfile(UP *op) noex {
	pwcache		pwc ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = pwcache_start(&pwc,MAXPWENT,MAXPWTTL)) >= 0) ylikely {
	    filemap	fm, *fmp = &fm ;
	    csize	fsize = MAXFSIZE ;
	    int		ll ;
	    cchar	*lp ;
	    if ((rs = filemap_open(fmp,op->fname,fsize)) >= 0) ylikely {
	        if (ustat sb ; (rs = filemap_stat(fmp,&sb)) >= 0) ylikely {
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
		} /* end if (_stat) */
	        rs1 = filemap_close(fmp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (filemap) */
	    rs1 = pwcache_finish(&pwc) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pwcache) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (userports_procfile) */

static int userports_procline(UP *op,pwcache *pwcp,cc *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) ylikely {
	    cint	pwlen = rs ;
	    if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) ylikely {
	        cchar	*fp ;
	        if (int fl ; (fl = fsb.get(ft.terms,&fp)) > 0) ylikely {
		    ucentpw	pw ;
		    if (char *ubuf ; (rs = malloc_un(&ubuf)) >= 0) ylikely {
			cint	ulen = rs ;
			auto	pwl = pwcache_lookup ;
	                strdcpy1w(ubuf,ulen,fp,fl) ;
	                if ((rs = pwl(pwcp,&pw,pwbuf,pwlen,ubuf)) >= 0) {
	                    const uid_t		uid = pw.pw_uid ;
		            while ((rs >= 0) && (fsb.term != '#')) {
	                        fl = fsb.get(ft.terms,&fp) ;
	                        if (fl == 0) continue ;
			        if (fl < 0) break ;
	                        rs = userports_procent(op,uid,fp,fl) ;
	                        if (rs > 0) c += rs ;
	                    } /* end if (ports) */
	                } else if (isNotPresent(rs)) {
	                    rs = SR_OK ;
		        }
			rs1 = uc_free(ubuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if */
	        rs1 = fsb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    rs1 = uc_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (userports_procline) */

static int userports_procent(UP *op,uid_t uid,cc *fp,int fl) noex {
	int		rs ;
	int		rs1 ;
	int		cl = 0 ;
	int		c = 0 ;
	cchar		*cp = nullptr ;
	cchar		*ps ;
	if (cchar *tp ; (tp = strnchr(fp,fl,':')) != nullptr) {
	    cp = fp ;
	    cl = intconv(tp - fp) ;
	    fl = intconv((fp + fl) - (tp + 1)) ;
	    fp = (tp + 1) ;
	}
	if (nulstr portstr ; (rs = nulstr_start(&portstr,fp,fl,&ps)) >= 0) {
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
	int		rs ;
	int		f = false ;
	if ((rs = getportnum(pn,ps)) >= 0) ylikely {
	    entry	e{} ;
	    e.port = rs ;
	    e.uid = uid ;
	    f = true ;
	    if (rs >= 0) {
		vecpstr		*plp = op->plp ; /* protocol-name */
	        rs = vecpstr_adduniq(plp,pn,-1) ;
		if (rs == INT_MAX) rs = vecpstr_find(plp,pn) ;
	        e.protoidx = rs ;
	    }
	    if (rs >= 0) ylikely {
		vecpstr		*olp = op->olp ; /* port-specification */
	        rs = vecpstr_adduniq(olp,ps,-1) ;
		if (rs == INT_MAX) rs = vecpstr_find(olp,ps) ;
	        e.portidx = rs ;
	    }
	    if (rs >= 0) ylikely {
	        rs = vecobj_add(op->elp,&e) ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (userports_procenter) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) ylikely {
	    var.usernamelen = rs ;
	}
	return rs ;
}
/* end method (vars::operator) */


