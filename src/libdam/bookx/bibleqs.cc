/* bibleqs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* bible-query database manager */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUGSTART	0		/* debug |isstart()| */
#define	CF_DEBUGN	0		/* special debugging */
#define	CF_DEBUGPW	0		/* debug |ktag_procword()| */
#define	CF_DEBUGPL	0		/* debug |ktac_procline()| */
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
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<estrings.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<vecint.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<eigendb.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<dirseen.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<ascii.h>		/* LIBU */
#include	<field.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	<naturalwords.h>	/* LIBDAM */
#include	<txtindexmk.h>		/* LIBDAM */
#include	<txtindex.h>		/* LIBDAM */
#include	<xwords.h>		/* LIBDAM */
#include	<searchkeys.h>		/* LIBDAM */

#include	"bibleqs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BIBLEQS_NVERSES	33000
#define	BIBLEQS_MINWLEN	2		/* minimum word-length */
#define	BIBLEQS_MAXWLEN	6		/* more chrs => less collisions? */
#define	BIBLEQS_NEIGEN	2000		/* number of keys in chunk */
#define	BIBLEQS_DIRMODE	0777		/* parent directory */
#define	BIBLEQS_IDXMODE	0664		/* the index files */
#define	BIBLEQS_DBDNAME	"share/bibledbs"
#define	BIBLEQS_DBNAME	"av"

#define	SI		subinfo
#define	SI_FL		subinfo_fl

#define	KTAG		ktag_head
#define	KTAG_PARAMS	ktag_params
#define	KTAG_KEY	TXTINDEXMK_KEY

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

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#undef	VARDBNAME
#define	VARDBNAME	"MKBIBLEQSI_DBNAME"

#undef	VARPRBIBLEQS
#define	VARPRBIBLEQS	"MKBIBLEQSI_PROGRAMROOT"

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#ifndef	VCNAME
#define	VCNAME		"var"
#endif

#define	INDDNAME	"bibleqs"

#define	DBSUF		"txt"
#define	INDSUF		"hash"
#define	TAGSUF		"tag"

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)

#define	PROG_MKBIBLEQSI	"mkbibleqsi"

#define	NDF		"/tmp/bibleqs.deb"


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

struct ktag_params {
	EIGENDB		*edbp ;
	uchar		*wterms ;
	int		minwlen ;
	int		f_eigen ;
} ; /* end struct */

struct ktag_head {
	KTAG_PARAMS	*kap ;
	TXTINDEXMK_KEY	*tkeys ;	/* storage for TXTMKINDEXMK_ADDTAGS */
	char		*fname ;
	vecobj		keys ;
	vecstr		store ;
	ulong		recoff ;
	ulong		reclen ;
	int		f_store ;
} ; /* end struct */


/* forward references */

local int	bibleqs_infoloadbegin(BIBLEQS *,cchar *,cchar *) ;
local int	bibleqs_infoloadend(BIBLEQS *) ;
local int	bibleqs_indopen(BIBLEQS *,SI *) ;

local int	bibleqs_indclose(BIBLEQS *) ;
local int	bibleqs_indmk(BIBLEQS *,cchar *,time_t) ;
local int	bibleqs_indmkeigen(BIBLEQS *,TXTINDEXMK *) ;
local int	bibleqs_indmkdata(BIBLEQS *,TXTINDEXMK *) ;
local int	bibleqs_dbmapcreate(BIBLEQS *,time_t) ;
local int	bibleqs_dbmapdestroy(BIBLEQS *) ;
local int	bibleqs_havekeys(BIBLEQS *,TXTINDEX_TAG *,int,SEARCHKEYS *) ;
local int	bibleqs_havekeysline(BIBLEQS *,SEARCHKEYS *,SEARCHKEYS_POP *,
			cchar *,int) ;
local int	bibleqs_matchkeys(BIBLEQS *,SEARCHKEYS *,SEARCHKEYS_POP *,
			cchar *,int) ;
local int	bibleqs_loadbuf(BIBLEQS *,uint,char *,int) ;
local int	bibleqs_mkhkeys(BIBLEQS *,vecstr *,SEARCHKEYS *) ;
local int	bibleqs_indopenseq(BIBLEQS *,SI *) ;
local int	bibleqs_indopenseqer(BIBLEQS *,SI *,DIRSEEN *,EXPCOOK *) ;
local int	bibleqs_indopencheck(BIBLEQS *,cchar *) ;
local int	bibleqs_indopenmk(BIBLEQS *,SI *,cchar *) ;

local int	bibleqs_loadcooks(BIBLEQS *,EXPCOOK *) ;
local int	bibleqs_dirok(BIBLEQS *,DIRSEEN *,ids *,cchar *,int) ;
local int	bibleqs_mkdir(BIBLEQS *,cchar *) ;

#if	CF_MKBIBLEQSI
local int	bibleqs_mkbibleqsi(BIBLEQS *,cchar *) ;
#endif

#if	CF_EXTRASTRONG
local int	bibleqs_eigenopen(BIBLEQS *) ;
local int	bibleqs_eigenclose(BIBLEQS *) ;
#endif

local int	bibleqs_lookuper(BIBLEQS *,BIBLEQS_CUR *,int,
			SEARCHKEYS *,vecstr *) ;

#ifdef	COMMENT
local int	bibleqs_mksysvarsi(BIBLEQS *,cchar *) ;
#endif

local int	subinfo_start(SI *) ;
local int	subinfo_finish(SI *) ;

#ifdef	COMMENT
local int	subinfo_ids(SI *) ;
#endif

local int	ktag_start(KTAG *,KTAG_PARAMS *,size_t,cchar *,int) ;
local int	ktag_add(KTAG *,cchar *,int) ;
local int	ktag_procline(KTAG *,cchar *,int) ;
local int	ktag_mktag(KTAG *,size_t,TXTINDEXMK_TAG *) ;
local int	ktag_finish(KTAG *) ;
local int	ktag_procword(KTAG *,cchar *,int) ;
local int	ktag_storelc(KTAG *,cchar **,cchar *,int) ;

local int	mkdname(cchar *,mode_t) ;
local int	checkdname(cchar *) ;

#if	CF_EXTRASTRONG
local int	eigenfind(EIGENDB *,cchar *,cchar *,int) noex ;
#endif

local int	isstart(cchar *,int,BIBLEQS_Q *,int *) ;
local int	mkfieldterms(uchar *) noex ;

local int	vesrch(cvoid *,cvoid *) noex ;
local int	vcmpint(cint *,cint *) noex ;

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

#if	CF_EXTRASTRONG

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

#else /* CF_EXTRASTRONG */

/* these are not likely to change since their publication in 1890! */
constexpr cpcchar	strongseigens[] = {
	"a", "an", "and", "are", "as", "be", "but", "by", "for",
	"from", "he", "her", "him", "his", "i", "in", "is", "it",
	"me", "my", "not", "o", "of", "our", "out", "shall", "shalt",
	"she", "that", "the", "thee", "their", "them", "they",
	"thou", "thy", "to", "unto", "up", "upon", "us", "was",
	"we", "were", "with", "ye", "you",
	nullptr
} ; /* end array (strongseigens) */

#endif /* CF_EXTRASTRONG */

constexpr int	rsneeds[] = {
	SR_STALE,
	0
} ; /* end array (rsneeds) */


/* exported variables */

extern const bibleqs_obj	bibleqs_modinfo = {
	"bibleqs",
	szof(bibleqs),
	szof(bibleqs_cur)
} ; /* end initialization */


/* exported subroutines */

int bibleqs_open(BIBLEQS *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("bibleqs_open: dbname=%s\n",dbname) ;
#endif

	memclear(op) ;
	subinfo		si ;
	if ((rs = subinfo_start(&si)) >= 0) {
	    op->minwlen = BIBLEQS_MINWLEN ;
	    if ((rs = bibleqs_infoloadbegin(op,pr,dbname)) >= 0) {
		if ((rs = bibleqs_dbmapcreate(op,si.dt)) >= 0) {
		    mkfieldterms(op->wterms) ;
		    if ((rs = bibleqs_indopen(op,&si)) >= 0) {
			op->magval = BIBLEQS_MAGIC ;
		    }
		    if (rs < 0) {
			bibleqs_dbmapdestroy(op) ;
		    }
	        } /* end if (bibleqs_dbmapcreate) */
		if (rs < 0) {
		    bibleqs_infoloadend(op) ;
		}
	    } /* end if (bibleqs_infoloadbegin) */
	    rs1 = subinfo_finish(&si) ;
	    if (rs >= 0) rs = rs1 ;
	    if (rs < 0) {
		bibleqs_close(op) ;
	    }
	} /* end if (subinfo) */

#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_open: ret rs=%d\n",rs) ;
#endif

#if	CF_DEBUGS
	debugprintf("bibleqs_open: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleqs_open) */

int bibleqs_close(BIBLEQS *op) {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQS_MAGIC) return SR_NOTOPEN ;

	{
	rs1 = bibleqs_indclose(op) ;
	if (rs >= 0) rs = rs1 ;
	}
#if	CF_EXTRASTRONG
	{
	rs1 = bibleqs_eigenclose(op) ;
	if (rs >= 0) rs = rs1 ;
	}
#endif
	{
	rs1 = bibleqs_dbmapdestroy(op) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = bibleqs_infoloadend(op) ;
	if (rs >= 0) rs = rs1 ;
	}
#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_close: ret rs=%d\n",rs) ;
	debugprintf("bibleqs_close: ret rs=%d\n",rs) ;
#endif

	op->magval = 0 ;
	return rs ;
} /* end subroutine (bibleqs_close) */

int bibleqs_count(BIBLEQS *op) {
	int		rs = SR_NOTOPEN ;

#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_count: ent\n") ;
#endif

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQS_MAGIC) return SR_NOTOPEN ;

	if (op->fl.ind) {
	   rs = txtindex_count(&op->ind) ;
	}

#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_count: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleqs_count) */

int bibleqs_audit(BIBLEQS *op) {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQS_MAGIC) return SR_NOTOPEN ;

	rs = txtindex_audit(&op->ind) ;

#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_audit: ret rs=%d\n",rs) ;
#endif

#if	CF_DEBUGS
	debugprintf("bibleqs_audit: txtindex_audit() rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleqs_audit) */

int bibleqs_curbegin(BIBLEQS *op,BIBLEQS_CUR *curp) {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQS_MAGIC) return SR_NOTOPEN ;

	memclear(curp) ;
	op->ncursors += 1 ;

#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_curbegin: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleqs_curbegin) */

int bibleqs_curend(BIBLEQS *op,BIBLEQS_CUR *curp) {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQS_MAGIC) return SR_NOTOPEN ;

	if (curp->verses != nullptr) {
	    rs1 = uc_free(curp->verses) ;
	    if (rs >= 0) rs = rs1 ;
	    curp->verses = nullptr ;
	}

	curp->nverses = 0 ;
	if (op->ncursors > 0)
	    op->ncursors -= 1 ;

#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_curend: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleqs_curend) */

int bibleqs_curlook(BIBLEQS *op,BIBLEQS_CUR *curp,int qo,cchar **qsp) {
	SEARCHKEYS	sk ;
	vecstr		hkeys ;			/* hash-keys */
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (qsp == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQS_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	{
	    int	i ;
	    for (i = 0 ; qsp[i] != nullptr ; i += 1)
		debugprintf("bibleqs_lookup: qs=>%s<\n",qsp[i]) ;
	}
#endif /* CF_DEBUGS */

	curp->nverses = 0 ;
	if (curp->verses != nullptr) {
	    uc_free(curp->verses) ;
	    curp->verses = nullptr ;
	}

	if ((rs = searchkeys_start(&sk,qsp)) >= 0) {
	    cint	vopts = (vecstrm.compact) ;
	    if ((rs = vecstr_start(&hkeys,10,vopts)) >= 0) {
	        if ((rs = bibleqs_mkhkeys(op,&hkeys,&sk)) >= 0) {
		    rs = bibleqs_lookuper(op,curp,qo,&sk,&hkeys) ;
		    c = rs ;
	        } /* end if (bibleqs_mkhkeys) */
	        rs1 = vecstr_finish(&hkeys) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr_start) */
	    rs1 = searchkeys_finish(&sk) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (searchkeys) */

#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_lookup: ret rs=%d\n",rs) ;
#endif

#if	CF_DEBUGS
	debugprintf("bibleqs_lookup: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_curlook) */

int bibleqs_curenum(BIBLEQS *op,BIBLEQS_CUR *curp,BIBLEQS_Q *citep,
		char *vbuf,int vlen) {
	int		rs = SR_OK ;
	int		len = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (citep == nullptr) return SR_FAULT ;
	if (vbuf == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEQS_MAGIC) return SR_NOTOPEN ;

	if ((curp->nverses > 0) && (curp->verses != nullptr)) {
	    uint	recoff ;
	    int		ei = (curp->i >= 0) ? curp->i : 0 ;
	    int		si ;
	    int		ml ;
	    cchar	*mp ;
#if	CF_DEBUGS
	debugprintf("bibleqs_read: c_i=%u\n",ei) ;
#endif
	recoff = curp->verses[ei] ;
	if ((ei < curp->nverses) && (recoff != UINT_MAX)) {

#if	CF_DEBUGS
	    debugprintf("bibleqs_read: recoff=%u\n",recoff) ;
#endif

	    mp = (cchar *) (op->dbmdata + recoff) ;
	    ml = (op->dbmsize - recoff) ;

#if	CF_DEBUGS
	    debugprintf("bibleqs_read: line=>%r<\n",
	        mp,strnlen(mp,MIN(ml,40))) ;
#endif

	    if (isstart(mp,ml,citep,&si)) {
	        recoff += si ;
	        mp += si ;
	        ml -= si ;
	        if ((ml > 0) && (mp[0] == '\n')) {
	            recoff += 1 ;
	        }
	    }

#if	CF_DEBUGS
	debugprintf("bibleqs_read: _loadbuf() \n") ;
#endif

	    rs = bibleqs_loadbuf(op,recoff,vbuf,vlen) ;
	    len = rs ;

#if	CF_DEBUGS
	debugprintf("bibleqs_read: _loadbuf() rs=%d\n",rs) ;
#endif

	} else {
	    rs = SR_NOTFOUND ;
	}
	if (rs >= 0) {
	    curp->i = (ei + 1) ;
	}

	} else {
	    rs = SR_NOTFOUND ;
	}

#if	CF_DEBUGN
	nprintf(NDF,"bibleqs_read: ret rs=%d\n",rs) ;
#endif

#if	CF_DEBUGS
	debugprintf("bibleqs_read: ret rs=%d len=%u\n",rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
} /* end subroutine (bibleqs_curenum) */


/* private subroutines */

local int bibleqs_infoloadbegin(BIBLEQS *op,cchar *pr,cchar *dbname) {
	int		rs ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if ((dbname == nullptr) || (dbname[0] == '\0'))
	    dbname = BIBLEQS_DBNAME ;

	op->pr = pr ;
	op->dbname = dbname ;
	if ((rs = mkpath3(tmpfname,pr,BIBLEQS_DBDNAME,dbname)) >= 0) {
	    char	dbfname[MAXPATHLEN + 1] ;
	    if ((rs = mkfnamesuf1(dbfname,tmpfname,DBSUF)) >= 0) {
		cchar	*cp ;
		int		fnl = rs ;
#if	CF_DEBUGS
	        debugprintf("bibleqs_infoloadbegin: dbfname=%s\n",dbfname) ;
#endif
	        if ((rs = uc_mallocstrw(dbfname,fnl,&cp)) >= 0) {
		    op->dbfname = cp ;
	            if (ustat sb ; (rs = u_stat(op->dbfname,&sb)) >= 0) {
	                if (S_ISREG(sb.st_mode)) {
	                    rs = perm(op->dbfname,-1,-1,nullptr,R_OK) ;
	                } else {
	                    rs = SR_NOTSUP ;
			}
	            } /* end if (stat) */
		    if (rs < 0) {
		        uc_free(op->dbfname) ;
		        op->dbfname = nullptr ;
		    } /* end if (error) */
	        } /* end if (m-a) */
	    } /* end if (mkfnamesuf) */
	} /* end if (mkpath) */

	return rs ;
} /* end subroutine (bibleqs_infoloadbegin) */

local int bibleqs_infoloadend(BIBLEQS *op) {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->dbfname != nullptr) {
	    rs1 = uc_free(op->dbfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbfname = nullptr ;
	}

	op->pr = nullptr ;
	op->dbname = nullptr ;
	return rs ;
} /* end subroutine (bibleqs_infoloadend) */

local int bibleqs_dbmapcreate(BIBLEQS *op,time_t dt) {
    	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	debugprintf("bibleqs_dbmapcreate: dbfname=%s\n",op->dbfname) ;
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
	                if ((rs = u_mmap(np,ms,mp,mf,fd,0z,&md)) >= 0) {
		            cint	madv = MADV_RANDOM ;
		            const caddr_t	ma = md ;
	                    if ((rs = u_madvise(ma,ms,madv)) >= 0) {
	                        op->dbmdata = md ;
	                        op->dbmsize = ms ;
	                        op->ti_map = dt ;
	                        op->ti_lastcheck = dt ;
	                    }
	                    if (rs < 0) {
		                u_munmap(md,ms) ;
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
	return rs ;
} /* end subroutine (bibleqs_dbmapcreate) */

local int bibleqs_dbmapdestroy(BIBLEQS *op) {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->dbmdata) {
	    rs1 = u_munmap(op->dbmdata,op->dbmsize) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbmdata = nullptr ;
	    op->dbmsize = 0 ;
	} /* end if */
	return rs ;
} /* end subroutine (bibleqs_dbmapdestroy) */

local int bibleqs_indopen(BIBLEQS *op,SI *sip) {
	int		rs ;

	rs = bibleqs_indopenseq(op,sip) ;

#if	CF_DEBUGS
	debugprintf("bibleqs_indopen: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleqs_indopen) */

local int bibleqs_indopenseq(BIBLEQS *op,SI *sip) {
	DIRSEEN		ds ;
	int		rs ;
	int		rs1 ;

	if ((rs = dirseen_start(&ds)) >= 0) {
	        EXPCOOK	cooks ;
	        if ((rs = expcook_start(&cooks)) >= 0) {
	            if ((rs = bibleqs_loadcooks(op,&cooks)) >= 0) {
	                rs = bibleqs_indopenseqer(op,sip,&ds,&cooks) ;
	            }
		    rs1 = expcook_finish(&cooks) ;
		    if (rs < 0) rs = rs1 ;
		} /* end if (cooks) */
	    rs1 = dirseen_finish(&ds) ;
	    if (rs < 0) rs = rs1 ;
	} /* end if (ds) */

	return rs ;
} /* end subroutines (bibleqs_indopenseq) */

local int bibleqs_indopenseqer(BIBLEQS *op,SI *sip,
		DIRSEEN *dsp,EXPCOOK *ckp) {
	ids		id ;
	cint	elen = MAXPATHLEN ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

#if	CF_DEBUGS
	debugprintf("bibleqs_indopenseqer: ent\n") ;
#endif

/* first phase: expand possible directory paths */

	if ((rs = ids_load(&id)) >= 0) {
	    int		i ;
	    char	ebuf[MAXPATHLEN + 1] ;
	    char	pbuf[MAXPATHLEN + 1] ;
	    for (i = 0 ; (rs >= 0) && (idxdirs[i] != nullptr) ; i += 1) {
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
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */

#if	CF_DEBUGS
	debugprintf("bibleqs_indopenseqer: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutines (bibleqs_indopenseqer) */

local int bibleqs_dirok(BIBLEQS *op,DIRSEEN *dsp,ids *idp,
		cchar *dp,int dl) {
	cint	rsn = SR_NOTFOUND ;
	int		rs ;
	int		f_ok = false ;
	if ((rs = dirseen_havename(dsp,dp,dl)) == rsn) {
	    USTAT	sb ;
	    if ((rs = uc_stat(dp,&sb)) >= 0) {
		if ((rs = dirseen_havedevino(dsp,&sb)) == rsn) {
		    cint	am = (W_OK|R_OK|X_OK) ;
		    if ((rs = permid(idp,&sb,am)) >= 0) {
			f_ok = TRUE ;
		    } else if (isNotPresent(rs)) {
			rs = dirseen_add(dsp,dp,dl,&sb) ;
		    }
		}
	    } else if (isNotPresent(rs)) {
		if ((rs = bibleqs_mkdir(op,dp)) > 0) {
		    f_ok = TRUE ;
		}
	    }
	} /* end if (dirseen_havename) */

	return (rs >= 0) ? f_ok : rs ;
} /* end subroutine (bibleqs_dirok) */

local int bibleqs_mkdir(BIBLEQS *op,cchar *dp) {
	cmode	dm = 0777 ;
	int		rs ;
	int		f_ok = false ;
	if ((rs = mkdirs(dp,dm)) >= 0) {
	     if ((rs = uc_minmod(dp,dm)) >= 0) {
		if ((rs = chownsame(dp,op->pr)) >= 0) {
	            f_ok = TRUE ;
	        }
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f_ok : rs ;
} /* end subroutine (bibleqs_mkdir) */

local int bibleqs_loadcooks(BIBLEQS *op,EXPCOOK *ecp) {
	int		rs = SR_OK ;
	int		i ;
	int		kch ;
	int		vl ;
	cchar		*tmpdname = getenv(VARTMPDNAME) ;
	cchar		*ks = "RST" ;
	cchar		*vp ;
	char		kbuf[2] ;

	if (tmpdname == nullptr) tmpdname = TMPDNAME ;

	kbuf[1] = '\0' ;
	for (i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	    kch = MKCHAR(ks[i]) ;
	    vp = nullptr ;
	    vl = -1 ;
	    switch (kch) {
	    case 'R':
		vp = op->pr ;
		break ;
	    case 'S':
		vp = INDDNAME ;
		break ;
	    case 'T':
		vp = tmpdname ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && (vp != nullptr)) {
		kbuf[0] = kch ;
		rs = expcook_add(ecp,kbuf,vp,vl) ;
	    }
	} /* end for */

	if (rs >= 0) {
	    cchar	*prname ;
	    if ((rs = sfbasename(op->pr,-1,&prname)) >= 0) {
	        rs = SR_NOENT ;
	        if (prname != nullptr) {
	            rs = expcook_add(ecp,"PRN",prname,-1) ;
		}
	    }
	}

	return rs ;
} /* end subroutines (bibleqs_loadcooks) */

local int bibleqs_indopencheck(BIBLEQS *op,cchar *idir) {
	int		rs ;
	int		c = 0 ;
	char		tbuf[MAXPATHLEN+1] ;

#if	CF_DEBUGS
	debugprintf("bibleqs_indopencheck: ent idxdir=%s\n",idir) ;
#endif

	if ((rs = mkpath2(tbuf,idir,op->dbname)) >= 0) {
	    if ((rs = txtindex_open(&op->ind,op->pr,tbuf)) >= 0) {
	        TXTINDEX_INFO	tinfo ;
		c = rs ;
	        op->fl.ind = TRUE ;
#if	CF_DEBUGS
	        debugprintf("bibleqs_indopencheck: txtindex_open() rs=%d\n",
			rs) ;
#endif
	        if ((rs = txtindex_getinfo(&op->ind,&tinfo)) >= 0) {
	            if (tinfo.ctime < op->ti_db) rs = SR_STALE ;
	        } /* end if (txtindex_getinfo) */
	        if (rs < 0) {
		    op->fl.ind = false ;
	            txtindex_close(&op->ind) ;
		}
	    } /* end if (txtindex_open) */
	} /* end if (mkpath) */

#if	CF_DEBUGS
	debugprintf("bibleqs_indopencheck: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indopencheck) */

local int bibleqs_indopenmk(BIBLEQS *op,SI *sip,cchar *idir) {
	int		rs ;
	int		c = 0 ;

	if ((rs = bibleqs_indmk(op,idir,sip->dt)) >= 0) {
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpath2(tbuf,idir,op->dbname)) >= 0) {
	        rs = txtindex_open(&op->ind,op->pr,tbuf) ;
		c = rs ;
	        op->fl.ind = (rs >= 0) ;
#if	CF_DEBUGS
	        debugprintf("bibleqs_indopenalt: txtindex_open() rs=%d\n",
	            rs) ;
#endif
	    } /* end if (mkpath) */
	} /* end if (bibleqs_indmk) */

	return (rs >= 0) ? c : rs ;
} /* end subroutines (bibleqs_indopenmk) */

local int bibleqs_indmk(BIBLEQS *op,cchar *dname,time_t dt) {
	cmode	dm = BIBLEQS_DIRMODE ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

#if	CF_DEBUGS
	debugprintf("bibleqs_indmk: ent dname=%s\n",dname) ;
#endif

	if ((rs = mkdname(dname,dm)) >= 0) {
	    char	indname[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(indname,dname,op->dbname)) >= 0) {
		TXTINDEXMK	mk ;
		TXTINDEXMK_PA	ta{} ;
		cmode	om = BIBLEQS_IDXMODE ;
		cint	of = 0 ; /* auto-make */

		ta.tablen = 0 ;			/* use default! */
		ta.minwlen = op->minwlen ;
		ta.maxwlen = BIBLEQS_MAXWLEN ;
		ta.sfn = op->dbfname ;

		if ((rs = txtindexmk_open(&mk,&ta,indname,of,om)) >= 0) {
		    if (rs == 0) {
		        if ((rs = bibleqs_indmkeigen(op,&mk)) >= 0) {
	    		    if ((rs = bibleqs_indmkdata(op,&mk)) >= 0) {
	    		        op->ti_tind = dt ;
			        c += rs ;
			    }
		        }
		    } else {
			c = rs ;
		    }
		    rs1 = txtindexmk_close(&mk) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (txtindexmk) */

	    } /* end if (mkpath) */
	} /* end if (mkdname) */

#if	CF_DEBUGS
	debugprintf("bibleqs_indmk: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indmk) */

#if	CF_EXTRASTRONG

local int bibleqs_indmkeigen(BIBLEQS *op,TXTINDEXMK *tip) {
	EIGENDB_CUR	ecur ;
	EIGENDB		*edbp = &op->edb ;
	TXTINDEXMK_KEY	*keys = nullptr ;
	cint	nkeys = BIBLEQS_NEIGEN ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

#if	CF_DEBUGS
	debugprintf("bibleqs_indmkeigen: f_edb=%u\n",op->fl.edb) ;
#endif

	if (op->fl.edb) {
	    cint	size = (nkeys + 1) * szof(TXTINDEXMK_KEY) ;
	if ((rs = uc_malloc(size,&keys)) >= 0) {
	    int	i = 0 ;
	int		wl ;
	cchar	*wp ;

	    if ((rs = eigendb_curbegin(edbp,&ecur)) >= 0) {

	        while ((wl = eigendb_enum(edbp,&ecur,&wp)) >= 0) {
	            if (wl == 0) continue ;

#if	CF_DEBUGS
	            debugprintf("bibleqs_indmkeigen: w=%r\n",wp,wl) ;
#endif

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
	    }

	    rs1 = uc_free(keys) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	} /* end if (EDB-open) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indmkeigen) */

#else /* CF_EXTRASTRONG */

local int bibleqs_indmkeigen(BIBLEQS *op,TXTINDEXMK *tip) {
	TXTINDEXMK_KEY	*keys = nullptr ;
	cint	nkeys = nelem(strongseigens) ;
	int		rs ;
	int		rs1 ;
	int		size ;
	int		i = 0 ;

	if (op == nullptr) return SR_FAULT ;

	size = (nkeys + 1) * szof(TXTINDEXMK_KEY) ;
	if ((rs = uc_malloc(size,&keys)) >= 0) {
	    int		wl ;
	    cchar	*wp ;

/* populate */

	    for (i = 0 ; (i < nkeys) && (strongseigens[i] != nullptr) ; i += 1) {
	        wp = strongseigens[i] ;
	        wl = lenstr(wp) ;
	        keys[i].kp = wp ;
	        keys[i].kl = wl ;
	    } /* end while */
	    keys[i].kp = nullptr ;		/* this is just for us (not needed) */
	    keys[i].kl = 0 ;

/* use */

	    rs = txtindexmk_addeigens(tip,keys,i) ;

/* despose of */

	    rs1 = uc_free(keys) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */

#if	CF_DEBUGS
	debugprintf("bibleqs_indmkeigen: ret rs=%d i=%u\n",rs,i) ;
#endif

	return (rs >= 0) ? i : rs ;
} /* end subroutine (bibleqs_indmkeigen) */

#endif /* CF_EXTRASTRONG */

local int bibleqs_indmkdata(BIBLEQS *op,TXTINDEXMK *tip) {
	int		rs = SR_OK ;
	int		c = 0 ;

#if	CF_DEBUGS
	debugprintf("bibleqs_indmkdata: ent\n") ;
#endif

	if (op->dbmdata != nullptr) {
	    BIBLEQS_Q		q ;
	    TXTINDEXMK_TAG	t ;
	    KTAG		e ;
	    KTAG_PARAMS		ka ;
	    size_t		fileoff = 0 ;
	    int			ml, ll ;
	    int			si ;
	    int			len ;
	    int			f_ent = false ;
	    cchar		*tp, *mp, *lp ;

/* paramters for KTAGing */

	ka.edbp = &op->edb ;
	ka.f_eigen = op->fl.edb ;
	ka.minwlen = op->minwlen ;
	ka.wterms = op->wterms ;

/* start in */

	mp = op->dbmdata ;
	ml = (op->dbmsize & INT_MAX) ;

	while ((tp = strnchr(mp,ml,'\n')) != nullptr) {

	    len = ((tp + 1) - mp) ;
	    lp = mp ;
	    ll = (len - 1) ;

#if	CF_DEBUGS
	    debugprintf("bibleqs_indmkdata: l=>%r<\n",
		lp,strlinelen(lp,ll,40)) ;
#endif

	    while ((ll > 0) && CHAR_ISWHITE(*lp)) {
	        lp += 1 ;
	        ll -= 1 ;
	    }

	    if ((tp = strnchr(lp,ll,'#')) != nullptr)
	        ll = (tp - lp) ;

	    if (ll > 0) {

	        if (isstart(lp,ll,&q,&si)) {

#if	CF_DEBUGS
	    	    debugprintf("bibleqs_indmkdata: isstart=YES f_ent=%u\n",
			f_ent) ;
#endif

	            if (f_ent) {
	                c += 1 ;
	                if ((rs = ktag_mktag(&e,fileoff,&t)) >= 0)
	                    rs = txtindexmk_addtags(tip,&t,1) ;
	                f_ent = false ;
	                ktag_finish(&e) ;
	            }

	            if (rs >= 0) {
	                rs = ktag_start(&e,&ka,fileoff,(lp+si),(ll-si)) ;
	                if (rs >= 0)
	                    f_ent = TRUE ;
	            }

	        } else {

#if	CF_DEBUGS
	    	    debugprintf("bibleqs_indmkdata: isstart=NO f_ent=%u\n",
				f_ent) ;
#endif
	            if (f_ent) {
	                rs = ktag_add(&e,lp,ll) ;
#if	CF_DEBUGS
	    	    debugprintf("bibleqs_indmkdata: ktab_add() rs=%d\n",rs) ;
#endif
		    }

	        } /* end if (entry start of add) */

	    } else {

#if	CF_DEBUGS
	    	    debugprintf("bibleqs_indmkdata: empty-line\n") ;
#endif

#if	CF_EMPTYTERM
	        if (f_ent) {
	            f_ent = false ;
	            c += 1 ;
	            rs = ktag_mktag(&e,fileoff,&t) ;
	            if (rs >= 0)
	                rs = txtindexmk_addtags(tip,&t,1) ;
	            f_ent = false ;
	            ktag_finish(&e) ;
	        }
#else
	        rs = SR_OK ;
#endif /* CF_EMPTYTERM */

	    } /* end if (not empty) */

	    fileoff += len ;
	    ml -= len ;
	    mp += len ;

	    if (rs < 0) break ;
	} /* end while (readling lines) */

#if	CF_DEBUGS
	debugprintf("bibleqs_indmkdata: while-out rs=%d f_ent=%u\n",
		rs,f_ent) ;
#endif

	if ((rs >= 0) && f_ent) {
	    c += 1 ;
	    if ((rs = ktag_mktag(&e,fileoff,&t)) >= 0) {
	        rs = txtindexmk_addtags(tip,&t,1) ;
	    }
	    f_ent = false ;
	    ktag_finish(&e) ;
	}

	if (f_ent)
	    ktag_finish(&e) ;

	} else
	    rs = SR_NOANODE ;

#if	CF_DEBUGS
	debugprintf("bibleqs_indmkdata: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_indmkdata) */

local int bibleqs_indclose(BIBLEQS *op) {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.ind) {
	    op->fl.ind = false ;
	    rs1 = txtindex_close(&op->ind) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (bibleqs_inclose) */

/* make the index */
#if	CF_MKBIBLEQSI
local int bibleqs_mkbibleqsi(BIBLEQS *op,cchar *dname) {
	int		rs ;
	int		rs1 ;
	char		dbname[MAXPATHLEN + 1] ;

#if	CF_DEBUGS
	debugprintf("bibleqs_mkbibleqsi: dname=%s\n",dname) ;
#endif

	if (dname == nullptr) return SR_FAULT ;

	if (dname[0] == '\0') return SR_INVALID ;

	if ((rs = mkpath2(dbname,dname,op->dbname)) >= 0) {
	pid_t		cpid = 0 ;
	int		i, cstat ;
	cchar	*prog = PROG_MKBIBLEQSI ;
	char		pbuf[MAXPATHLEN + 1] ;

	for (i = 0 ; prbins[i] != nullptr ; i += 1) {
	    if ((rs = mkpath3(pbuf,op->pr,prbins[i],prog)) >= 0) {
	        rs = perm(pbuf,-1,-1,nullptr,X_OK) ;
	    }
	    if (rs >= 0) break ;
	} /* end for */

#if	CF_DEBUGS
	debugprintf("bibleqs_mkbibleqsi: pr=%s\n",op->pr) ;
	debugprintf("bibleqs_mkbibleqsi: pbuf=%s\n",pbuf) ;
	debugprintf("bibleqs_mkbibleqsi: perm() rs=%d\n",rs) ;
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
	        for (i = 0 ; envchild[i] != nullptr ; i += 1) {
	            if ((cp = getenv(envchild[i])) != nullptr) {
	                rs = vecstr_envadd(&envs,envchild[i],cp,-1) ;
	            }
		    if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
		    cchar	**ev ;
	            if ((rs = vecstr_getvec(&envs,&ev)) >= 0) {
			SPAWNPROC	ps{} ;
			cchar	*av[10] ;
	    		i = 0 ;
	    		av[i++] = prog ;
	    		av[i++] = nullptr ;
	                ps.opts |= SPAWNPROC_OIGNINTR ;
	                ps.opts |= SPAWNPROC_OSETPGRP ;
	                for (i = 0 ; i < 3 ; i += 1) {
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

#if	CF_DEBUGS
	debugprintf("bibleqs_mkbibleqsi: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleqs_mkbibleqsi) */
#endif /* CF_MKBIBLEQSI */

/* does this primary tag have the query keys? */
local int bibleqs_havekeys(BIBLEQS *op,TXTINDEX_TAG *tagp,int qo,
		SEARCHERS *skp) noex {
	SEARCHKEYS_POP	pkeys ;
	cint		f_prefix = (qo & BIBLEQS_OPREFIX) ;
	int		rs ;
	int		rs1 ;
	int		f = false ;

	if ((rs = searchkeys_popbegin(skp,&pkeys,f_prefix)) >= 0) {
	    BIBLEQS_Q	q ;
	    int		c = rs ;
	    int		si ;
	    int		len ;
	    int		ml, ll ;
	    cchar	*tp, *mp, *lp ;

	    if (c == 0) f = TRUE ;

/* process this tag */

	    mp = (cchar *) (op->dbmdata + tagp->recoff) ;
	    ml = tagp->reclen ;

	    while ((! f) && ((tp = strnchr(mp,ml,'\n')) != nullptr)) {

	        len = ((tp + 1) - mp) ;
	        lp = mp ;
	        ll = (len - 1) ;

	        if ((tp = strnchr(lp,ll,'#')) != nullptr) {
	            ll = (tp - lp) ;
		}

	        while ((ll > 0) && CHAR_ISWHITE(*lp)) {
	            lp += 1 ;
	            ll -= 1 ;
	        }

	        if (ll > 0) {

	            if (isstart(lp,ll,&q,&si)) {
	                lp += si ;
	                ll -= si ;
	            }

	            if (ll > 0) {
	                rs = bibleqs_havekeysline(op,skp,&pkeys,lp,ll) ;
	                f = (rs > 0) ;
	                if (rs < 0) break ;

	                if (f) break ;
	            } /* end if */

	        } /* end if (not empty) */

	        mp += len ;
	        ml -= len ;

	    } /* end while (readling lines) */

	    rs1 = searchkeys_popend(skp,&pkeys) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (searchkeys-pop) */

#if	CF_DEBUGS
	debugprintf("bibleqs_havekeys: ret rs=%d f=%u\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleqs_havekeys) */

local int bibleqs_havekeysline(BIBLEQS *op,SEARCHKEYS *skp,
		SEARCHKEYS_POP *pkp,cchar *lp,int ll) noex {
	field		fsb ;
	int		rs ;
	int		f = false ;

#if	CF_DEBUGS
	debugprintf("bibleqs_havekeysline: line\n") ;
	debugprintf(">%r<\n",lp,ll) ;
#endif

	if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	    int		fl, kl ;
	    cchar	*fp, *kp ;
	    char	keybuf[KEYBUFLEN + 1] ;

	    while ((fl = field_word(&fsb,op->wterms,&fp)) >= 0) {

	        if (fl && (fp[0] == CH_SQUOTE)) {
	            fp += 1 ;
	            fl -= 1 ;
	        }

	        if (fl == 0) continue ;

#if	CF_DEBUGS
	        debugprintf("bibleqs_havekeysline: fl=%u fp=>%r<\n",
	            fl,fp,fl) ;
#endif

	        kl = sfword(fp,fl,&kp) ;

#if	CF_DEBUGS
	        debugprintf("bibleqs_havekeysline: kl=%u k=>%r<\n",
	            kl,kp,kl) ;
#endif

	        if (kl <= 0) continue ;

	        if (kl > KEYBUFLEN)		/* prevents overflow */
	            kl = KEYBUFLEN ;

	        if (hasuc(kp,kl)) {
	            strwcpylc(keybuf,kp,kl) ;	/* can't overflow */
	            kp = keybuf ;
	        }

	        rs = bibleqs_matchkeys(op,skp,pkp,kp,kl) ;
	        f = (rs > 0) ;

#if	CF_DEBUGS
	        debugprintf("bibleqs_havekeysline: match? w=>%r< f=%u\n",
	            kp,kl,f) ;
#endif

	        if (f) break ;
	        if (rs < 0) break ;
	    } /* end while (fielding words) */

	    field_finish(&fsb) ;
	} /* end if (field) */

#if	CF_DEBUGS
	debugprintf("bibleqs_havekeysline: ret rs=%d f=%u\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (bibleqs_havekeysline) */


/* do the keys match? */
local int bibleqs_matchkeys(op,skp,pkp,sp,sl)
BIBLEQS		*op ;
SEARCHKEYS	*skp ;
SEARCHKEYS_POP	*pkp ;
cchar	*sp ;
int		sl ;
{
	xwords		xw ;
	int		rs ;
	int		f = false ;

#if	CF_DEBUGS 
	debugprintf("bibleqs_matchkeys: ent s=>%r<\n",sp,sl) ;
#endif

	if (op == nullptr) return SR_FAULT ;

/* deal with extra (ex: possessive) words */

#if	CF_SINGLEWORD
	if ((rs = xwords_start(&xw,sp,sl)) >= 0) {

	    rs = searchkeys_processxw(skp,pkp,&xw) ;
	    f = (rs > 0) ;

	    xwords_finish(&xw) ;
	} /* end if (xwords) */
#else /* CF_SINGLEWORD */
	if ((rs = xwords_start(&xw,sp,sl)) >= 0) {
	    int		rs1 ;
	    int		wi ;
	    int		cl ;
	    cchar	*cp ;

	    f = false ;
	    for (wi = 0 ; ((cl = xwords_get(&xw,wi,&cp)) > 0) ; wi += 1) {

#if	CF_DEBUGS 
	        debugprintf("bibleqs_matchkeys: xwords_get() rs=%d\n",cl) ;
	        if (cl >= 0)
	            debugprintf("bibleqs_matchkeys: c=>%r<\n",cp,cl) ;
#endif

	        rs1 = searchkeys_process(skp,pkp,cp,cl) ;
	        f = (rs1 > 0) ;

#if	CF_DEBUGS 
	        debugprintf("bibleqs_matchkeys: searchkeys_process() f=%u\n",
	            f) ;
#endif

	        if (f) break ;
	    } /* end for (matching words) */

	    xwords_finish(&xw) ;
	} /* end if (xwords) */
#endif /* CF_SINGLEWORD */

#if	CF_DEBUGS
	debugprintf("bibleqs_matchkeys: ret rs=%d f=%u\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (bibleqs_matchkeys) */


local int bibleqs_loadbuf(op,recoff,vbuf,vlen)
BIBLEQS		*op ;
uint		recoff ;
char		vbuf[] ;
int		vlen ;
{
	SBUF		b ;
	int		rs ;
	int		ml ;
	int		len = 0 ;
	cchar	*mp ;

#if	CF_DEBUGS
	debugprintf("bibleqs_loadbuf: ent\n") ;
#endif

	mp = (cchar *) (op->dbmdata + recoff) ;
	ml = (op->dbmsize - recoff) ;

	if ((rs = sbuf_start(&b,vbuf,vlen)) >= 0) {
	    int		ll ;
	    int		j = 0 ;
	    cchar	*tp, *lp ;

	    while ((tp = strnchr(mp,ml,'\n')) != nullptr) {

	        len = ((tp + 1) - mp) ;
	        lp = mp ;
	        ll = (len - 1) ;
	        if (ll <= 0) break ;

#if	CF_DEBUGS
	        debugprintf("bibleqs_loadbuf: line=>%r<¬\n",
	            lp,strnlen(lp,MIN(ll,40))) ;
#endif

	        if (j++ > 0)
	            rs = sbuf_chr(&b,' ') ;

	        if (rs >= 0)
	            rs = sbuf_strw(&b,lp,ll) ;

	        ml -= ((tp + 1) - mp) ;
	        mp = (tp + 1) ;

	        if (rs < 0) break ;
	    } /* end while */

	    len = sbuf_finish(&b) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */

#if	CF_DEBUGS
	debugprintf("bibleqs_loadbuf: ret rs=%d len=%u\n",rs,len) ;
#endif

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (bibleqs_loadbuf) */


#if	CF_EXTRASTRONG

local int bibleqs_eigenopen(BIBLEQS *op)
{
	int		rs1 = 0 ;
	int		f = false ;

	if (! op->fl.edbinit) {
	op->fl.edbinit = TRUE ;
	rs1 = eigenfind(&op->edb,op->pr,op->dbname,op->minwlen) ;
	op->fl.edb = (rs1 > 0) ;
	f = op->fl.edb ;
	}

#if	CF_DEBUGS
	debugprintf("bibleqs_eigenopen: eigenfind() rs=%d\n",rs1) ;
#endif

	return f ;
}
/* end subroutine (bibleqs_eigenopen) */


local int bibleqs_eigenclose(BIBLEQS *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->fl.edb) {
	    op->fl.edb = false ;
	    rs1 = eigendb_close(&op->edb) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
} /* end subroutine (bibleqs_eigenopen) */

#endif /* CF_EXTRASTRONG */

local int bibleqs_lookuper(op,curp,qo,skp,hkp)
BIBLEQS		*op ;
BIBLEQS_CUR	*curp ;
int		qo ;
SEARCHKEYS	*skp ;
vecstr		*hkp ;
{
	VECINT		recoffs ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = vecint_start(&recoffs,10,0)) >= 0) {
	    cchar	**hkeya ;
	    if ((rs = vecstr_getvec(hkp,&hkeya)) >= 0) {
	        TXTINDEX_CUR	tcur ;

	        if ((rs = txtindex_curbegin(&op->ind,&tcur)) >= 0) {
	            TXTINDEX_TAG	ttag ;
		    int			ntags ;

	            rs = txtindex_lookup(&op->ind,&tcur,hkeya) ;
	            ntags = rs ;

	            while ((rs >= 0) && (ntags-- > 0)) {
	                rs1 = txtindex_read(&op->ind,&tcur,&ttag) ;
	                if (rs1 == SR_NOTFOUND) break ;
	                rs = rs1 ;
	                if (rs >= 0) {
	                    if ((rs = bibleqs_havekeys(op,&ttag,qo,skp)) > 0) {
	                        c += 1 ;
	                        rs = vecint_add(&recoffs,ttag.recoff) ;
	                    }
	                }
	            } /* end while */
	            rs1 = txtindex_curend(&op->ind,&tcur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */

/* sort the secondary tags */

	        if ((rs >= 0) && (c > 1)) {
	            vecint_sort(&recoffs,vcmpint) ;
		}

/* store results (file-record offsets) */

	        if (rs >= 0) {
	            cint	size = (c + 1) * szof(uint) ;
	            if ((rs = uc_malloc(size,&curp->verses)) >= 0) {
	                int *a ;
	                if ((rs = vecint_getvec(&recoffs,&a)) >= 0) {
	                    curp->nverses = c ;
	                    memcpy(curp->verses,a,size) ;
	                    curp->verses[c] = UINT_MAX ;
		        }
	            } else
	                curp->verses = nullptr ;
	        } /* end if (ok) */

	    } /* end if (getvec) */
	    rs1 = vecint_finish(&recoffs) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (recoffs) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleqs_lookuper) */

local int bibleqs_mkhkeys(BIBLEQS *op,vecstr *hkp,SEARCHKEYS *skp) noex {
	SEARCHKEYS_CUR	cur ;
	int		rs ;
	int		rs1 ;
	int		nkeys = 0 ;

	if ((rs = searchkeys_curbegin(skp,&cur)) >= 0) {
	    int		kl ;
	    cchar	*kp ;

	    while (rs >= 0) {

	        kl = searchkeys_enum(skp,&cur,&kp) ;
	        if (kl == SR_NOTFOUND) break ;
	        rs = kl ;
	        if (rs < 0) break ;

	        if (kp == nullptr) continue ;

	        if (kl < op->minwlen) continue ;

	        rs1 = SR_NOTFOUND ;

#if	CF_EXTRAEIGEN
	        if (op->fl.edb)
	            rs1 = eigendb_exists(&op->edb,kp,kl) ;
#endif

	        if (rs1 == SR_NOTFOUND) {
	            rs = vecstr_adduniq(hkp,kp,kl) ;
	            if (rs < INT_MAX)
	                nkeys += 1 ;
	        }

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
	    sip->fl.id = TRUE ;
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

local int ktag_start(KTAG *kop,KTAG_PARAMS *kap,size_t soff,cchar *lp,int ll) {
	int		rs ;
	int		sz = szof(KTAG_KEY) ;
	int		vo = vecobjm.compact ;

	memclear(kop) ;
	kop->kap = kap ;
	kop->recoff = soff ;
	if ((rs = vecobj_start(&kop->keys,sz,0,vopts)) >= 0) {
	    rs = ktag_procline(kop,lp,ll) ;
	    if (rs < 0)
		vecobj_finish(&kop->keys) ;
	} /* end if (vecobj_start) */

	return rs ;
}
/* end subroutine (ktag_start) */

local int ktag_add(KTAG *kop,cchar *lp,int ll) noex {
	int		rs ;
	{
	rs = ktag_procline(kop,lp,ll) ;
	}
	return rs ;
} /* end subroutine (ktag_add) */

local int ktag_mktag(kop,endoff,tagp)
KTAG		*kop ;
size_t		endoff ;
TXTINDEXMK_TAG	*tagp ;
{
	TXTINDEXMK_KEY	*kea = nullptr ;
	KTAG_KEY	**va ;
	int		rs ;
	int		size ;
	int		i ;

	kop->reclen = (endoff - kop->recoff) ;
	memclear(tagp) ;
	tagp->fname = kop->fname ;	/* it is nullptr! */
	tagp->recoff = kop->recoff ;
	tagp->reclen = kop->reclen ;

	if ((rs = vecobj_getvec(&kop->keys,&va)) >= 0) {
	    tagp->nkeys = rs ;

#if	CF_DEBUGS && 0
	{
	    KTAG_KEY	*ep ;
	    for (i = 0 ; vecobj_get(&kop->keys,i,&ep) >= 0 ; i += 1) {
	        debugprintf("ktag_mktag: key=>%r<\n",ep->kp,ep->kl) ;
	    }
	}
#endif /* CF_DEBUGS */

	size = tagp->nkeys * szof(TXTINDEXMK_KEY) ;
	if ((rs = uc_malloc(size,&kea)) >= 0) {

	    kop->tkeys = kea ;		/* kea: save for us (free later) */
	    for (i = 0 ; i < tagp->nkeys ; i += 1) {
	        kea[i] = *(va[i]) ;
	    } /* end for */

	    tagp->keys = kea ;		/* kea: store in the tag */

#if	CF_DEBUGS && 0
	    for (i = 0 ; i < tagp->nkeys ; i += 1) {
	        debugprintf("ktag_mktag: key=>%r<\n",
	            tagp->keys[i].kp, tagp->keys[i].kl) ;
	    }
#endif /* CF_DEBUGS */

	} /* end if (memory-allocation) */

	} /* end if (vecobj_getvec) */

	return rs ;
} /* end subroutine (ktag_mktag) */

local int ktag_procline(KTAH *kop,cchar *lp,int ll) noex {
	int		rs ;

#if	CF_DEBUGS && CF_DEBUGPL
	debugprintf("bibleqs/ktab_procline: ent l=>%r<\n",
		lp,strlinelen(lp,ll,40)) ;
#endif

	field		fsb ;
	if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	    KTAG_PARAMS	*kap = kop->kap ;
	    xwords	w ;
	    int		fl, sl ;
	    int		wl ;
	    cchar	*fp ;
	    cchar	*wp ;
	    cchar	*sp ;

	    while ((fl = field_word(&fsb,kap->wterms,&fp)) >= 0) {

#if	CF_DEBUGS && CF_DEBUGPL
		debugprintf("bibleqs/ktab_procline: f=>%r<\n",fp,fl) ;
#endif

/* remove possible apostrophe (single-quote) from leading edge */

	        if (fl && (fp[0] == CH_SQUOTE)) {
	            fp += 1 ;
	            fl -= 1 ;
	        }

	        if (fl < kap->minwlen) continue ;

/* remove possible trailing apostrophe (single-quote) */

	        sl = sfword(fp,fl,&sp) ;

#if	CF_DEBUGS && CF_DEBUGPL
		debugprintf("bibleqs/ktab_procline: sfword() sl=%d\n",sl) ;
#endif

/* remove short words */

	        if (sl < kap->minwlen) continue ;

/* be liberal and fabricate extra keys for matching purposes */

	        if (sl > 0) {
		    if ((rs = xwords_start(&w,sp,sl)) >= 0) {
		        int	i = 0 ;
	                while ((wl = xwords_get(&w,i++,&wp)) > 0) {
	                    if (wl >= kap->minwlen) {
	                        rs = ktag_procword(kop,wp,wl) ;
			    }
			    if (rs < 0) break ;
	                } /* end while */
	                xwords_finish(&w) ;
	            } /* end if (xwords) */
		} /* end if (positive) */

	        if (rs < 0) break ;
	    } /* end while */

	    field_finish(&fsb) ;
	} /* end if (field) */

#if	CF_DEBUGS && CF_DEBUGPL
	debugprintf("bibleqs/ktab_procline: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (ktag_procline) */

local int ktag_procword(KTAG *kop,cchar *cp,int cl) noex {
	VECOBJ		*klp = &kop->keys ;
	KTAG_KEY	key ;
	cint	nrs = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		f_needstore = false ;
	int		f_cont = TRUE ;
	cchar	*lcp ;
	char		kbuf[KEYBUFLEN + 1] ;

#if	CF_DEBUGS && CF_DEBUGPW
	debugprintf("ktag_procword: ent k=%r\n",cp,cl) ;
#endif

	if (cl > KEYBUFLEN)
	    cl = KEYBUFLEN ;

	if (hasuc(cp,cl)) {
	    f_needstore = TRUE ;
	    strwcpylc(kbuf,cp,cl) ;
	    cp = kbuf ;
	}

/* note that the TXTINDEX object filters out eigen keys also */

#if	CF_EXTRASTRONG
	{
	    KTAG_PARAMS	*kap = kop->kap ;
	    if (kap->f_eigen && (eigendb_exists(kap->edbp,cp,cl) >= 0)) {
	        f_cont = false ;
	    }
	}
#endif /* CF_EXTRASTRONG */

	if (f_cont) {
	key.kp = cp ;
	key.kl = cl ;
	if ((rs = vecobj_search(klp,&key,vesrch,nullptr)) == nrs) {
	    rs = SR_OK ;

	    if (f_needstore) {
	        rs = ktag_storelc(kop,&lcp,cp,cl) ;
	        cl = rs ;
	        cp = lcp ;
	    }

	    if (rs >= 0) {
	        key.kp = cp ;
	        key.kl = cl ;
	        rs = vecobj_add(klp,&key) ;
	    }

	} /* end if (unique key) */
	} /* end if (continue) */

#if	CF_DEBUGS && CF_DEBUGPW
	debugprintf("bibleqs/ktab_procword: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (ktag_procword) */


local int ktag_storelc(KTAG *kop,cchar **rpp,cchar *cp,int cl)
{
	int		rs = SR_OK ;

	if (! kop->f_store) {
	    rs = vecstr_start(&kop->store,5,0) ;
	    kop->f_store = (rs >= 0) ;
	}

	if (rs >= 0) {
	    if (kop->f_store) {
	        if ((rs = vecstr_add(&kop->store,cp,cl)) >= 0) {
	            rs = vecstr_get(&kop->store,rs,rpp) ;
		}
	    } else {
	        rs = SR_NOANODE ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (ktag_storelc) */


local int ktag_finish(KTAG *kop)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (kop->tkeys != nullptr) {
	    rs1 = uc_free(kop->tkeys) ;
	    if (rs >= 0) rs = rs1 ;
	    kop->tkeys = nullptr ;
	}

	if (kop->f_store) {
	    rs1 = vecstr_finish(&kop->store) ;
	    if (rs >= 0) rs = rs1 ;
	}

	rs1 = vecobj_finish(&kop->keys) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (ktag_finish) */


local int mkdname(cchar *dname,mode_t dm)
{
	cint	nrs = SR_NOENT ;
	int		rs ;
	if ((rs = checkdname(dname)) == nrs) {
	    rs = mkdirs(dname,dm) ;
	}
	return rs ;
}
/* end subroutine (mkdname) */


local int checkdname(cchar *dname)
{
	int		rs = SR_OK ;

#if	CF_DEBUGS
	debugprintf("checkdname: ent dname=%s\n",dname) ;
#endif

	if (dname[0] == '/') {
	    USTAT		sb ;
	    if ((rs = u_stat(dname,&sb)) >= 0) {
		if (! S_ISDIR(sb.st_mode)) rs = SR_NOTDIR ;
		if (rs >= 0) {
	    	    rs = perm(dname,-1,-1,nullptr,W_OK) ;
		}
	    } /* end if (stat) */
	} else
	    rs = SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("checkdname: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (checkdname) */


local int isstart(cchar *lp,int ll,BIBLEQS_Q *qp,int *sip)
{
	int		rs1 ;
	int		sl = ll ;
	int		ch ;
	int		si = 0 ;
	int		f = false ;
	cchar	*sp = lp ;

#if	CF_DEBUGS && CF_DEBUGSTART
	    debugprintf("bibleqs/isstart: ent l=>%r<\n",lp,
		strlinelen(lp,ll,40)) ;
#endif

	if (CHAR_ISWHITE(sp[0]) && ((si = siskipwhite(lp,ll)) > 0)) {
	    sp += si ;
	    sl -= si ;
	}

	ch = MKCHAR(sp[0]) ;
	if ((sl >= 5) && isdigitlatin(ch)) {
	    int		i, v ;
	    int		cl ;
	    cchar	*tp, *cp ;

	    for (i = 0 ; i < 3 ; i += 1) {

	    cp = sp ;
	    cl = sl ;
	    if ((tp = strnbrk(sp,sl,": \t\n")) != nullptr) {
	        cl = (tp - sp) ;
	        sl -= ((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	    } else {
	        cl = sl ;
	        sp += sl ;
	        sl = 0 ;
	    }

	    if (cl == 0)
	        break ;

	    si = ((cp + cl) - lp) ;
	    rs1 = cfdeci(cp,cl,&v) ;

#if	CF_DEBUGS && CF_DEBUGSTART
	    debugprintf("bibleqs/isstart: cfdeci() rs=%d\n",rs1) ;
#endif

	    if (rs1 < 0)
	        break ;

	    switch (i) {
	    case 0:
	        qp->b = (uchar) v ;
	        break ;
	    case 1:
	        qp->c = (uchar) v ;
	        break ;
	    case 2:
	        qp->v = (uchar) v ;
	        break ;
	    } /* end switch */

	} /* end for */

	f = (i == 3) ;
	if (f)
	    si += siskipwhite(sp,sl) ;

	} /* end if (have a start) */

	if (sip != nullptr)
	    *sip = (f) ? si : 0 ;

#if	CF_DEBUGS && CF_DEBUGSTART
	debugprintf("bibleqs/isstart: f=%u si=%u\n",f,si) ;
#endif

	return (f) ? si : 0  ;
}
/* end subroutine (isstart) */

#if	CF_EXTRASTRONG

local int eigenfind(eigendb *edbp,cc *pr,cc *dbname,int minwlen) noex {
	int		rs ;
	int		rs1 ;
	if (ids id ; (rs = ids_load(&id)) >= 0) {
	    if (expcool	cooks ; (rs = expcook_start(&cooks)) >= 0) {

	if ((rs = expcook_add(&cooks,"n",dbname,-1)) >= 0) {
	    rs = expcook_add(&cooks,"f","eign",-1) ;
	}

	if (rs >= 0) {
	USTAT		sb ;
	int		i ;
	int		efl ;
	cchar	*efp = nullptr ;
	char		tmpfname[MAXPATHLEN + 1] ;
	char		efname[MAXPATHLEN + 1] ;

	rs = SR_NOTOPEN ;
	efname[0] = '\0' ;
	for (i = 0 ; eigenfnames[i] != nullptr ; i += 1) {

	    rs = SR_OK ;
	    efp = eigenfnames[i] ;
	    efl = -1 ;
	    if (efp[0] != '/') {
	        rs = mkpath2(tmpfname,pr,efp) ;
	        efl = rs ;
	        if (rs <= 0) {
	            rs = SR_NOENT ;
	            efp = nullptr ;
	        } else
	            efp = tmpfname ;
	    }

	    if (rs >= 0) {
	        rs = expcook_exp(&cooks,0,efname,MAXPATHLEN,efp,efl) ;
	    }
	    if (rs >= 0) {
	        rs = u_stat(efname,&sb) ;
	    }
	    if ((rs >= 0) && S_ISDIR(sb.st_mode)) {
	        rs = SR_ISDIR ;
	    }
	    if (rs >= 0) {
	        rs = permid(&id,&sb,R_OK) ;

#if	CF_DEBUGS
	    debugprintf("bibleqs/eigenfind: fname=%s rs=%d\n",
	        efname,rs) ;
#endif

	    if (rs >= 0) break ;
	} /* end for */

	if (rs >= 0) {
	    rs = eigendb_open(edbp,efname) ;
	}

	} /* end if (ok) */

#if	CF_DEBUGS
	debugprintf("bibleqs/eigenfind: eigendb_open() rs=%d\n",rs) ;
#endif

	        rs1 = expcook_finish(&cooks) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

	return rs ;
} /* end subroutine (eigenfind) */

#endif /* CF_EXTRASTRONG */

local int mkfieldterms(uchar *wterms) noex {
	for (int i = 0 ; i < 32 ; i += 1) {
	    wterms[i] = 0xFF ;
	}
	for (int i = 0 ; i < 256 ; i += 1) {
	    if (isalnumlatin(i)) {
	        BACLR(wterms,i) ;
	    }
	} /* end for */
	BACLR(wterms,CH_SQUOTE) ;		/* allow apostrophes */
	return i ;
} /* end subroutine (mkfieldterms) */

/* find if two entries match (we do not need a "comparison") */
local int vesrch(cvoid *v1p,cvoid *v2p) noex {
	KTAG_KEY	**e1pp = (KTAG_KEY **) v1p ;
	KTAG_KEY	**e2pp = (KTAG_KEY **) v2p ;
	int		rc = 0 ;
	if ((*e1pp != nullptr) || (*e2pp != nullptr)) {
	    if (*e1pp != nullptr) {
	        if (*e2pp != nullptr) {
	            if ((rc = ((*e1pp)->kl - (*e2pp)->kl)) == 0) {
	                if ((rc = ((*e1pp)->kp[0] - (*e2pp)->kp[0])) == 0) {
	                    rc = memcmp((*e1pp)->kp,(*e2pp)->kp,(*e1pp)->kl) ;
		        }
	            }
	        } else
	            rc = -1 ;
	    } else
	        rc = 1 ;
	}
	return rc ;
} /* end subroutine (vesrch) */

local int vcmpint(cint *i1p,cint *i2p) noex {
	int		rc = 0 ;
	if (i1p || i2p) {
	    if (i1p) {
	        if (i2p) {
	            rc = (*i1p - *i2p) ;
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = 1 ;
	    }
	}
	return rc ;
} /* end subroutine (vcmpint) */

local bool isNeedIndex(int rs) noex {
	bool		f = false ;
	f = f || isOneOf(rsneeds,rs) ;
	f = f || isNotPresent(rs) ;
	return f ;
} /* end subroutine (isNeedIndex) */


