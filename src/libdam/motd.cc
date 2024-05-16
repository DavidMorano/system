/* motd SUPPORT */
/* lang=C++20 */

/* object to help (manage) MOTD messages */
/* version %I% last-modified %G% */

#define	CF_TESTPROC	0		/* test using |uc_openfsvc(3uc)| */
#define	CF_WRITETO	1		/* time writes */
#define	CF_FINDUID	1		/* use |finduid(3c)| */
#define	CF_UCPWCACHE	1		/* use |ucpwcache(3uc)| */

/* revision history:

	= 2003-10-01, David A­D­ Morano
	This is a hack from numerous previous hacks (not enumerated
	here). This is a new version of this hack that is entirely
	different (much simpler).

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object module writes the contents of various MOTDs (as
	specified by the caller) to an open file descriptor (also
	specified by the caller).

	Implementation notes:
	When processing, we time-out writes to the caller-supplied
	file-descriptor because we do not know if it is a non-regular
	file that might be flow-controlled.  We don't wait forever
	for those sorts of outputs.  So let us say that the output
	is a terminal that is currently flow-controlled.  We will
	time-out on our writes and the user will not get this whole
	MOTD text!

	The FINDUID feature:
	This has got to be a feature that has more code than is
	ever executed of all features. This feature handles an
	extremely small corner case where there are two or more
	USERNAMEs sharing a single UID (in the system PASSWD
	database). Further, the code comes into play when one of
	the users is already logged in and one of the other users
	sharing the same UID goes to log in. Without this code a
	random username among those sharing the same UID would be
	selected for the new user logging in. The reason for this
	is that in daemon mode we only get UIDs back from the kernel
	on a connection request. So we have to guess what the
	corresponding username might be for that connection request.
	With the FINDUID feature, we do this guessing a little bit
	more intelligently by using the username from that last
	user with the given UID who logged into the system (by
	searching the system UTMPX database). The latest user logged
	in will get his own username (within a few split seconds
	or so) but a consequence is that all other users sharing
	that same UID will also see this same username. But this
	is not usually a big problem since the read-out of the MOTD
	file is usually done at login time and often only done at
	that time. Outside of daemon mode, or stand-alone mode, the
	feature does not come into play and the correct username
	(within extremely broad limits) is always divined. So there
	it is, good and bad.  But there are not a lot of ways to
	handle it better and this feature already handles these
	cases much better than nothing at all.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<time.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<estrings.h>
#include	<ascii.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vechand.h>
#include	<getax.h>
#include	<getxid.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getusername.h>
#include	<ptma.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<strpack.h>
#include	<bfile.h>
#include	<ctdec.h>
#include	<xperm.h>
#include	<writeto.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<mknpathx.h>
#include	<strwcpy.h>
#include	<matxstr.h>
#include	<mkchar.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"motd.h"


/* local defines */

#define	MOTD_DEFGROUP	"default"
#define	MOTD_ALLGROUP	"all"
#define	MOTD_NAME	"motd"
#define	MOTD_DIRSFNAME	"dirs"
#define	MOTD_MAPMAGIC	0x21367425

#define	MD		motd_mapdir

#define	MR		motd_mr

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UCPWCACHE */

#define	NDF		"motd.deb"

#ifndef	MAXNAMELEN
#define	MAXNAMELEN	256
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	2048
#endif

#define	ADMINLEN	USERNAMELEN

#undef	ENVBUFLEN
#define	ENVBUFLEN	(10 + MAX(ADMINLEN,MAXPATHLEN))

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
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

#ifndef	CF_WRITETO
#define	CF_WRITETO	1
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */


/* external variables */

extern mainv	environ ;


/* local structures */

struct motd_mapdir {
	lockrw 		rwm ;
	cchar		*admin ;
	cchar		*dirname ;	/* raw */
	cchar		*dname ;	/* expanded */
} ;

typedef MD *		mdp ;


/* forward references */

static int	motd_mapfind(motd *,time_t) noex ;
static int	motd_maplose(motd *) noex ;
static int	motd_mapfname(motd *,char *) noex ;
static int	motd_schedload(motd *,vecstr *) noex ;
static int	motd_checker(motd *,time_t) noex ;
static int	motd_envbegin(motd *) noex ;
static int	motd_envend(motd *) noex ;
static int	motd_envadds(motd *,strpack *,cchar **,motd_id *) noex ;
static int	motd_envstore(motd *,strpack *,cchar **,int,cchar *,int) noex ;
static int 	motd_processor(motd *,cchar **,cchar **,cchar *,int) noex ;
static int	motd_idcheck(motd *,motd_id *,char *) noex ;
static int	motd_ufindbegin(motd *) noex ;
static int	motd_ufindend(motd *) noex ;
static int	motd_ufindlook(motd *,char *,uid_t) noex ;

static int	mapper_start(MR *,time_t,cchar *) noex ;
static int	mapper_finish(MR *) noex ;
static int	mapper_check(MR *,time_t) noex ;
static int	mapper_process(MR *,cc **,cc **,cc *,int) noex ;
static int	mapper_processor(MR *,cc **,cc **,cc *,int) noex ;
static int	mapper_mapload(MR *) noex ;
static int	mapper_mapadd(MR *,cchar *,int,cchar *,int) noex ;
static int	mapper_mapfins(MR *) noex ;

#if	CF_TESTPROC
static int	mapper_lockcheck(MR *,cchar *) noex ;
#endif

static int	mapdir_start(MD *,cchar *,int,cchar *,int) noex ;
static int	mapdir_finish(MD *) noex ;
static int	mapdir_process(MD *,cc **,cc **,cc *,int) noex ;
static int	mapdir_expand(MD *) noex ;
static int	mapdir_expander(MD *) noex ;
static int	mapdir_processor(MD *,cchar **,cchar *,int) noex ;
static int	mapdir_procout(MD *,cc **,cc *,cc *,int) noex ;
static int	mapdir_procouter(MD *,cc **,cc *,int) noex ;


/* local variables */

static constexpr cpcchar	schedmaps[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	"%n.%f",
	nullptr
} ;

static constexpr cpcchar	envbad[] = {
	"TMOUT",
	"A__z",
	nullptr

} ;

enum envstrs {
	envstr_username,
	envstr_groupname,
	envstr_uid,
	envstr_gid,
	envstr_admin,
	envstr_admindir,
	envstr_overlast
} ;

static constexpr cpcchar	envstrs[] = {
	"USERNAME",
	"GROUPNAME",
	"UID",
	"GID",
	"ADMIN",
	"ADMINDIR",
	nullptr
} ;

constexpr uid_t		uidend = -1 ;
constexpr gid_t		gidend = -1 ;

constexpr cchar		envpre[] = "MOTD_" ;	/* environment prefix */

constexpr bool		f_writeto = CF_WRITETO ;


/* exported variables */


/* exported subroutines */

int motd_open(motd *op,cchar *pr) noex {
	const time_t	dt = time(nullptr) ;
	int		rs ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	memclear(op) ;
	op->fe = MOTD_DIRSFNAME ;

	if ((rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	    op->pr = cp ;
	    if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
	        if ((rs = motd_mapfind(op,dt)) >= 0) {
	            if ((rs = motd_envbegin(op)) >= 0) {
	                op->ti_lastcheck = dt ;
	                op->magic = MOTD_MAGIC ;
	            } /* end if (envbegin) */
	            if (rs < 0)
	                motd_maplose(op) ;
	        } /* end if (mapfind) */
	        if (rs < 0)
	            ptm_destroy(op->mxp) ;
	    } /* end if (ptm_begin) */
	    if (rs < 0) {
		uc_free(op->pr) ;
		op->pr = nullptr ;
	    }
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (motd_open) */

int motd_close(motd *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MOTD_MAGIC) return SR_NOTOPEN ;

	rs1 = motd_ufindend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = motd_envend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = motd_maplose(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = ptm_destroy(op->mxp) ;
	if (rs >= 0) rs = rs1 ;

	if (op->pr != nullptr) {
	    rs1 = uc_free(op->pr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pr = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (motd_close) */

int motd_check(motd *op,time_t dt) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MOTD_MAGIC) return SR_NOTOPEN ;

	rs = motd_checker(op,dt) ;

	return rs ;
}
/* end subroutine (motd_check) */

int motd_process(motd *op,cchar groupname[],cchar *admins[],int fd) noex {
	motd_id		id{} ;
	int		rs ;
	id.groupname = groupname ;
	id.uid = -1 ;
	id.gid = -1 ;
	rs = motd_processid(op,&id,admins,fd) ;
	return rs ;
}
/* end subroutine (motd_process) */

int motd_processid(motd *op,motd_id *idp,cchar *admins[],int fd) noex {
	motd_id		id{} ;
	int		rs ;
	int		wlen = 0 ;
	cchar		*groupname ;
	char		ubuf[USERNAMELEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (idp == nullptr) return SR_FAULT ;

	if (op->magic != MOTD_MAGIC) return SR_NOTOPEN ;

	if (fd < 0) return SR_BADF ;

	groupname = idp->groupname ;
	if (groupname == nullptr)
	    return SR_FAULT ;

	if (groupname[0] == '\0')
	    return SR_INVALID ;

/* fill in some missing elements */

	id = *idp ;			/* copy for possible modification */
	if ((rs = motd_idcheck(op,&id,ubuf)) >= 0) {
	    cint	n = nelem(envstrs) ;
	    int		sz ;
	    void	*p ;
	    sz = (op->nenv + n + 1) * sizeof(cchar *) ;
	    if ((rs = uc_malloc(sz,&p)) >= 0) {
	        strpack	packer ;
	        cchar	**ev = (cchar **) p ;
	        if ((rs = strpack_start(&packer,128)) >= 0) {
	            if ((rs = motd_envadds(op,&packer,ev,&id)) >= 0) {
	                rs = motd_processor(op,ev,admins,groupname,fd) ;
	                wlen = rs ;
	            }
	            strpack_finish(&packer) ;
	        } /* end if (packer) */
	        uc_free(p) ;
	    } /* end if (memory allocation) */
	} /* end if (motd_idcheck) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (motd_processid) */

int motdid_load(motd_id *idp,cchar *un,cchar *gn,uid_t uid,gid_t gid) noex {
	if (idp == nullptr) return SR_FAULT ;

	memclear(idp) ;
	idp->uid = uid ;
	idp->gid = gid ;
	idp->username = un ;
	idp->groupname = gn ;

	return SR_OK ;
}
/* end subroutine (motdid_load) */


/* private subroutines */

static int motd_mapfind(motd *op,time_t dt) noex {
	int		rs ;
	char		mapfname[MAXPATHLEN + 1] ;

	mapfname[0] = '\0' ;
	if ((rs = motd_mapfname(op,mapfname)) >= 0) {
	    if (mapfname[0] != '\0') {
	        if ((rs = mapper_start(&op->mapper,dt,mapfname)) >= 0) {
	            op->nmaps += 1 ;
		}
	    }
	}

	return rs ;
}
/* end subroutine (motd_mapfind) */

static int motd_maplose(motd *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->nmaps > 0) {
	    rs1 = mapper_finish(&op->mapper) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nmaps = 0 ;
	}

	return rs ;
}
/* end subroutine (motd_maplose) */

static int motd_mapfname(motd *op,char *fbuf) noex {
	vecstr		scheds ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;

	fbuf[0] = '\0' ;
	if ((rs = vecstr_start(&scheds,6,0)) >= 0) {
	    if ((rs = motd_schedload(op,&scheds)) >= 0) {
	        cint	flen = MAXPATHLEN ;

	        rs1 = permsched(schedmaps,&scheds,fbuf,flen,op->fe,R_OK) ;

	        if ((rs1 == SR_NOENT) || (rs1 == SR_ACCESS)) {
	            if (rs1 == SR_NOENT) {
	                fbuf[0] = '\0' ;
	            } else {
	                rs = rs1 ;
		    }
	        } else if (rs1 == SR_OK) {
	            c = 1 ;
	        } else {
	            rs = rs1 ;
	  	}

	    } /* end if (motd-schedload) */
	    rs1 = vecstr_finish(&scheds) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (scheds) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (motd_mapfname) */

static int motd_schedload(motd *op,vecstr *slp) noex {
	int		rs = SR_OK ;
	cchar		*keys = "pen" ;
	cchar		*name = MOTD_NAME ;
	for (int i = 0 ; keys[i] != '\0' ; i += 1) {
	    cint	kch = mkchar(keys[i]) ;
	    int		vl = -1 ;
	    cchar	*vp = nullptr ;
	    switch (kch) {
	    case 'p':
		vp = op->pr ;
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
/* end subroutine (motd_schedload) */

static int motd_checker(motd *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		nchanged = 0 ;
	if (op->nmaps > 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (dt == 0) dt = time(nullptr) ;
	        if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	            rs = mapper_check(&op->mapper,dt) ;
	            nchanged = rs ;
	            op->ti_lastcheck = dt ;
	        } /* end if */
	        ptm_unlock(op->mxp) ;
	    } /* end if (mutex) */
	} /* end if (positive) */
	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (motd_checker) */

static int motd_envbegin(motd *op) noex {
	cint	es = envpre[0] ;
	cint	envprelen = strlen(envpre) ;
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
	        f = true ;
	        f = f && (ep[0] != '_') ;
	        f = f && (matstr(envbad,ep,-1) < 0) ;
	        if (f && (ep[0] == es)) {
	            f = (strncmp(envpre,ep,envprelen) != 0) ;
		}
	        if (f) {
	            va[c++] = ep ;
		}
	    } /* end for */
	    va[c] = nullptr ;
	    op->nenv = c ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (motd_envbegin) */

static int motd_envend(motd *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->envv != nullptr) {
	    rs1 = uc_free(op->envv) ;
	    if (rs >= 0) rs = rs1 ;
	    op->envv = nullptr ;
	}

	return rs ;
}
/* end subroutine (motd_envend) */

static int motd_envadds(motd *op,strpack *spp,cchar **ev,motd_id *idp) noex {
	cint	envlen = ENVBUFLEN ;
	uint		uv ;
	int		rs = SR_OK ;
	int		n, i ;
	int		el ;
	cchar		**envv = op->envv ;
	cchar		*pre = envpre ;
	cchar		*cp ;
	char		envbuf[ENVBUFLEN + 1] ;
	char		digbuf[DIGBUFLEN + 1] ;

	for (n = 0 ; n < op->nenv ; n += 1) {
	    ev[n] = envv[n] ;
	}

	for (i = 0 ; (rs >= 0) && (envstrs[i] != nullptr) ; i += 1) {
	    envbuf[0] = '\0' ;
	    el = -1 ;
	    switch (i) {
	    case envstr_uid:
	        if (idp->uid != uidend) {
	            uv = idp->uid ;
	            rs = ctdecui(digbuf,DIGBUFLEN,uv) ;
	            if (rs >= 0) {
	                rs = sncpy4(envbuf,envlen,pre,envstrs[i],"=",digbuf) ;
	                el = rs ;
	            }
	        }
	        break ;
	    case envstr_gid:
	        if (idp->gid != gidend) {
	            uv = idp->gid ;
	            rs = ctdecui(digbuf,DIGBUFLEN,uv) ;
	            if (rs >= 0) {
	                rs = sncpy4(envbuf,envlen,pre,envstrs[i],"=",digbuf) ;
	                el = rs ;
	            }
	        }
	        break ;
	    case envstr_username:
	        cp = idp->username ;
	        if ((cp != nullptr) && (cp[0] != '\0')) {
	            rs = sncpy4(envbuf,envlen,pre,envstrs[i],"=",cp) ;
	            el = rs ;
	        }
	        break ;
	    case envstr_groupname:
	        cp = idp->groupname ;
	        if ((cp != nullptr) && (cp[0] != '\0')) {
	            rs = sncpy4(envbuf,envlen,pre,envstrs[i],"=",cp) ;
	            el = rs ;
	        }
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && (envbuf[0] != '\0')) {
	        rs = motd_envstore(op,spp,ev,n,envbuf,el) ;
	        if (rs > 0) n += 1 ;
	    }
	} /* end for */
	ev[n] = nullptr ; /* very important! */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (motd_envadds) */

static int motd_envstore(motd *op,strpack *spp,cchar **ev,int n,
		cchar *ep,int el) noex {
	int		rs = SR_OK ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;

	if (ep != nullptr) {
	    rs = strpack_store(spp,ep,el,&cp) ;
	    if (rs >= 0) {
	        ev[n++] = cp ;
	        rs = n ;
	    }
	}

	return rs ;
}
/* end subroutine (motd_envstore) */

static int motd_idcheck(motd *op,motd_id *idp,char *ubuf) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (idp == nullptr) return SR_FAULT ;
	if (ubuf == nullptr) return SR_FAULT ;

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
	        ubuf[0] = '\0' ;
	        rs = SR_OK ; /* needed for later test */
#if	CF_FINDUID
	        if ((tun == nullptr) || (tun[0] == '\0')) {
	            rs = motd_ufindlook(op,ubuf,idp->uid) ;
	        }
#endif /* CF_FINDUID */
	        if (rs == SR_OK) { /* this is the "later" test */
	            rs = getusername(ubuf,USERNAMELEN,idp->uid) ;
		}
	        idp->username = ubuf ;
	    }
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (motd_idcheck) */

static int motd_processor(motd *op,cchar **ev,cchar **adms,cchar *gn,
		int fd) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = motd_checker(op,0)) >= 0) {
	    if (op->nmaps > 0) {
	        rs = mapper_process(&op->mapper,ev,adms,gn,fd) ;
	        wlen += rs ;
	    }
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (motd_processor) */

static int motd_ufindbegin(motd *op) noex {
	cint		maxent = 30 ;
	cint		ttl = (1*60*60) ;
	int		rs = SR_OK ;
	if (! op->open.ufind) {
	    rs = finduid_start(op->ufp,maxent,ttl) ;
	    op->open.ufind = (rs >= 0) ;
	}
	return rs ;
}
/* end subroutine (motd_ufindbegin) */

static int motd_ufindend(motd *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->open.ufind) {
	    op->open.ufind = false ;
	    rs1 = finduid_finish(op->ufp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (motd_ufindend) */

static int motd_ufindlook(motd *op,char *ubuf,uid_t uid) noex {
	int		rs ;
	int		rs1 ;
	int		ul = 0 ;
	if ((rs = getbufsize(getbufsize_un)) >= 0) {
	    cint	ulen = rs ;
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (! op->open.ufind) {
		    rs = motd_ufindbegin(op) ;
		}
	        if (rs >= 0) {
	            rs = finduid_lookup(op->ufp,ubuf,ulen,uid) ;
		    ul = rs ;
	        } /* end if */
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (getbusize) */
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (motd_ufindlook) */

static int mapper_start(MR *mmp,time_t dt,cchar *fname) noex {
	int		rs ;
	cchar		**evp = (cchar **) environ ;
	cchar		*ccp ;

	memclear(mmp) ;

	if ((rs = lockrw_create(&mmp->rwm,0)) >= 0) {
	    if ((rs = uc_mallocstrw(fname,-1,&ccp)) >= 0) {
		mmp->fname = ccp ;
		if ((rs = vechand_start(&mmp->mapdirs,4,0)) >= 0) {
		    cchar	*fn = mmp->fname ;
		    if ((rs = paramfile_open(&mmp->dirsfile,evp,fn)) >= 0) {
			cint	to = TO_MAPCHECK ;
			if ((rs = paramfile_checkint(&mmp->dirsfile,to)) >= 0) {
	    		    mmp->magic = MOTD_MAPMAGIC ;
	    		    rs = mapper_mapload(mmp) ;
	    		    mmp->ti_check = dt ;
			    if (rs < 0) {
	    		    	mmp->magic = 0 ;
			    }
			}
	    		if (rs < 0) {
			    paramfile_close(&mmp->dirsfile) ;
			}
		    } /* end if (paramfile_open) */
		    if (rs < 0) {
			vechand_finish(&mmp->mapdirs) ;
		    }
		} /* end if (vechand_start) */
		if (rs < 0) {
		    uc_free(mmp->fname) ;
		    mmp->fname = nullptr ;
		}
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
		lockrw_destroy(&mmp->rwm) ;
	    }
	} /* end if (lockrw_create) */

	return rs ;
}
/* end subroutine (mapper_start) */

static int mapper_finish(MR *mmp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

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

static int mapper_check(MR *mmp,time_t dt) noex {
	cint	to_lock = TO_LOCK ;
	int		rs ;
	int		rs1 ;
	int		nchanged = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

	if ((rs = lockrw_wrlock(&mmp->rwm,to_lock)) >= 0) {
	    cint	to = TO_MAPCHECK ;

	    if (dt == 0) dt = time(nullptr) ;

	    if ((dt - mmp->ti_check) >= to) {

	        if ((rs = paramfile_check(&mmp->dirsfile,dt)) > 0) {

	            {
	                mapper_mapfins(mmp) ;
	                vechand_delall(&mmp->mapdirs) ;
	            }

	            rs = mapper_mapload(mmp) ;
	            nchanged = rs ;

	        } /* end if */

	    } /* end if (map-object check) */

	    rs1 = lockrw_unlock(&mmp->rwm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (read-write lock) */

	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (mapper_check) */

static int mapper_process(MR *mmp,cchar **ev,cchar **adms,
		cchar *gn,int fd) noex {
	cint	to_lock = TO_LOCK ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

	if ((rs = lockrw_rdlock(&mmp->rwm,to_lock)) >= 0) {

#if	CF_TESTPROC
	    {
	        cmode	om = 0666 ;
	        cint	of = O_RDONLY ;
	        cint	to = 5 ;
	        cchar		*pr = "/home/genserv" ;
	        cchar		*prn = "genserv" ;
	        cchar		*svc = "hello" ;
	        cchar		*argv[2] ;

	        argv[0] = svc ;
	        argv[1] = nullptr ;
	        if ((rs = uc_openfsvc(pr,prn,svc,of,om,argv,ev,to)) >= 0) {
	            cint	rfd = rs ;
	            char	buf[BUFLEN+1] ;
	            while ((rs = uc_reade(rfd,buf,BUFLEN,5,0)) > 0) {
	                int	len = rs ;
	                rs = writeto(fd,buf,len,10) ;
	                wlen = rs ;
	                if (rs < 0) break ;
	            } /* end while */
	            u_close(rfd) ;
	        } /* end if (uc_openfsvc) */
	    }
#else /* CF_TESTPROC */
	    rs = mapper_processor(mmp,ev,adms,gn,fd) ;
	    wlen += rs ;
#endif /* CF_TESTPROC */

	    rs1 = lockrw_unlock(&mmp->rwm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (read-write lock) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_process) */

static int mapper_processor(MR *mmp,cchar **ev,cchar **adms,
		cchar	*gn,int fd) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

	void	*vp{} ;
	for (int i = 0 ; vechand_get(&mmp->mapdirs,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		MD	*ep = mdp(vp) ;
	        rs = mapdir_process(ep,ev,adms,gn,fd) ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_processor) */

static int mapper_mapload(MR *mmp) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		c = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

	if (u_stat(mmp->fname,&sb) >= 0) {
	    paramfile		*pfp = &mmp->dirsfile ;
	    paramfile_ent	pe ;
	    paramfile_cur	cur ;
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

	        paramfile_curend(pfp,&cur) ;
	    } /* end if (paramfile-cursor) */
	} /* end if (stat) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mapper_mapload) */

static int mapper_mapadd(MR *mmp,cc *kp,int kl,cc *vp,int vl) noex {
	MD	*ep ;
	cint	sz = sizeof(MD) ;
	int		rs ;

	if ((kp == nullptr) || (vp == nullptr)) return SR_FAULT ;

	if ((kl == 0) || (vl == 0)) return SR_INVALID ;

	if ((rs = uc_malloc(sz,&ep)) >= 0) {
	    if ((rs = mapdir_start(ep,kp,kl,vp,vl)) >= 0) {
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

static int mapper_mapfins(MR *mmp) noex {
	vechand		*mlp = &mmp->mapdirs ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

	void	*vp{} ;
	for (int i = 0 ; vechand_get(mlp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		MD	*ep = mdp(vp) ;
		{
	            rs1 = mapdir_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = vechand_del(mlp,i--) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(ep) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (mapper_mapfins) */

#if	CF_TESTPROC

static int mapper_lockcheck(MR *mmp,cchar *s) noex {
	cint	to_lock = TO_LOCK ;
	int		rs ;
	int		rs1 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

	if ((rs = lockrw_rdlock(&mmp->rwm,to_lock)) >= 0) {
	    rs1 = lockrw_unlock(&mmp->rwm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (read-write lock) */

	return rs ;
}
/* end subroutine (mapper_lockcheck) */

#endif /* CF_TESTPROC */

static int mapdir_start(MD *ep,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs ;

	if (ep == nullptr) return SR_FAULT ;
	if ((kp == nullptr) || (vp == nullptr)) return SR_FAULT ;

	if ((kl == 0) || (vl == 0)) return SR_INVALID ;

	memclear(ep) ;

	if (kl < 0)
	    kl = strlen(kp) ;

	if (vl < 0)
	    vl = strlen(vp) ;

	{
	    cint	sz = (kl + 1 + vl + 1) ;
	    char	*p ;
	    if ((rs = uc_malloc(sz,&p)) >= 0) {
	        char	*bp = p ;
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
	} /* end if (memory-allocation) */

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

static int mapdir_process(MD *ep,cchar **ev,cchar **admins,
		cchar *gn,int fd) noex {
	cint	to_lock = TO_LOCK ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;

	if (ep->dirname[0] != '\0') {
	    int		f_continue = true ;
	    if ((admins != nullptr) && (admins[0] != nullptr)) {
	        f_continue = (matstr(admins,ep->admin,-1) >= 0) ;
	    } /* end if (admins) */
	    if (f_continue) {
	        if ((ep->dirname[0] == '~') && (ep->dname == nullptr)) {
	            rs = mapdir_expand(ep) ;
	        }
	        if (rs >= 0) {
	            if ((ep->dirname[0] != '~') || (ep->dname != nullptr)) {
	                if ((rs = lockrw_rdlock(&ep->rwm,to_lock)) >= 0) {
			    cchar	*dn = ep->dirname ;
	                    if ((dn[0] != '~') || (ep->dname != nullptr)) {
	            		rs = mapdir_processor(ep,ev,gn,fd) ;
	            		wlen += rs ;
	    		    } /* end if */
	    		    rs1 = lockrw_unlock(&ep->rwm) ;
	    		    if (rs >= 0) rs = rs1 ;
			} /* end if (locked) */
		    } /* end if (ready) */
		} /* end if (ok) */
	    } /* end if (continued) */
	} /* end if (non-nul) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_process) */

static int mapdir_expand(MD *ep) noex {
	cint		to_lock = TO_LOCK ;
	int		rs ;
	int		rs1 ;
	if ((rs =  lockrw_wrlock(&ep->rwm,to_lock)) >= 0) {
	    if ((ep->dirname[0] == '~') && (ep->dname == nullptr)) {
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
	int		rs1 ;
	int		fl = 0 ;

	if ((ep->dirname != nullptr) && (ep->dirname[0] == '~')) {
	    int		unl = -1 ;
	    cchar	*un = (ep->dirname+1) ;
	    cchar	*tp ;
	    cchar	*pp = nullptr ;
	    char	ubuf[USERNAMELEN + 1] ;
	    if ((tp = strchr(un,'/')) != nullptr) {
	        unl = (tp - un) ;
	        pp = tp ;
	    }
	    if ((unl == 0) || (un[0] == '\0')) {
	        un = ep->admin ;
	        unl = -1 ;
	    }
	    if (unl >= 0) {
	   	strwcpy(ubuf,un,MIN(unl,USERNAMELEN)) ;
		un = ubuf ;
	    }
	    if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	        ucentpw		pw ;
	        cint		pwlen = rs ;
	        char		*pwbuf ;
	        if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	            if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,un)) >= 0) {
		        cchar	*uh = pw.pw_dir ;
	    	        char	hbuf[MAXPATHLEN + 1] ;
	                if (pp != nullptr) {
	                    rs = mkpath2(hbuf,uh,pp) ;
	                    fl = rs ;
	                } else {
	                    rs = mkpath1(hbuf,uh) ;
	                    fl = rs ;
	                }
	                if (rs >= 0) {
	                    cchar	*cp ;
	                    rs = uc_mallocstrw(hbuf,fl,&cp) ;
	                    if (rs >= 0) ep->dname = cp ;
	                }
		    } else if (isNotPresent(rs)) {
		        rs = SR_OK ;
	            } /* end if (getpw_name) */
	            rs1 = uc_free(pwbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (getbufsize) */
	} else {
	    rs = SR_INVALID ;
	}

	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (mapdir_expander) */

static int mapdir_processor(MD *ep,cchar **ev,cchar *gn,int fd) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	int		f_continue = true ;
	cchar		*dn = ep->dirname ;

	if (dn[0] == '~') {
	    dn = ep->dname ;
	    f_continue = ((dn != nullptr) && (dn[0] != '\0')) ;
	}
	if (f_continue) {
	    cint	envlen = ENVBUFLEN ;
	    int		n ;
	    cchar	*pre = envpre ;
	    cchar	*post ;
	    cchar	*defname = MOTD_DEFGROUP ;
	    cchar	*allname = MOTD_ALLGROUP ;
	    char	env_admin[ENVBUFLEN+1] ;
	    char	env_admindir[ENVBUFLEN+1] ;
	    post = envstrs[envstr_admin] ;
	    strdcpy4(env_admin,envlen,pre,post,"=",ep->admin) ;
	    post = envstrs[envstr_admindir] ;
	    strdcpy4(env_admindir,envlen,pre,post,"=",dn) ;
	    for (n = 0 ; ev[n] != nullptr ; n += 1) ;
	    ev[n+0] = env_admin ;
	    ev[n+1] = env_admindir ;
	    ev[n+2] = nullptr ;
	    rs1 = mapdir_procout(ep,ev,dn,gn,fd) ;
	    if (isNotPresent(rs1)) {
	        gn = defname ;
	        rs1 = mapdir_procout(ep,ev,dn,gn,fd) ;
	        if (! isNotPresent(rs1)) rs = rs1 ;
	    } else {
	        rs = rs1 ;
	    }
	    if (rs > 0) wlen += rs ;
	    if (rs >= 0) {
	        gn = allname ;
	        rs1 = mapdir_procout(ep,ev,dn,gn,fd) ;
	        if (! isNotPresent(rs1)) rs = rs1 ;
	        if (rs > 0) wlen += rs ;
	    }
	    {
	        ev[n] = nullptr ;
	    }
	} /* end if (continued) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_processor) */

static int mapdir_procout(MD *ep,cchar **ev,cchar *dn,cchar *gn,int fd) noex {
	cint	clen = MAXNAMELEN ;
	int		rs ;
	int		wlen = 0 ;
	cchar		*name = MOTD_NAME ;
	char		cbuf[MAXNAMELEN + 1] ;

/* we ignore buffer overflow here */

	if ((rs = snsds(cbuf,clen,gn,name)) >= 0) {
	    char	fname[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(fname,dn,cbuf)) >= 0) {
	        rs = mapdir_procouter(ep,ev,fname,fd) ;
	        wlen += rs ;
	    } else if (rs == SR_OVERFLOW) {
	        rs = SR_NOENT ;
	    }
	} else if (rs == SR_OVERFLOW) {
	    rs = SR_NOENT ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procout) */

static int mapdir_procouter(MD *ep,cchar **ev,cchar *fname,int ofd) noex {
	cmode	operms = 0664 ;
	cint	oflags = O_RDONLY ;
	cint	to_open = TO_OPEN ;
	cint	to_read = TO_READ ;
	cint	to_write = TO_WRITE ;
	int		rs ;
	int		wlen = 0 ;

	if (ep == nullptr) return SR_FAULT ;

	if ((rs = uc_openenv(fname,oflags,operms,ev,to_open)) >= 0) {
	    cint	mfd = rs ;
	    cint	olen = MSGBUFLEN ;
	    char	obuf[MSGBUFLEN + 1] ;
	    if_constexpr (f_writeto) {
	        while ((rs = uc_reade(mfd,obuf,olen,to_read,0)) > 0) {
	            rs = writeto(ofd,obuf,rs,to_write) ;
	            wlen += rs ;
	            if (rs < 0) break ;
	        } /* end while */
	    } else {
	        rs = uc_writedesc(ofd,mfd,-1) ;
	        wlen += rs ;
	    }

	    u_close(mfd) ;
	} /* end if (open) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procouter) */


