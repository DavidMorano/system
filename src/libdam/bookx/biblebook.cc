/* biblebook SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* BIBLEBOOK object-load management */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	biblebook

	Description:
	This module implements an interface (a trivial one) that
	allows access to the BIBLEBOOK datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"biblebook.h"
#include	"biblebooks.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEBOOK_MODBNAME	"biblebooks"
#define	BIBLEBOOK_OBJNAME	"biblebooks"

#define	BB		biblebook
#define	BB_FL		biblebook_fl
#define	BB_CA		biblebook_calls
#define	BB_MAG		BIBLEBOOK_MAGIC
#define	BB_LEN		BIBLEBOOK_LEN

#define	tolc(c)		CHAR_TOLC(c)
#define	touc(c)		CHAR_TOUC(c)
#define	tofc(c)		CHAR_TOFC(c)

#define	ISWHT(ch)	CHAR_ISWHITE(ch)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libic::mem ;		/* variable */


/* local typedefs */

extern "C" {
    typedef int	(*soopen_f)	(void *,cchar *,cchar *) noex ;
    typedef int	(*socount_f)	(void *) noex ;
    typedef int	(*somax_f)	(void *) noex ;
    typedef int	(*solook_f)	(void *,char *,int,int) noex ;
    typedef int	(*soget_f)	(void *,int,char *,int) noex ;
    typedef int	(*somatch_f)	(void *,cchar *,int) noex ;
    typedef int	(*sosize_f)	(void *) noex ;
    typedef int	(*soaudit_f)	(void *) noex ;
    typedef int	(*soclose_f)	(void *) noex ;
} /* end extern (C) */


/* external subroutines */


/* local structures */

struct biblebook_calls {
    soopen_f		open ;
    socount_f		count ;
    somax_f		max ;
    solook_f		look ;
    soget_f		get ;
    somatch_f		match ;
    sosize_f		size ;
    soaudit_f		audit ;
    soclose_f		close ;
} ; /* end struct (biblebook_calls) */

typedef biblebook_calls *	callsp ;


/* forward references */

template<typename ... Args>
local int biblebook_ctor(BB *op,Args ... args) noex {
	BIBLEBOOK	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) modload) != np) ylikely {
		biblebook_calls    *callp ;
                if ((callp = new(nt) biblebook_calls) != np) ylikely {
                    op->callp = callp ;
                    rs = SR_OK ;
                } /* end if (new-biblebook_calls) */
                if (rs < 0) {
                    delete op->mlp ;
                    op->mlp = nullptr ;
                } /* end if (error) */
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (biblebook_ctor) */

local int biblebook_dtor(BB *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
            if (op->callp) ylikely {
                biblebook_calls    *callp = callsp(op->callp) ;
                delete callp ;
                op->callp = nullptr ;
            }
	    if (op->mlp) ylikely {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (biblebook_dtor) */

template<typename ... Args>
local inline int biblebook_magic(BB *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BB_MAG) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (biblebook_magic) */

local int	biblebook_objloadbegin	(BB *,cchar *,cchar *) noex ;
local int	biblebook_objloadend	(BB *) noex ;
local int	biblebook_loadcalls	(BB *,vecstr *) noex ;

local int	biblebook_opendb(BB *,cchar *,cchar *) noex ;
local int	biblebook_openlocal(BB *) noex ;

local int	biblebook_matcher(BB *,cchar *,int) noex ;
local int	biblebook_loadnames(BB *) noex ;
local int	biblebook_loadnameslocal(BB *) noex ;
local int	biblebook_loadnamesremote(BB *) noex ;

local bool	isrequired(int) noex ;

local char	*strwcpyspecial(char *,cchar *,int) noex ;


/* external variables */


/* local variables */

enum subs {
	sub_open,
	sub_count,
	sub_max,
	sub_look,
	sub_get,
	sub_match,
	sub_size,
	sub_audit,
	sub_close,
	sub_overlast
} ; /* end enums (subs) */

namespace {
    struct subnamer {
	cchar		*n[sub_overlast + 1] ;
	consteval subnamer() noex {
	    n[sub_open]		= "open" ;
	    n[sub_count]	= "count" ;
	    n[sub_max]		= "max" ;
	    n[sub_look]		= "look" ;
	    n[sub_get]		= "get" ;
	    n[sub_match]	= "match" ;
	    n[sub_size]		= "size" ;
	    n[sub_audit]	= "audit" ;
	    n[sub_close]	= "close" ;
	    n[sub_overlast]	= nullptr ;
	} ; /* end ctor */
    } ; /* end struct (subnamer) */
} /* end namespace */

constexpr cpcchar	booknames[] = {
	"Bible",
	"Genesis",
	"Exodus",
	"Leviticus",
	"Numbers",
	"Deuteronomy",
	"Joshua",
	"Judges",
	"Ruth",
	"I Samuel",
	"II Samuel",
	"I Kings",
	"II Kings",
	"I Chronicles",
	"II Chronicles",
	"Ezra",
	"Nehemiah",
	"Esther",
	"Job",
	"Psalms",
	"Proverbs",
	"Ecclesiastes",
	"Song of Solomon",
	"Isaiah",
	"Jeremiah",
	"Lamentations",
	"Ezekiel",
	"Daniel",
	"Hosea",
	"Joel",
	"Amos",
	"Obadiah",
	"Jonah",
	"Micah",
	"Nahum",
	"Habakkuk",
	"Zephaniah",
	"Haggai",
	"Zechariah",
	"Malachi",
	"Matthew",
	"Mark",
	"Luke",
	"John",
	"Acts",
	"Romans",
	"I Corinthians",
	"II Corinthians",
	"Galatians",
	"Ephesians",
	"Philippians",
	"Colossians",
	"I Thessalonians",
	"II Thessalonians",
	"I Timothy",
	"II Timothy",
	"Titus",
	"Philemon",
	"Hebrews",
	"James",
	"I Peter",
	"II Peter",
	"I John",
	"II John",
	"III John",
	"Jude",
	"Revelation",
	nullptr
} ; /* end array (booknames) */

constexpr subnamer	subname ;
cint			bb_len		= BB_LEN ;


/* exported variables */


/* exported subroutines */

local int biblebook_opens(BB *op,cchar *pr,cchar *dbn) noex {
    	int		rs ;
	if ((rs = biblebook_opendb(op,pr,dbn)) == SR_NOENT) {
	    rs = biblebook_openlocal(op) ;
	}
	return rs ;
} /* end subroutine (biblebook_opens) */

int biblebook_open(BB *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	if ((rs = biblebook_ctor(op,pr,dbn)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0] && dbn[0]) {
		if ((rs = biblebook_opens(op,pr,dbn)) >= 0) {
	    	    op->magval = BIBLEBOOK_MAGIC ;
		}
	    } /* end if (valid) */
	    if (rs < 0) {
		biblebook_dtor(op) ;
	    } /* end if (error) */
	} /* end if (biblebook_ctor) */
	return rs ;
} /* end subroutine (biblebook_open) */

int biblebook_close(BB *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs = biblebook_close(op)) >= 0) {
	if (op->names) {
	    voidp vp = voidp(op->names) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->names = nullptr ;
	}
	if (! op->fl.localdb) {
	    {
	    rs1 = (*op->call.close)(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    }
	    {
	    rs1 = biblebook_objloadend(op) ;
	    if (rs >= 0) rs = rs1 ;
	    }

	} /* end if */
	op->magval = 0 ;
	} /* end if (biblebook_magic) */
	return rs ;
} /* end subroutine (biblebook_close) */

int biblebook_count(BB *op) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEBOOK_MAGIC) return SR_NOTOPEN ;

	if (op->fl.localdb) {
	    rs = nelem(booknames) - 1 ;
	} else {
	    if (op->call.count != nullptr) {
	        rs = (*op->call.count)(op->obj) ;
	    }
	} /* end if */

	return rs ;
} /* end subroutine (biblebook_count) */

int biblebook_max(BB *op) noex {
	int		rs ;
	int		max = 0 ;

	if ((rs = biblebook_count(op)) >= 0) {
	    max = (rs - 1) ;
	}

	return (rs >= 0) ? max : rs ;
} /* end subroutine (biblebook_max) */

int biblebook_audit(BB *op) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEBOOK_MAGIC) return SR_NOTOPEN ;

	if (! op->fl.localdb) {
	    rs = SR_NOSYS ;
	    if (op->call.audit != nullptr) {
	        rs = (*op->call.audit)(op->obj) ;
	    }
	} /* end if */

	return rs ;
} /* end subroutine (biblebook_audit) */

int biblebook_look(BB *op,char *rbuf,int rlen,int bi) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEBOOK_MAGIC) return SR_NOTOPEN ;

	if (bi < 0) return SR_INVALID ;

	if (op->fl.localdb) {
	    cint	n = (nelem(booknames) - 1) ;
	    if (bi < n) {
	        rs = sncpy1(rbuf,rlen,booknames[bi]) ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} else {
	    rs = (*op->call.look)(op->obj,rbuf,rlen,bi) ;
	}

	return rs ;
} /* end subroutine (biblebook_look) */

int biblebook_read(BB *op,char *rbuf,int rlen,int bi) noex {
	return biblebook_look(op,rbuf,rlen,bi) ;
} /* end subroutine (biblebook_read) */

int biblebook_get(BB *op,int bi,char *rbuf,int rlen) noex {
	return biblebook_look(op,rbuf,rlen,bi) ;
} /* end subroutine (biblebook_get) */

int biblebook_match(BB *op,cchar *mbuf,int mlen) noex {
	int		rs = SR_NOSYS ;

	if (op == nullptr) return SR_FAULT ;
	if (mbuf == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEBOOK_MAGIC) return SR_NOTOPEN ;

	if (! op->fl.localdb) {
	    rs = SR_NOSYS ;
	    if (op->call.match != nullptr) {
	        rs = (*op->call.match)(op->obj,mbuf,mlen) ;
	    }
	} 

	if (op->fl.localdb || (rs == SR_NOSYS)) {
	    rs = biblebook_matcher(op,mbuf,mlen) ;
	}

	return rs ;
} /* end subroutine (biblebook_match) */

int biblebook_size(BB *op) noex {
    	int		rs ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magval != BIBLEBOOK_MAGIC) return SR_NOTOPEN ;
	return biblebook_loadnames(op) ;
} /* end subroutine (biblebook_size) */


/* private subroutines */

local int biblebook_opendb(BB *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	cchar		*objname = BIBLEBOOK_OBJNAME ;
        if ((rs = biblebook_objloadbegin(op,pr,objname)) >= 0) ylikely {
            biblebook_calls *callp = callsp(op->callp) ;
            rs = SR_NOSYS ;
            if (cauto co = callp->open ; co) ylikely {
                if ((rs = co(op->obj,pr,dbn)) >= 0) ylikely {
                    op->magval = BQ_MAG ;
                }
            } /* end if (non-null) */
            if (rs < 0) {
                biblebook_objloadend(op) ;
            } /* end if (error) */
        } /* end if (objload-begin) */
	return rs ;
} /* end subroutine (biblebook_opendb) */

local int biblebook_openlocal(BB *op) noex {
    	int		rs = SR_BUGCHECK ;
	if (op) {
	    op->fl.localdb = true ;
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (biblebook_openlocal) */

local int biblebook_objloadbegin(BB *op,cchar *pr,cchar *objn) noex {
	modload		*mlp = op->mlp ;
	cint		vn = sub_overlast ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	if (vecstr syms ; (rs = syms.start(vn,vo)) >= 0) ylikely {
	    if ((rs = syms.addsyms(objn,subname.n)) >= 0) ylikely {
	        if (mainv sv ; (rs = syms.getvec(&sv)) >= 0) ylikely {
	            cchar	*mn = BB_MODBNAME ;
	            cchar	*on = objn ;
	            int		mo = 0 ;
	            mo |= modloadm.libvar ;
	            mo |= modloadm.libprs ;
	            mo |= modloadm.libsdirs ;
	            mo |= modloadm.avail ;
	            if ((rs = modload_open(mlp,pr,mn,on,mo,sv)) >= 0) ylikely {
		        op->fl.modload = true ;
	                if (int mv[2] ; (rs = modload_getmva(mlp,mv,2)) >= 0) {
			    cint	osz = mv[0] ;
	                    op->objsz = mv[0] ;
	                    op->cursz = mv[1] ;
			    if (void *vp ; (rs = mem.mall(osz,&vp)) >= 0) {
	                        op->obj = vp ;
	                        rs = biblebook_loadcalls(op,&syms) ;
	                        if (rs < 0) {
	                            mem.free(op->obj) ;
	                            op->obj = nullptr ;
	                        } /* end if (error) */
	                    } /* end if (memory-allocation) */
	                } /* end if (modload_getmva) */
	                if (rs < 0) {
		            op->fl.modload = false ;
	                    modload_close(mlp) ;
	                } /* end if (error) */
	            } /* end if (modload_open) */
		} /* end if (vecstr_getvec) */
	    } /* end if (vecstr_addsyms) */
	    rs1 = syms.finish ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && op->fl.modload) {
		op->fl.modload = false ;
		modload_close(mlp) ;
	    } /* end if (error) */
	} /* end if (vecstr-syms) */
	return rs ;
} /* end subroutine (biblebook_objloadbegin) */

local int biblebook_objloadend(BB *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = mem.free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	} /* end if (memory-release) */
	if (op->mlp && op->fl.modload) {
	    op->fl.modload = false ;
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return rs ;
} /* end subroutine (biblebook_objloadend) */

local int biblebook_loadcalls(BB *op,vecstr *slp) noex {
	modload		*mlp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*sname{} ;
	for (int i = 0 ; (rs1 = slp->get(i,&sname)) >= 0 ; i += 1) ylikely {
	    if (cvoid *snp{} ; (rs = modload_getsym(mlp,sname,&snp)) >= 0) {
                biblebook_calls   *callp = callsp(op->callp) ;
                c += 1 ;
                switch (i) {
                case sub_open:
                    callp->open		= soopen_f(snp) ;
                    break ;
                case sub_count:
                    callp->count	= socount_f(snp) ;
                    break ;
                case sub_curbegin:
                    callp->curbegin	= socurbegin_f(snp) ;
                    break ;
                case sub_curend:
                    callp->curend	= socurend_f(snp) ;
                    break ;
                case sub_curenum:
                    callp->curenum	= socurenum_f(snp) ;
                    break ;
                case sub_curlook:
                    callp->curlook	= socurlook_f(snp) ;
                    break ;
                case sub_audit:
                    callp->audit	= soaudit_f(snp) ;
                    break ;
                case sub_close:
                    callp->close	= soclose_f(snp) ;
                    break ;
                } /* end switch */
            } else if (rs == rsn) {
                if (! isrequired(i)) rs = SR_OK ;
            } /* end if (it had the call) */
	    if (rs < 0) break ;
	} /* end for (vecstr_get) */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (biblebook_loadcalls) */

#ifdef	COMMENT
local int biblebook_loadcalls(BB *op,cchar *objname) noex {
	modload		*lp = &op->loader ;
	cint		nlen = SYMNAMELEN ;
	int		rs = SR_OK ;
	int		c = 0 ;
	char		nbuf[SYMNAMELEN + 1] ;
	cvoid		*snp ;

	for (int i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(nbuf,nlen,objname,"_",subs[i])) >= 0) {
	         if ((rs = modload_getsym(lp,nbuf,&snp)) == SR_NOTFOUND) {
		     snp = nullptr ;
		     if (! isrequired(i)) rs = SR_OK ;
		}
	    }

	    if (rs < 0) break ;

	    if (snp != nullptr) {
	        c += 1 ;
	        switch (i) {
	        case sub_open:
	            op->call.open = 
	                (int (*)(void *,cchar *,cchar *)) snp ;
	            break ;
	        case sub_count:
	            op->call.count = (int (*)(void *)) snp ;
	            break ;
	        case sub_max:
	            op->call.max = (int (*)(void *)) snp ;
	            break ;
	        case sub_look:
	            op->call.look = (int (*)(void *,char *,int,int)) snp ;
	            break ;
	        case sub_get:
	            op->call.get = (int (*)(void *,int,char *,int)) snp ;
	            break ;
	        case sub_match:
	            op->call.match = (int (*)(void *,cchar *,int)) snp ;
	            break ;
	        case sub_size:
	            op->call.size = (int (*)(void *)) snp ;
	            break ;
	        case sub_audit:
	            op->call.audit = (int (*)(void *)) snp ;
	            break ;
	        case sub_close:
	            op->call.close = (int (*)(void *)) snp ;
	            break ;
	        } /* end switch */
	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (biblebook_loadcalls) */
#endif /* COMMENT */

local int biblebook_matcher(BB *op,cchar *mbuf,int mlen) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* return-value */
	if (op->names == nullptr) {
	    rs = biblebook_loadnames(op) ;
	}
	if (rs >= 0) {
	    int		ml = MIN(mlen,bb_len) ;
	    int		bl ;
	    char	bbuf[bb_len + 1] ;
	    {
	        bl = strwcpyspecial(bbuf,mbuf,ml) - bbuf ;
	    }
	    i = matostr(op->names,1,bbuf,bl) ;
	    if (i < 0) rs = SR_NOTFOUND ;
	} /* end if (ok) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (biblebook_matcher) */

local int biblebook_loadnames(BB *op) noex {
	int		rs = SR_OK ;
	if (op->names == nullptr) {
	    if (op->fl.localdb) {
	        rs = biblebook_loadnameslocal(op) ;
	    } else {
	        rs = biblebook_loadnamesremote(op) ;
	    }
	} else {
	    rs = op->namesize ;
	}
	return rs ;
} /* end subroutine (biblebook_loadnames) */

local int biblebook_loadnameslocal(BB *op) noex {
	int		rs ;
	int		sizetab ;
	int		sz = 0 ;
	int		namesize = 0 ; /* return-value */
	int		n ; /* used-afterwards */
	char		*mp ;
	for (n = 0 ; booknames[n] ; n += 1) {
	    namesize += (lenstr(booknames[n]) + 1) ;
	} /* end for */
	sizetab = (n + 1) * szof(cchar *) ;
	sz += sizetab ;
	sz += namesize ;
	if ((rs = mem.mall(sz,&mp)) >= 0) {
	    int		i ; /* used-afterwards */
	    cchar	**names = (cchar **) mp ;
	    char	*bp = mp ;
	    bp = (mp + sizetab) ;
	    op->namestrs = bp ;
	    for (i = 0 ; i < n ; i += 1) {
	        names[i] = bp ;
	        bp = strwcpyspecial(bp,booknames[i],-1) + 1 ;
	    } /* end for */
	    names[i] = nullptr ;
	    op->names = names ;
	    op->namesize = namesize ;
	} /* end if (memory-acquire) */
	return (rs >= 0) ? namesize : rs ;
} /* end subroutine (biblebook_loadnameslocal) */

local int biblebook_loadnamesremote(BB *op) noex {
	cint		blen = bb_len ;
	int		rs = SR_OK ;
	int		n = 0 ;
	int		namesize = 0 ; /* return-value */
	char		bbuf[bb_len + 1] ;
	if (op->call.size != nullptr) {
	    if ((rs = (*op->call.size)(op->obj)) >= 0) {
		namesize = rs ;
		rs = (*op->call.count)(op->obj) ;
		n = rs ;
	    }
	} else {
	    for (n = 0 ; rs >= 0 ; n += 1) {
	        if ((rs = (*op->call.get)(op->obj,n,bbuf,blen)) >= 0) {
		    namesize += (rs+1) ;
	        } else {
		    if (rs == SR_NOTFOUND) rs = SR_OK ;
		    break ;
	        }
	    } /* end for */
	} /* end if */
	if (rs >= 0) {
	    cint	sizetab = (n + 1) * szof(cchar *) ;
	    int		sz = namesize ;
	    int		bl ;
	    char	*mp ;
	    sz += sizetab ;
	    if ((rs = mem.mall(sz,&mp)) >= 0) {
		int	i ; /* used-afterwards */
		cchar	**names = (cchar **) mp ;
		char	*bp = (mp + sizetab) ;
	        op->namestrs = bp ;
	        for (i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	            if ((rs = (*op->call.get)(op->obj,i,bbuf,blen)) >= 0) {
	                bl = rs ;
	                names[i] = bp ;
	                bp = strwcpyspecial(bp,bbuf,bl) + 1 ;
	            }
	        } /* end for */
	        names[i] = nullptr ;
	        op->namesize = namesize ;
	        if (rs < 0) { /* should not or cannot fail! */
	            mem._free(mp) ;
	        } else {
	            op->names = names ;
		}
	    } /* end if (m-a) */
	} /* end if (ok) */
	return (rs >= 0) ? namesize : rs ;
} /* end subroutine (biblebook_loadnamesremote) */

local bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_count:
	case sub_look:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
} /* end subroutine (isrequired) */

local char *strwcpyspecial(char *dp,cchar *sp,int sl) {
	if (sl >= 0) {
	    while (sl && (*sp != '\0')) {
		if (! CHAR_ISWHITE(*sp)) *dp++ = tolc(*sp) ;
		sp += 1 ;
		sl -= 1 ;
	    }
	} else {
	    while (*sp != '\0') {
		if (! CHAR_ISWHITE(*sp)) *dp++ = tolc(*sp) ;
		sp += 1 ;
	    }
	} /* end if */
	*dp = '\0' ;
	return dp ;
} /* end subroutine (strwcpyspecial) */


