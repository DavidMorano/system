/* statmsg SUPPORT */
/* lang=C++20 */

/* object to help (manage) STATMSG messages */
/* version %I% last-modified %G% */

#define	CF_WRITETO	1		/* time out writes */
#define	CF_PARAMFILE	1		/* use |paramfile(3dam)| */

/* revision history:

	= 2003-10-01, David A­D­ Morano
	This is a hack from numerous previous hacks (not enumerated
	here).  This is a new version of this hack that is entirely
	different (much simpler).

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module writes the contents of various STATMSGs
	(as specified by the caller) to an open file descriptor
	(also specified by the caller).

	Implementation notes:

	When processing, we time-out writes to the caller-supplied
	file-descriptor because we do not  know if it is a non-regular
	file that might be flow-controlled.  We do not wait forever
	for those sorts of outputs.  So let us say that the output
	is a terminal that is currently flow-controlled.  We will
	time-out on our writes and the user will not get this whole
	STATMSG text!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |environ| */
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vechand.h>
#include	<getax.h>
#include	<getusername.h>
#include	<getuserhome.h>
#include	<getxid.h>
#include	<ptma.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<strpack.h>
#include	<bfile.h>
#include	<fsdir.h>
#include	<ascii.h>
#include	<sfx.h>
#include	<snx.h>
#include	<mkx.h>
#include	<strwcpy.h>
#include	<nleadstr.h>
#include	<xperm.h>
#include	<matxstr.h>
#include	<ctdec.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"statmsg.h"


/* local defines */

#define	STATMSG_DEFGROUP	"default"
#define	STATMSG_ALLGROUP	"all"
#define	STATMSG_NAME		"statmsg"
#define	STATMSG_SUF		"sm"
#define	STATMSG_DIRSFNAME	"dirs"
#define	MAMAGIC	0x21367425

#define	SM		statmsg
#define	SM_ID		statmsg_id

#define	MA		statmsg_mapper

#define	MD		statmsg_mapdir

#define	NDEBFNAME	"statmsg.deb"

#ifndef	MAXNAMELEN
#define	MAXNAMELEN	256
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	2048
#endif

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#define	ADMINLEN	USERNAMELEN

#undef	ENVBUFLEN
#define	ENVBUFLEN	(10 + MAX(ADMINLEN,MAXPATHLEN))

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#ifndef	POLLMULT
#define	POLLMULT	1000
#endif

#define	PBUFLEN		MAXPATHLEN

#undef	TO_POLL
#define	TO_POLL		5

#define	TO_LOCK		30
#define	TO_OPEN		10
#define	TO_READ		20
#define	TO_WRITE	50
#define	TO_CHECK	5		/* object checking */
#define	TO_MAPCHECK	10		/* mapper checking */
#define	TO_FILEAGE	5		/* directory map-file age */

#undef	NLPS
#define	NLPS		2		/* number ? polls per second */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */

mainv		environ ;


/* local structures */

struct statmsg_mapdir {
	lockrw		rwm ;
	cchar		*username ;
	cchar		*userhome ;
	cchar		*admin ;
	cchar		*dirname ;	/* raw */
	cchar		*dname ;	/* expanded */
} ;


/* forward references */

static int	statmsg_userbegin(STATMSG *,cchar *) noex ;
static int	statmsg_userend(STATMSG *) noex ;

static int	statmsg_mapfind(STATMSG *,time_t) noex ;
static int	statmsg_maplose(STATMSG *) noex ;
static int	statmsg_mapfname(STATMSG *,char *) noex ;
static int	statmsg_schedload(STATMSG *,vecstr *) noex ;
static int	statmsg_checker(STATMSG *,time_t) noex ;
static int	statmsg_envbegin(STATMSG *) noex ;
static int	statmsg_envend(STATMSG *) noex ;
static int	statmsg_envadds(STATMSG *,strpack *,cchar **,
			STATMSG_ID *,cchar *) noex ;
static int	statmsg_envstore(STATMSG *,strpack *,cchar **,int,
			cchar *,int) noex ;
static int 	statmsg_processor(STATMSG *,cchar **,cchar **,
			cchar *,cchar *,int) noex ;
static int	statmsg_idcheck(STATMSG *,STATMSG_ID *,char *) noex ;

static int	mapper_start(MA *,time_t,cchar *,cchar *,
			cchar *) noex ;
static int	mapper_finish(MA *) noex ;
static int	mapper_check(MA *,time_t) noex ;
static int	mapper_process(MA *,cchar **,cchar **,
			cchar *,cchar *,int) noex ;
static int	mapper_processor(MA *,cchar **,cchar **,
			cchar *,cchar *,int) noex ;
static int	mapper_mapload(MA *) noex ;
static int	mapper_mapadd(MA *,cchar *,int,
			cchar *,int) noex ;
static int	mapper_mapfins(MA *) noex ;

static int	mapdir_start(MD *,cchar *,
			cchar *,cchar *,int,cchar *,int) noex ;
static int	mapdir_finish(MD *) noex ;
static int	mapdir_process(MD *,cchar **,cchar **,
			cchar *,cchar *,int) noex ;
static int	mapdir_expand(MD *) noex ;
static int	mapdir_expander(MD *) noex ;
static int	mapdir_processor(MD *,cchar **,
			cchar *,cchar *,int) noex ;
static int	mapdir_processorthem(MD *,cchar **,
			cchar *,vecstr *,cchar **,int) noex ;
static int	mapdir_processorone(MD *,cchar **,
			cchar *,vecstr *,cchar *,int) noex ;
static int	mapdir_procout(MD *,cchar **,cchar *,
			cchar *,int) noex ;
static int	mapdir_procouter(MD *,cchar **,
			cchar *,int) noex ;

static int	writeto(int,cchar *,int,int) noex ;
static int	loadstrs(cchar **,cchar *,cchar *,cchar *,cchar *) noex ;
static bool	isBaseMatch(cchar *,cchar *,cchar *) noex ;


/* local variables */

constexpr cpcchar	schedmaps[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	"%n.%f",
	nullptr
} ;

constexpr cpcchar	envbad[] = {
	"TMOUT",
	"A__z",
	nullptr
} ;

enum envstrs {
	envstr_username,
	envstr_groupname,
	envstr_uid,
	envstr_gid,
	envstr_keyname,
	envstr_admin,
	envstr_admindir,
	envstr_overlast
} ;

constexpr cpcchar	envstrs[] = {
	"USERNAME",
	"GROUPNAME",
	"UID",
	"GID",
	"KEYNAME",
	"ADMIN",
	"ADMINDIR",
	nullptr
} ;

constexpr uid_t		uidend = -1 ;
constexpr gid_t		gidend = -1 ;

constexpr cchar		envpre[] = "STATMSG_" ;	/* environment prefix */


/* exported variables */


/* exported subroutines */

int statmsg_open(STATMSG *op,cchar *username) noex {
	const time_t	dt = time(nullptr) ;
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (username == nullptr) return SR_FAULT ;

	if (username[0] == '\0') return SR_INVALID ;

	memclear(op) ;
	op->fe = STATMSG_DIRSFNAME ;

	if ((rs = ptm_create(&op->m,nullptr)) >= 0) {
	    if ((rs = statmsg_userbegin(op,username)) >= 0) {
		if ((rs = statmsg_mapfind(op,dt)) >= 0) {
		    if ((rs = statmsg_envbegin(op)) >= 0) {
			op->ti_lastcheck = dt ;
			op->magic = STATMSG_MAGIC ;
		    }
		    if (rs < 0) {
			statmsg_maplose(op) ;
		    }
		}
		if (rs < 0) {
		    statmsg_userend(op) ;
		}
	    }
	    if (rs < 0) {
		ptm_destroy(&op->m) ;
	    }
	} /* end if (ptm) */

	return rs ;
}
/* end subroutine (statmsg_open) */

int statmsg_close(STATMSG *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != STATMSG_MAGIC) return SR_NOTOPEN ;

	rs1 = statmsg_envend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = statmsg_maplose(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = statmsg_userend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = ptm_destroy(&op->m) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (statmsg_close) */

int statmsg_check(STATMSG *op,time_t dt) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != STATMSG_MAGIC) return SR_NOTOPEN ;

	rs = statmsg_checker(op,dt) ;

	return rs ;
}
/* end subroutine (statmsg_check) */

int statmsg_process(STATMSG *op,cchar *gn,cchar **adms,cchar *kn,int fd) noex {
	STATMSG_ID	id{} ;
	int		rs ;
	id.groupname = gn ;
	id.uid = -1 ;
	id.gid = -1 ;
	rs = statmsg_processid(op,&id,adms,kn,fd) ;

	return rs ;
}
/* end subroutine (statmsg_process) */

int statmsg_processid(STATMSG *op,STATMSG_ID *idp,cc **adms,
		cc *kn,int fd) noex {
	STATMSG_ID	id ;
	int		rs ;
	int		wlen = 0 ;
	cchar		*groupname ;
	char		ubuf[USERNAMELEN + 1] ;
	char		kbuf[2] ;

	if (op == nullptr) return SR_FAULT ;
	if (idp == nullptr) return SR_FAULT ;

	if (op->magic != STATMSG_MAGIC) return SR_NOTOPEN ;

	if (fd < 0) return SR_BADF ;

	groupname = idp->groupname ;
	if (groupname == nullptr)
	    return SR_FAULT ;

	if (groupname[0] == '\0')
	    return SR_INVALID ;

	if (kn == nullptr) {
	    kn = kbuf ;
	    kbuf[0] = '\0' ;
	}

/* fill in some missing elements */

	id = *idp ;			/* copy for possible modification */
	if ((rs = statmsg_idcheck(op,&id,ubuf)) >= 0) {
	    cint	n = nelem(envstrs) ;
	    int		sz ;
	    void	*p ;
	    sz = (op->nenv + n + 1) * sizeof(cchar *) ;
	    if ((rs = uc_malloc(sz,&p)) >= 0) {
	        strpack		packer ;
	        cchar	**ev = (cchar **) p ;

	    if ((rs = strpack_start(&packer,128)) >= 0) {

	        if ((rs = statmsg_envadds(op,&packer,ev,&id,kn)) >= 0) {
	            rs = statmsg_processor(op,ev,adms,groupname,kn,fd) ;
	            wlen = rs ;
	        }

	        strpack_finish(&packer) ;
	    } /* end if (packer) */

	    uc_free(p) ;
	} /* end if (memory allocation) */
	} /* end if (statmsg_idcheck) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (statmsg_processid) */

int statmsgid_load(STATMSG_ID *idp,cc *un,cc *gn,uid_t uid,gid_t gid) noex {

	if (idp == nullptr) return SR_FAULT ;

	memclear(idp) ;
	idp->uid = uid ;
	idp->gid = gid ;
	idp->username = un ;
	idp->groupname = gn ;

	return SR_OK ;
}
/* end subroutine (statmsgid_load) */


/* private subroutines */

static int statmsg_userbegin(STATMSG *op,cchar *username) noex {
	cint	hlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	char		hbuf[MAXPATHLEN+1] ;
	char		ubuf[USERNAMELEN+1] ;

	if (username == nullptr) return SR_FAULT ;

	if (username[0] == '\0') return SR_INVALID ;

	if (username[0] == '-') {
	    rs = getusername(ubuf,USERNAMELEN,-1) ;
	    username = ubuf ;
	}

	if (rs >= 0) {
	    if ((rs = getuserhome(hbuf,hlen,username)) >= 0) {
		int	sz = 0 ;
		char	*bp ;
		sz += (strlen(username) + 1) ;
		sz += (strlen(hbuf) + 1) ;
		if ((rs = uc_malloc(sz,&bp)) >= 0) {
		    op->useralloc = bp ;
		    op->username = bp ;
		    bp = (strwcpy(bp,username,-1) + 1) ;
		    op->userhome = bp ;
		    bp = (strwcpy(bp,hbuf,-1) + 1) ;
		} /* end if (memory-allocation) */
	    } /* end if (getuserhome) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (statmsg_userbegin) */

static int statmsg_userend(STATMSG *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->useralloc != nullptr) {
	    rs1 = uc_free(op->useralloc) ;
	    if (rs >= 0) rs = rs1 ;
	    op->useralloc = nullptr ;
	    op->username = nullptr ;
	    op->userhome = nullptr ;
	}
	return rs ;
}
/* end subroutine (statmsg_userend) */

static int statmsg_mapfind(STATMSG *op,time_t dt) noex {
	int		rs ;
	char		mapfname[MAXPATHLEN + 1] ;
	mapfname[0] = '\0' ;
	if ((rs = statmsg_mapfname(op,mapfname)) >= 0) {
	    if (mapfname[0] != '\0') {
		cchar	*un = op->username ;
		cchar	*uh = op->userhome ;
	        if ((rs = mapper_start(&op->mapper,dt,un,uh,mapfname)) >= 0) {
	            op->nmaps += 1 ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (statmsg_mapfind) */

static int statmsg_maplose(STATMSG *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nmaps > 0) {
	    rs1 = mapper_finish(&op->mapper) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nmaps = 0 ;
	}
	return rs ;
}
/* end subroutine (statmsg_maplose) */

static int statmsg_mapfname(STATMSG *op,char *fbuf) noex {
	vecstr		scheds ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	fbuf[0] = '\0' ;
	if ((rs = vecstr_start(&scheds,6,0)) >= 0) {
	    if ((rs = statmsg_schedload(op,&scheds)) >= 0) {
		cint	flen = MAXPATHLEN ;
	        rs1 = permsched(schedmaps,&scheds,fbuf,flen,op->fe,R_OK) ;
	        if ((rs1 == SR_NOENT) || (rs1 == SR_ACCESS)) {
	            if (rs1 == SR_NOENT) {
	                fbuf[0] = '\0' ;
	            } else
	                rs = rs1 ;
	        } else if (rs1 == SR_OK) {
	            c = 1 ;
		} else
		    rs = rs1 ;
	    } /* end if (statmsg-schedload) */
	    vecstr_finish(&scheds) ;
	} /* end if (vecstr) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (statmsg_mapfname) */

static int statmsg_schedload(STATMSG *op,vecstr *slp) noex {
	int		rs = SR_OK ;
	cchar		*keys = "pen" ;
	cchar		*name = STATMSG_NAME ;
	for (int i = 0 ; keys[i] != '\0' ; i += 1) {
	    cint	kch = MKCHAR(keys[i]) ;
	    int		vl = -1 ;
	    cchar	*vp = nullptr ;
	    switch (kch) {
	    case 'p':
		vp = op->userhome ;
		break ;
	    case 'e':
		vp = "etc" ;
		break ;
	    case 'n':
		vp = name ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && (vp != nullptr)) {
		char	kbuf[2] = { 0, 0 } ;
		kbuf[0] = kch ;
		rs = vecstr_envset(slp,kbuf,vp,vl) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (statmsg_schedload) */

static int statmsg_checker(STATMSG *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		nchanged = 0 ;

	if (op->nmaps > 0) {
	if ((rs = ptm_lock(&op->m)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;

	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {

	        rs = mapper_check(&op->mapper,dt) ;
	        nchanged = rs ;
	        op->ti_lastcheck = dt ;

	    } /* end if */

	    ptm_unlock(&op->m) ;
	} /* end if (mutex) */
	} /* end if (positive) */

	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (statmsg_checker) */

static int statmsg_envbegin(STATMSG *op) noex {
	int		rs = SR_OK ;
	int		sz ;
	int		i ;
	int		c = 0 ;
	int		f ;
	void		*p ;

	for (i = 0 ; environ[i] != nullptr ; i += 1) ;

	sz = (i + 1) * sizeof(cchar *) ;
	if ((rs = uc_malloc(sz,&p)) >= 0) {
	    cchar	*ep ;
	    cchar	**va = (cchar **) p ;
	    op->envv = va ;
	    for (i = 0 ; environ[i] != nullptr ; i += 1) {
	        ep = environ[i] ;
	        f = TRUE ;
	        f = f && (ep[0] != '-') ;
	        f = f && (matstr(envbad,ep,-1) < 0) ;
	        if (f && (ep[0] == 'M')) f = (strncmp(envpre,ep,5) != 0) ;
	        if (f)
	            va[c++] = ep ;
	    } /* end for */
	    va[c] = nullptr ;
	    op->nenv = c ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (statmsg_envbegin) */

static int statmsg_envend(STATMSG *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->envv != nullptr) {
	    rs1 = uc_free(op->envv) ;
	    if (rs >= 0) rs = rs1 ;
	    op->envv = nullptr ;
	}

	return rs ;
}
/* end subroutine (statmsg_envend) */

static int statmsg_envadds(STATMSG *op,strpack *spp,cchar **ev,
		STATMSG_ID *idp,cchar *kn) noex {
	uint		uv ;
	cint	envlen = ENVBUFLEN ;
	int		rs = SR_OK ;
	int		n, i ;
	int		el ;
	cchar		**envv = op->envv ;
	cchar		*es ;
	cchar		*cp ;
	char		envbuf[ENVBUFLEN + 1] ;
	char		digbuf[DIGBUFLEN + 1] ;

	for (n = 0 ; n < op->nenv ; n += 1) ev[n] = envv[n] ;

	for (i = 0 ; (rs >= 0) && (envstrs[i] != nullptr) ; i += 1) {
	    envbuf[0] = '\0' ;
	    es = envstrs[i] ;
	    el = -1 ;
	    switch (i) {
	    case envstr_uid:
	        if (idp->uid != uidend) {
	            uv = idp->uid ;
	            rs = ctdecui(digbuf,DIGBUFLEN,uv) ;
	            if (rs >= 0) {
	                rs = sncpy4(envbuf,envlen,envpre,es,"=",digbuf) ;
	                el = rs ;
	            }
	        }
	        break ;
	    case envstr_gid:
	        if (idp->gid != gidend) {
	            uv = idp->gid ;
	            rs = ctdecui(digbuf,DIGBUFLEN,uv) ;
	            if (rs >= 0) {
	                rs = sncpy4(envbuf,envlen,envpre,es,"=",digbuf) ;
	                el = rs ;
	            }
	        }
	        break ;
	    case envstr_username:
	        cp = idp->username ;
	        if ((cp != nullptr) && (cp[0] != '\0')) {
	            rs = sncpy4(envbuf,envlen,envpre,es,"=",cp) ;
	            el = rs ;
	        }
	        break ;
	    case envstr_groupname:
	        cp = idp->groupname ;
	        if ((cp != nullptr) && (cp[0] != '\0')) {
	            rs = sncpy4(envbuf,envlen,envpre,es,"=",cp) ;
	            el = rs ;
	        }
	        break ;
	    case envstr_keyname:
	        if ((kn != nullptr) && (kn[0] != '\0')) {
	            rs = sncpy4(envbuf,envlen,envpre,es,"=",kn) ;
	            el = rs ;
	        }
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && (envbuf[0] != '\0')) {
	        rs = statmsg_envstore(op,spp,ev,n,envbuf,el) ;
	        if (rs > 0) n += 1 ;
	    }
	} /* end for */
	ev[n] = nullptr ; /* very important! */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (statmsg_envadds) */

static int statmsg_envstore(STATMSG *op,strpack *spp,cchar **ev,int n,
		cchar *ep,int el) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (ep != nullptr) {
	    cchar	*cp ;
	    if ((rs = strpack_store(spp,ep,el,&cp)) >= 0) {
	        ev[n++] = cp ;
	        rs = n ;
	    }
	}

	return rs ;
}
/* end subroutine (statmsg_envstore) */

static int statmsg_idcheck(STATMSG *op,STATMSG_ID *idp,char *ubuf) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (idp->groupname == nullptr) return SR_FAULT ;

	if (idp->groupname[0] == '\0') return SR_INVALID ;

	if ((rs >= 0) && (idp->uid == uidend)) {
	    idp->uid = getuid() ;
	}

	if ((rs >= 0) && (idp->gid == gidend)) {
	    rs = getgid_group(idp->groupname,-1) ;
	    idp->gid = rs ;
	}

	if (rs >= 0) {
	    cchar	*tun = idp->username ;
	    if ((tun == nullptr) || (tun[0] == '\0') || (tun[0] == '-')) {
	        rs = getusername(ubuf,USERNAMELEN,idp->uid) ;
	        idp->username = ubuf ;
	    }
	}

	return rs ;
}
/* end subroutine (statmsg_idcheck) */

static int statmsg_processor(STATMSG *op,cchar **ev,cchar **adms,cchar *gn,
		cchar *kn,int fd) noex {
	int		rs ;
	int		wlen = 0 ;

	if ((rs = statmsg_checker(op,0)) >= 0) {
	    if (op->nmaps > 0) {
	        rs = mapper_process(&op->mapper,ev,adms,gn,kn,fd) ;
	        wlen += rs ;
	    }
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (statmsg_processor) */

static int mapper_start(MA *mmp,time_t dt,cc *un,cc *uh,cc *fname) noex {
	cint	to = TO_MAPCHECK ;
	int		rs ;
	cchar		**evp = (cchar **) environ ;
	cchar		*ccp ;

	memclear(mmp) ;
	mmp->username = un ;
	mmp->userhome = uh ;

	if ((rs = lockrw_create(&mmp->rwm,0)) >= 0) {
	    if ((rs = uc_mallocstrw(fname,-1,&ccp)) >= 0) {
	        mmp->fname = ccp ;
	        if ((rs = vechand_start(&mmp->mapdirs,4,0)) >= 0) {
		    PARAMFILE	*dfp = &mmp->dirsfile ;
	            if ((rs = paramfile_open(dfp,evp,ccp)) >= 0) {
	                if ((rs = paramfile_checkint(dfp,to)) >= 0) {
	                    mmp->magic = MAMAGIC ;
	                    rs = mapper_mapload(mmp) ;
	                    mmp->ti_check = dt ;
			    if (rs < 0)
			        mmp->magic = 0 ;
	                }
	                if (rs < 0)
		            paramfile_close(&mmp->dirsfile) ;
	            } /* end if (paramfile_open) */
	            if (rs < 0)
		        vechand_finish(&mmp->mapdirs) ;
	        } /* end if (vechand_start) */
	        if (rs < 0) {
	            uc_free(mmp->fname) ;
	            mmp->fname = nullptr ;
	        }
	    } /* end if (memory-allocation) */
	    if (rs < 0)
	        lockrw_destroy(&mmp->rwm) ;
	} /* end if (lockrw_create) */

	return rs ;
}
/* end subroutine (mapper_start) */

static int mapper_finish(MA *mmp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MAMAGIC) return SR_NOTOPEN ;

	rs1 = paramfile_close(&mmp->dirsfile) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = mapper_mapfins(mmp) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(&mmp->mapdirs) ;
	if (rs >= 0) rs = rs1 ;

	if (mmp->fname != nullptr) {
	    rs1 = uc_free(mmp->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    mmp->fname = nullptr ;
	}

	rs1 = lockrw_destroy(&mmp->rwm) ;
	if (rs >= 0) rs = rs1 ;

	mmp->magic = 0 ;

	return rs ;
}
/* end subroutine (mapper_finish) */

static int mapper_check(MA *mmp,time_t dt) noex {
	cint	to_lock = TO_LOCK ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		nchanged = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MAMAGIC) return SR_NOTOPEN ;

	if ((rs = lockrw_wrlock(&mmp->rwm,to_lock)) >= 0) {

	    if (dt == 0)
	        dt = time(nullptr) ;

	    if ((dt - mmp->ti_check) >= TO_MAPCHECK) {

#if	CF_PARAMFILE
	        if ((rs = paramfile_check(&mmp->dirsfile,dt)) > 0) {

	            {
	                mapper_mapfins(mmp) ;
	                vechand_delall(&mmp->mapdirs) ;
	            }

	            rs = mapper_mapload(mmp) ;
	            nchanged = rs ;

	        } /* end if */
#else /* CF_PARAMFILE */
	        {
	            USTAT	sb ;

	            int	rs1 = u_stat(mmp->fname,&sb) ;


	            if ((rs1 >= 0) && (sb.st_mtime > mmp->ti_mtime)) {

	                {
	                    mapper_mapfins(mmp) ;
	                    vechand_delall(&mmp->mapdirs) ;
	                }

	                rs = mapper_mapload(mmp) ;
	                nchanged = rs ;

	            } /* end if (file mtime check) */

	            mmp->ti_check = dt ;
	        }
#endif /* CF_PARAMFILE */

	    } /* end if (map-object check) */

	    rs1 = lockrw_unlock(&mmp->rwm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (read-write lock) */

	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (mapper_check) */

static int mapper_process(MA *mmp,cc **ev,cc **adms,
		cc *gn,cc *kn,int fd) noex {
	cint	to_lock = TO_LOCK ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MAMAGIC) return SR_NOTOPEN ;

	if ((rs = lockrw_rdlock(&mmp->rwm,to_lock)) >= 0) {
	    {
	        rs = mapper_processor(mmp,ev,adms,gn,kn,fd) ;
	        wlen += rs ;
	    }
	    rs1 = lockrw_unlock(&mmp->rwm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (read-write lock) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_process) */

static int mapper_processor(MA *mmp,cc **ev,cc **adms,
		cc *gn,cc *kn,int fd) noex {
	MD	*ep ;
	int		rs = SR_OK ;
	int		i ;
	int		wlen = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MAMAGIC) return SR_NOTOPEN ;

	for (i = 0 ; vechand_get(&mmp->mapdirs,i,&ep) >= 0 ; i += 1) {
	    if (ep != nullptr) {
	        rs = mapdir_process(ep,ev,adms,gn,kn,fd) ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_processor) */

#if	CF_PARAMFILE
static int mapper_mapload(MA *mmp) noex {
	USTAT	sb ;
	PARAMFILE	*pfp = &mmp->dirsfile ;
	PARAMFILE_ENT	pe ;
	PARAMFILE_CUR	cur ;
	int		rs = SR_OK ;
	int		c = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MAMAGIC) return SR_NOTOPEN ;

	if ((rs = u_stat(mmp->fname,&sb)) >= 0) {
	    mmp->ti_mtime = sb.st_mtime ;
	    if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	        cint	plen = PBUFLEN ;
		int		kl, vl ;
		int		fl ;
		cchar		*kp, *vp ;
		char		pbuf[PBUFLEN + 1] ;

	        while (rs >= 0) {

	            kl = paramfile_enum(pfp,&cur,&pe,pbuf,plen) ;
	            if (kl == SR_NOTFOUND) break ;
	            rs = kl ;
	            if (rs < 0) break ;

	            kp = pe.key ;
	            vp = pe.value ;
	            vl = pe.vlen ;

	            while ((fl = sichr(vp,vl,CH_FS)) >= 0) {
			if (fl > 0) {
	                    c += 1 ;
	                    rs = mapper_mapadd(mmp,kp,kl,vp,fl) ;
			}
			vl -= (fl+1) ;
			vp = (vp+(fl+1)) ;
	                if (rs < 0) break ;
	            } /* end while */

		    if ((rs >= 0) && (vl > 0)) {
	                c += 1 ;
	                rs = mapper_mapadd(mmp,kp,kl,vp,vl) ;
		    }

	        } /* end while */

	        paramfile_curend(&mmp->dirsfile,&cur) ;
	    } /* end if (paramfile-cursor) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (stat) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mapper_mapload) */

#else /* CF_PARAMFILE */

static int mapper_mapload(MA *mmp) noex {
	bfile		mfile, *mfp = &mfile ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		kl, vl ;
	int		sl ;
	int		c = 0 ;
	cchar		*tp, *sp ;
	cchar		*kp, *vp ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MAMAGIC) return SR_NOTOPEN ;

	if ((rs1 = bopen(mfp,mmp->fname,"r",0666)) >= 0) {
	    USTAT	sb ;
	    if ((rs = bcontrol(mfp,BC_STAT,&sb)) >= 0) {
		cint	llen ;
		int		len ;
		char		lbuf[LINEBUFLEN + 1] ;

		mmp->ti_mtime = sb.st_mtime ;
		while ((rs = breadln(mfp,lbuf,llen)) > 0) {
	    	    len = rs ;

	    sp = lbuf ;
	    sl = len ;
	    if (sp[0] == '#') continue ;

	    if ((tp = strnchr(sp,sl,'#')) != nullptr)
	        sl = (tp - sp) ;

	    kl = nextfield(sp,sl,&kp) ;
	    if (kl == 0) continue ;

	    sl -= ((kp + kl) - sp) ;
	    sp = (kp + kl) ;

	    vl = nextfield(sp,sl,&vp) ;
	    if (vl == 0) continue ;

	    c += 1 ;
	    rs = mapper_mapadd(mmp,kp,kl,vp,vl) ;

	    	if (rs < 0) break ;
		} /* end while (reading lines) */

	    } /* end if (bcontrol) */
	    bclose(mfp) ;
	} /* end if (file-open) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mapper_mapload) */

#endif /* CF_PARAMFILE */

static int mapper_mapadd(MA *mmp,cchar *kp,int kl,cchar *vp,int vl) noex {
	MD	*ep ;
	cint	sz = sizeof(MD) ;
	int		rs ;

	if ((kp == nullptr) || (vp == nullptr)) return SR_FAULT ;
	if ((kl == 0) || (vl == 0)) return SR_INVALID ;

	if ((rs = uc_malloc(sz,&ep)) >= 0) {
	    cchar	*un = mmp->username ;
	    cchar	*uh = mmp->userhome ;
	    if ((rs = mapdir_start(ep,un,uh,kp,kl,vp,vl)) >= 0) {
	        rs = vechand_add(&mmp->mapdirs,ep) ;
	        if (rs < 0)
	            mapdir_finish(ep) ;
	    }
	    if (rs < 0)
	        uc_free(ep) ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (mapper_mapadd) */

static int mapper_mapfins(MA *mmp) noex {
	MD	*ep ;
	vechand		*mlp = &mmp->mapdirs ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MAMAGIC) return SR_NOTOPEN ;

	for (i = 0 ; vechand_get(mlp,i,&ep) >= 0 ; i += 1) {
	    if (ep != nullptr) {
	        rs1 = mapdir_finish(ep) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = vechand_del(mlp,i--) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = uc_free(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (mapper_mapfins) */

static int mapdir_start(MD *ep,cchar *un,cchar *uh,
		cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_OK ;

	if (ep == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;
	if (vp == nullptr) return SR_FAULT ;

	if ((kl == 0) || (vl == 0)) return SR_INVALID ;

	memclear(ep) ;
	ep->username = un ;
	ep->userhome = uh ;

	if (kl < 0)
	    kl = strlen(kp) ;

	if (vl < 0)
	    vl = strlen(vp) ;

	{
	    cint	sz = (kl + 1 + vl + 1) ;
	    char	*bp ;
	    if ((rs = uc_malloc(sz,&bp)) >= 0) {
	        ep->admin = bp ;
		bp = strwcpy(bp,kp,kl) + 1 ;
		ep->dirname = bp ;
		bp = strwcpy(bp,vp,vl) + 1 ;
		rs = lockrw_create(&ep->rwm,0) ;
		if (rs < 0) {
	    	    uc_free(ep->admin) ;
		    ep->admin = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end block */

	return rs ;
}
/* end subroutine (mapdir_start) */

static int mapdir_finish(MD *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (ep == nullptr) return SR_FAULT ;

	if (ep->dname != nullptr) {
	    rs1 = uc_free(ep->dname) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->dname = nullptr ;
	}

	rs1 = lockrw_destroy(&ep->rwm) ;
	if (rs >= 0) rs = rs1 ;

	if (ep->admin != nullptr) {
	    rs1 = uc_free(ep->admin) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->admin = nullptr ;
	    ep->dirname = nullptr ;
	}

	return rs ;
}
/* end subroutine (mapdir_finish) */

static int mapdir_process(MD *ep,cchar **ev,cchar **adms,
		cchar *gn,cchar *kn,int fd) noex {
	cint	to_lock = TO_LOCK ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;

	if (ep->dirname[0] != '\0') {
	    int	f_continue = TRUE ;
	    if ((adms != nullptr) && (adms[0] != nullptr)) {
	        f_continue = (matstr(adms,ep->admin,-1) >= 0) ;
	    } /* end if (adms) */
	    if (f_continue) {
	        if ((ep->dirname[0] != '/') && (ep->dname == nullptr)) {
	            rs = mapdir_expand(ep) ;
	        }
	        if (rs >= 0) {
	            if ((ep->dirname[0] == '/') || (ep->dname != nullptr)) {
	                if ((rs = lockrw_rdlock(&ep->rwm,to_lock)) >= 0) {
			    cchar	*dn = ep->dirname ;
	                    if ((dn[0] != '~') || (ep->dname != nullptr)) {
	                        rs = mapdir_processor(ep,ev,gn,kn,fd) ;
	            		wlen += rs ;
	    		    } /* end if */
	    		    rs1 = lockrw_unlock(&ep->rwm) ;
	    		    if (rs >= 0) rs = rs1 ;
			} /* end if (locked) */
		    } /* end if (acceptable) */
		} /* end if (ok) */
	    } /* end if (continued) */
	} /* end if */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_process) */

static int mapdir_expand(MD *ep) noex {
	cint	to_lock = TO_LOCK ;
	int		rs ;
	int		rs1 ;

	if ((rs = lockrw_wrlock(&ep->rwm,to_lock)) >= 0) {

	    if ((ep->dirname[0] != '/') && (ep->dname == nullptr)) {
	        rs = mapdir_expander(ep) ;
	    } /* end if */

	    rs1 = lockrw_unlock(&ep->rwm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (read-write lock) */

	return rs ;
}
/* end subroutine (mapdir_expand) */

static int mapdir_expander(MD *ep) noex {
	int		rs = SR_OK ;
	int		pl = 0 ;

	if (ep->dirname != nullptr) {
	    cchar	*pp ;
	    char	tmpfname[MAXPATHLEN + 1] ;

	if (ep->dirname[0] == '~') {
	    pp = tmpfname ;
	    rs = mkuserpath(tmpfname,ep->username,ep->dirname,-1) ;
	    pl = rs ;
	} else if (ep->dirname[0] != '/') {
	    pp = tmpfname ;
	    rs = mkpath2(tmpfname,ep->userhome,ep->dirname) ;
	    pl = rs ;
	} else {
	    pp = ep->dirname ;
	    pl = -1 ;
	}
	   
	    if (rs >= 0) {
		cchar	*cp ;
	        rs = uc_mallocstrw(pp,pl,&cp) ;
	        if (rs >= 0) ep->dname = cp ;
		if (pl < 0) pl = (rs-1) ;
	    }

	} else
		rs = SR_INVALID ;

	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mapdir_expander) */

static int mapdir_processor(MD *ep,cchar **ev,cchar *gn,
		cchar *kn,int fd) noex {
	USTAT	sb ;
	vecstr		nums ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		n ;
	int		wlen = 0 ;
	int		f_continue = TRUE ;
	cchar		*defname = STATMSG_DEFGROUP ;
	cchar		*allname = STATMSG_ALLGROUP ;
	cchar		*suf = STATMSG_SUF ;
	cchar		*dn ;
	char		env_admin[ENVBUFLEN+1] ;
	char		env_admindir[ENVBUFLEN+1] ;

	dn = ep->dirname ;
	if (dn[0] == '~') {
	    dn = ep->dname ;
	    f_continue = ((dn != nullptr) && (dn[0] != '\0')) ;
	}
	if (f_continue) {
	    if ((rs1 = u_stat(dn,&sb)) >= 0) {
	        cint	envlen = ENVBUFLEN ;
	        cchar		*post ;
	        post = envstrs[envstr_admin] ;
	        strdcpy4(env_admin,envlen,envpre,post,"=",ep->admin) ;
	        post = envstrs[envstr_admindir] ;
	        strdcpy4(env_admindir,envlen,envpre,post,"=",dn) ;
	        for (n = 0 ; ev[n] != nullptr ; n += 1) ;
	        ev[n+0] = env_admin ;
	        ev[n+1] = env_admindir ;
	        ev[n+2] = nullptr ;
	        if ((rs = vecstr_start(&nums,0,0)) >= 0) {
	            FSDIR	d ;
	            FSDIR_ENT	de ;
	            int		i ;
	            cchar	*strs[5] ;
	            loadstrs(strs,kn,defname,allname,suf) ;
	            if ((rs = fsdir_open(&d,dn)) >= 0) {
	                cchar	*tp ;
	                while ((rs = fsdir_read(&d,&de)) > 0) {
	                    cchar	*den = de.name ;
	                    if (den[0] != '.') {
	            	        tp = strchr(den,'.') ;
	            		if ((tp != nullptr) && (strcmp((tp+1),suf) == 0)) {
				    int		f = TRUE ;
	   			    cchar	*digp ;
	                	    digp = strnpbrk(den,(tp-den),"0123456789") ;
	                	    if (digp != nullptr) {
	                    		f = hasalldig(digp,(tp-digp)) ;
				    }
				    if (f) {
	                	        if ((kn[0] != '\0') && (kn[0] != '-')) {
			    		    for (i = 0 ; i < 3 ; i += 1) {
					    f = isBaseMatch(den,strs[i],digp) ;
					    if (f) break ;
			    		    } /* end for */
	                		}
				    }
	                	    if (f) {
	                    		rs = vecstr_add(&nums,den,(tp-den)) ;
				    }
	            		} /* end if (have an STATMSG file) */
			    }
	           	    if (rs < 0) break ;
	        	} /* end while (reading directory entries) */
	        	rs1 = fsdir_close(&d) ;
			if (rs >= 0) rs = rs1 ;
	  	    } /* end if (fsdir) */
		    if (rs >= 0) {
	        	vecstr_sort(&nums,nullptr) ;
	        	rs = mapdir_processorthem(ep,ev,dn,&nums,strs,fd) ;
	        	wlen += rs ;
	   	    } /* end if */
	   	    vecstr_finish(&nums) ;
		} /* end if (vecstr-nums) */
		{
	    	    ev[n] = nullptr ;
		}
	    } /* end if (u_stat) */
	} /* end if (continued) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_processor) */

static int mapdir_processorthem(MD *ep,cchar **ev,cchar *dn,
		vecstr *blp,cchar **strs,int fd) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*kn ;
	char		kbuf[2] ;

	kn = strs[0] ;

	if (kn[0] == '-') {
	    kn = kbuf ;
	    kbuf[0] = '\0' ;
	}

	rs1 = mapdir_processorone(ep,ev,dn,blp,kn,fd) ;

	if (kn[0] != '\0') {

	if (isNotPresent(rs1)) {
	    kn = strs[1] ;
	    rs1 = mapdir_processorone(ep,ev,dn,blp,kn,fd) ;
	    if (! isNotPresent(rs1)) rs = rs1 ;
	} else {
	    rs = rs1 ;
	}
	if (rs > 0) wlen += rs ;
	if (rs >= 0) {
	    kn = strs[2] ;
	    rs1 = mapdir_processorone(ep,ev,dn,blp,kn,fd) ;
	    if (! isNotPresent(rs1)) rs = rs1 ;
	    if (rs > 0) wlen += rs ;
	}

	} /* end if */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_processorthem) */

static int mapdir_processorone(MD *ep,cchar **ev,cchar *dn,
		vecstr *blp,cchar *kn,int fd) noex {
	cint	kl = strlen(kn) ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		c = 0 ;
	int		wlen = 0 ;
	cchar		*bep ;

	for (i = 0 ; vecstr_get(blp,i,&bep) >= 0 ; i += 1) {
	    if (bep != nullptr) {

	    if (strncmp(bep,kn,kl) == 0) {
	        c += 1 ;
	        rs1 = mapdir_procout(ep,ev,dn,bep,fd) ;
	        if (rs1 >= 0) {
	            wlen += rs1 ;
	        } else if (! isNotPresent(rs1))
	            rs = rs1 ;
	    }

	    }
	    if (rs < 0) break ;
	} /* end for */

	if ((rs >= 0) && (c == 0)) rs = SR_NOENT ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_processorone) */

static int mapdir_procout(MD *ep,cchar **ev,cchar *dn,cchar *bn,int fd) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*suf = STATMSG_SUF ;
	char		cname[MAXNAMELEN + 1] ;
	char		fname[MAXPATHLEN + 1] ;

/* we ignore buffer overflow here */

	rs1 = snsds(cname,MAXNAMELEN,bn,suf) ;
	if (rs1 >= 0)
	    rs1 = mkpath2(fname,dn,cname) ;

	if (rs1 >= 0) {
	    rs = mapdir_procouter(ep,ev,fname,fd) ;
	    wlen += rs ;
	}

	if ((rs >= 0) && (rs1 == SR_OVERFLOW)) rs = SR_NOENT ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procout) */

static int mapdir_procouter(MD *ep,cchar **ev,cchar *fn,int ofd) noex {
	const mode_t	operms = 0664 ;
	cint	oflags = O_RDONLY ;
	cint	to_open = TO_OPEN ;
	cint	to_read = TO_READ ;
	cint	to_write = TO_WRITE ;
	int		rs ;
	int		wlen = 0 ;

	if (ep == nullptr) return SR_FAULT ;

	if ((rs = uc_openenv(fn,oflags,operms,ev,to_open)) >= 0) {
	    cint	mfd = rs ;
	    cint	olen = MSGBUFLEN ;
	    char	obuf[MSGBUFLEN + 1] ;

#if	CF_WRITETO
	    while ((rs = uc_reade(mfd,obuf,olen,to_read,0)) > 0) {
	        rs = writeto(ofd,obuf,rs,to_write) ;
	        wlen += rs ;
	        if (rs < 0) break ;
	    } /* end while */
#else /* CF_WRITETO */
	    rs = uc_writedesc(ofd,mfd,-1) ;
	    wlen += rs ;
#endif /* CF_WRITETO */

	    u_close(mfd) ;
	} /* end if (open) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procouter) */

#if	CF_WRITETO

static int writeto(int wfd,cchar *wbuf,int wlen,int wto) noex {
	POLLFD		fds[2] ;
	time_t		dt = time(nullptr) ;
	time_t		ti_write ;
	int		rs = SR_OK ;
	int		i ;
	int		pt = TO_POLL ;
	int		pto ;
	int		tlen = 0 ;

	if (wbuf == nullptr) return SR_FAULT ;

	if (wfd < 0) return SR_BADF ;

	if (wlen < 0)
	    wlen = strlen(wbuf) ;

	if (pt > wto)
	    pt = wto ;

	i = 0 ;
	fds[i].fd = wfd ;
	fds[i].events = POLLOUT ;
	i += 1 ;
	fds[i].fd = -1 ;
	fds[i].events = 0 ;

	ti_write = dt ;
	pto = (pt * POLLMULT) ;
	while ((rs >= 0) && (tlen < wlen)) {

	    rs = u_poll(fds,1,pto) ;

	    dt = time(nullptr) ;
	    if (rs > 0) {
	        cint	re = fds[0].revents ;

	        if (re & POLLOUT) {
	            rs = u_write(wfd,(wbuf+tlen),(wlen-tlen)) ;
	            tlen += rs ;
	            ti_write = dt ;
	        } else if (re & POLLHUP) {
	            rs = SR_HANGUP ;
	        } else if (re & POLLERR) {
	            rs = SR_POLLERR ;
	        } else if (re & POLLNVAL) {
	            rs = SR_NOTOPEN ;
	        } /* end if (poll returned) */

	    } /* end if (got something) */

	    if (rs == SR_INTR)
	        rs = SR_OK ;

	    if ((dt - ti_write) >= wto)
	        break ;

	} /* end while */

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (writeto) */

#endif /* CF_WRITETO */

static int loadstrs(cc **strs,cc *gn,cc *def,cc *all,cc *name) noex {
	int		i = 0 ;

#ifdef	COMMENT
	{
	    cchar	*cp ;
	    for (i = 0 ; i < 4 ; i += 1) {
	        switch (i) {
	        case 0: 
	            cp = gn ; 
	            break ;
	        case 1: 
	            cp = def ; 
	            break ;
	        case 2: 
	            cp = all ; 
	            break ;
	        case 3: 
	            cp = name ; 
	            break ;
	        } /* end switch */
	        strs[i] = cp ;
	    } /* end for */
	}
#else
	strs[i++] = gn ;
	strs[i++] = def ;
	strs[i++] = all ;
	strs[i++] = name ;
#endif /* COMMENT */
	strs[i] = nullptr ;

	return SR_OK ;
}
/* end subroutine (loadstrs) */

static bool isBaseMatch(cchar *den,cchar *bname,cchar *digp) noex {
	bool		f = FALSE ;
	if (digp == nullptr) {
	    int	bl = strlen(bname) ;
	    int	m = nleadstr(den,bname,bl) ;
	    f = (m == bl) && (den[m] == '.') ;
	} else {
	    f = (strncmp(den,bname,(digp-den)) == 0) ;
	}
	return f ;
}
/* end subroutine (isBaseMatch) */


