/* clusterdb SUPPORT */
/* lang=C++20 */

/* perform access table file related functions */
/* version %I% last-modified %G% */


/* revision history:

	- 2004-05-25, David A�D� Morano
	This subroutine was generalized from a particular program
	for wider use.

*/

/* Copyright � 2004 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This object processes an access table for use by daemon
	multiplexing server programs that want to control access
	to their sub-servers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<algorithm>
#include	<netdb.h>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<storeitem.h>
#include	<kvsfile.h>
#include	<localmisc.h>

#include	"clusterdb.h"


/* local defines */

#define	CLUSTERDB_MAGIC		0x31835926
#define	CLUSTERDB_MINCHECKTIME	5	/* file check interval (seconds) */
#define	CLUSTERDB_CHECKTIME	60	/* file check interval (seconds) */
#define	CLUSTERDB_CHANGETIME	3	/* wait change interval (seconds) */
#define	CLUSTERDB_DEFNETGROUP	"DEFAULT"

#define	CLUSTERDB_FILE		struct clusterdb_file
#define	CLUSTERDB_KEYNAME		struct clusterdb_keyname

#define	CLUSTERDB_KA		sizeof(char *(*)[2])
#define	CLUSTERDB_BO(v)		\
	((CLUSTERDB_KA - ((v) % CLUSTERDB_KA)) % CLUSTERDB_KA)

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN		MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN		2048
#endif
#endif

#undef	ARGSBUFLEN
#define	ARGSBUFLEN		(3 * MAXHOSTNAMELEN)

#undef	DEFCHUNKSIZE
#define	DEFCHUNKSIZE		512

#define	KEYALIGNMENT		sizeof(char *(*)[2])

#define	CD			clusterdb
#define	CD_CUR			clusterdb_cur


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		clusterdb_fileadd(CD *,cchar *) noex ;
    int		clusterdb_curbegin(CD *,CD_CUR *) noex ;
    int		clusterdb_curend(CD *,CD_CUR *) noex ;
}


/* external variables */


/* local structures */

struct clusterdb_file {
	cchar		*fname ;
	time_t		mtime ;
	uint		size ;
} ;

struct clusterdb_keyname {
	cchar		*kname ;
	int		count ;
} ;

struct clusterdb_ie {
	cchar		*(*keys)[2] ;
	cchar		*svc, *clu, *sys ;
	int		nkeys ;			/* number of keys */
	int		size ;			/* total size */
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
	int		kl, al ;
} ;


/* forward references */

template<typename ... Args>
static int clusterdb_ctor(clusterdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->ctp = new(nothrow) kvsfile) != np) {
		rs = SR_OK ;
	    } /* end if (new_kvsfile) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clusterdb_ctor) */

static int clusterdb_dtor(clusterdb *op) noex {
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
static int clusterdb_magic(clusterdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CLUSTERDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
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
	    memclear(op) ;
	    if (fname[0]) {
	         USTAT		sb ;
	         if ((rs = u_stat(fname,&sb)) >= 0) {
	             cint	n = MAX((sb.st_size / 4),10) ;
	             if ((rs = kvsfile_open(op->ctp,n,fname)) >= 0) {
	                 op->magic = CLUSTERDB_MAGIC ;
	             }
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
	    {
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
	if ((rs = clusterdb_magic(op,curp)) >= 0) {
	    rs = kvsfile_curbegin(op->ctp,&curp->cur) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_curbegin) */

int clusterdb_curend(CD *op,CD_CUR *curp) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp)) >= 0) {
	    rs = kvsfile_curend(op->ctp,&curp->cur) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_curend) */

int clusterdb_enumcluster(CD *op,CD_CUR *curp,char *kbuf,int klen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp,kbuf)) >= 0) {
	    KVSFILE_CUR	*kcp = (curp) ? &curp->cur : nullptr ;
	    rs = kvsfile_enumkey(op->ctp,kcp,kbuf,klen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_enumcluster) */

int clusterdb_enum(CD *op,CD_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp,kbuf,vbuf)) >= 0) {
	    KVSFILE_CUR		*kcp = (curp) ? &curp->cur : nullptr ;
	    rs = kvsfile_enum(op->ctp,kcp,kbuf,klen,vbuf,vlen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_enum) */

int clusterdb_fetch(CD *op,cc *cn,CD_CUR *curp,char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,cn,curp,vbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (cn[0]) {
	        KVSFILE_CUR	*kcp = (curp) ? &curp->cur : nullptr ;
	        rs = kvsfile_fetch(op->ctp,cn,kcp,vbuf,vlen) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_fetch) */

int clusterdb_fetchrev(CD *op,cc *nn,CD_CUR *curp,char *kbuf,int klen) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = clusterdb_magic(op,nn,kbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (nn[0]) {
		char	*nbuf{} ;
		if ((rs = malloc_mn(&nbuf)) >= 0) {
		    cint	nlen = rs ;
	KVSFILE		*kop ;
	KVSFILE_CUR	vcur ;
	cint	nrs = SR_NOTFOUND ;
	int	f_match = FALSE ;
	kop = op->ctp ;
	if (curp != nullptr) {
	    KVSFILE_CUR	*kcp = &curp->cur ;
	    while ((rs = kvsfile_enumkey(kop,kcp,kbuf,klen)) >= 0) {
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
	            rs = kvsfile_enum(kop,&vcur,kbuf,klen,nbuf,nlen) ;
	            if ((rs >= 0) && (strcmp(nn,nbuf) == 0)) break ;
	        } /* end while (enumerating) */
	        rs1 = kvsfile_curend(kop,&vcur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (kvsfile-cursor) */
	} /* end if */

		rs1 = uc_free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_fetchrev) */

int clusterdb_check(CD *op,time_t daytime) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op)) >= 0) {
	    rs = kvsfile_check(op->ctp,daytime) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_check) */

