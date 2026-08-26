/* printdb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* printer database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	printdb

	Descrption:
	This subroutine manages the default printer database.

	Synopsis:
	int printdb_open(printdb *op,cc *pr,cc *ur,cc *uname,cc *fname) noex

	Arguments:
	op		printer database handle
	pr		program root (distribution root)
	ur		user root
	uname		utility name
	fname		filename to open

	Returns:
	>=0		good
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<svcfile.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"printdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PRINTDB_TOFIND	5
#define	PRINTDB_TOCHECK	5
#define	PRINTDB_TOOPEN	60
#define	PRINTDB_TOCALC	30

#define	NBUF		4


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	maxpathlen ;
	int mkvars() noex ;
    } ; /* end if (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int printdb_ctor(printdb *op,Args ... args) noex {
    	PRINTDB		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (printdb_ctor) */

local int printdb_dtor(printdb *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (printdb_dtor) */

template<typename ... Args>
local inline int printdb_magic(printdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PRINTDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (printdb_magic) */

local int printdb_fetcher	(printdb *,char *,int,cchar *,cchar *,int) noex ;
local int printdb_dbopen	(printdb *,int) noex ;
local int printdb_dbclose	(printdb *,int) noex ;
local int printdb_dbcheck	(printdb *,int) noex ;
local int printdb_findfile	(printdb *,char *,int) noex ;


/* local variables */

constexpr cpcchar	dbsched[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%e/%f",
	"%p/%n.%f",
	"%p/%f",
	"%n.%f",
	"%f",
	nullptr
} ; /* end array */

static vars		var ;


/* exported variables */


/* exported subroutines */

int printdb_open(printdb *op,cc *pr,cc *ur,cc *uname,cc *fname) noex {
	int		rs ;
	if ((rs = printdb_ctor(op,pr,ur,uname,fname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		if (static cint	rsv = var.mkvars() ; (rs = rsv) >= 0) ylikely {
	            int		sz = 0 ;
	            sz += (lenstr(pr)+1) ;
	            sz += (lenstr(ur)+1) ;
	            sz += (lenstr(uname)+1) ;
	            sz += (lenstr(fname)+1) ;
	            if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) ylikely {
	                op->a = bp ;
	                op->pr = bp ;
	                bp = (strwcpy(bp,pr,-1)+1) ;
	                op->ur = bp ;
	                bp = (strwcpy(bp,ur,-1)+1) ;
	                op->uname = bp ;
	                bp = (strwcpy(bp,uname,-1)+1) ;
	                op->fname = bp ;
	                bp = (strwcpy(bp,fname,-1)+1) ;
	                op->magval = PRINTDB_MAGIC ;
	            } /* end if (memory-acquire) */
		} /* end if (mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		printdb_dtor(op) ;
	    } /* end if (error) */
	} /* end if (printdb_ctor) */
	return rs ;
} /* end subroutine (printdb_open) */

int printdb_close(printdb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = printdb_magic(op)) >= 0) ylikely {
	    /* close out the DBs */
	    for (int w = 0 ; w < printdb_overlast ; w += 1) {
	       rs1 = printdb_dbclose(op,w) ;
	       if (rs >= 0) rs = rs1 ;
	    }
	    /* free everything else */
	    if (op->a) ylikely {
	        rs1 = lm_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = printdb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (printdb_close) */

int printdb_fetch(printdb *op,char *vbuf,int vlen,cc *printer,cc *key) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = printdb_magic(op,vbuf,printer,key)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (printer[0] && key[0]) {
	        int w = printdb_local ;
	        if ((rs = printdb_fetcher(op,vbuf,vlen,printer,key,w)) == rsn) {
	            w = printdb_system ;
	            rs = printdb_fetcher(op,vbuf,vlen,printer,key,w) ;
	        } /* end if (searching system DB) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (printdb_fetch) */

int printdb_check(printdb *op,time_t dt) noex {
    	int		rs ;
	int		rs1 ;
	int		f_changed = false ;
	if ((rs = printdb_magic(op)) >= 0) {
	    op->dt = (dt != 0) ? dt : time(nullptr) ;
	    for (int w = 0 ; w < printdb_overlast ; w += 1) {
	        rs1 = printdb_dbcheck(op,w) ;
	        if (! f_changed) {
		    f_changed = (rs1 > 0) ;
	        }
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? f_changed : rs ;
} /* end if (printdb_check) */


/* private subroutines*/

local int printdb_fetcher(printdb *op,char *vbuf,int vlen,cchar *printer,
		cchar *key,int w) noex {
    	cint		rso = SR_OVERFLOW ;
	int		rs ;
	int		rs1 ;
	if ((rs = printdb_magic(op,vbuf,printer,key)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (w < printdb_overlast) {
	        printdb_db	*dbp = (op->dbs + w) ;
	        if (! dbp->f_open) {
	            rs = printdb_dbopen(op,w) ;
	        }
	        if (rs >= 0) {
		    cint	slen = (NBUF * var.maxpathlen) ;
		    char	*sbuf{} ; 
		    if ((rs = lm_mall((slen + 1),&sbuf)) >= 0) {
	                svcfile		*sfp = dbp->sfp ;
	                auto		sv_cb = svcfile_curbegin ;
	                if (svcfile_cur cur ; (rs = sv_cb(sfp,&cur)) >= 0) {
	                    svcfile_ent		ste ;
			    auto		sv_f = svcfile_fetch ;
	                    forever {
	                        rs = sv_f(sfp,printer,&cur,&ste,sbuf,slen) ;
	                        if (rs < 0) break ;
	                        rs = SR_NOTFOUND ;
	                        for (int i = 0 ; ste.keyvals[i][0] ; i += 1) {
	                            if (strcmp(key,ste.keyvals[i][0]) == 0) {
				        cchar	*valp = ste.keyvals[i][1] ;
	                                rs = sncpy1(vbuf,vlen,valp) ;
	                                if ((rs >= 0) || (rs == rso)) break ;
	                            } /* end if (got a key match) */
	                        } /* end for (looping through entry keys) */
	                        if ((rs >= 0) || (rs == rso)) break ;
	                    } /* end while (looping through enties) */
	                    rs1 = svcfile_curend(dbp->sfp,&cur) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (DB is open) */
		    	rs1 = lm_free(sbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (printdb_magic) */
	return rs ;
} /* end subroutine (printdb_fetcher) */

local int printdb_dbopen(printdb *op,int w) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	printdb_db	*dbp = (op->dbs + w) ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->dt == 0) op->dt = getustime ;
	if (! dbp->f_open) {
	    cint	intfind = intsat(op->dt - dbp->ti_find) ;
	    /* do not try to open if already tried */
	    if (intfind >= PRINTDB_TOFIND) {
	        if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) {
	            dbp->ti_find = op->dt ;
	            /* try to open the DB file */
	            dbuf[0] = '\0' ;
	            if ((rs = printdb_findfile(op,dbuf,w)) >= 0) {
	                if (dbuf[0] != '\0') {
			    if ((dbp->sfp = new(nt) svcfile) != np) {
	                        if ((rs = svcfile_open(dbp->sfp,dbuf)) >= 0) {
	    	                    dbp->f_open = true ;
	                            dbp->ti_open = op->dt ;
	                        }
				if (rs < 0) {
				    delete dbp->sfp ;
				    dbp->sfp = nullptr ;
				} /* end if (error) */
			    } /* end if (new-svcfile) */
			} /* end if (have filename) */
	            } /* end if (trying to open) */
	    	    rs1 = lm_free(dbuf) ;
		    if (rs >= 0) rs = rs1 ;
		    if (rs < 0) {	
			printdb_dbclose(op,w) ;
		    } /* end if (error) */
	        } /* end if (m-a-f) */
	    } else {
	        rs = SR_NOENT ;
	    }
	} /* end if (needed) */
	return rs ;
} /* end subroutine (printdb_dbopen) */

local int printdb_dbclose(printdb *op,int w) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	if (w < printdb_overlast) {
	    printdb_db	*dbp = (op->dbs + w) ;
	    rs = SR_OK ;
	    if (dbp->f_open) {
	        dbp->ti_find = 0 ;
	        dbp->ti_mtime = 0 ;
	        dbp->ti_open = 0 ;
	        dbp->f_open = false ;
		if (dbp->sfp) {
		    {
	                rs1 = svcfile_close(dbp->sfp) ;
			if (rs >= 0) rs = rs1 ;
		    }
		    {
			delete dbp->sfp ;
			dbp->sfp = nullptr ;
		    }
		}
	    }
	} /* end if (valid) */
	return rs ;
} /* end subroutine (printdb_dbclose) */

local int printdb_findfile(printdb *op,char *rbuf,int w) noex {
	cint		rlen = var.maxpathlen ;
	int		rs = SR_OK ;
	int		rs1 ;
	rbuf[0] = '\0' ;
	if (strchr(op->fname,'/') == nullptr) {
	    if (vecstr sv ; (rs = sv.start(6,0)) >= 0) {
		cchar	*scp = "/" ;	/* <- default value */
		switch (w) {
		case printdb_local:
	            if ((op->ur != nullptr) && (op->ur[0] != '\0')) {
			scp = op->ur ;
		    }
		    break ;
		case printdb_system:
	            if ((op->pr != nullptr) && (op->pr[0] != '\0')) {
			scp = op->pr ;
		    }
		    break ;
		} /* end switch */
	        sv.envset("p",scp) ;
	        sv.envset("e","etc") ;
	        if ((op->uname != nullptr) && (op->uname[0] != '\0')) {
	            sv.envset("n",op->uname) ;
		}
		if (rs >= 0) {
	            rs = permsched(dbsched,&sv,rbuf,rlen,op->fname,R_OK) ;
		}
	        rs1 = sv.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	} else {
	    rs = mkpath1(rbuf,op->fname) ;
	}
	if ((rs >= 0) && rbuf[0]) {
	    if (USTAT sb ; (rs = uc_stat(rbuf,&sb)) >= 0) {
	        if (S_ISDIR(sb.st_mode)) {
		    rbuf[0] = '\0' ;
	            rs = SR_ISDIR ;
		} else {
		    printdb_db	*dbp = (op->dbs + w) ;
		    dbp->ti_mtime = sb.st_mtime ;
		}
	    } /* end if (uc_stat) */
	} /* end if (directory check) */
	return rs ;
} /* end subroutine (printdb_findfile) */

local int printdb_dbcheck(printdb *op,int w) noex {
	int		rs = SR_INVALID ;
	if (w < printdb_overlast) {
	    printdb_db	*dbp = (op->dbs + w) ;
	    if (dbp->f_open) {
	        rs = svcfile_check(dbp->sfp,op->dt) ;
	    }
	} /* end if (valid) */
	return rs ;
} /* end subroutine (printdb_dbcheck) */

int vars::mkvars() noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	} /* end if (bufsizeget) */
    	return rs ;
} /* end method (vars:mkvars) */

