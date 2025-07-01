/* statmsg SUPPORT */
/* charset=ISO8859-1 */
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

  	Object:
	statmsg

	Description:
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
#include	<cstring>		/* |UINT_MAX| */
#include	<usystem.h>
#include	<getax.h>
#include	<getusername.h>
#include	<getuserhome.h>
#include	<getxid.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vechand.h>
#include	<ptma.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<strpack.h>
#include	<bfile.h>
#include	<fsdir.h>
#include	<ascii.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snx.h>
#include	<mkx.h>
#include	<strwcpy.h>
#include	<nleadstr.h>
#include	<permx.h>
#include	<writeto.h>
#include	<matxstr.h>
#include	<ctdec.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"statmsg.h"

import libutil ;			/* |loadstrs(3u)| */

/* local defines */

#define	SM_MAGIC	STATMSG_MAGIC
#define	SM_DEFGROUP	"default"
#define	SM_ALLGROUP	"all"
#define	SM_NAME		"statmsg"
#define	SM_SUF		"sm"
#define	SM_DIRSFNAME	"dirs"
#define	SM		statmsg

#define	MAMAGIC		0x21367425

#define	PF		paramfile
#define	PF_ENT		paramfile_ent
#define	PF_CUR		paramfile_cur

#define	MA		statmsg_mapper

#define	MD		statmsg_mapdir

#define	VS		vecstr

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
#define	CF_WRITETO	1		/* time out writes */
#endif
#ifndef	CF_PARAMFILE
#define	CF_PARAMFILE	1		/* use |paramfile(3dam)| */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef	mainv		mv ;


/* external subroutines */


/* external variables */

extern mainv	environ ;


/* local structures */

struct statmsg_mapdir {
	lockrw		rwm ;
	cchar		*username ;
	cchar		*userhome ;
	cchar		*admin ;
	cchar		*dirname ;	/* raw */
	cchar		*dname ;	/* expanded */
} ;

typedef statmsg_mapdir *	mdp ;

namespace {
    struct vars {
	int	maxnamelen ;
	int	maxpathlen ;
	int	maxlinelen ;
	int	usernamelen ;
	int	envlen ;
	int	parambuflen ;
	operator int () noex ;
    } ; /* end struct */
}


/* forward references */

template<typename ... Args>
static int statmsg_ctor(statmsg *op,Args ... args) noex {
    	STATMSG		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (statmsg_ctor) */

static int statmsg_dtor(statmsg *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (statmsg_dtor) */

template<typename ... Args>
static inline int statmsg_magic(statmsg *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == STATMSG_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (statmsg_magic) */

static int	statmsg_userbegin(SM *,cchar *) noex ;
static int	statmsg_userend(SM *) noex ;

static int	statmsg_mapfind(SM *,time_t) noex ;
static int	statmsg_maplose(SM *) noex ;
static int	statmsg_mapfname(SM *,char *) noex ;
static int	statmsg_schedload(SM *,vecstr *) noex ;
static int	statmsg_checker(SM *,time_t) noex ;
static int	statmsg_envbegin(SM *) noex ;
static int	statmsg_envend(SM *) noex ;
static int	statmsg_envadds(SM *,strpack *,cchar **,
			userid *,cchar *) noex ;
static int	statmsg_envstore(SM *,strpack *,cchar **,int,
			cchar *,int) noex ;
static int 	statmsg_processor(SM *,cc **,mainv,cc *,cc *,int) noex ;

static int	mapper_start(MA *,time_t,cchar *,cchar *,
			cchar *) noex ;
static int	mapper_finish(MA *) noex ;
static int	mapper_check(MA *,time_t) noex ;
static int	mapper_process(MA *,cc **,mainv,cc *,cc *,int) noex ;
static int	mapper_processor(MA *,cc **,mainv,cc *,cc *,int) noex ;
static int	mapper_mapload(MA *) noex ;
static int	mapper_mapadd(MA *,cchar *,int,
			cchar *,int) noex ;
static int	mapper_mapfins(MA *) noex ;

static int	mapdir_start(MD *,cchar *,
			cchar *,cchar *,int,cchar *,int) noex ;
static int	mapdir_finish(MD *) noex ;
static int	mapdir_process(MD *,cc **,mainv,cc *,cc *,int) noex ;
static int	mapdir_expand(MD *) noex ;
static int	mapdir_expander(MD *) noex ;
static int	mapdir_processor(MD *,cchar **,
			cchar *,cchar *,int) noex ;
static int	mapdir_processorthem(MD *,cchar **,
			cchar *,vecstr *,mainv,int) noex ;
static int	mapdir_processorone(MD *,cchar **,
			cchar *,vecstr *,cchar *,int) noex ;
static int	mapdir_procout(MD *,cchar **,cchar *,
			cchar *,int) noex ;
static int	mapdir_procouter(MD *,cchar **,
			cchar *,int) noex ;

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

static vars		var ;

constexpr uid_t		uidend = -1 ;
constexpr gid_t		gidend = -1 ;

cchar			envpre[] = "STATMSG_" ;	/* environment prefix */

cint			msgbuflen	= MSGBUFLEN ;
cint			diglen		= DECBUFLEN ;

cbool			f_writeto 	= CF_WRITETO ;
cbool			f_paramfile 	= CF_PARAMFILE ;


/* exported variables */


/* exported subroutines */

int statmsg_open(SM *op,cchar *username) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = statmsg_ctor(op,username)) >= 0) {
	    rs = SR_INVALID ;
	    if (username[0]) {
		if ((rs = var) >= 0) {
	            op->fe = SM_DIRSFNAME ;
	            if ((rs = ptm_create(&op->mx,nullptr)) >= 0) {
	                if ((rs = statmsg_userbegin(op,username)) >= 0) {
		            if ((rs = statmsg_mapfind(op,dt)) >= 0) {
		                if ((rs = statmsg_envbegin(op)) >= 0) {
			            op->ti_lastcheck = dt ;
			            op->magic = SM_MAGIC ;
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
		            ptm_destroy(&op->mx) ;
	                }
	            } /* end if (ptm) */
	        } /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		statmsg_dtor(op) ;
	    }
	} /* end if (statmsg_ctor) */
	return rs ;
}
/* end subroutine (statmsg_open) */

int statmsg_close(SM *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = statmsg_magic(op)) >= 0) {
	    {
	        rs1 = statmsg_envend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = statmsg_maplose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = statmsg_userend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&op->mx) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = statmsg_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (statmsg_close) */

int statmsg_check(SM *op,time_t dt) noex {
	int		rs ;
	if ((rs = statmsg_magic(op)) >= 0) {
	    rs = statmsg_checker(op,dt) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (statmsg_check) */

int statmsg_process(SM *op,cchar *gn,cchar **adms,cchar *kn,int fd) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = statmsg_magic(op,adms,kn)) >= 0) {
	    if (userid id ; (rs = id.start(nullptr,gn)) >= 0) {
		{
		    rs = statmsg_processid(op,&id,adms,kn,fd) ;
		    rv = rs ; /* not used */
		}
		rs1 = id.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (userid) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (statmsg_process) */

static int statmsg_procidx(statmsg *,userid *,mv,int,cc *) noex ;

int statmsg_processid(SM *op,userid *idp,cc **adms,cc *kn,int fd) noex {
	int		rs ;
	int		wlen = 0 ; /* return-value */
	if ((rs = statmsg_magic(op,idp,adms,kn)) >= 0) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
		cchar	*groupname = idp->groupname ;
		rs = SR_INVALID ;
	        if (groupname[0]) {
		    rs = statmsg_procidx(op,idp,adms,fd,kn) ;
		    wlen = rs ;
	        } /* end if (valid) */
	    } /* end if (open) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (statmsg_processid) */

static int statmsg_procidx(statmsg *op,userid *idp,mv adms,int fd,cc *kn) noex {
	cint		n = nelem(envstrs) ;
    	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	char		kbuf[2] = {} ;
	if (kn == nullptr) kn = kbuf ;
	{
	    cint	sz = (op->nenv + n + 1) * szof(cchar *) ;
	    if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
		cchar	**ev = (cchar **) p ;
		if (strpack packer ; (rs = packer.start(128)) >= 0) {
	            if ((rs = statmsg_envadds(op,&packer,ev,idp,kn)) >= 0) {
			cchar	*gn = idp->groupname ;
	                rs = statmsg_processor(op,ev,adms,gn,kn,fd) ;
	                wlen = rs ;
	            }
	            rs1 = packer.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (packer) */
	        rs1 = uc_free(p) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory allocation) */
	} /* end block */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (statmsg_procidx) */


/* private subroutines */

static int statmsg_userbegin(SM *op,cchar *username) noex {
	int		rs = SR_FAULT ;
	if (op && username) {
	    rs = SR_INVALID ;
	    if (username[0]) {
		cint maxpath = var.maxpathlen ;
		cint asz = ((var.maxpathlen + 1) + (var.usernamelen + 1)) ;
		int ai = 0 ;
		if (char *a ; (rs = uc_malloc(asz,&a)) >= 0) {
		    cint	hlen = var.maxpathlen ;
		    cint	ulen = var.usernamelen ;
		    char	*hbuf = (a + ((maxpath + 1) * ai++)) ;
		    char	*ubuf = (a + ((maxpath + 1) * ai++)) ;
	            if (username[0] == '-') {
	                rs = getusername(ubuf,ulen,-1) ;
	                username = ubuf ;
	            }
	            if (rs >= 0) {
	                if ((rs = getuserhome(hbuf,hlen,username)) >= 0) {
		            int	sz = 0 ;
		            sz += (lenstr(username) + 1) ;
		            sz += (lenstr(hbuf) + 1) ;
		            if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) {
		                op->useralloc = bp ;
		                op->username = bp ;
		                bp = (strwcpy(bp,username,-1) + 1) ;
		                op->userhome = bp ;
		                bp = (strwcpy(bp,hbuf,-1) + 1) ;
		            } /* end if (memory-allocation) */
	                } /* end if (getuserhome) */
	            } /* end if (ok) */
		    rs = rsfree(rs,a) ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (statmsg_userbegin) */

static int statmsg_userend(SM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->useralloc) {
	    rs1 = uc_free(op->useralloc) ;
	    if (rs >= 0) rs = rs1 ;
	    op->useralloc = nullptr ;
	    op->username = nullptr ;
	    op->userhome = nullptr ;
	}
	return rs ;
}
/* end subroutine (statmsg_userend) */

static int statmsg_mapfind(SM *op,time_t dt) noex {
	int		rs ;
	if (char *mbuf ; (rs = malloc_mp(&mbuf)) >= 0) {
	    mbuf[0] = '\0' ;
	    if ((rs = statmsg_mapfname(op,mbuf)) >= 0) {
	        if (mbuf[0]) {
		    cchar	*un = op->username ;
		    cchar	*uh = op->userhome ;
	            if ((rs = mapper_start(&op->mapper,dt,un,uh,mbuf)) >= 0) {
	                op->nmaps += 1 ;
		    }
	        }
	    } /* end if (statmsg_mapfname) */
	    rs = rsfree(rs,mbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (statmsg_mapfind) */

static int statmsg_maplose(SM *op) noex {
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

static int statmsg_mapfname(SM *op,char *fbuf) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	fbuf[0] = '\0' ;
	if (vecstr scheds ; (rs = scheds.start(6,0)) >= 0) {
	    if ((rs = statmsg_schedload(op,&scheds)) >= 0) {
	    	cint	am = R_OK ;
		cint	flen = var.maxpathlen ;
		cchar	*fe = op->fe ;
	        if ((rs = permsched(schedmaps,&scheds,fbuf,flen,fe,am)) >= 0) {
		    c = 1 ;
		} else if (isNotAccess(rs)) {
		    rs = SR_OK ;
		    fbuf[0] = '\0' ;
		}
	    } /* end if (statmsg-schedload) */
	    rs1 = scheds.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (statmsg_mapfname) */

static int statmsg_schedload(SM *op,vecstr *slp) noex {
	int		rs = SR_OK ;
	cchar		keys[] = "pen" ;
	cchar		name[] = SM_NAME ;
	for (int i = 0 ; keys[i] != '\0' ; i += 1) {
	    cint	kch = mkchar(keys[i]) ;
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
	    if ((rs >= 0) && vp) {
		char	kbuf[2] = {} ;
		kbuf[0] = char(kch) ;
		rs = vecstr_envset(slp,kbuf,vp,vl) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (statmsg_schedload) */

static int statmsg_checker(SM *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		nchanged = 0 ;/* return-value */
	if (op->nmaps > 0) {
	    ptm	*mxp = &op->mx ;
	    if ((rs = mxp->lockbegin) >= 0) {
	        if (dt == 0) dt = getustime ;
	        if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	            rs = mapper_check(&op->mapper,dt) ;
	            nchanged = rs ;
	            op->ti_lastcheck = dt ;
	        } /* end if (timed out) */
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (positive) */
	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (statmsg_checker) */

static int statmsg_envbegin(SM *op) noex {
	int		rs = SR_OK ;
	int		n = lenstrarr(environ) ;
	int		c = 0 ; /* return-value */
	{
	    cint	sz = (n + 1) * szof(cchar *) ;
	    if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	        cchar	**va = ccharpp(p) ;
	        op->envv = va ;
	        for (int i = 0 ; environ[i] ; i += 1) {
	            bool	f = true ;
	            cchar	*ep = environ[i] ;
	            f = f && (ep[0] != '-') ;
	            f = f && (matstr(envbad,ep,-1) < 0) ;
	            if (f && (ep[0] == 'M')) {
		        f = (strncmp(envpre,ep,5) != 0) ;
		    }
	            if (f) {
	                va[c++] = ep ;
		    }
	        } /* end for */
	        va[c] = nullptr ;
	        op->nenv = c ;
	    } /* end if (memory-allocation) */
	} /* end block */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (statmsg_envbegin) */

static int statmsg_envend(SM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->envv) {
	    rs1 = uc_free(op->envv) ;
	    if (rs >= 0) rs = rs1 ;
	    op->envv = nullptr ;
	}
	return rs ;
}
/* end subroutine (statmsg_envend) */

static int mkdigenv(char *ebuf,int elen,cc *pre,cc *key,uint uv) noex {
    	int	rs ;
    	char	digbuf[diglen + 1] ;
	if ((rs = ctdec(digbuf,diglen,uv)) >= 0) {
	    rs = sncpy(ebuf,elen,pre,key,"=",digbuf) ;
	}
	return rs ;
} /* end subroutine (mkdigenv) */

static int statmsg_envadds(SM *op,strpack *spp,cchar **ev,
		userid *idp,cchar *kn) noex {
    	cint		envlen = var.envlen ;
	int		rs = SR_OK ;
	int		n ;
	mainv		envv = op->envv ;
	cchar		*pre = envpre ;
	char		envbuf[var.envlen + 1] ;
	for (n = 0 ; n < op->nenv ; n += 1) {
	    ev[n] = envv[n] ;
	}
	for (int i = 0 ; (rs >= 0) && envstrs[i] ; i += 1) {
	    uint	uv = UINT_MAX ;
	    int		el = -1 ;
	    cchar	*es = envstrs[i] ;
	    cchar	*cp ;
	    envbuf[0] = '\0' ;
	    switch (i) {
	    case envstr_uid:
	        if (idp->uid != uidend) {
	            uv = idp->uid ;
		    rs = mkdigenv(envbuf,envlen,pre,es,uv) ;
		    el = rs ;
	        }
	        break ;
	    case envstr_gid:
	        if (idp->gid != gidend) {
	            uv = idp->gid ;
		    rs = mkdigenv(envbuf,envlen,pre,es,uv) ;
		    el = rs ;
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
	{
	    ev[n] = nullptr ; /* very important! */
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (statmsg_envadds) */

static int statmsg_envstore(SM *op,strpack *spp,cc **ev,int n,
		cc *ep,int el) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (ep) {
	        if (cc *cp ; (rs = strpack_store(spp,ep,el,&cp)) >= 0) {
	            ev[n++] = cp ;
	            rs = n ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (statmsg_envstore) */

static int statmsg_processor(SM *op,cc **ev,mv adms,cc *gn,cc *kn,int fd) noex {
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
	cint		to = TO_MAPCHECK ;
	int		rs ;
	mainv		evp = mainv(environ) ;
	memclear(mmp) ;
	mmp->username = un ;
	mmp->userhome = uh ;
	if ((rs = lockrw_create(&mmp->rwm,0)) >= 0) {
	    if (cc *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        mmp->fname = cp ;
	        if ((rs = vechand_start(&mmp->mapdirs,4,0)) >= 0) {
		    PF	*dfp = &mmp->dirsfile ;
	            if ((rs = paramfile_open(dfp,evp,cp)) >= 0) {
	                if ((rs = paramfile_checkint(dfp,to)) >= 0) {
	                    mmp->magic = MAMAGIC ;
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

static int mapper_finish(MA *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MAMAGIC) {
		rs = SR_OK ;
	        {
	            rs1 = paramfile_close(&mmp->dirsfile) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = mapper_mapfins(mmp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = vechand_finish(&mmp->mapdirs) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (mmp->fname) {
	            rs1 = uc_free(mmp->fname) ;
	            if (rs >= 0) rs = rs1 ;
	            mmp->fname = nullptr ;
	        }
	        {
	            rs1 = lockrw_destroy(&mmp->rwm) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        mmp->magic = 0 ;
	    } /* end if (magic) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_finish) */

static int mapper_check(MA *mmp,time_t dt) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		nchanged = 0 ; /*return-value */
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MAMAGIC) {
	         if ((rs = lockrw_wrlock(&mmp->rwm,to_lock)) >= 0) {
	             if (dt == 0) dt = getustime ;
	             if ((dt - mmp->ti_check) >= TO_MAPCHECK) {
		         if_constexpr (f_paramfile) {
			     paramfile *pfp = &mmp->dirsfile ;
	                     if ((rs = pfp->check(dt)) > 0) {
	                         {
	                             mapper_mapfins(mmp) ;
	                             vechand_delall(&mmp->mapdirs) ;
	                         }
	                         rs = mapper_mapload(mmp) ;
	                         nchanged = rs ;
	                     } /* end if */
		         } else {
			     cchar *fn = mmp->fname ;
	                     if (ustat sb ; (rs = u_stat(fn,&sb)) >= 0) {
	            	        if (sb.st_mtime > mmp->ti_mtime) {
	                            mapper_mapfins(mmp) ;
	                            vechand_delall(&mmp->mapdirs) ;
				} else if (isNotPresent(rs)) {
				    rs = SR_OK ;
				}
				if (rs >= 0) {
	                            rs = mapper_mapload(mmp) ;
	                            nchanged = rs ;
				} /* end if (OK) */
	                    } /* end if (file mtime check) */
	                    mmp->ti_check = dt ;
	                } /* end if_constexpr (f_paramfile) */
	            } /* end if (map-object check) */
	            rs1 = lockrw_unlock(&mmp->rwm) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (read-write lock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (mapper_check) */

static int mapper_process(MA *mmp,cc **ev,mv adms,cc *gn,cc *kn,int fd) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MAMAGIC) {
	        lockrw	*lp = &mmp->rwm ;
	        if ((rs = lockrw_rdlock(lp,to_lock)) >= 0) {
	            {
	                rs = mapper_processor(mmp,ev,adms,gn,kn,fd) ;
	                wlen += rs ;
	            }
	            rs1 = lockrw_unlock(lp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (read-write lock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_process) */

static int mapper_processor(MA *mmp,cc **ev,mv adms,cc *gn,cc *kn,int fd) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MAMAGIC) {
	        vechand		*mlp = &mmp->mapdirs ;
	        void		*vp{} ;
		rs = SR_OK ;
	        for (int i = 0 ; mlp->get(i,&vp) >= 0 ; i += 1) {
		    MD	*ep = mdp(vp) ;
	            if (vp) {
	                rs = mapdir_process(ep,ev,adms,gn,kn,fd) ;
	                wlen += rs ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_processor) */

static int mapper_maploadparam(MA *mmp) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (ustat sb ; (rs = u_stat(mmp->fname,&sb)) >= 0) {
	    cint	plen = var.parambuflen ;
	    mmp->ti_mtime = sb.st_mtime ;
	    if (char *pbuf ; (rs = uc_malloc((plen + 1),&pbuf)) >= 0) {
	        PF	*pfp = &mmp->dirsfile ;
	        PF_ENT	pe ;
	        if (PF_CUR cur ; (rs = pfp->curbegin(&cur)) >= 0) {
		    int		kl, vl ;
		    int		fl ;
		    cchar	*kp, *vp ;
	            while (rs >= 0) {
	                kl = pfp->curenum(&cur,&pe,pbuf,plen) ;
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
	            rs1 = pfp->curend(&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (paramfile-cursor) */
		rs = rsfree(rs,pbuf) ;
	    } /* end if (m-a-f) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (mapper_maploadparam) */

static int mapper_maploadfiler(MA *,cchar *,int) noex ;

static int mapper_maploadfile(MA *mmp) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    cmode	om = 0 ;
	    if (bfile mf ; (rs = mf.open(mmp->fname,"r",om)) >= 0) {
	        if (ustat sb ; (rs = mf.control(BC_STAT,&sb)) >= 0) {
		    mmp->ti_mtime = sb.st_mtime ;
		    while ((rs = mf.readln(lbuf,llen)) > 0) {
			cchar	*cp{} ;
			if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
			    rs = mapper_maploadfiler(mmp,cp,cl) ;
			    c += rs ;
			}
	    	        if (rs < 0) break ;
		    } /* end while (reading lines) */
	        } /* end if (bcontrol) */
	        rs1 = mf.close ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (file-open) */
	    rs = rsfree(rs,lbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (mapper_maploadfile) */

static int mapper_maploadfiler(MA *mmp,cchar *sp,int sl) noex {
    	int		rs = SR_OK ;
	int		c = 0 ;
	cchar	*kp ;
  	if (int kl ; (kl = sfnext(sp,sl,&kp)) > 0) {
	    sl -= intconv((kp + kl) - sp) ;
	    sp = (kp + kl) ;
	    cchar	*vap ;
	    if (int val ; (val = sfnext(sp,sl,&vap)) > 0) {
	        c += 1 ;
	        rs = mapper_mapadd(mmp,kp,kl,vap,val) ;
	    }
	} /* end if (key) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (mapper_maploadfiler) */

static int mapper_mapload(MA *mmp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MAMAGIC) {
    	        if_constexpr (f_paramfile) {
		    rs = mapper_maploadparam(mmp) ;
		    c = rs ;
		} else {
		    rs = mapper_maploadfile(mmp) ;
		    c = rs ;
		} /* end if_constexpr (f_paramfile) */
	    } /* end if (magic) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mapper_mapload) */

static int mapper_mapadd(MA *mmp,cc *kp,int kl,cc *vap,int val) noex {
	cint		sz = szof(MD) ;
	int		rs = SR_FAULT ;
	if (mmp && kp && vap) {
	    rs = SR_INVALID ;
	    if ((kl > 0) && (val > 0)) {
	        if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
	            MD		*ep = (MD *) vp ;
	            cchar	*un = mmp->username ;
	            cchar	*uh = mmp->userhome ;
	            if ((rs = mapdir_start(ep,un,uh,kp,kl,vap,val)) >= 0) {
	                rs = vechand_add(&mmp->mapdirs,ep) ;
	                if (rs < 0) {
	                    mapdir_finish(ep) ;
		        }
	            } /* end if (mapdir_start) */
	            if (rs < 0) {
	                uc_free(ep) ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_mapadd) */

static int mapper_mapfins(MA *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mmp) {
	    rs =  SR_NOTOPEN ;
	    if (mmp->magic == MAMAGIC) {
		vechand		*mlp = &mmp->mapdirs ;
	        void		*vp{} ;
		rs = SR_OK ;
	        for (int i = 0 ; mlp->get(i,&vp) >= 0 ; i += 1) {
		    MD	*ep = mdp(vp) ;
	            if (vp) {
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
	    } /* end if (magic) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_mapfins) */

static int mapdir_start(MD *ep,cc *un,cc *uh,
		cc *kp,int kl,cc *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (ep && kp && vp) {
	    memclear(ep) ;
	    rs = SR_INVALID ;
	    if ((kl > 0) && (vl > 0)) {
	        ep->username = un ;
	        ep->userhome = uh ;
	        if (kl < 0) kl = lenstr(kp) ;
	        if (vl < 0) vl = lenstr(vp) ;
	        {
	            cint	sz = (kl + 1 + vl + 1) ;
	            if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) {
	                ep->admin = bp ;
		        bp = strwcpy(bp,kp,kl) + 1 ;
		        ep->dirname = bp ;
		        bp = strwcpy(bp,vp,vl) + 1 ;
		        rs = lockrw_create(&ep->rwm,0) ;
		        if (rs < 0) {
	    	            uc_free(ep->admin) ;
		            ep->admin = nullptr ;
		        } /* end if (error) */
	            } /* end if (memory-allocation) */
	        } /* end block */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapdir_start) */

static int mapdir_finish(MD *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->dname) {
	        rs1 = uc_free(ep->dname) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->dname = nullptr ;
	    }
	    {
	        rs1 = lockrw_destroy(&ep->rwm) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->admin) {
	        rs1 = uc_free(ep->admin) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->admin = nullptr ;
	        ep->dirname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapdir_finish) */

static int mapdir_process(MD *ep,cc **ev,mv adms,cc *gn,cc *kn,int fd) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	if (ep->dirname[0] != '\0') {
	    bool	f_continue = true ;
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
	cint		to_lock = TO_LOCK ;
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
	int		rs = SR_INVALID ;
	int		pl = 0 ;
	if (ep->dirname) {
	    if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	        cchar	*pp ;
	        if (ep->dirname[0] == '~') {
	            pp = tbuf ;
	            rs = mkuserpath(tbuf,ep->username,ep->dirname,-1) ;
	            pl = rs ;
	        } else if (ep->dirname[0] != '/') {
	            pp = tbuf ;
	            rs = mkpath(tbuf,ep->userhome,ep->dirname) ;
	            pl = rs ;
	        } else {
	            pp = ep->dirname ;
	            pl = -1 ;
	        }
	        if (rs >= 0) {
		    if (cchar *cp ; (rs = uc_mallocstrw(pp,pl,&cp)) >= 0) {
	                ep->dname = cp ;
		        if (pl < 0) pl = rs ;
		    } /* end if (memory-allocation) */
	        } /* end if (OK) */
	        rs = rsfree(rs,tbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mapdir_expander) */

static int mapdir_processor(MD *ep,cchar **ev,cchar *,cchar *kn,int fd) noex {
    	cint		envlen = var.envlen ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	bool		f_continue = true ;
	cchar		defname[] = SM_DEFGROUP ;
	cchar		allname[] = SM_ALLGROUP ;
	cchar		suf[] = SM_SUF ;
	cchar		*dn = ep->dirname ;
	char		env_admin[var.envlen +1] ;
	char		env_admindir[var.envlen +1] ;
	if (dn[0] == '~') {
	    dn = ep->dname ;
	    f_continue = ((dn != nullptr) && (dn[0] != '\0')) ;
	}
	if (f_continue) {
	    if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
		int	n = lenstrarr(ev) ;
	        cchar	*post = envstrs[envstr_admin] ;
	        strdcpy4(env_admin,envlen,envpre,post,"=",ep->admin) ;
	        post = envstrs[envstr_admindir] ;
	        strdcpy4(env_admindir,envlen,envpre,post,"=",dn) ;
	        ev[n+0] = env_admin ;
	        ev[n+1] = env_admindir ;
	        ev[n+2] = nullptr ;
		cint	nstrs = 4 ;
		if (vecstr nums ; (rs = nums.start(0,0)) >= 0) {
	            cchar	*strs[nstrs + 1] ;
		    if (char *nbuf ; (rs = malloc_mn(&nbuf)) >= 0) {
		    cint	nlen = rs ;
	            loadstrs(strs,nstrs,kn,defname,allname,suf) ;
	            if (fsdir d ; (rs = fsdir_open(&d,dn)) >= 0) {
	                fsdir_ent	de ;
	                while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
	                    cchar	*den = de.name ;
	                    if (den[0] != '.') {
	                	cchar	*tp = strchr(den,'.') ;
	            		if (tp && (strcmp((tp+1),suf) == 0)) {
				    int		tl ;
				    bool	f = true ;
	   			    cchar	*digp ;
				    tl = intconv(tp - den) ;
	                	    digp = strnbrk(den,tl,"0123456789") ;
	                	    if (digp) {
					tl = intconv(tp - digp) ;
	                    		f = hasalldig(digp,tl) ;
				    }
				    if (f) {
	                	        if ((kn[0] != '\0') && (kn[0] != '-')) {
			    		    for (int i = 0 ; i < 3 ; i += 1) {
					    f = isBaseMatch(den,strs[i],digp) ;
					    if (f) break ;
			    		    } /* end for */
	                		}
				    }
	                	    if (f) {
					tl = intconv(tp - den) ;
	                    		rs = vecstr_add(&nums,den,tl) ;
				    }
	            		} /* end if (have an SM file) */
			    }
	           	    if (rs < 0) break ;
	        	} /* end while (reading directory entries) */
	        	rs1 = fsdir_close(&d) ;
			if (rs >= 0) rs = rs1 ;
	  	    } /* end if (fsdir) */
			rs1 = uc_free(nbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		    if (rs >= 0) {
	        	vecstr_sort(&nums,nullptr) ;
	        	rs = mapdir_processorthem(ep,ev,dn,&nums,strs,fd) ;
	        	wlen += rs ;
	   	    } /* end if */
	   	    rs1 = vecstr_finish(&nums) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (vecstr-nums) */
		{
	    	    ev[n] = nullptr ;
		}
	    } else if (isNotAccess(rs)) {
		rs = SR_OK ;
	    } /* end if (u_stat) */
	} /* end if (continued) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_processor) */

static int mapdir_processorthem(MD *ep,cc **ev,cc *dn,
		vecstr *blp,mv strs,int fd) noex {
	int		rs ;
	int		wlen = 0 ; /* return-value */
	cchar		*kn = strs[0] ;
	char		kbuf[2] = {} ;
	if (kn[0] == '-') kn = kbuf ; /* <- blank out */
	if ((rs = mapdir_processorone(ep,ev,dn,blp,kn,fd)) >= 0) {
	    wlen += rs ;
	} else if (isNotPresent(rs)) {
	    kn = strs[1] ;
	    if ((rs = mapdir_processorone(ep,ev,dn,blp,kn,fd)) >= 0) {
	        wlen += rs ;
	    } else if (isNotPresent(rs)) {
	        kn = strs[2] ;
	        if ((rs = mapdir_processorone(ep,ev,dn,blp,kn,fd)) >= 0) {
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
	    }
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_processorthem) */

static int mapdir_processorone(MD *ep,cc **ev,cc *dn,
		VS *blp,cc *kn,int fd) noex {
	cint		kl = lenstr(kn) ;
	int		rs = SR_OK ;
	int		c = 0 ;
	int		wlen = 0 ; /* return-value */
	cchar		*bep ;
	for (int i = 0 ; blp->get(i,&bep) >= 0 ; i += 1) {
	    if (bep) {
	        if (strncmp(bep,kn,kl) == 0) {
	            c += 1 ;
	            if ((rs = mapdir_procout(ep,ev,dn,bep,fd)) >= 0) {
	                wlen += rs ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            }
	        } /* end if (strncmp) */
	    }
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && (c == 0)) rs = SR_NOENT ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_processorone) */

static int mapdir_procout(MD *ep,cchar **ev,cchar *dn,cchar *bn,int fd) noex {
	cint		sz = (2 * (var.maxpathlen + 1)) ;
	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	int		ai = 0 ;
	cchar		suf[] = SM_SUF ;
	if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
	    char	*fname = (a + ((maxpath + 1) * ai++)) ;
	    char	*cname = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = snsds(cname,maxpath,bn,suf)) >= 0) {
	        if ((rs = mkpath(fname,dn,cname)) >= 0) {
	            rs = mapdir_procouter(ep,ev,fname,fd) ;
	            wlen += rs ;
		}
	    }
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procout) */

static int mapdir_procouter(MD *ep,cchar **ev,cchar *fn,int ofd) noex {
	cint		to_open = TO_OPEN ;
	cint		to_read = TO_READ ;
	cint		to_write = TO_WRITE ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (ep && ev && fn) {
	    cint	olen = MSGBUFLEN ;
	    if (char *obuf ; (rs = uc_malloc((olen + 1),&obuf)) >= 0) {
		cint	of = O_RDONLY ;
		cmode	om = 0 ;
	        if ((rs = uc_openenv(fn,of,om,ev,to_open)) >= 0) {
	            cint	mfd = rs ;
	            if_constexpr (f_writeto) {
	                while ((rs = uc_reade(mfd,obuf,olen,to_read,0)) > 0) {
	                    rs = writeto(ofd,obuf,rs,to_write) ;
	                    wlen += rs ;
	                    if (rs < 0) break ;
	                } /* end while */
	            } else {
	                rs = uc_writedesc(ofd,mfd,-1) ;
	                wlen += rs ;
	            } /* end if_constexpr (f_writeto) */
	            rs1 = u_close(mfd) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (open) */
		rs = rsfree(rs,obuf) ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procouter) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = maxpathlen) == 0) {
	    if ((rs = getbufsize(getbufsize_mn)) >= 0) {
		maxnamelen = rs ;
	        if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	            maxpathlen = rs ;
		    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
			maxlinelen = rs ;
		        if ((rs = getbufsize(getbufsize_un)) >= 0) {
		            usernamelen = rs ;
			    envlen = (2 * maxpathlen) ;
			    parambuflen = maxpathlen ;
			}
		    }
		}
	    }
	} /* end if (needed) */
	return rs ;
} /* end method (vars::operator) */

static bool isBaseMatch(cchar *den,cchar *bname,cchar *digp) noex {
	bool		f = false ;
	if (digp == nullptr) {
	    int	bl = lenstr(bname) ;
	    int	m = nleadstr(den,bname,bl) ;
	    f = (m == bl) && (den[m] == '.') ;
	} else {
	    csize nsize = size_t(digp - den) ;
	    f = (strncmp(den,bname,nsize) == 0) ;
	}
	return f ;
}
/* end subroutine (isBaseMatch) */


