/* clusterdb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage a database of cluster names */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-04-13, David A­D­ Morano
	This subroutine was generalized from a particular program
	for wider use.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	clusterdb

	Description:
	This object processes an access table for use by daemon
	multiplexing server programs that want to control access
	to their sub-servers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<ctime>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<storeitem.h>
#include	<kvsfile.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"clusterdb.h"


/* local defines */

#define	CD_MAGIC	0x31835926
#define	CD_INTCKMIN	5		/* file check interval (seconds) */
#define	CD_INTCK	60		/* file check interval (seconds) */
#define	CD_INTCHG	3		/* wait change interval (seconds) */
#define	CD_DEFNETGRP	"DEFAULT"

#define	CD		clusterdb
#define	CD_CUR		clusterdb_cur
#define	CD_FILE		clusterdb_file
#define	CD_KEYNAME	clusterdb_keyname

#define	CD_KA		szof(char *(*)[2])
#define	CD_BO(v)		\
	((CD_KA - ((v) % CLUSTERDB_KA)) % CLUSTERDB_KA)

#define	KEYALIGNMENT	szof(char *(*)[2])



/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct clusterdb_file {
	cchar		*fname ;
	time_t		mtime ;
	uint		fsize ;
} ;

struct clusterdb_keyname {
	cchar		*kname ;
	int		count ;
} ;

struct clusterdb_ie {
	cchar		*(*keys)[2] ;
	cchar		*svc ;
	cchar		*clu ;
	cchar		*sys ;
	int		nkeys ;			/* number of keys */
	int		fsize ;			/* total size */
	int		fi ;			/* file index */
} ;

struct svcentry {
	vecobj		keys ;
	cchar		*svc ;
	cchar		*clu ;
	cchar		*sys ;
} ;

struct svcentry_key {
	cchar		*kname ;
	cchar		*args ;
	int		kl ;
	int		al ;
} ;


/* forward references */

template<typename ... Args>
local int clusterdb_ctor(clusterdb *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->ctp = new(nothrow) kvsfile) != np) {
		rs = SR_OK ;
	    } /* end if (new_kvsfile) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clusterdb_ctor) */

local int clusterdb_dtor(clusterdb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->ctp) {
		delete op->ctp ;
		op->ctp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clusterdb_dtor) */

template<typename ... Args>
local int clusterdb_magic(clusterdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (clusterdb_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int clusterdb_open(clusterdb *op,cchar *fname) noex {
	int		rs ;
	if ((rs = clusterdb_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) {
		    csize	fsz = size_t(sb.st_size) ;
		    int		ne ;
		    {
		        cint	isz = intsat(fsz / 4) ;
	                ne = max(isz,10) ;
	                if ((rs = kvsfile_open(op->ctp,ne,fname)) >= 0) {
	                    op->magic = CD_MAGIC ;
	                }
		    } /* end block */
	        } /* end if (stat) */
	    } /* end if (valid) */
	    if (rs < 0) {
		clusterdb_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clusterdb_open) */

int clusterdb_close(clusterdb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = clusterdb_magic(op)) >= 0) {
	    if (op->ctp) {
		rs1 = kvsfile_close(op->ctp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = clusterdb_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_close) */

int clusterdb_fileadd(clusterdb *op,cchar *fname) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		rs = kvsfile_fileadd(op->ctp,fname) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_fileadd) */

int clusterdb_curbegin(CD *op,CD_CUR *curp) noex {
	int		rs ;
	int		rsc = 0 ;
	if ((rs = clusterdb_magic(op,curp)) >= 0) {
	    cint	kcurlen = szof(kvsfile_cur) ;
	    void	*vp{} ;
	    if ((rs = uc_malloc(kcurlen,&vp)) >= 0) {
		curp->kcurp = (kvsfile_cur *) vp ;
		{
	            rs = kvsfile_curbegin(op->ctp,curp->kcurp) ;
		    rsc = rs ;
		    if (rs < 0) {
		        uc_free(vp) ;
		        curp->kcurp = nullptr ;
		    }
		} /* end block */
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return (rs >= 0) ? rsc : rs ;
}
/* end subroutine (clusterdb_curbegin) */

int clusterdb_curend(CD *op,CD_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = clusterdb_magic(op,curp)) >= 0) {
	    {
	        rs1 = kvsfile_curend(op->ctp,curp->kcurp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = uc_free(curp->kcurp) ;
		if (rs >= 0) rs = rs1 ;
		curp->kcurp = nullptr ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_curend) */

int clusterdb_curenumcluster(CD *op,CD_CUR *curp,char *kbuf,int klen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp,kbuf)) >= 0) {
	    kvsfile_cur	*kcp = (curp) ? curp->kcurp : nullptr ;
	    rs = kvsfile_curenumkey(op->ctp,kcp,kbuf,klen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_curenumcluster) */

int clusterdb_curenum(CD *op,CD_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp,kbuf,vbuf)) >= 0) {
	    kvsfile_cur	*kcp = (curp) ? curp->kcurp : nullptr ;
	    rs = kvsfile_curenum(op->ctp,kcp,kbuf,klen,vbuf,vlen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_curenum) */

int clusterdb_curfetch(CD *op,cc *cn,CD_CUR *curp,char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,cn,curp,vbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (cn[0]) {
	        kvsfile_cur	*kcp = (curp) ? curp->kcurp : nullptr ;
	        rs = kvsfile_fetch(op->ctp,cn,kcp,vbuf,vlen) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_curfetch) */

namespace {
    struct fetcher {
	CD		*op ;
	CD_CUR		*curp ;
	cchar		*nn ;
	char		*nbuf{} ;
	char		*kbuf ;
	int		nlen ;
	int		klen ;
	fetcher(CD *aop,CD_CUR *acp,cc *ann,char *akbuf,int aklen) noex {
		op = aop ;
		curp = acp ;
		nn = ann ;
		kbuf = akbuf ;
		klen = aklen ;
	} ; /* end ctor */
	int start() noex {
	    int		rs ;
	    if ((rs = malloc_mn(&nbuf)) >= 0) {
		nlen = rs ;
	    } /* end if */
	    return rs ;
	} ;
	int finish() noex {
	    int		rs = SR_OK ;
	    int		rs1 ;
	    if (nbuf) {
		rs1 = uc_free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    return rs ;
	} ;
	operator int () noex ;
    } ; /* end struct (fetcher) */
} /* end namespace */

int clusterdb_curfetchrev(CD *op,cc *nn,CD_CUR *curp,char *kbuf,int klen) noex {
	int		rs ;
	int		rs1 ;
	int		kl = 0 ;
	if ((rs = clusterdb_magic(op,nn,kbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (nn[0]) {
		if (fetcher fo(op,curp,nn,kbuf,klen) ; (rs = fo.start()) >= 0) {
		    {
		        rs = fo ;
			kl = rs ;
		    }
		    rs1 = fo.finish() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (fetcher) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (clusterdb_curfetchrev) */

fetcher::operator int () noex {
	kvsfile		*kop = op->ctp ;
	kvsfile_cur	vcur ;
	cint		nrs = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (curp != nullptr) {
	    kvsfile_cur		*kcp = curp->kcurp ;
	    bool		f_match = false ;
	    while ((rs = kvsfile_curenumkey(kop,kcp,kbuf,klen)) >= 0) {
	        if ((rs = kvsfile_curbegin(kop,&vcur)) >= 0) {
	            while (rs >= 0) {
	                rs1 = kvsfile_fetch(kop,kbuf,&vcur,nbuf,nlen) ;
	                if (rs1 == nrs) break ;
	                rs = rs1 ;
	                if (rs >= 0) {
	                    f_match = (strcmp(nn,nbuf) == 0) ;
	                }
	                if (f_match) break ;
	            } /* end while (fetching) */
	            rs1 = kvsfile_curend(kop,&vcur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (kvsfile-cursor) */
	        if (f_match) break ;
	        if (rs < 0) break ;
	    } /* end while (enumerating keys) */
	} else {
	    if ((rs = kvsfile_curbegin(kop,&vcur)) >= 0) {
	        while (rs >= 0) {
	            rs = kvsfile_curenum(kop,&vcur,kbuf,klen,nbuf,nlen) ;
	            if ((rs >= 0) && (strcmp(nn,nbuf) == 0)) break ;
	        } /* end while (enumerating) */
	        rs1 = kvsfile_curend(kop,&vcur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (kvsfile-cursor) */
	} /* end if */
	return rs ;
}
/* end method (fetcher::operator) */

int clusterdb_check(CD *op,time_t daytime) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op)) >= 0) {
	    rs = kvsfile_check(op->ctp,daytime) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_check) */


