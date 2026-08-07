/* bibdb_entfileproc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Bibliographical DataBase */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* compile-time debug print-outs */
#define	CF_NEWTHING	1		/* compile in the new thing */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

	= 2005-10-01, David A­D­ Morano
	This was changed to work in the MMCITE program.  The old
	REFERM program is really obsolete.  It used a database lookup
	strategy to remote databases.  The high-level problem is:
	what to do if the cited BIB entry is not found?  How does a
	maintainer of the present (local) document know what that
	BIB entry was?  The new strategy (implemented by the MMCITE
	program) is more like what is done with BibTeX in the TeX
	(or LaTeX) world.  All BIB databases are really expected to
	be maintained by the document creator -- not some centralized
	entity.  The older centralized model reflected more the use
	in the corporate world (where different people create BIB
	entries) than in the more "modern" personal-responsibility
	type of world! :-)  Anyway, this is the way the gods seem
	to now want to do things.  Deal with it!

*/

/* Copyright © 1998,2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	bibdb

	Description:
	This code object module implements a little DB query facility.
	The database is a set of files that contain bibliographical
	entries in the "REFER" format.  Queries to the database
	that succeed return a structure with the elements of the
	bibliographic entry.  All queries are to database files
	that were referenced last (most previously) in the processing
	of the document.  The idea is that a more recent reference
	to a database file means that it is the preferred one to
	use since it is closest to the user (or the most preferred
	by the user).  A small nice thing about this database is
	that database files (files containing "REFER"-formatted
	entries) are not indexed unless they are needed (due to a
	failure to find the query in existing indexed files).  Only
	after all database files have been indexed and scanned to
	try to satisfy the query (and the request os not found),
	does the query fail.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD placement-new */
#include	<memory>                /* C++STD |destroy_at(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<rebuf.hh>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG */
#include	<bfile.h>		/* LIBB */

#include	"bibdb_bibentry.hh"
#include	"bibdb_entfile.hh"
#include	"bibdb_key.hh"
#include	"bibdb_keyval.hh"
#include	"bibdb_ent.hh"
#include	"bibdb_readent.hh"
#include	"bibdb_entfileproc.hh"
#include	"bibdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import bibdb_mag ;

/* local defines */

#define	BDB		bibdb		/* type */
#define	BDB_CUR		bibdb_cur	/* type */
#define	BDB_ENT		bibdb_ent	/* type */
#define	BDB_KEY		bibdb_key	/* type */
#define	BDB_FI		bibdb_entfile	/* type */


/* imported namespaces */

using namespace bibdbx ;

using std::destroy_at ;                 /* subroutime */
using bibdbx::bibdb_readent ;		/* subroutine */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct procflags {
	uint		fbol:1 ;
	uint		feol:1 ;
	uint		finkey:1 ;
	uint		finent:1 ;
	uint		fdone:1 ;
    } ; /* end struct (procflags) */
    struct readenter {
	BDB		*op ;
	BDB_FI		*bfep ;
	charp		cbuf ;
	uint		foff ;
	uint		citeoff ;
	int		fi ;
	int		clen ;
	int		state ;
	procflags	pf ;
	readenter(BDB *o,int i,BDB_FI *ep) noex {
	    op		= o ;
	    bfep	= ep ;
	    fi		= i ;
	    pf = {} ;
	    {
		cbuf = nullptr ;
		clen = 0 ;
		state = 0 ;
	    }
	} ; /* end ctor */
	operator int () noex ;
	int reader	(rebuf *,charp,int,bfile *) noex ;
	int readln	(rebuf *,ccharp,int) noex ;
	int readlner	(rebuf *,ccharp,int) noex ;
	int bufendcheck	(rebuf *) noex ;
    } ; /* end struct (readenter) */
} /* end namespace */


/* forward references */

[[maybe_unused]]
local int bibdb_inskey(bibdb *,int,uint,cchar *,int) noex ;
[[maybe_unused]]
local bool	iskey(cchar *,int) noex ;


/* local variables */

enum states {
	state_search,
	state_have,
	state_overlast
} ; /* end enum (stats) */

#if	CF_DEBUG
constexpr cpcchar	states[] = {
	"search",
	"have",
	nullptr
} ; /* end array */
#endif /* CF_DEBUG */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

#if	CF_NEWTHING
namespace bibdbx {
    int bibdb_entfileproc(bibdb *op,int fi,BDB_FI *bfep) noex {
	int		rs = SR_BUGCHECK ;
	DEBUGPRINTF("ent fi=%u\n",fi) ;
	if (op && bfep) ylikely {
	    readenter ro(op,fi,bfep) ;
	    rs = ro ;
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
    } /* end subroutine if (bibdb_entfileproc) */
} /* end namespace (bibdbx) */

readenter::operator int () noex {
    	cnothrow	nt{} ;
    	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	DEBUGPRINTF("ent fi=%u\n",fi) ;
	op->unindexed -= 1 ;
	if ((rs = mem.ml(&cbuf)) >= 0) ylikely {
	    clen = rs ;
	    if (rebuf *rbp = new(nt) rebuf ; rbp) ylikely {
		if ((rs = rbp->start(cbuf,clen)) >= 0) ylikely {
	            if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) ylikely {
	                cint llen = rs ;
	                if (bfile ef ; (rs = ef.open(bfep->fname,"r")) >= 0) {
	                    {
		                rs = reader(rbp,lbuf,llen,&ef) ;
		                rv = rs ;
	                    }
	                    rs1 = ef.close ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (bfile) */
	                rs1 = mem.free(lbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
		    rs1 = rbp->finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (rebuf) */
	        delete rbp ;
	    } /* end if (m-a-f) */
	    rs1 = mem.free(cbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? rv : rs ;
} /* end method (readenter::operator) */

int readenter::reader(rebuf *rbp,charp lbuf,int llen,bfile *bfp) noex {
    	int		rs ;
	int		ne = 0 ; /* return-value */
	pf.fbol = true ;
	while ((rs = bfp->readln(lbuf,llen)) > 0) {
	    cint len = rs ;
	    {
        	pf.feol = (lbuf[len - 1] == '\n') ;
		rs = readln(rbp,lbuf,len) ;
		ne += rs ;
	    }
	    pf.fbol = pf.feol ;
	    if (pf.fdone || (rs < 0)) break ;
	} /* end while */
#ifdef	COMMENT
	if ((rs >= 0) && pf.finkey) {
	    rs = bibentry_end(iep) ;
	    pf.finkey = false ;
	}
#endif /* COMMENT */
	/* OK, load this internal entry */
#ifdef	COMMENT
	if (rs >= 0) {
	    rs = entry_load(bep,bebuf,belen,iep) ;
	    ne += 1 ;
	} /* end if (ok) */
#endif /* COMMENT */
	return (rs >= 0) ? ne : rs ;
} /* end method (readenter::reader) */

int readenter::readln(rebuf *rbp,cchar *lbuf,int len) noex {
    	int		rs = SR_OK ;
	cchar		*lp = lbuf ;
	cint		ll = (pf.feol) ? (len - 1) : len ;
	switch (state) {
	case state_search:
	    if ((! pf.fbol) || (! iskey(lp,ll))) break ;
	    state = state_have ;
	    citeoff = foff ;
	    falldown ;
	    /* FALLTHROUGH */
	case state_have:
	    rs = readlner(rbp,lp,ll) ;
	    break ;
	default:
	    rs = SR_BUGCHECK ;
	    break ;
	} /* end switch */
	return rs ;
} /* end method (readenter::readln) */

int readenter::readlner(rebuf *rbp,cchar *lp,int ll) noex {
    	int		rs = SR_OK ;
	DEBUGPRINTF("ent\n") ;
	if (pf.fbol && iskey(lp,ll)) ylikely {
	    DEBUGPRINTF("key=%c\n",lp[1]) ;
	    DEBUGPRINTF("f_inkey=%u\n",pf.finkey) ;
	    if ((rs = bufendcheck(rbp)) >= 0) ylikely {
                lp += 1 ;
                ll -= 1 ;
		cchar *cp ;
                if (int cl ; (cl = sfnext(lp,ll,&cp)) > 0) {
                    DEBUGPRINTF("cl=%u c=%r\n",cl,cp,cl) ;
                    DEBUGPRINTF("ql=%u q=%r\n",op->qkl,op->qkp,op->qkl) ;
	            bool f = true ;
	            f = f && (cl == op->qkl) ;
                    f = f && (strncmp(cp,op->qkp,op->qkl) == 0) ;
	            if (f) {
                        ll -= conv<int>((cp + cl) - lp) ;
                        lp = (cp + cl) ;
                        DEBUGPRINTF("rline=%r\n",lp,ll) ;
			cchar *kp ;
                        if (int kl ; (kl = sfnext(lp,ll,&kp)) > 0) {
                            DEBUGPRINTF("cite-key start\n") ;
                            DEBUGPRINTF("k=%r\n",kp,kl) ;
                            if ((rs = rbp->bufbeg) >= 0) {
                                pf.finkey = true ;
                                if (kl > 0) {
                                    rs = rbp->strw(kp,kl) ;
		                }
                            } /* end if (bufbeg) */
			} /* end if */
                    } /* end if (found cite-key) */
		} /* end if (non-zero positive) */
	    } /* end if (bufendcheck) */
        } else if (pf.finkey) {
            if ((rs = rbp->getlen) > 0) {
                rs = rbp->chr(' ') ;
            }
	    if (rs >= 0) {
                rs = rbp->strw(lp,ll) ;
	    }
        } /* end if */
        /* are we leaving an entry? */
        DEBUGPRINTF("leaving?\n") ;
        if ((rs >= 0) && ((ll == 0) || (lp[0] == '\n'))) {
            DEBUGPRINTF("end-of-entry \n") ;
            DEBUGPRINTF("f_inkey=%u\n",pf.finkey) ;
            state = state_search ;
            if (pf.finkey) {
                pf.finkey = false ;
	        rs = rbp->bufend ;
            }
            DEBUGPRINTF("cbuf=%s\n",cbuf) ;
            if (cbuf[0] != '\0') {
                cint        colen = int(citeoff - foff) ;
                DEBUGPRINTF("inserting key=%s\n", cbuf) ;
                DEBUGPRINTF("fi=%u foff=%u citeoff=%u\n",fi,foff,citeoff) ;
                rs = bibdb_inskey(op,fi,foff,cbuf,colen) ;
                DEBUGPRINTF("bibdb_inskey() " "rs=%d\n",rs) ;
                cbuf[0] = '\0' ;
            } /* end if */
        } /* end if (state transition) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (readenter::readlner) */

int readenter::bufendcheck(rebuf *rbp) noex {
    	int		rs = SR_OK ;
	if (pf.finkey) {
	    pf.finkey = false ;
	    rs = rbp->bufend ;
   	} /* end if (had key) */
	return rs ;
} /* end method (readenter::bufendcheck) */


#else /* CF_NEWTHING */
namespace bibdbx {
    int bibdb_entfileproc(bibdb *op,int fi,BDB_FI *bfep) noex {
    	(void) op ;
	(void) fi ;
	(void) bfep ;
    	return SR_OK ;
    } /* end subroutine */
} /* end namespace (bibdbx) */
#endif /* CF_NEWTHING */


/* private subroutines */

#ifdef	COMMENT


	sbuf		citebuf ;
	bfile		bibfile, *bfp = &bibfile ;
	uint		foff = 0 ;
	uint		foff ;
	int		rs1 ;
	int		len ;
	int		f_inkey = false ;
	DEBUGPRINTF("ent fi=%u\n",fi) ;
	op->unindexed -= 1 ;
	bfile		bibfile, *bfp = &bibfile ;
	if ((rs = bopen(bfp,bfep->fname,"r",0666)) >= 0) {
	    cint	llen = LINEBUFLEN ;
	    cint	clen = CITEBUFLEN  ;
	    int		state = state_search ;
	    int		ll, cl, kl ;
	    int		f_bol = true ;
	    int		f_eol ;
	    cchar	*lp, *cp, *kp ;
	    char	lbuf[LINEBUFLEN + 1] ;
	    char	cbuf[CITEBUFLEN + 1] ;

	    while ((rs = breadln(bfp,lbuf,llen)) > 0) {
	        len = rs ;

	        f_eol = (lbuf[len - 1] == '\n') ;

	        lp = lbuf ;
	        ll = (f_eol) ? (len - 1) : len ;

#if	CF_DEBUG
	        debugprintf("bibdb_entfileproc: line=>%r<\n",
	            lbuf,
	            ((f_eol) ? (len - 1) : len)) ;
	        debugprintf("bibdb_entfileproc: f_bol=%u state=%s(%u)\n",
	            f_bol,states[state],state) ;
#endif

	        switch (state) {
	        case state_search:
	            if (! f_bol) break ;
	            if (! iskey(lbuf,len)) break ;
	            state = state_have ;
	            citeoff = foff ;
		    falldown ;
		    /* FALLTHROUGH */
	        case state_have:



	            if (pf.fbol && iskey(lbuf,len)) {

#if	CF_DEBUG
	                debugprintf("bibdb_entfileproc: key=%c\n",lbuf[1]) ;
	                debugprintf("bibdb_entfileproc: f_inkey=%u\n",f_inkey) ;
#endif

	                if (f_inkey) {
	                    f_inkey = false ;
	                    sbuf_finish(&citebuf) ;
	                }

	                lp += 1 ;
	                ll -= 1 ;

	                cl = sfnext(lp,ll,&cp) ;

#if	CF_DEBUG
	                DEBUGPRINTF("cl=%u c=%r\n",cl,cp,cl) ;
	                DEBUGPRINTF("ql=%u q=%r\n",
	                    op->qkl,op->qkp,op->qkl) ;
#endif

	                if ((strncmp(cp,op->qkp,op->qkl) == 0) &&
	                    (cl == op->qkl)) {

	                    ll -= ((cp + cl) - lp) ;
	                    lp = (cp + cl) ;

#if	CF_DEBUG
	                    DEBUGPRINTF("rline=%r\n",lp,ll) ;
#endif

	                    kl = sfnext(lp,ll,&kp) ;

#if	CF_DEBUG
	                    DEBUGPRINTF("cite-key start\n") ;
	                    DEBUGPRINTF("k=%r\n",kp,kl) ;
#endif

	                    rs = sbuf_start(&citebuf,cbuf,clen) ;
	                    f_inkey = (rs >= 0) ;

	                    if ((rs >= 0) && (kl > 0)) {
	                        sbuf_strw(&citebuf,kp,kl) ;
			    }

	                } /* end if (found cite-key) */

	            } else if (f_inkey) {
	                if (sbuf_getlen(&citebuf) > 0) {
	                    sbuf_chr(&citebuf,' ') ;
			}
	                sbuf_strw(&citebuf,lbuf,len) ;
	            } /* end if */

/* are we leaving an entry? */

#if	CF_DEBUG
	            debugprintf("bibdb_entfileproc: leaving?\n") ;
	            debugprintf("bibdb_entfileproc: len=%d\n",len) ;
	            debugprintf("bibdb_entfileproc: lbuf=%s\n",lbuf) ;
#endif

	            if ((rs >= 0) && ((len == 0) || (lbuf[0] == '\n'))) {

#if	CF_DEBUG
	                DEBUGPRINTF("end-of-entry \n") ;
	                DEBUGPRINTF("f_inkey=%u\n",f_inkey) ;
#endif

	                state = state_search ;
	                if (f_inkey) {
	                    f_inkey = false ;
	                    sbuf_finish(&citebuf) ;
	                }

#if	CF_DEBUG
	                debugprintf("bibdb_entfileproc: cbuf=%s\n",cbuf) ;
#endif

	                if (cbuf[0] != '\0') {
			    cint	colen = (foff - foff) ;

#if	CF_DEBUG
	                    DEBUGPRINTF("inserting key=%s\n",
	                        cbuf) ;
	                    DEBUGPRINTF("fi=%u foff=%u "
	                        "foff=%u\n",
	                        fi,foff,foff) ;
#endif

	                    rs = bibdb_inskey(op,fi,foff,cbuf,colen) ;

#if	CF_DEBUG
	                    DEBUGPRINTF("bibdb_inskey() "
	                        "rs=%d\n",rs) ;
#endif

	                    cbuf[0] = '\0' ;

	                } /* end if */

	            } /* end if (state transition) */
	            break ;

	        } /* end switch */

	        foff += len ;
	        f_bol = f_eol ;
	        if (rs < 0) break ;
	    } /* end while */

	    rs1 = bclose(bfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */

	if (f_inkey) {
	    f_inkey = false ;
	    sbuf_finish(&citebuf) ;
	}

/* mark this file as being indexed */

	if (rs >= 0) {
	    rs = bibdbentfile_indexed(bfep) ;
	}
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
    } /* end subroutine if (bibdb_entfileproc) */
} /* end namespace (bibdbx) */

#endif /* COMMENT */

/* insert a citation-key into the BIB key DB */
[[maybe_unused]]
local int bibdb_inskey(bibdb *op,int fi,uint foff,cchar *cbuf,int clen) noex {
	cint		ksz = szof(BDB_KEY) ;
	int		rs = SR_BUGCHECK ;
	DEBUGPRINTF("foff=%u\n",foff) ;
	DEBUGPRINTF("cbuf=%s\n",cbuf) ;
	DEBUGPRINTF("clen=%d\n",clen) ;
	if (hdb *klp = resumelife<hdb>(op->klp) ; klp) ylikely {
	    if (void *vp ; (rs = mem.mall(ksz,&vp)) >= 0) ylikely {
	        rs = SR_BUGCHECK ;
	        if (BDB_KEY *bkp = new(vp) BDB_KEY ; bkp) ylikely {
	            if ((rs = bibdbkey_start(bkp,fi,foff,cbuf,clen)) >= 0) {
	                hdb_dat	key{} ;
	                hdb_dat	val{} ;
	                key.buf = bkp->ckey ;
	                key.len = lenstr(bkp->ckey) ;
	                val.buf = bkp ;
	                val.len = ksz ;
	                rs = klp->store(key,val) ;
	                if (rs < 0) {
	                    bibdbkey_finish(bkp) ;
		        } /* end if (error) */
	            } /* end if (bibdbkey_start) */
		    if (rs < 0) {
			destroy_at(bkp) ;
		    } /* end if (error) */
	        } /* end if (new-bibdb_key) */
	        if (rs < 0) {
	            mem.free(vp) ;
	        } /* end if (error) */
	    } /* end if (memory-acquire) */
	} /* end if (bug-check) */
	DEBUGPRINTF("ret rs=%d clen=%u\n",rs,clen) ;
	return (rs >= 0) ? clen : rs ;
} /* end subroutine (bibdb_inskey) */

/* is there a key? */
[[maybe_unused]]
local bool iskey(cchar *lp,int ll) noex {
	bool		f = false ;
	if (ll >= 2) {
	    f = (lp[0] == '%') ;
	    if (f) {
	        cint ch = lp[1] & UCHAR_MAX ;
	        f = isalphalatin(ch) ;
	    }
	} /* end if */
	return f ;
} /* end subroutine (iskey) */


