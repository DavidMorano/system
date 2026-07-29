/* bibleqs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* bible-query database manager */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */
#define	CF_EMPTYTERM	1		/* empty line terminates entry */
#define	CF_EXTRASTRONG	0		/* do not use Strong's eigen-words */
#define	CF_EXTRAEIGEN	0		/* perform extra EIGEN-DB check */
#define	CF_SINGLEWORD	1		/* treat extra words as single */
#define	CF_MKBIBLEQSI	0		/* |bibleqs_mkbibleqsi()| */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bibleqs

	Description:
	This little object provides access to the BIBLEQS database
	and index (if any).

	Note on Strong's eigen-words: There is a compile-time switch
	('CF_EXTRASTRONG') that chooses between using an internal
	list of Strong's 1980 set of eigen-words; or, alternatively,
	to use an eigen-database on the current system.  Using the
	Strong's list (an internally stored list) has the advantage
	of giving consistent query results with what would be
	returned if one was to actually use Strong's concordance.
	The disadvantage of using the internal list (Strong's list)
	is that it is small and may make queries a little bit more
	time consuming than would be the case when using a typical
	system eigen-word list (although this should be a very small
	effect at best).

	Note that any eigen-word list can be used because the list
	is stored in the index of the DB so that the same list is
	always used on queries as was used in the original creation
	of the index itself.  The DB proper only stores the real
	data, no eigen-words; so eigen-word lists can be changed
	on every recreation of the index.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/mman.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<estrings.h>		/* LIBUC */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<vecint.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<eigendb.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<dirseen.h>		/* LIBUC */
#include	<mkdirs.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<chownsame.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<path.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<strongeigens.h>	/* LIBDAM */
#include	<biblecite.h>		/* LIBDAM */
#include	<ktag.h>		/* LIBDAM */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	<naturalwords.h>	/* LIBDAM */
#include	<txtindexmk.h>		/* LIBDAM */
#include	<txtindex.h>		/* LIBDAM */
#include	<xwords.h>		/* LIBDAM */
#include	<searchkeys.h>		/* LIBDAM */

#include	"bibleqs.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |varname(3u)| + |sysword(3u)| */

/* local defines */

#define	BIBLEQS_NVERSES	33000
#define	BIBLEQS_MINWLEN	2		/* minimum word-length */
#define	BIBLEQS_MAXWLEN	6		/* more chrs => less collisions? */
#define	BIBLEQS_NEIGEN	2000		/* number of keys in chunk */
#define	BIBLEQS_DIRMODE	0777		/* parent directory */
#define	BIBLEQS_IDXMODE	0664		/* the index files */

#define	VS		vecstr

#define	DS		dirseen

#define	EC		expcook

#define	SK		searchkeys
#define	SK_P		searchkeys_pop
#define	SK_C		searchkeys_cur
#define	SK_KW		searchkeys_kw
#define	SK_PH		searchkeys_ph

#define	TI		txtindex
#define	TI_CUR		txtindex_cur
#define	TI_TAG		txtindex_tag
#define	TI_INFO		txtindex_info

#define	TIM		txtindexmk
#define	TIM_FL		txtindexmk_fl
#define	TIM_PA		txtindexmk_pa
#define	TIM_TAG		txtindexmk_tag
#define	TIM_KEY		txtindexmk_key

#define	BQS		bibleqs
#define	BQS_Q		bibleqs_q
#define	BQS_C		bibleqs_cur
#define	BQS_MAG		BIBLEQS_MAGIC

#define	SI		subinfo
#define	SI_FL		subinfo_fl

#define	EDB		eigendb
#define	EDB_C		eigendb_cur

#ifndef	VARSYSNAME
#define	VARSYSNAME	"SYSNAME"
#endif

#ifndef	VARRELEASE
#define	VARRELEASE	"RELEASE"
#endif

#ifndef	VARVERSION
#define	VARVERSION	"VERSION"
#endif

#ifndef	VARMACHINE
#define	VARMACHINE	"MACHINE"
#endif

#ifndef	VARARCHITECTURE
#define	VARARCHITECTURE	"ARCHITECTURE"
#endif

#ifndef	VARHZ
#define	VARHZ		"HZ"
#endif

#ifndef	VARDOMAIN
#define	VARDOMAIN	"DOMAIN"
#endif

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif

#ifndef	VARHOMEDNAME
#define	VARHOMEDNAME	"HOME"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARLOGNAME
#define	VARLOGNAME	"LOGNAME"
#endif

#ifndef	VARTZ
#define	VARTZ		"TZ"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#undef	VARDBNAME
#define	VARDBNAME	"MKBIBLEQSI_DBNAME"

#undef	VARPRBIBLEQS
#define	VARPRBIBLEQS	"MKBIBLEQSI_PROGRAMROOT"

#ifndef	VCNAME
#define	VCNAME		"var"
#endif

#define	INDDNAME	"bibleqs"

#define	DBSUF		"txt"
#define	INDSUF		"hash"
#define	TAGSUF		"tag"
#define	PROG_MKBIBLEQSI	"mkbibleqsi"

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)

#define	ISWHT(ch)	CHAR_ISWHITE(ch)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif
#ifndef	CF_EMPTYTERM
#define	CF_EMPTYTERM	1		/* empty line terminates entry */
#endif
#ifndef	CF_EXTRASTRONG
#define	CF_EXTRASTRONG	0		/* do not use Strong's eigen-words */
#endif
#ifndef	CF_EXTRAEIGEN
#define	CF_EXTRAEIGEN	0		/* perform extra EIGEN-DB check */
#endif
#ifndef	CF_SINGLEWORD
#define	CF_SINGLEWORD	1		/* treat extra words as single */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_fl {
	uint		id:1 ;
} ; /* end struct */

struct subinfo {
	ids		id ;
	SI_FL		fl ;
	time_t		dt ;
} ; /* end struct */

namespace {
    struct vars {
	int	maxnamelen ;
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local inline int bibleqs_ctor(bibleqs *op,Args ... args) noex {
    	BIBLEQS		*hop = op ;
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    op->pr	= np ;
	    op->dbname	= np ;
	    op->dbfname	= np ;
	    op->dbmdata = np ;
	    op->indp	= np ;
	    op->magval	= 0 ;
	    if ((op->edbp = new(nt) eigendb) != np) {
	        if ((op->indp = new(nt) txtindex) != np) {
		    rs = SR_OK ;
	        } /* end if (new-txtindex) */
		if (rs < 0) {
		    delete op->edbp ;
		    op->edbp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-eigendb) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleqs_ctor) */

local int bibleqs_dtor(bibleqs *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->indp) {
		delete op->indp ;
		op->indp = nullptr ;
	    } /* end if (memory-release) */
	    if (op->edbp) {
		delete op->edbp ;
		op->edbp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (bibleqs_dtor) */

template<typename ... Args>
local inline int bibleqs_magic(bibleqs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BQS_MAG) ? SR_OK : SR_NOTOPEN ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleqs_magic) */

local int	bibleqs_infoloadbegin(BQS *,cchar *,cchar *) noex ;
local int	bibleqs_infoloadend(BQS *) noex ;
local int	bibleqs_indopen(BQS *,SI *) noex ;

local int	bibleqs_indclose(BQS *) noex ;
local int	bibleqs_indmk(BQS *,cchar *,time_t) noex ;
local int	bibleqs_indmkeigen(BQS *,TIM *) noex ;
local int	bibleqs_indmkdata(BQS *,TIM *) noex ;
local int	bibleqs_dbmapcreate(BQS *,time_t) noex ;
local int	bibleqs_dbmapdestroy(BQS *) noex ;
local int	bibleqs_havekeys(BQS *,TI_TAG *,int,SK *) noex ;
local int	bibleqs_havekeysline(BQS *,SK *,SK_P *,
			cchar *,int) noex ;
local int	bibleqs_matchkeys(BQS *,SK *,SK_P *,
			cchar *,int) noex ;
local int	bibleqs_loadbuf(BQS *,uint,char *,int) noex ;
local int	bibleqs_mkhkeys(BQS *,vecstr *,SK *) noex ;
local int	bibleqs_indopenseq(BQS *,SI *) noex ;
local int	bibleqs_indopenseqer(BQS *,SI *,DS *,EC *) noex ;
local int	bibleqs_indopencheck(BQS *,cchar *) noex ;
local int	bibleqs_indopenmk(BQS *,SI *,cchar *) noex ;

local int	bibleqs_loadcooks(BQS *,EC *) noex ;
local int	bibleqs_dirok(BQS *,DS *,ids *,cchar *,int) noex ;
local int	bibleqs_mkdir(BQS *,cchar *) noex ;

#if	CF_MKBIBLEQSI
local int	bibleqs_mkbibleqsi(BQS *,cchar *) noex ;
#endif

local int	bibleqs_eigenopen(BQS *) noex ;
local int	bibleqs_eigenclose(BQS *) noex ;
local int	bibleqs_curlooks(BQS *,BQS_C *,int,SK *,vecstr *) noex ;

#ifdef	COMMENT
local int	bibleqs_mksysvarsi(BQS *,cchar *) noex ;
#endif

local int	subinfo_start(SI *) noex ;
local int	subinfo_finish(SI *) noex ;

#ifdef	COMMENT
local int	subinfo_ids(SI *) noex ;
#endif

local int	mkdname(cchar *,mode_t) noex ;
local int	checkdname(cchar *) noex ;
local int	eigenfind(EDB *,cchar *,cchar *,int) noex ;
local int	mkfieldterms(char *) noex ;

local bool	isNeedIndex(int) noex ;


/* local variables */

#if	CF_MKBIBLEQSI
constexpr cpcchar	envchild[] = {
	VARSYSNAME,
	VARRELEASE,
	VARVERSION,
	VARMACHINE,
	VARARCHITECTURE,
	VARHZ,
	VARNODE,
	VARDOMAIN,
	VARHOMEDNAME,
	VARUSERNAME,
	VARLOGNAME,
	VARTZ,
	VARPWD,
	nullptr
} ; /* end array (envchild) */
#endif /* CF_MKBIBLEQSI */

/* use fixed locations for security reasons (like we care!) */
#if	CF_MKBIBLEQSI
constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ; /* end array (prbins) */
#endif /* CF_MKBIBLEQSI */

constexpr cpcchar	idxdirs[] = {
	"/var/tmp/%{PRN}/%S",
	"/tmp/%{PRN}/%S",
	"%R/var/%S",
	"/var/tmp",
	"/tmp",
	"%T",
	nullptr
} ; /* end array (ifxdirs) */

constexpr cpcchar	eigenfnames[] = {
	"lib/bibleqs/%n.%f",
	"lib/bibleqs/%f",
	"share/dict/%n.%f",
	"share/dict/%f",
	"share/dict/eign",
	"/usr/share/dict/eign",
	"/usr/share/lib/dict/eign",
	nullptr
} ; /* end array (eigenfnames) */

constexpr int	rsneeds[] = {
	SR_STALE,
	0
} ; /* end array (rsneeds) */

static vars		var ;
static char		wterms[fieldterms_termsize] ;
cint			nchars		= (UCHAR_MAX + 1) ;
cint			nstrongeigens	= lenstrarr(strongeigens) ;
cbool			f_debug		= CF_DEBUG ;
cbool			f_emptyterm	= CF_EMPTYTERM ;
cbool			f_extrastrong	= CF_EXTRASTRONG ;
cbool			f_extraeigen	= CF_EXTRAEIGEN ;
cbool			f_singleword	= CF_SINGLEWORD	;


/* exported variables */

const bibleqs_obj	bibleqs_modinfo = {
	"bibleqs",
	szof(bibleqs),
	szof(bibleqs_cur)
} ; /* end initialization */

const bibleqms		bibleqm ;


/* exported subroutines */

int bibleqs_open(BQS *op,cchar *pr,cchar *dbn) noex {
	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("ent dbn=%s\n",dbn) ;
	if ((rs = bibleqs_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		if (static cint rsv = var ; (rs = rsv) >= 0) {
		    if (SI si ; (rs = subinfo_start(&si)) >= 0) {
	    		op->minwlen = BIBLEQS_MINWLEN ;
	    		if ((rs = bibleqs_infoloadbegin(op,pr,dbn)) >= 0) {
			    if ((rs = bibleqs_dbmapcreate(op,si.dt)) >= 0) {
		    		if ((rs = bibleqs_indopen(op,&si)) >= 0) {
				    op->magval = BIBLEQS_MAGIC ;
		    		}
		    		if (rs < 0) {
				    bibleqs_dbmapdestroy(op) ;
		    		} /* end if (error) */
	        	    } /* end if (bibleqs_dbmapcreate) */
			    if (rs < 0) {
		    	        bibleqs_infoloadend(op) ;
			    } /* end if (error) */
	    		} /* end if (bibleqs_infoloadbegin) */
	    		rs1 = subinfo_finish(&si) ;
	    		if (rs >= 0) rs = rs1 ;
	    		if (rs < 0) {
			    bibleqs_close(op) ;
	    		} /* end if (error) */
		    } /* end if (subinfo) */
	        } /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		bibleqs_dtor(op) ;
	    } /* end if (error) */
        } /* end if (bibleparas_ctor) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bibleqs_open) */

int bibleqs_close(BQS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleqs_magic(op)) >= 0) {
	    {
	        rs1 = bibleqs_indclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if_constexpr (f_extrastrong) {
	        rs1 = bibleqs_eigenclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if_constexpr (f_extrastrong) */
	    {
	        rs1 = bibleqs_dbmapdestroy(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = bibleqs_infoloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = bibleqs_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
        } /* end if (bibleqs_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bibleqs_close) */

int bibleqs_count(BQS *op) noex {
	int		rs ;
	if ((rs = bibleqs_magic(op)) >= 0) {
	    if (op->fl.ind) {
	       rs = txtindex_count(op->indp) ;
	    }
        } /* end if (bibleqs_magic) */
	return rs ;
} /* end subroutine (bibleqs_count) */

int bibleqs_audit(BQS *op) noex {
	int		rs ;
	if ((rs = bibleqs_magic(op)) >= 0) {
	    rs = txtindex_audit(op->indp) ;
        } /* end if (bibleqs_magic) */
	return rs ;
} /* end subroutine (bibleqs_audit) */

int bibleqs_curbegin(BQS *op,BQS_C *curp) noex {
	int		rs ;
	if ((rs = bibleqs_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    op->ncursors += 1 ;
        } /* end if (bibleqs_magic) */
	return rs ;
} /* end subroutine (bibleqs_curbegin) */

int bibleqs_curend(BQS *op,BQS_C *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleqs_magic(op,curp)) >= 0) {
	    if (curp->verses) {
	        voidp vp = voidp(curp->verses) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        curp->verses = nullptr ;
	    } /* end if (memory-release) */
	    curp->nverses = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
        } /* end if (bibleqs_magic) */
	return rs ;
} /* end subroutine (bibleqs_curend) */

int bibleqs_curlook(BQS *op,BQS_C *curp,int qo,cchar **qsp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if ((rs = bibleqs_magic(op,curp,qsp)) >= 0) {
#if	CF_DEBUG
	    {
	        for (int i = 0 ; qsp[i] != nullptr ; i += 1) {
		    DEBUGPRINTF("qs=>%s<\n",qsp[i]) ;
	        } /* end for */
	    }
#endif /* CF_DEBUG */
	    curp->nverses = 0 ;
	    if (curp->verses) {
	        voidp vp = voidp(curp->verses) ;
	        mem.free(vp) ;
	        curp->verses = nullptr ;
	    } /* end if (memory-release) */
	    if (SK sk ; (rs = searchkeys_start(&sk,qsp)) >= 0) {
		cint	vn = 10 ;
	        cint	vo = (vecstrm.compact) ;
	        if (vecstr hkeys ; (rs = vecstr_start(&hkeys,vn,vo)) >= 0) {
	            if ((rs = bibleqs_mkhkeys(op,&hkeys,&sk)) >= 0) {
		        rs = bibleqs_curlooks(op,curp,qo,&sk,&hkeys) ;
		        c = rs ;
	            } /* end if (bibleqs_mkhkeys) */
	            rs1 = vecstr_finish(&hkeys) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (vecstr_start) */
	        rs1 = searchkeys_finish(&sk) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (searchkeys) */
	    DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
        } /* end if (bibleqs_magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_curlook) */

int bibleqs_curenum(BQS *op,BQS_C *curp,BQS_Q *citep,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = bibleqs_magic(op,curp,citep,vbuf)) >= 0) {
	    if ((curp->nverses > 0) && (curp->verses != nullptr)) {
	        uint	recoff ;
		int	ei = (curp->i >= 0) ? curp->i : 0 ;
		int	ml ;
		cchar	*mp ;
		DEBUGPRINTF("c_i=%u\n",ei) ;
		recoff = curp->verses[ei] ;
	        if ((ei < curp->nverses) && (recoff != UINT_MAX)) {
	            DEBUGPRINTF("recoff=%u\n",recoff) ;
	            mp = ccharp(op->dbmdata + recoff) ;
	            ml = intconv(op->dbmsize - recoff) ;
	            DEBUGPRINTF("line=>%r<\n",mp,lenstr(mp,MIN(ml,40))) ;
	            if ((rs = biblecite_ver(citep,mp,ml)) > 0) {
			cint si = rs ;
	                recoff += si ;
	                mp += si ;
	                ml -= si ;
	                if ((ml > 0) && (mp[0] == '\n')) {
	                    recoff += 1 ;
	                }
	            } /* end if (biblecite_ver) */
	            DEBUGPRINTF("_loadbuf() \n") ;
		    if (rs >= 0) {
	                rs = bibleqs_loadbuf(op,recoff,vbuf,vlen) ;
	                len = rs ;
		    }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	        if (rs >= 0) {
	            curp->i = (ei + 1) ;
	        }
	    } else {
	        rs = SR_NOTFOUND ;
	    }
        } /* end if (bibleqs_magic) */
	DEBUGPRINTF("ret rs=%d len=%u\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (bibleqs_curenum) */


/* private subroutines */

local int bibleqs_infoloadbegin(BQS *op,cchar *pr,cchar *dbn) noex {
	cint		psz = (2 * (var.maxpathlen + 1)) ;
	cint		maxpath = var.maxpathlen ;
    	int		ai = 2 ; /* two path buffers */
	int		rs ;
	int		rs1 ;
	if ((dbn == nullptr) || (dbn[0] == '\0')) {
	    dbn = BIBLEQS_DBNAME ;
	}
	op->pr = pr ;
	op->dbname = dbn ;
	if (char *a ; (rs = mem.mall(psz,&a)) >= 0) {
	    char	*tbuf = (a + (--ai * (maxpath + 1))) ;
	    if ((rs = mkpath3(tbuf,pr,BIBLEQS_DBDNAME,dbn)) >= 0) {
	        char	*dbuf = (a + (--ai * (maxpath + 1))) ;
	        if ((rs = mkfnamesuf1(dbuf,tbuf,DBSUF)) >= 0) {
		    cint	fnl = rs ;
		    if (cchar *cp ; (rs = mem.strw(dbuf,fnl,&cp)) >= 0) {
		        op->dbfname = cp ;
	                if (ustat sb ; (rs = u_stat(op->dbfname,&sb)) >= 0) {
	                    if (S_ISREG(sb.st_mode)) {
	                        rs = perm(op->dbfname,-1,-1,nullptr,R_OK) ;
	                    } else {
	                        rs = SR_NOTSUP ;
			    }
	                } /* end if (stat) */
		        if (rs < 0) {
		            voidp vp = voidp(op->dbfname) ;
		            mem.free(vp) ;
		            op->dbfname = nullptr ;
		        } /* end if (error) */
	            } /* end if (m-a) */
	        } /* end if (mkfnamesuf) */
	    } /* end if (mkpath) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (bibleqs_infoloadbegin) */

local int bibleqs_infoloadend(BQS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->dbfname) {
	    voidp vp = voidp(op->dbfname) ;
	    rs1 = mem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbfname = nullptr ;
	} /* end if (memory-release) */
	op->pr = nullptr ;
	op->dbname = nullptr ;
	return rs ;
} /* end subroutine (bibleqs_infoloadend) */

local int bibleqs_dbmapcreate(BQS *op,time_t dt) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	DEBUGPRINTF("ent dbfname=%s\n",op->dbfname) ;
	if ((rs = u_open(op->dbfname,O_RDONLY,0666)) >= 0) {
	    cint	fd = rs ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
		csize	fsize = size_t(sb.st_size) ;
		if (S_ISREG(sb.st_mode) && (sb.st_size >= 0)) {
		    csize	ms = fsize ;
		    cint	mp = PROT_READ ;
		    cint	mf = MAP_SHARED ;
		    void	*md ;
		    op->ti_db = sb.st_mtime ;
	            if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
		        cint	madv = MADV_RANDOM ;
		        const caddr_t	ma = caddr_t(md) ;
	                if ((rs = u_madvise(ma,ms,madv)) >= 0) {
	                    op->dbmdata = charp(md) ;
	                    op->dbmsize = ms ;
	                    op->ti_map = dt ;
	                    op->ti_lastcheck = dt ;
	                } /* end if (u_madvise) */
	                if (rs < 0) {
		            u_mmapend(md,ms) ;
	                    op->dbmdata = nullptr ;
	                    op->dbmsize = 0 ;
		        } /* end if (error) */
	            } /* end if (u_mmap) */
	        } /* end if (ok) */
	    } else {
		rs = SR_NOTSUP ;
	    }
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (map-file) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (bibleqs_dbmapcreate) */

local int bibleqs_dbmapdestroy(BQS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->dbmdata) {
	    voidp	md = op->dbmdata ;
	    csize	ms = op->dbmsize ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbmdata = nullptr ;
	    op->dbmsize = 0 ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleqs_dbmapdestroy) */

local int bibleqs_indopen(BQS *op,SI *sip) noex {
	int		rs ;
	{
	    rs = bibleqs_indopenseq(op,sip) ;
	}
	return rs ;
} /* end subroutine (bibleqs_indopen) */

local int bibleqs_indopenseq(BQS *op,SI *sip) noex {
	int		rs ;
	int		rs1 ;
	if (DS ds ; (rs = ds.start) >= 0) {
	    if (EC cooks ; (rs = cooks.start) >= 0) {
	        if ((rs = bibleqs_loadcooks(op,&cooks)) >= 0) {
	            rs = bibleqs_indopenseqer(op,sip,&ds,&cooks) ;
	        }
		rs1 = cooks.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cooks) */
	    rs1 = ds.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ds) */
	return rs ;
} /* end subroutine (bibleqs_indopenseq) */

local int bibleqs_indopenseqer(BQS *op,SI *sip,DS *dsp,EC *ckp) noex {
    	cint		psz = (2 * (var.maxpathlen + 1)) ;
	cint		maxpath = var.maxpathlen ;
    	int		ai = 2 ; /* two path buffers */
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (char *a ; (rs = mem.mall(psz,&a)) >= 0) {
	    cint	elen = maxpath ;
	    char	*ebuf = (a + (--ai * (maxpath + 1))) ;
	    char	*pbuf = (a + (--ai * (maxpath + 1))) ;
	    /* first phase: expand possible directory paths */
	    if (ids id ; (rs = id.load) >= 0) {
	        for (int i = 0 ; (rs >= 0) && idxdirs[i] ; i += 1) {
	            cchar	*dir = idxdirs[i] ;
	            if ((rs = expcook_exp(ckp,'\0',ebuf,elen,dir,-1)) >= 0) {
	                if ((rs = pathclean(pbuf,ebuf,rs)) > 0) {
		            if ((rs = bibleqs_dirok(op,dsp,&id,pbuf,rs)) > 0) {
	            	        rs = bibleqs_indopencheck(op,pbuf) ;
			        c = rs ;
			        if ((rs < 0) && isNeedIndex(rs)) {
			            rs = bibleqs_indopenmk(op,sip,pbuf) ;
			            c = rs ;
			        }
		            } /* end if (bibleqs_dirok) */
		        } /* end if (pathclean) */
	            } /* end if (expcook_exp) */
		    if (c > 0) break ;
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = id.release ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ids) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indopenseqer) */

local int bibleqs_dirok(BQS *op,DS *dsp,ids *idp,cchar *dp,int dl) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		f_ok = false ; /* return-value */
	if ((rs = dirseen_havename(dsp,dp,dl)) == rsn) {
	    if (ustat sb ; (rs = uc_stat(dp,&sb)) >= 0) {
		if ((rs = dirseen_havedevino(dsp,&sb)) == rsn) {
		    cint	am = (W_OK|R_OK|X_OK) ;
		    if ((rs = permid(idp,&sb,am)) >= 0) {
			f_ok = true ;
		    } else if (isNotPresent(rs)) {
			rs = dirseen_add(dsp,dp,dl,&sb) ;
		    }
		}
	    } else if (isNotPresent(rs)) {
		if ((rs = bibleqs_mkdir(op,dp)) > 0) {
		    f_ok = true ;
		}
	    } /* end if */
	} /* end if (dirseen_havename) */
	return (rs >= 0) ? f_ok : rs ;
} /* end subroutine (bibleqs_dirok) */

local int bibleqs_mkdir(BQS *op,cchar *dp) noex {
	cmode		dm = 0777 ;
	int		rs ;
	int		f_ok = false ;
	if ((rs = mkdirs(dp,dm)) >= 0) {
	     if ((rs = uc_minmod(dp,dm)) >= 0) {
		if ((rs = chownsame(dp,op->pr)) >= 0) {
	            f_ok = true ;
	        }
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f_ok : rs ;
} /* end subroutine (bibleqs_mkdir) */

local int bibleqs_loadcooks(BQS *op,EC *ecp) noex {
	static cchar	*tmpdname = getenver(varname.tmpdir) ;
	int		rs = SR_OK ;
	cchar		*ks = "RST" ;
	char		kbuf[2] = {} ;
	if (tmpdname == nullptr) {
	    tmpdname = sysword.w_tmpdir ;
	}
	for (int i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	    cint	kch = MKCHAR(ks[i]) ; /* used-throughout */
	    ccharp	vap = nullptr ;
	    int		val = -1 ;
	    switch (kch) {
	    case 'R':
		vap = op->pr ;
		break ;
	    case 'S':
		vap = INDDNAME ;
		break ;
	    case 'T':
		vap = tmpdname ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && vap) {
		kbuf[0] = char(kch) ;
		rs = expcook_add(ecp,kbuf,vap,val) ;
	    } /* end if (ok) */
	} /* end for */
	if (rs >= 0) {
	    if (cchar *prname ; (rs = sfbasename(op->pr,-1,&prname)) >= 0) {
	        rs = SR_NOENT ;
	        if (prname) {
	            rs = ecp->add("PRN",prname,-1) ;
		}
	    } /* end if (sfbasename) */
	} /* end if (ok) */
	return rs ;
} /* end subroutine (bibleqs_loadcooks) */

local int bibleqs_indopencheck(BQS *op,cchar *idir) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent idxdir=%s\n",idir) ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	    if ((rs = mkpath2(tbuf,idir,op->dbname)) >= 0) {
	        if ((rs = txtindex_open(op->indp,op->pr,tbuf)) >= 0) {
	            TI_INFO	tinfo ;
		    c = rs ;
	            op->fl.ind = true ;
	            if ((rs = txtindex_getinfo(op->indp,&tinfo)) >= 0) {
	                if (tinfo.ticreat < op->ti_db) rs = SR_STALE ;
	            } /* end if (txtindex_getinfo) */
	            if (rs < 0) {
		        op->fl.ind = false ;
	                txtindex_close(op->indp) ;
		    } /* end if (error) */
	        } /* end if (txtindex_open) */
	    } /* end if (mkpath) */
	    rs1 = mem.free(&tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indopencheck) */

local int bibleqs_indopenmk(BQS *op,SI *sip,cchar *idir) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if ((rs = bibleqs_indmk(op,idir,sip->dt)) >= 0) {
	    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	        if ((rs = mkpath2(tbuf,idir,op->dbname)) >= 0) {
	            rs = txtindex_open(op->indp,op->pr,tbuf) ;
		    c = rs ;
	            op->fl.ind = (rs >= 0) ;
	        } /* end if (mkpath) */
	        rs1 = mem.free(tbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (bibleqs_indmk) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indopenmk) */

local int bibleqs_indmk(BQS *op,cchar *dname,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cmode		dm = BIBLEQS_DIRMODE ;
	DEBUGPRINTF("ent dname=%s\n",dname) ;
	if ((rs = mkdname(dname,dm)) >= 0) {
	    if (char *ibuf ; (rs = mem.mp(&ibuf)) >= 0) {
	        if ((rs = mkpath2(ibuf,dname,op->dbname)) >= 0) {
		    TIM_PA	ta{} ;
		    cmode	om = BIBLEQS_IDXMODE ;
		    cint	of = 0 ; /* auto-make */
		    ta.tablen = 0 ;			/* use default! */
		    ta.minwlen = op->minwlen ;
		    ta.maxwlen = BIBLEQS_MAXWLEN ;
		    ta.sfn = op->dbfname ;
		    TIM mk ;
		    if ((rs = txtindexmk_open(&mk,&ta,ibuf,of,om)) >= 0) {
		        if (rs == 0) {
		            if ((rs = bibleqs_indmkeigen(op,&mk)) >= 0) {
	    		        if ((rs = bibleqs_indmkdata(op,&mk)) >= 0) {
	    		            op->ti_tind = dt ;
			            c += rs ;
			        }
		            } /* end if (bibleqs_indmkeigen) */
		        } else {
			    c = rs ;
		        }
		        rs1 = txtindexmk_close(&mk) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (txtindexmk) */
	        } /* end if (mkpath) */
	        rs1 = mem.free(ibuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (mkdname) */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indmk) */

local int bibleqs_indmkeigenloc(BQS *op,TIM *tip) noex ;
local int bibleqs_indmkeigenrem(BQS *op,TIM *tip) noex ;

local int bibleqs_indmkeigen(BQS *op,TIM *tip) noex {
    	int		rs ;
	if_constexpr (f_extrastrong) {
	    if ((rs = bibleqs_eigenopen(op)) >= 0) {
	        rs = bibleqs_indmkeigenrem(op,tip) ;
	    }
	} else {
	    rs = bibleqs_indmkeigenloc(op,tip) ;
	} /* end if_constexpr (f_extrastrong) */
	return rs ;
} /* end subroutine (bibleqs_indmkeigen) */

local int bibleqs_indmkeigenrem(BQS *op,TIM *tip) noex {
	EDB		*edbp = op->edbp ;
	cint		nkeys = BIBLEQS_NEIGEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent f_edb=%u\n",op->fl.edb) ;
	if (op->fl.edb) {
	    cint sz = (nkeys + 1) * szof(TIM_KEY) ;
	    if (voidp vp ; (rs = mem.mall(sz,&vp)) >= 0) {
		TIM_KEY	*keys = resumelife<TIM_KEY>(vp) ;
	    	int	i = 0 ; /* used-afterwards */
		if (EDB_C ecur ; (rs = eigendb_curbegin(edbp,&ecur)) >= 0) {
		    int		wl ;
		    cchar	*wp ;
	            while ((wl = eigendb_curenum(edbp,&ecur,&wp)) >= 0) {
	                if (wl == 0) continue ;
	                DEBUGPRINTF("w=%r\n",wp,wl) ;
	                if (i >= nkeys) {
	                    c += i ;
	                    rs = txtindexmk_addeigens(tip,keys,i) ;
	                    i = 0 ;
	                }
	                keys[i].kp = wp ;
	                keys[i].kl = wl ;
	                i += 1 ;
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = eigendb_curend(edbp,&ecur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	        if ((rs >= 0) && (i > 0)) {
	            c += i ;
	            rs = txtindexmk_addeigens(tip,keys,i) ;
	            i = 0 ;
	        } /* end if (afterwards) */
	        rs1 = uc_free(keys) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	} /* end if (EDB-open) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indmkeigenrem) */

local int bibleqs_indmkeigenloc(BQS *op,TIM *tip) noex {
	cint		nkeys = nstrongeigens ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		i = 0 ; /* return-value */
	if (op) {
	    int		sz = (nkeys + 1) * szof(TIM_KEY) ;
	    if (voidp vp ; (rs = mem.mall(sz,&vp)) >= 0) {
	        TIM_KEY *keys = resumelife<TIM_KEY>(vp) ;
	        int	wl ;
	        cchar	*wp ;
	        /* populate */
	        for (i = 0 ; (i < nkeys) && strongeigens[i] ; i += 1) {
	            wp = strongeigens[i] ;
	            wl = lenstr(wp) ;
	            keys[i].kp = wp ;
	            keys[i].kl = wl ;
	        } /* end while */
	        keys[i].kp = nullptr ;	/* just for us (not needed) */
	        keys[i].kl = 0 ;
	        /* use */
	        rs = txtindexmk_addeigens(tip,keys,i) ;
	        /* despose of */
	        rs1 = mem.free(keys) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return (rs >= 0) ? i : rs ;
} /* end subroutine (bibleqs_indmkeigenloc) */

local int bibleqs_indmkdata(BQS *op,TIM *tip) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if (op->dbmdata != nullptr) {
	    BQS_Q	q ;
	    TIM_TAG	t ;
	    ktag	e ;
	    ktag_pa	ka ;
	    uint	foff = 0 ;
	    int		ml, ll ;
	    int		f_ent = false ;
	    cchar	*mp, *lp ;
	    /* paramters for ktaging */
	ka.edbp = op->edbp ;
	ka.f_eigen = op->fl.edb ;
	ka.minwlen = op->minwlen ;
	    /* start in */
	mp = op->dbmdata ;
	ml = intconv(op->dbmsize & INT_MAX) ;
	for (cc *tp ; (tp = strnchr(mp,ml,'\n')) != np ; ) {
	    cint len = intconv((tp + 1) - mp) ;
	    lp = mp ;
	    ll = (len - 1) ;
	    DEBUGPRINTF("l=>%r<\n", lp,strlinelen(lp,ll,40)) ;
	    while ((ll > 0) && ISWHT(*lp)) {
	        lp += 1 ;
	        ll -= 1 ;
	    } /* end hwile */
	    if ((tp = strnchr(lp,ll,'#')) != nullptr) {
	        ll = intconv(tp - lp) ;
	    }
	    if (ll > 0) {
	        if ((rs = biblecite_ver(&q,lp,ll)) > 0) {
		    cint si = rs ;
	    	    DEBUGPRINTF("biblecite=YES f_ent=%u\n", f_ent) ;
	            if (f_ent) {
	                c += 1 ;
	                if ((rs = ktag_mktag(&e,foff,&t)) >= 0) {
	                    rs = txtindexmk_addtags(tip,&t,1) ;
			}
	                f_ent = false ;
	                ktag_finish(&e) ;
	            } /* end if (entry) */
	            if (rs >= 0) {
	                rs = ktag_start(&e,&ka,foff,(lp+si),(ll-si)) ;
	                if (rs >= 0)
	                    f_ent = true ;
	            } /* end if (ok) */
	        } else {
	    	    DEBUGPRINTF("biblecite=NO f_ent=%u\n", f_ent) ;
	            if (f_ent) {
	                rs = ktag_add(&e,lp,ll) ;
		    }
	        } /* end if (entry start of add) */
	    } else {
		if_constexpr (f_emptyterm) {
	        if (f_ent) {
	            f_ent = false ;
	            c += 1 ;
	            if ((rs = ktag_mktag(&e,foff,&t)) >= 0) {
	                rs = txtindexmk_addtags(tip,&t,1) ;
		    }
	            f_ent = false ;
	            ktag_finish(&e) ;
	        }
		} else {
	        rs = SR_OK ;
		} /* end if_constexpr (f_emptyterm) */
	    } /* end if (not empty) */
	    foff += len ;
	    ml -= len ;
	    mp += len ;
	    if (rs < 0) break ;
	} /* end while (readling lines) */
	DEBUGPRINTF("while-out rs=%d f_ent=%u\n", rs,f_ent) ;
	if ((rs >= 0) && f_ent) {
	    c += 1 ;
	    if ((rs = ktag_mktag(&e,foff,&t)) >= 0) {
	        rs = txtindexmk_addtags(tip,&t,1) ;
	    }
	    f_ent = false ;
	    rs1 = ktag_finish(&e) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ok - entry) */
	if (f_ent) {
	    ktag_finish(&e) ;
	} /* end if (entry) */
	} else {
	    rs = SR_NOANODE ;
	}
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indmkdata) */

local int bibleqs_indclose(BQS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.ind) {
	    op->fl.ind = false ;
	    rs1 = txtindex_close(op->indp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (close) */
	return rs ;
} /* end subroutine (bibleqs_inclose) */

/* make the index */
#if	CF_MKBIBLEQSI
local int bibleqs_mkbibleqsi(BQS *op,cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	char		dbname[MAXPATHLEN + 1] ;

#if	CF_DEBUG
	DEBUGPRINTF("bibleqs_mkbibleqsi: dname=%s\n",dname) ;
#endif

	if (dname == nullptr) return SR_FAULT ;

	if (dname[0] == '\0') return SR_INVALID ;

	if ((rs = mkpath2(dbname,dname,op->dbname)) >= 0) {
	pid_t		cpid = 0 ;
	int		cstat ;
	cchar		*prog = PROG_MKBIBLEQSI ;
	char		pbuf[MAXPATHLEN + 1] ;

	for (int i = 0 ; prbins[i] != nullptr ; i += 1) {
	    if ((rs = mkpath3(pbuf,op->pr,prbins[i],prog)) >= 0) {
	        rs = perm(pbuf,-1,-1,nullptr,X_OK) ;
	    }
	    if (rs >= 0) break ;
	} /* end for */

#if	CF_DEBUG
	DEBUGPRINTF("bibleqs_mkbibleqsi: pr=%s\n",op->pr) ;
	DEBUGPRINTF("bibleqs_mkbibleqsi: pbuf=%s\n",pbuf) ;
	DEBUGPRINTF("bibleqs_mkbibleqsi: perm() rs=%d\n",rs) ;
#endif

	if (rs >= 0) {
	    vecstr	envs ;
	    cint	vo = vecstrm.compact ;

	if ((rs = vecstr_start(&envs,20,vecstrm.compact)) >= 0) {
	    if (rs >= 0) {
	        rs = vecstr_envadd(&envs,VARPRBIBLEQS,op->pr,-1) ;
	    }
	    if (rs >= 0) {
	        rs = vecstr_envadd(&envs,VARDBNAME,dbname,-1) ;
	    }
	    if (rs >= 0) {
		cchar	*cp ;
	        for (int i = 0 ; envchild[i] ; i += 1) {
	            if ((cp = getenv(envchild[i])) != nullptr) {
	                rs = vecstr_envadd(&envs,envchild[i],cp,-1) ;
	            }
		    if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
		    mainv ev ;
	            if ((rs = vecstr_getvec(&envs,&ev)) >= 0) {
			SPAWNPROC	ps{} ;
			cchar	*av[10] ;
	    		int ai = 0 ;
	    		av[ai++] = prog ;
	    		av[ai++] = nullptr ;
	                ps.opts |= SPAWNPROC_OIGNINTR ;
	                ps.opts |= SPAWNPROC_OSETPGRP ;
	                for (int i = 0 ; i < 3 ; i += 1) {
		            switch (i) {
		            case 0:
		            case 1:
	                        ps.disp[i] = SPAWNPROC_DCLOSE ;
		                break ;
		            case 2:
	                        ps.disp[i] = SPAWNPROC_DINHERIT ;
		                break ;
		            } /* end switch */
	                } /* end for */
	                rs = spawnproc(&ps,pbuf,av,ev) ;
	                cpid = rs ;
		    } /* end if (vecstr_getvec) */
	        } /* end if (ok) */
	    } /* end if (ok) */
	    vecstr_finish(&envs) ;
	} /* end if (vecstr) */

	if (rs >= 0) {
	    cstat = 0 ;
	    rs = 0 ;
	    while (rs == 0) {
	        rs = u_waitpid(cpid,&cstat,0) ;
	        if (rs == SR_INTR) rs = 0 ;
	    } /* end while */
	    if (rs >= 0) {
	        int	cex = 0 ;
	        if (WIFSIGNALED(cstat)) rs = SR_UNATCH ;
	        if ((rs >= 0) && WIFEXITED(cstat)) {
	            cex = WEXITSTATUS(cstat) ;
	            if (cex != 0) rs = SR_LIBBAD ;
	        }
	    } /* end if (process finished) */
	} /* end if (ok) */

	} /* end if (ok) */

	} /* end if (mkpath) */

#if	CF_DEBUG
	DEBUGPRINTF("bibleqs_mkbibleqsi: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleqs_mkbibleqsi) */
#endif /* CF_MKBIBLEQSI */

/* does this primary tag have the query keys? */
local int bibleqs_havekeys(BQS *op,TI_TAG *tagp,int qo,SK *skp) noex {
    	cnullptr	np{} ;
	cint		f_prefix = !!(qo & bibleqm.prefix) ; /* <- integer */
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	SK_P	pkeys ;
	if ((rs = searchkeys_popbegin(skp,&pkeys,f_prefix)) >= 0) {
	    BQS_Q	q ;
	    int		c = rs ;
	    int		ml, ll ;
	    cchar	*tp, *mp, *lp ;
	    if (c == 0) f = true ;
	   /* process this tag */
	    mp = (cchar *) (op->dbmdata + tagp->recoff) ;
	    ml = tagp->reclen ;
	    while ((! f) && ((tp = strnchr(mp,ml,'\n')) != np)) {
	        cint len = intconv((tp + 1) - mp) ;
	        lp = mp ;
	        ll = (len - 1) ;
	        if ((tp = strnchr(lp,ll,'#')) != np) {
	            ll = intconv(tp - lp) ;
		}
	        while ((ll > 0) && ISWHT(*lp)) {
	            lp += 1 ;
	            ll -= 1 ;
	        } /* end while */
	        if (ll > 0) {
	            if ((rs = biblecite_ver(&q,lp,ll)) > 0) {
			cint si = rs ;
	                lp += si ;
	                ll -= si ;
	            } /* end if */
	            if (ll > 0) {
	                rs = bibleqs_havekeysline(op,skp,&pkeys,lp,ll) ;
	                f = (rs > 0) ;
	                if (rs < 0) break ;
	                if (f) break ;
	            } /* end if */
	        } /* end if (not empty) */
	        mp += len ;
	        ml -= len ;
		if (rs < 0) break ;
	    } /* end while (readling lines) */
	    rs1 = searchkeys_popend(skp,&pkeys) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (searchkeys-pop) */
	DEBUGPRINTF("ret rs=%d f=%u\n",rs,f) ;
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleqs_havekeys) */

local int bibleqs_havekeysline(BQS *op,SK *skp,
		SK_P *pkp,cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ; 
	int		f = false ;
	DEBUGPRINTF("ent line\n") ;
	DEBUGPRINTF(">%r<\n",lp,ll) ;
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    int		fl, kl ;
	    cchar	*fp, *kp ;
	    char	keybuf[KEYBUFLEN + 1] ;
	    while ((fl = fsb.word(wterms,&fp)) >= 0) {
	        if (fl && (fp[0] == CH_SQUOTE)) {
	            fp += 1 ;
	            fl -= 1 ;
	        }
	        if (fl == 0) continue ;
	        DEBUGPRINTF("fl=%u fp=>%r<\n", fl,fp,fl) ;
	        kl = sfword(fp,fl,&kp) ;
	        DEBUGPRINTF("kl=%u k=>%r<\n", kl,kp,kl) ;
	        if (kl <= 0) continue ;
	        if (kl > KEYBUFLEN) {		/* prevents overflow */
	            kl = KEYBUFLEN ;
		}
	        if (hasuc(kp,kl)) {
	            strwcpylc(keybuf,kp,kl) ;	/* can't overflow */
	            kp = keybuf ;
	        }
	        rs = bibleqs_matchkeys(op,skp,pkp,kp,kl) ;
	        f = (rs > 0) ;
	        DEBUGPRINTF("match? w=>%r< f=%u\n", kp,kl,f) ;
	        if (f) break ;
	        if (rs < 0) break ;
	    } /* end while (fielding words) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	DEBUGPRINTF("ret rs=%d f=%u\n",rs,f) ;
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleqs_havekeysline) */

/* do the keys match? */
local int bibleqs_matchkeys(BQS *op,SK *skp,
		SK_P *pkp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if (op) {
	   /* deal with extra (ex: possessive) words */
	    if_constexpr (f_singleword) {
	        if (xwords xw ; (rs = xw.start(sp,sl)) >= 0) {
	    {
	    rs = searchkeys_processxw(skp,pkp,&xw) ;
	    f = (rs > 0) ;
	    }
	    rs1 = xw.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (xwords) */
	    } else {
	        if (xwords xw ; (rs = xw.start(sp,sl)) >= 0) {
	    int		wi ;
	    int		cl ;
	    cchar	*cp ;
	    f = false ;
	    for (wi = 0 ; ((cl = xw.get(wi,&cp)) > 0) ; wi += 1) {
	        DEBUGPRINTF("xwords_get() rs=%d\n",cl) ;
	        if (cl >= 0) {
	            DEBUGPRINTF("c=>%r<\n",cp,cl) ;
		}
	        rs1 = searchkeys_process(skp,pkp,cp,cl) ;
	        f = (rs1 > 0) ;
	        DEBUGPRINTF("searchkeys_process() f=%u\n", f) ;
	        if (f) break ;
	    } /* end for (matching words) */
	    rs1 = xw.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (xwords) */
	} /* end if (f_singleword) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d f=%u\n",rs,f) ;
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleqs_matchkeys) */

local int bibleqs_loadbuf(BQS *op,uint recoff,char *vbuf,int vlen) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		ml = intconv(op->dbmsize - recoff) ;
	int		len = 0 ; /* return-value */
	cchar		*mp = ccharp(op->dbmdata + recoff) ;
	DEBUGPRINTF("ent\n") ;
	if (sbuf b ; (rs = b.start(vbuf,vlen)) >= 0) {
	    int		ll ;
	    int		j = 0 ;
	    cchar	*lp ;
	    for (cc *tp ; (tp = strnchr(mp,ml,'\n')) != np ; ) {
	        len = intconv((tp + 1) - mp) ;
	        lp = mp ;
	        ll = (len - 1) ;
	        if (ll <= 0) break ;
	        DEBUGPRINTF("line=>%r<¬\n", lp,lenstr(lp,MIN(ll,40))) ;
	        if (j++ > 0) {
	            rs = b.chr(' ') ;
		}
	        if (rs >= 0) {
	            rs = b.strw(lp,ll) ;
		}
	        ml -= intconv((tp + 1) - mp) ;
	        mp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    len = b.finish ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	DEBUGPRINTF("ret rs=%d len=%u\n",rs,len) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (bibleqs_loadbuf) */

local int bibleqs_eigenopen(BQS *op) noex {
    	int		rs = SR_OK ;
	int		rs1 = 0 ;
	int		f = false ; /* return-value */
	if (! op->fl.edbinit) {
	    op->fl.edbinit = true ;
	    rs1 = eigenfind(op->edbp,op->pr,op->dbname,op->minwlen) ;
	    op->fl.edb = (rs1 > 0) ;
	    f = op->fl.edb ;
	} /* end if */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleqs_eigenopen) */

local int bibleqs_eigenclose(BQS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.edb) {
	    op->fl.edb = false ;
	    rs1 = eigendb_close(op->edbp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (eigen-db close) */
	return rs ;
} /* end subroutine (bibleqs_eigenclose) */

local int bibleqs_curlooks(BQS *op,BQS_C *curp,int qo,SK *skp,VS *hkp) noex {
    	cint		vn = 10 ;
	cint		vo = 0 ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (vecint recoffs ; (rs = recoffs.start(vn,vo)) >= 0) {
	    if (mainv hkeya ; (rs = hkp->getvec(&hkeya)) >= 0) {
		if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	            TI_CUR	tcur ;
		    cint	tlen = rs ;
	            if ((rs = txtindex_curbegin(op->indp,&tcur)) >= 0) {
	                TI_TAG	ttag ;
			cauto	ce =  txtindex_curenum ;
	                cauto	hk =  bibleqs_havekeys ;
		        int	ntags ;
		        {
	                    rs = txtindex_curlook(op->indp,&tcur,hkeya) ;
	                    ntags = rs ;
		        }
	                while ((rs >= 0) && (ntags-- > 0)) {
	                    rs1 = ce(op->indp,&tcur,&ttag,tbuf,tlen) ;
	                    if (rs1 == SR_NOTFOUND) break ;
	                    rs = rs1 ;
	                    if (rs >= 0) {
	                        if ((rs = hk(op,&ttag,qo,skp)) > 0) {
	                            c += 1 ;
	                            rs = recoffs.add(ttag.recoff) ;
	                        }
	                    } /* end if (ok) */
	                } /* end while */
	                rs1 = txtindex_curend(op->indp,&tcur) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (cursor) */
		    rs1 = mem.free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
		/* sort the secondary tags */
	        if ((rs >= 0) && (c > 1)) {
	            rs = recoffs.sort ;
		}
		/* store results (file-record offsets) */
	        if (rs >= 0) {
	            cint	sz = (c + 1) * szof(uint) ;
		    voidp vp ;
	            if ((rs = mem.mall(sz,&vp)) >= 0) {
	                curp->verses = resumelife<uint>(vp) ;
	                int *a ;
	                if ((rs = recoffs.getvec(&a)) >= 0) {
	                    curp->nverses = c ;
	                    memcopy(curp->verses,a,sz) ;
	                    curp->verses[c] = UINT_MAX ;
		        } /* end if (vecint_getvec) */
	            } else {
	                curp->verses = nullptr ;
		    }
	        } /* end if (ok) */
	    } /* end if (getvec) */
	    rs1 = recoffs.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (recoffs) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_curlooks) */

local int bibleqs_mkhkeys(BQS *op,vecstr *hkp,SK *skp) noex {
	int		rs ;
	int		rs1 ;
	int		nkeys = 0 ; /* return-value */
	if (SK_C cur ; (rs = searchkeys_curbegin(skp,&cur)) >= 0) {
	    int		kl ;
	    cchar	*kp ;
	    while (rs >= 0) {
	        kl = searchkeys_curenum(skp,&cur,&kp) ;
	        if (kl == SR_NOTFOUND) break ;
	        rs = kl ;
	        if (rs < 0) break ;
	        if (kp == nullptr) continue ;
	        if (kl < op->minwlen) continue ;
	        rs1 = SR_NOTFOUND ;
		if_constexpr (f_extraeigen) {
	            if (op->fl.edb) {
	                rs1 = eigendb_exists(op->edbp,kp,kl) ;
		    }
		} /* end if_constexpr (f_extraeigen) */
	        if (rs1 == SR_NOTFOUND) {
	            rs = vecstr_adduniq(hkp,kp,kl) ;
	            if (rs < INT_MAX)
	                nkeys += 1 ;
	        } /* end if (not found) */
	        if (rs < 0) break ;
	    } /* end while (enumerating search-keys) */
	    rs1 = searchkeys_curend(skp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return (rs >= 0) ? nkeys : rs ;
} /* end subroutine (bibleqs_mkhkeys) */

local int subinfo_start(SI *sip) noex {
	int		rs = SR_OK ;
	memclear(sip) ;
	sip->dt = time(nullptr) ;
	return rs ;
} /* end subroutine (subinfo_start) */

#ifdef	COMMENT
local int subinfo_ids(SI *sip) noex {
	int		rs = SR_OK ;
	if (! sip->fl.id) {
	    sip->fl.id = true ;
	    rs = ids_load(&sip->id) ;
	}
	return rs ;
} /* end subroutine (subinfo_ids) */
#endif /* COMMENT */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->fl.id) {
	    sip->fl.id = false ;
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (subinfo_finish) */

local int mkdname(cchar *dname,mode_t dm) noex {
	cint		nrs = SR_NOENT ;
	int		rs ;
	if ((rs = checkdname(dname)) == nrs) {
	    rs = mkdirs(dname,dm) ;
	}
	return rs ;
} /* end subroutine (mkdname) */

local int checkdname(cchar *dname) noex {
	int		rs = SR_INVALID ;
	DEBUGPRINTF("ent dname=%s\n",dname) ;
	if (dname[0] == '/') {
	    if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) {
		rs = SR_NOTDIR ;
		if (S_ISDIR(sb.st_mode)) {
	    	    rs = perm(dname,-1,-1,nullptr,W_OK) ;
		} /* end if (directory) */
	    } /* end if (u_stat) */
	} /* end if */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (checkdname) */

local int eigenfinds(EDB *edbp,cc *,ids *idp,EC *ecp) noex ;

local int eigenfind(EDB *edbp,cc *pr,cc *dbname,int minwlen) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	(void) minwlen ;
	if (ids id ; (rs = id.load) >= 0) {
	    if (EC ck ; (rs = ck.start) >= 0) {
	        if ((rs = ck.add("n",dbname,-1)) >= 0) {
	            if ((rs = ck.add("f","eign",-1)) >= 0) {
			rs = eigenfinds(edbp,pr,&id,&ck) ;
		    }
		} /* end if */
	        rs1 = ck.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (expcook) */
	    rs1 = id.release ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (eigenfind) */

local int eigenfinds(EDB *edbp,cc *pr,ids *idp,EC *ecp) noex {
	cint		psz = (2 * (var.maxpathlen + 1)) ;
	cint		maxpath = var.maxpathlen ;
	int		ai = 2 ; /* two path buffers */
	int		rs ;
	int		rs1 ;
	if (char *a ; (rs = mem.mall(psz,&a)) >= 0) {
	    cint elen = maxpath ;
	    char *tbuf = (a + (--ai * (maxpath + 1))) ;
	    char *ebuf = (a + (--ai * (maxpath + 1))) ;
	    rs = SR_NOTOPEN ;
	    ebuf[0] = '\0' ;
	    for (int i = 0 ; eigenfnames[i] ; i += 1) {
	        cchar *efp = eigenfnames[i] ;
	        int   efl = -1 ;
	        rs = SR_OK ;
	        if (efp[0] != '/') {
	            if ((rs = mkpath2(tbuf,pr,efp)) > 0) {
	                efp = tbuf ;
	                efl = rs ;
		    } else if (rs == 0) {
	                rs = SR_NOENT ;
	                efp = nullptr ;
		    }
	        } /* end if */
	        if (rs >= 0) {
	            if ((rs = ecp->exp(0,ebuf,elen,efp,efl)) >= 0) {
		        if (ustat sb ; (rs = u_stat(ebuf,&sb)) >= 0) {
			    rs = SR_ISDIR ;
			    if (! S_ISDIR(sb.st_mode)) {
	        	        rs = permid(idp,&sb,R_OK) ;
			    }
		        } /* end if (u_stat) */
		    } /* end if (expcook_exp) */
	        } /* end if (ok) */
	        if (rs >= 0) break ;
	    } /* end for */
	    if (rs >= 0) {
	        rs = edbp->open(ebuf) ;
	    }
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (eigenfinds) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mn)) >= 0) {
	    maxnamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	        maxpathlen = rs ;
		rs = mkfieldterms(wterms) ;
	    } /* end if (bufsizeget) */
	} /* end if (bufsizeget) */
    	return rs ;
} /* end if (vars::operator) */

local int mkfieldterms(char *terms) noex {
    	cint		n = fieldterms_termsize ;
    	int		rs = SR_FAULT ;
	if (terms) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < n ; i += 1) {
	        terms[i] = char(UCHAR_MAX) ;
	    } /* end for */
	    for (int i = 0 ; i < nchars ; i += 1) {
	        if (isalnumlatin(i)) {
	            BACLR(terms,i) ;
	        }
	    } /* end for */
	    BACLR(terms,CH_SQUOTE) ;		/* allow apostrophes */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkfieldterms) */

local bool isNeedIndex(int rs) noex {
	bool		f = false ;
	f = f || isOneOf(rsneeds,rs) ;
	f = f || isNotPresent(rs) ;
	return f ;
} /* end subroutine (isNeedIndex) */


