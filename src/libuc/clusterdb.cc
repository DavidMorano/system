/* clusterdb SUPPORT */
/* lang=C++20 */

/* perform access table file related functions */
/* version %I% last-modified %G% */


/* revision history:

	- 2004-05-25, David A­D­ Morano
	This subroutine was generalized from a particular program
	for wider use.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

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
#include	<netdb.h>
#include	<usystem.h>
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
static int vecpstr_ctor(vecpstr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    nullptr_t	np{} ;
	    rs = SR_OK ;

	}
	return rs ;
}
/* end subroutine (clusterdb_ctor) */

static int clusterdb_dtor(clusterdb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
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

int clusterdb_open(clusterddb *atp,cchar *fname) noex {
	USTAT		sb ;
	int		rs ;

	if (atp == NULL) return SR_FAULT ;
	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	memset(atp,0,sizeof(CLUSTERDB)) ;

	if ((rs = u_stat(fname,&sb)) >= 0) {
	    int	n ;
	    n = MAX((sb.st_size / 4),10) ;
	    if ((rs = kvsfile_open(&atp->clutab,n,fname)) >= 0) {
	        atp->magic = CLUSTERDB_MAGIC ;
	    }
	} /* end if (stat) */

	return rs ;
}
/* end subroutine (clusterdb_open) */

int clusterdb_close(clusterdb *atp) noex {
	int	rs = SR_OK ;
	int	rs1 ;


	if (atp == NULL)
	    return SR_FAULT ;

	if (atp->magic != CLUSTERDB_MAGIC)
	    return SR_NOTOPEN ;

	rs1 = kvsfile_close(&atp->clutab) ;
	if (rs >= 0) rs = rs1 ;

	atp->magic = 0 ;
	return rs ;
}
/* end subroutine (clusterdb_close) */

int clusterdb_fileadd(clusterdb *atp,cchar *fname) noex {
	int	rs ;

	if (atp == NULL) return SR_FAULT ;
	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	if (atp->magic != CLUSTERDB_MAGIC) return SR_NOTOPEN ;

	rs = kvsfile_fileadd(&atp->clutab,fname) ;

	return rs ;
}
/* end subroutine (clusterdb_fileadd) */

int clusterdb_curbegin(CD *atp,CD_CUR *curp) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp)) >= 0) {
	    rs = kvsfile_curbegin(&atp->clutab,&curp->cur) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_curbegin) */

int clusterdb_curend(CD *atp,CD_CUR *curp) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp)) >= 0) {
	    rs = kvsfile_curend(&atp->clutab,&curp->cur) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_curend) */

int clusterdb_enumcluster(CD *atp,CD_CUR *curp,char *kbuf,int klen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp,kbuf)) >= 0) {
	    KVSFILE_CUR	*ccp = (curp) ? &curp->cur : NULL ;
	    rs = kvsfile_enumkey(&atp->clutab,ccp,kbuf,klen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_enumcluster) */

int clusterdb_enum(CD *atp,CD_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,curp,kbuf,vbuf)) >= 0) {
	    KVSFILE_CUR	*ccp = (curp) ? &curp->cur : NULL ;
	    rs = kvsfile_enum(&atp->clutab,ccp,kbuf,klen,vbuf,vlen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_enum) */

int clusterdb_fetch(CD *atp,char *cluname,CD_CUR *curp,
		char *vbuf,int vlen) noex {
	int		rs ;
	if ((rs = clusterdb_magic(op,cluname,curp,vbuf)) >= 0) {
	    KVSFILE_CUR	*ccp = (curp) ? &curp->cur : NULL ;
	    rs = kvsfile_fetch(&atp->clutab,cluname,ccp,vbuf,vlen) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (clusterdb_fetch) */


/* return cluster names given a node name */
int clusterdb_fetchrev(atp,nodename,curp,kbuf,klen)
CLUSTERDB	*atp ;
char		nodename[] ;
CLUSTERDB_CUR	*curp ;
char		kbuf[] ;
int		klen ;
{
	KVSFILE		*kop ;
	KVSFILE_CUR	vcur ;
	const int	nlen = NODENAMELEN ;
	const int	nrs = SR_NOTFOUND ;
	int	rs = SR_OK ;
	int	rs1 ;
	int	f_match = FALSE ;
	char	nbuf[NODENAMELEN + 1] ;


	if (atp == NULL) return SR_FAULT ;
	if (nodename == NULL) return SR_FAULT ;
	if (kbuf == NULL) return SR_FAULT ;

	if (nodename[0] == '\0') return SR_INVALID ;

	if (atp->magic != CLUSTERDB_MAGIC) return SR_NOTOPEN ;

	kop = &atp->clutab ;
	if (curp != NULL) {
	    KVSFILE_CUR	*ccp = &curp->cur ;
	    while ((rs = kvsfile_enumkey(kop,ccp,kbuf,klen)) >= 0) {
	        if ((rs = kvsfile_curbegin(kop,&vcur)) >= 0) {
	            while (rs >= 0) {
	                rs1 = kvsfile_fetch(kop,kbuf,&vcur,nbuf,nlen) ;
	                if (rs1 == nrs) break ;
	                rs = rs1 ;
	                if (rs >= 0) {
	                    f_match = (strcmp(nodename,nbuf) == 0) ;
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
	            if ((rs >= 0) && (strcmp(nodename,nbuf) == 0)) break ;
	        } /* end while (enumerating) */
	        rs1 = kvsfile_curend(kop,&vcur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (kvsfile-cursor) */
	} /* end if */

	return rs ;
}
/* end subroutine (clusterdb_fetchrev) */


/* check if the access tables files have changed */
int clusterdb_check(atp,daytime)
CLUSTERDB	*atp ;
time_t		daytime ;
{
	int	rs = SR_OK ;


	if (atp == NULL)
	    return SR_FAULT ;

	if (atp->magic != CLUSTERDB_MAGIC)
	    return SR_NOTOPEN ;

	rs = kvsfile_check(&atp->clutab,daytime) ;

	return rs ;
}
/* end subroutine (clusterdb_check) */


