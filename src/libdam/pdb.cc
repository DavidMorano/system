/* pdb SUPPORT */
/* encoding=ISO8859-1 */
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
	pdb

	Descrption:
	This subroutine manages the default printer database.

	Synopsis:
	int pdb_open(pdb *op,cc *pr,cc *ur,cc *uname,cc *fname) noex

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<mallocstuff.h>
#include	<svcfile.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<xperm.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"pdb.h"


/* local defines */

#define	PDB_TOFIND	5
#define	PDB_TOCHECK	5
#define	PDB_TOOPEN	60
#define	PDB_TOCALC	30

#define	NBUF		4


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
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
}


/* forward references */

template<typename ... Args>
static int pdb_ctor(pdb *op,Args ... args) noex {
    	PDB		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pdb_ctor) */

static int pdb_dtor(pdb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pdb_dtor) */

template<typename ... Args>
static inline int pdb_magic(pdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (pdb_magic) */

static int pdb_fetcher(pdb *,char *,int,cchar *,cchar *,int) noex ;
static int pdb_dbopen(pdb *,int) noex ;
static int pdb_dbclose(pdb *,int) noex ;
static int pdb_dbcheck(pdb *,int) noex ;
static int pdb_findfile(pdb *,char *,int) noex ;


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
} ;

static vars		var ;


/* exported variables */


/* exported subroutines */

int pdb_open(pdb *op,cchar *pr,cchar *ur,cchar *uname,cchar *fname) noex {
	int		rs ;
	if ((rs = pdb_ctor(op,pr,ur,uname,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		static cint	rsv = var.mkvars() ;
		if ((rs = rsv) >= 0) {
	            int		sz = 0 ;
	            sz += (strlen(pr)+1) ;
	            sz += (strlen(ur)+1) ;
	            sz += (strlen(uname)+1) ;
	            sz += (strlen(fname)+1) ;
	            if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) {
	                op->a = bp ;
	                op->pr = bp ;
	                bp = (strwcpy(bp,pr,-1)+1) ;
	                op->ur = bp ;
	                bp = (strwcpy(bp,ur,-1)+1) ;
	                op->uname = bp ;
	                bp = (strwcpy(bp,uname,-1)+1) ;
	                op->fname = bp ;
	                bp = (strwcpy(bp,fname,-1)+1) ;
	                op->magic = PDB_MAGIC ;
	            } /* end if (m-a) */
		} /* end if (mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		pdb_dtor(op) ;
	    }
	} /* end if (pdb_ctor) */
	return rs ;
}
/* end subroutine (pdb_open) */

int pdb_close(pdb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pdb_magic(op)) >= 0) {
	    /* close out the DBs */
	    for (int w = 0 ; w < pdb_overlast ; w += 1) {
	       rs1 = pdb_dbclose(op,w) ;
	       if (rs >= 0) rs = rs1 ;
	    }
	    /* free everything else */
	    if (op->a) {
	        rs1 = uc_free(op->a) ;
	        if (rs >= 0) rs = rs1 ;
	        op->a = nullptr ;
	    }
	    {
	        rs1 = pdb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pdb_close) */

int pdb_fetch(pdb *op,char *vbuf,int vlen,cchar *printer,cchar *key) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	if ((rs = pdb_magic(op,vbuf,printer,key)) >= 0) {
	    rs = SR_INVALID ;
	    if (printer[0] && key[0]) {
	        int	w = pdb_local ;
	        if ((rs = pdb_fetcher(op,vbuf,vlen,printer,key,w)) == rsn) {
	            w = pdb_system ;
	            rs = pdb_fetcher(op,vbuf,vlen,printer,key,w) ;
	        } /* end if (searching system DB) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pdb_fetch) */

int pdb_check(pdb *op,time_t dt) noex {
    	int		rs ;
	int		rs1 ;
	int		f_changed = false ;
	if ((rs = pdb_magic(op)) >= 0) {
	    op->dt = (dt != 0) ? dt : time(nullptr) ;
	    for (int w = 0 ; w < pdb_overlast ; w += 1) {
	        rs1 = pdb_dbcheck(op,w) ;
	        if (! f_changed) {
		    f_changed = (rs1 > 0) ;
	        }
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end if (pdb_check) */


/* private subroutines*/

int pdb_fetcher(pdb *op,char *vbuf,int vlen,cchar *printer,
		cchar *key,int w) noex {
    	cint		rso = SR_OVERFLOW ;
	int		rs ;
	int		rs1 ;
	if ((rs = pdb_magic(op,vbuf,printer,key)) >= 0) {
	    rs = SR_INVALID ;
	    if (w < pdb_overlast) {
	        pdb_db	*dbp = (op->dbs + w) ;
	        if (! dbp->f_open) {
	            rs = pdb_dbopen(op,w) ;
	        }
	        if (rs >= 0) {
		    cint	slen = (NBUF * var.maxpathlen) ;
		    char	*sbuf{} ; 
		    if ((rs = uc_malloc((slen + 1),&sbuf)) >= 0) {
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
		    	rs = rsfree(rs,sbuf) ;
		    } /* end if (m-a-f) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (pdb_magic) */
	return rs ;
}
/* end subroutine (pdb_fetcher) */

static int pdb_dbopen(pdb *op,int w) noex {
    	cnullptr	np{} ;
	pdb_db		*dbp = (op->dbs + w) ;
	int		rs = SR_OK ;
	if (op->dt == 0) op->dt = getustime ;
	if (! dbp->f_open) {
	    cint	intfind = (op->dt - dbp->ti_find) ;
	    /* do not try to open if already tried */
	    if (intfind >= PDB_TOFIND) {
	        if (char *dbuf{} ; (rs = malloc_mp(&dbuf)) >= 0) {
	            dbp->ti_find = op->dt ;
	            /* try to open the DB file */
	            dbuf[0] = '\0' ;
	            if ((rs = pdb_findfile(op,dbuf,w)) >= 0) {
	                if (dbuf[0] != '\0') {
			    if ((dbp->sfp = new(nothrow) svcfile) != np) {
	                        if ((rs = svcfile_open(dbp->sfp,dbuf)) >= 0) {
	    	                    dbp->f_open = true ;
	                            dbp->ti_open = op->dt ;
	                        }
				if (rs < 0) {
				    delete dbp->sfp ;
				    dbp->sfp = nullptr ;
				}
			    } /* end if (new-svcfile) */
			} /* end if (have filename) */
	            } /* end if (trying to open) */
	    	    rs = rsfree(rs,dbuf) ;
		    if (rs < 0) {	
			pdb_dbclose(op,w) ;
		    }
	        } /* end if (m-a-f) */
	    } else {
	        rs = SR_NOENT ;
	    }
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (pdb_dbopen) */

static int pdb_dbclose(pdb *op,int w) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	if (w < pdb_overlast) {
	    pdb_db	*dbp = (op->dbs + w) ;
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
}
/* end subroutine (pdb_dbclose) */

static int pdb_findfile(pdb *op,char *rbuf,int w) noex {
	cint		rlen = var.maxpathlen ;
	int		rs = SR_OK ;
	int		rs1 ;
	rbuf[0] = '\0' ;
	if (strchr(op->fname,'/') == nullptr) {
	    if (vecstr sv ; (rs = sv.start(6,0)) >= 0) {
		cchar	*scp = "/" ;	/* <- default value */
		switch (w) {
		case pdb_local:
	            if ((op->ur != nullptr) && (op->ur[0] != '\0')) {
			scp = op->ur ;
		    }
		    break ;
		case pdb_system:
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
		    pdb_db	*dbp = (op->dbs + w) ;
		    dbp->ti_mtime = sb.st_mtime ;
		}
	    } /* end if (uc_stat) */
	} /* end if (directory check) */
	return rs ;
}
/* end subroutine (pdb_findfile) */

static int pdb_dbcheck(pdb *op,int w) noex {
	int		rs = SR_INVALID ;
	if (w < pdb_overlast) {
	    pdb_db	*dbp = (op->dbs + w) ;
	    if (dbp->f_open) {
	        rs = svcfile_check(dbp->sfp,op->dt) ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (pdb_dbcheck) */

int vars::mkvars() noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	} /* end if (getbufsize) */
    	return rs ;
}
/* end method (vars:mkvars) */

