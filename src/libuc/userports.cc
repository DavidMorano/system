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

	Descriptiopn:
	This object reads the USERPORTS DB and provides for queries
	to it.

	Synopsis:

	int userports_query(op,uid,protoname,port)
	USERPORTS	*op ;
	uid_t		uid ;
	cchar	protoname[] ;
	int		port ;

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

static int userports_procfile(USERPORTS *) noex ;
static int userports_procline(USERPORTS *,PWCACHE *,cchar *,int) noex ;
static int userports_procent(USERPORTS *,uid_t,cchar *,int) noex ;
static int userports_procenter(USERPORTS *,uid_t,cchar *,cchar *) noex ;


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


/* exported subroutines */

int userports_open(USERPORTS *op,cchar *fname) noex {
	cint	dents = DEFENTS ;
	cint	dsize = DEFSIZE ;
	cint	vo = 0 ; /* sorting is not needed (now) */
	int		rs ;
	cchar	*cp ;

	if (op == nullptr)
	    return SR_FAULT ;

	if ((fname == nullptr) || (fname[0] == '\0'))
	    fname = USERPORTS_FNAME ;

	memclear(op) ;

	if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	    cint	size = sizeof(ENTRY) ;
	    op->fname = cp ;
	    if ((rs = vecobj_start(&op->ents,size,dents,vo)) >= 0) {
	        if ((rs = vecpstr_start(&op->protos,dents,dsize,0)) >= 0) {
	            if ((rs = vecpstr_start(&op->ports,dents,dsize,0)) >= 0) {
	                if ((rs = userports_procfile(op)) >= 0) {
	                    op->magic = USERPORTS_MAGIC ;
	                } /* end if (procfile) */
		        if (rs < 0) {
		            vecpstr_finish(&op->ports) ;
			}
	            } /* end if (ports) */
		    if (rs < 0) {
		        vecpstr_finish(&op->protos) ;
		    }
	        } /* end if (protos) */
		if (rs < 0) {
		    vecobj_finish(&op->ents) ;
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
/* end subroutine (userports_open) */

int userports_close(USERPORTS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != USERPORTS_MAGIC)
	    return SR_NOTOPEN ;

	rs1 = vecpstr_finish(&op->ports) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecpstr_finish(&op->protos) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&op->ents) ;
	if (rs >= 0) rs = rs1 ;

	if (op->fname != nullptr) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (userports_close) */

int userports_query(USERPORTS *op,uid_t uid,cc *protoname,int port) noex {
	ENTRY		*ep ;
	int		rs = SR_OK ;
	int		protoidx = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != USERPORTS_MAGIC) return SR_NOTOPEN ;

	if (uid < 0) return SR_INVALID ;
	if (port < 0) return SR_INVALID ;

	if ((protoname != nullptr) && (protoname[0] != '\0')) {
	    rs = vecpstr_already(&op->protos,protoname,-1) ;
	    protoidx = rs ;
	}

	if (rs >= 0) {
	    int	i ;
	    int	f ;
	    for (i = 0 ; (rs = vecobj_get(&op->ents,i,&ep)) >= 0 ; i += 1) {
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

int userports_curbegin(USERPORTS *op,USERPORTS_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	curp->i = -1 ;
	return rs ;
}
/* end subroutine (userports_curbegin) */

int userports_curend(USERPORTS *op,USERPORTS_CUR *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	curp->i = -1 ;
	return rs ;
}
/* end subroutine (userports_curend) */

int userports_enum(USERPORTS *op,USERPORTS_CUR *curp,
		USERPORTS_ENT *entp) noex {
	ENTRY		*ep ;
	int		rs = SR_OK ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (entp == nullptr) return SR_FAULT ;

	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;

	while ((rs = vecobj_get(&op->ents,i,&ep)) >= 0) {
	    if (ep != nullptr) break ;
	    i += 1 ;
	} /* end while */

	if (rs >= 0) {
	    cchar	*cp ;
	    entp->uid = ep->uid ;
	    if (rs >= 0) {
	        rs = vecpstr_get(&op->protos,ep->protoidx,&cp) ;
	        entp->protocol = cp ;
	    }
	    if (rs >= 0) {
	        rs = vecpstr_get(&op->ports,ep->portidx,&cp) ;
	        entp->portname = cp ;
	    }
	    curp->i = i ;
	} /* end if (found entry) */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (userports_enum) */


int userports_fetch(USERPORTS *op,USERPORTS_CUR *curp,uid_t uid,
		USERPORTS_ENT *entp) noex {
	ENTRY		*ep ;
	int		rs = SR_OK ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (entp == nullptr) return SR_FAULT ;

	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;

	while ((rs = vecobj_get(&op->ents,i,&ep)) >= 0) {
	    if (ep != nullptr) {
		if (ep->uid == uid) break ;
	    }
	    i += 1 ;
	} /* end while */

	if (rs >= 0) {
	    cchar	*cp ;
	    entp->uid = ep->uid ;
	    if (rs >= 0) {
	        rs = vecpstr_get(&op->protos,ep->protoidx,&cp) ;
	        entp->protocol = cp ;
	    }
	    if (rs >= 0) {
	        rs = vecpstr_get(&op->ports,ep->portidx,&cp) ;
	        entp->portname = cp ;
	    }
	    curp->i = i ;
	} /* end if (found entry) */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (userports_fetch) */


/* private subroutines */

static int userports_procfile(USERPORTS *op) noex {
	PWCACHE		pwc ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = pwcache_start(&pwc,MAXPWENT,MAXPWTTL)) >= 0) {
	    USTAT	sb ;
	    FILEMAP	fm, *fmp = &fm ;
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

static int userports_procline(USERPORTS *op,pwentpw *pwcp,cc *lp,int ll) noex {
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

static int userports_procent(USERPORTS *op,uid_t uid,cc *fp,int fl) noex {
	nulstr		portstr ;
	int		rs ;
	int		cl = 0 ;
	int		c = 0 ;
	cchar	*tp, *cp ;
	cchar	*ps ;

	cp = nullptr ;
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

	            rs = userports_procenter(op,uid,pn,ps) ;
	            if (rs > 0) c += 1 ;

	            nulstr_finish(&protostr) ;
	        } /* end if (nulstr) */

	    } else {
	        int	i ;

	        for (i = 0 ; defprotos[i] != nullptr ; i += 1) {
	            pn = defprotos[i] ;

	            rs = userports_procenter(op,uid,pn,ps) ;
	            if (rs > 0) c += 1 ;

	            if (rs < 0) break ;
	        } /* end for */

	    } /* end if */

	    nulstr_finish(&portstr) ;
	} /* end if (nulstr) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (userports_procent) */

static int userports_procenter(USERPORTS *op,uid_t uid,cc *pn,cc *ps) noex {
	ENTRY		e{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;

	e.uid = uid ;

	if ((rs1 = getportnum(pn,ps)) >= 0) {
	    e.port = rs1 ;
	    f = true ;

	    if (rs >= 0) {
		vecpstr	*plp = &op->protos ;
	        rs = vecpstr_adduniq(plp,pn,-1) ;
		if (rs == INT_MAX) rs = vecpstr_find(plp,pn) ;
	        e.protoidx = rs ;
	    }

	    if (rs >= 0) {
		vecpstr	*plp = &op->ports ;
	        rs = vecpstr_adduniq(plp,ps,-1) ;
		if (rs == INT_MAX) rs = vecpstr_find(plp,ps) ;
	        e.portidx = rs ;
	    }

	    if (rs >= 0)
	        rs = vecobj_add(&op->ents,&e) ;

	} else if (rs1 != SR_NOTFOUND) {
	    rs = rs1 ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (userports_procenter) */


