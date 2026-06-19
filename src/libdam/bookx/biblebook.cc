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

	Notes:
	1. What am I allocating space to hold the name-strings and
	a separate array for pointers to said strings?  Because I am
	compacting and lower-casing those strings.

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
#include	<sncpyx.h>		/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

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
#define	BB_MOD		BIBLEBOOK_MODBNAME

#define	tolc(c)		CHAR_TOLC(c)
#define	touc(c)		CHAR_TOUC(c)
#define	tofc(c)		CHAR_TOFC(c)

#define	ISWHT(ch)	CHAR_ISWHITE(ch)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libuc::mem ;		/* variable */


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

local int	biblebook_opendb	(BB *,cchar *,cchar *) noex ;
local int	biblebook_openlocal	(BB *) noex ;

local int	biblebook_matcher	(BB *,cchar *,int) noex ;
local int	biblebook_loadnames	(BB *) noex ;
local int	biblebook_loadnamesloc	(BB *) noex ;
local int	biblebook_loadnamesrem	(BB *) noex ;

local bool	isrequired(int) noex ;

local char	*strwcpynon(char *,cchar *,int) noex ;


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
} ; /* end enum (subs) */

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
cbool			f_debug		= CF_DEBUG ;


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
	if ((rs = biblebook_magic(op)) >= 0) ylikely {
            rs = SR_BUGCHECK ;
            if (biblebook_calls *callp = callsp(op->callp) ; callp) ylikely {
		rs = SR_OK ;
	        if (op->names) {
	            voidp vp = voidp(op->names) ;
	            rs1 = mem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            op->names = nullptr ;
	        } /* end if (memory-release) */
	        if (! op->fl.localdb) {
                    if (cauto co = callp->close ; co) ylikely {
                        rs1 = co(op->obj) ;
                        if (rs >= 0) rs = rs1 ;
                    } else {
                        rs = SR_NOSYS ;
                    }
	            {
	                rs1 = biblebook_objloadend(op) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end if (not local DB) */
		{
		    rs1 = biblebook_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->magval = 0 ;
	    } /* end if (valid) */
	} /* end if (biblebook_magic) */
	return rs ;
} /* end subroutine (biblebook_close) */

int biblebook_count(BB *op) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
        if ((rs = biblebook_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
            if (biblebook_calls *callp = callsp(op->callp) ; callp) ylikely {
		if (op->fl.localdb) {
		    rs = SR_OK ;
	            c = (nelem(booknames) - 1) ;
		} else {
		    rs = SR_NOSYS ;
                    if (cauto co = callp->count ; co) ylikely {
                        rs = co(op->obj) ;
			c = rs ;
                    }
		} /* end if (local or remote) */
	    } /* end if (valid) */
        } /* end if (biblebook_magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (biblebook_count) */

int biblebook_max(BB *op) noex {
	int		rs ;
	int		max = 0 ;
	if ((rs = biblebook_count(op)) > 0) ylikely {
	    max = (rs - 1) ;
	}
	return (rs >= 0) ? max : rs ;
} /* end subroutine (biblebook_max) */

int biblebook_look(BB *op,char *rbuf,int rlen,int bi) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
        if ((rs = biblebook_magic(op,rbuf)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
            if (biblebook_calls *callp = callsp(op->callp) ; callp) ylikely {
	        rs = SR_INVALID ;
	        if (bi >= 0) {
	            rs = SR_OK ;
	            if (op->fl.localdb) {
			rs = SR_NOTFOUND ;
	                if (bi < (nelem(booknames) - 1)) {
	                    rs = sncpy1(rbuf,rlen,booknames[bi]) ;
			    len = rs ;
	                }
	            } else {
		        rs = SR_NOSYS ;
                        if (cauto co = callp->look ; co) ylikely {
                            rs = co(op->obj,rbuf,rlen,bi) ;
			    len = rs ;
                        }
		    } /* end if */
	        } /* end if (valid) */
	    } /* end if (bug-check) */
        } /* end if (biblebook_magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (biblebook_look) */

int biblebook_read(BB *op,char *rbuf,int rlen,int bi) noex {
	return biblebook_look(op,rbuf,rlen,bi) ;
} /* end subroutine (biblebook_read) */

int biblebook_get(BB *op,int bi,char *rbuf,int rlen) noex {
	return biblebook_look(op,rbuf,rlen,bi) ;
} /* end subroutine (biblebook_get) */

int biblebook_match(BB *op,cchar *mbuf,int mlen) noex {
	int		rs ;
        if ((rs = biblebook_magic(op,mbuf)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
            if (biblebook_calls *callp = callsp(op->callp) ; callp) {
	    rs = SR_OK ;
	    if (! op->fl.localdb) {
	        rs = SR_NOSYS ;
                if (cauto co = callp->match ; co) ylikely {
                    rs = co(op->obj,mbuf,mlen) ;
	        }
	    } /* end if (local) */
	    if (op->fl.localdb || (rs == SR_NOSYS)) {
	        rs = biblebook_matcher(op,mbuf,mlen) ;
	    }
	    } /* end if (valid) */
        } /* end if (biblebook_magic) */
	return rs ;
} /* end subroutine (biblebook_match) */

int biblebook_size(BB *op) noex {
    	int		rs ;
        if ((rs = biblebook_magic(op)) >= 0) ylikely {
	    rs = biblebook_loadnames(op) ;
	} /* end if (biblebook_magic) */
	return rs ;
} /* end subroutine (biblebook_size) */

int biblebook_audit(BB *op) noex {
	int		rs ;
        if ((rs = biblebook_magic(op)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
            if (biblebook_calls *callp = callsp(op->callp) ; callp) {
                rs = SR_OK ; /* <- OK if no |audit| method */
                if (cauto co = callp->audit ; co) ylikely {
                    rs = co(op->obj) ;
                }
	    } /* end if (valid) */
        } /* end if (biblebook_magic) */
	return rs ;
} /* end subroutine (biblebook_audit) */


/* private subroutines */

local int biblebook_opendb(BB *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	cchar		*objname = BIBLEBOOK_OBJNAME ;
        if ((rs = biblebook_objloadbegin(op,pr,objname)) >= 0) ylikely {
	    rs = SR_BUGCHECK ;
            if (biblebook_calls *callp = callsp(op->callp) ; callp) ylikely {
                rs = SR_NOSYS ;
                if (cauto co = callp->open ; co) ylikely {
                    if ((rs = co(op->obj,pr,dbn)) >= 0) ylikely {
                        op->magval = BB_MAG ;
                    }
                } /* end if (non-null) */
	    } /* end if (valid) */
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
	            cchar	*mn = BB_MOD ;
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
	                    } /* end if (memory-acquire) */
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
                case sub_max:
                    callp->max		= somax_f(snp) ;
                    break ;
                case sub_look:
                    callp->look		= solook_f(snp) ;
                    break ;
                case sub_get:
                    callp->get		= soget_f(snp) ;
                    break ;
                case sub_match:
                    callp->match	= somatch_f(snp) ;
                    break ;
                case sub_size:
                    callp->size		= sosize_f(snp) ;
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

local int biblebook_matcher(BB *op,cchar *mbuf,int mlen) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = biblebook_loadnames(op)) >= 0) {
	    cint	ml = MIN(mlen,bb_len) ;
	    int		bl ;
	    char	bbuf[bb_len + 1] ;
	    {
	        bl = intconv(strwcpynon(bbuf,mbuf,ml) - bbuf) ;
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
	        rs = biblebook_loadnamesloc(op) ;
	    } else {
	        rs = biblebook_loadnamesrem(op) ;
	    }
	} else {
	    rs = op->namesz ;
	}
	return rs ;
} /* end subroutine (biblebook_loadnames) */

local int biblebook_loadnamesloc(BB *op) noex {
	int		rs ;
	int		sztab ;
	int		sz = 0 ;
	int		namesz = 0 ; /* return-value */
	int		n ; /* used-afterwards */
	char		*mp ;
	for (n = 0 ; booknames[n] ; n += 1) {
	    namesz += (lenstr(booknames[n]) + 1) ;
	} /* end for */
	sztab = (n + 1) * szof(cchar *) ;
	sz += sztab ;
	sz += namesz ;
	if ((rs = mem.mall(sz,&mp)) >= 0) {
	    int		i ; /* used-afterwards */
	    cchar	**names = (cchar **) mp ;
	    char	*bp = mp ;
	    bp = (mp + sztab) ;
	    op->namestrs = bp ;
	    for (i = 0 ; i < n ; i += 1) {
	        names[i] = bp ;
	        bp = strwcpynon(bp,booknames[i],-1) + 1 ;
	    } /* end for */
	    names[i] = nullptr ;
	    op->names = names ;
	    op->namesz = namesz ;
	} /* end if (memory-acquire) */
	return (rs >= 0) ? namesz : rs ;
} /* end subroutine (biblebook_loadnamesloc) */

local int biblebook_loadnamesremn(BB *op) noex {
	int		rs = SR_BUGCHECK ;
	int		n = 0 ;
        if (biblebook_calls *callp = callsp(op->callp) ; callp) {
            rs = SR_OK ; /* <- OK if no |size| method */
            if (cauto co_size = callp->size ; co_size) ylikely {
                if (cauto co_count = callp->count ; co_count) ylikely {
                    if ((rs = co_size(op->obj)) >= 0) {
		        op->namesz = rs ;
                        if ((rs = co_count(op->obj)) >= 0) {
			    n = rs ;
			}
		    }
                } /* end if (non-null) */
	    } /* end if (non-null) */
	} /* end if (bug-check) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (biblebook_loadnamesremn) */

local int biblebook_loadnamesremsz(BB *op) noex {
	int		rs = SR_BUGCHECK ;
	int		n = 0 ;
        if (biblebook_calls *callp = callsp(op->callp) ; callp) {
	    if ((rs = biblebook_loadnamesremn(op)) == 0) {
	        rs = SR_NOSYS ;
                if (cauto co = callp->get ; co ) ylikely {
	    	    cint	blen = bb_len ;
	    	    char	bbuf[bb_len + 1] ;
		    int		namesz = 0 ;
		    rs = SR_OK ;
	            for (n = 0 ; rs >= 0 ; n += 1) {
	                if ((rs = co(op->obj,n,bbuf,blen)) >= 0) {
		            namesz += (rs + 1) ;
	                } else {
		            if (rs == SR_NOTFOUND) rs = SR_OK ;
		            break ;
	                }
	            } /* end for */
	            op->namesz = namesz ;
	        } /* end if (biblebook_loadnamesremn) */
	    } /* end if (non-null) */
        } /* end if (bug-check) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (biblebook_loadnamesremsz) */

local int biblebook_loadnamesrem(BB *op) noex {
	int		rs = SR_BUGCHECK ;
	int		namesz = 0 ; /* return-value */
        if (biblebook_calls *callp = callsp(op->callp) ; callp) {
	    if ((rs = biblebook_loadnamesremsz(op)) >= 0) {
	        cint	sztab = (rs + 1) * szof(cchar *) ;
	        cint	n = rs ;
	        rs = SR_NOSYS ;
                if (cauto co = callp->get ; co ) ylikely {
	            cint	blen = bb_len ;
	            char	bbuf[bb_len + 1] ;
	            cint	sz = sztab + op->namesz ;
	            if (char *mp ; (rs = mem.mall(sz,&mp)) >= 0) {
		        int	i ; /* used-afterwards */
	                int	bl ;
		        cchar	**names = ccharpp(mp) ;
		        char	*bp = (mp + sztab) ;
	                op->namestrs = bp ;
	                for (i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	                    if ((rs = co(op->obj,i,bbuf,blen)) >= 0) {
	                        bl = rs ;
	                        names[i] = bp ;
	                        bp = strwcpynon(bp,bbuf,bl) + 1 ;
	                    }
	                } /* end for */
	                names[i] = nullptr ;
	                if (rs < 0) { /* should not or cannot fail! */
	                    mem.free(mp) ;
	                } else {
			    namesz = op->namesz ;
	                    op->names = names ;
		        }
	            } /* end if (memory-acquire) */
	        } /* end if (non-null) */
	    } /* end if (biblebook_loadnamesremsz) */
        } /* end if (bug-check) */
	return (rs >= 0) ? namesz : rs ;
} /* end subroutine (biblebook_loadnamesrem) */

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

/* compaction and lower-case simultaneously */
local char *strwcpynon(char *dp,cchar *sp,int sl) noex {
	if (int ch ; sl >= 0) {
	    for ( ; sl-- && ((ch = mkchar(*sp))) ; sp += 1) {
		if (! ISWHT(ch)) *dp++ = tolc(ch) ;
	    } /* end for */
	} else {
	    for ( ; ((ch = mkchar(*sp))) ; sp += 1) {
		if (! ISWHT(ch)) *dp++ = tolc(ch) ;
	    } /* end for */
	} /* end if */
	*dp = '\0' ;
	return dp ;
} /* end subroutine (strwcpynon) */


