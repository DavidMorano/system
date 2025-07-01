/* motd SUPPORT */
/* charset=ISO8859-1 */
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

  	Object:
	motd

	Description:
	This object module writes the contents of various MOTDs (as
	specified by the caller) to an open file descriptor (also
	specified by the caller).

	Implementation notes:
	When processing, we time-out writes to the caller-supplied
	file-descriptor because we do not know if it is a non-regular
	file that might be flow-controlled.  We do not wait forever
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
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |UINT_MAX| */
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
#include	<getpwx.h>
#include	<mallocxx.h>
#include	<ptma.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<strpack.h>
#include	<bfile.h>
#include	<ctdec.h>
#include	<permx.h>
#include	<writeto.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<mknpathx.h>
#include	<strwcpy.h>
#include	<matstr.h>
#include	<matxstr.h>
#include	<mkchar.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"motd.h"

import libutil ;			/* |lenstrarr(3u)| */

/* local defines */

#define	MOTD_DEFGROUP	"default"
#define	MOTD_ALLGROUP	"all"
#define	MOTD_NAME	"motd"
#define	MOTD_DIRSFNAME	"dirs"
#define	MOTD_MAPMAGIC	0x21367425

#define	MD		motd_mapdir

#define	MR		motd_mr

#define	SP		strpack

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

template<typename ... Args>
static int motd_ctor(motd *op,Args ... args) noex {
    	MOTD		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (motd_ctor) */

static int motd_dtor(motd *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (motd_dtor) */

template<typename ... Args>
static inline int motd_magic(motd *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MOTD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (motd_magic) */

static int	motd_mapfind(motd *,time_t) noex ;
static int	motd_maplose(motd *) noex ;
static int	motd_mapfname(motd *,char *) noex ;
static int	motd_schedload(motd *,vecstr *) noex ;
static int	motd_checker(motd *,time_t) noex ;
static int	motd_envbegin(motd *) noex ;
static int	motd_envend(motd *) noex ;
static int	motd_envadds(motd *,strpack *,ccharpp,userid *) noex ;
static int	motd_envstore(motd *,strpack *,ccharpp,int,cchar *,int) noex ;
static int 	motd_processor(motd *,ccharpp,mainv,cchar *,int) noex ;
static int	motd_ufindbegin(motd *) noex ;
static int	motd_ufindend(motd *) noex ;
static int	motd_ufindlook(motd *,char *,uid_t) noex ;

static int	mapper_start(MR *,time_t,cchar *) noex ;
static int	mapper_finish(MR *) noex ;
static int	mapper_check(MR *,time_t) noex ;
static int	mapper_process(MR *,ccharpp,mainv,cc *,int) noex ;
static int	mapper_processor(MR *,ccharpp,mainv,cc *,int) noex ;
static int	mapper_mapload(MR *) noex ;
static int	mapper_mapadd(MR *,cchar *,int,cchar *,int) noex ;
static int	mapper_mapfins(MR *) noex ;

#if	CF_TESTPROC
static int	mapper_lockcheck(MR *,cchar *) noex ;
#endif

static int	mapdir_start(MD *,cchar *,int,cchar *,int) noex ;
static int	mapdir_finish(MD *) noex ;
static int	mapdir_process(MD *,ccharpp,mainv,cc *,int) noex ;
static int	mapdir_expand(MD *) noex ;
static int	mapdir_expander(MD *) noex ;
static int	mapdir_processor(MD *,ccharpp ,cchar *,int) noex ;
static int	mapdir_procout(MD *,mainv,cc *,cc *,int) noex ;
static int	mapdir_procouter(MD *,mainv,cc *,int) noex ;


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
	envstr_admin,
	envstr_admindir,
	envstr_overlast
} ;

constexpr cpcchar	envstrs[] = {
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

cchar			envpre[] = "MOTD_" ;	/* environment prefix */

cint			msgbuflen = MSGBUFLEN ;
cint			envlen = ENVBUFLEN ;
cint			diglen = DECBUFLEN ;

cbool			f_comment = false ;
cbool			f_writeto = CF_WRITETO ;


/* exported variables */


/* exported subroutines */

int motd_open(motd *op,cchar *pr) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = motd_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        op->fe = MOTD_DIRSFNAME ;
	        if (cchar *cp ; (rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	            op->pr = cp ;
	            if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
	                if ((rs = motd_mapfind(op,dt)) >= 0) {
	                    if ((rs = motd_envbegin(op)) >= 0) {
	                        op->ti_lastcheck = dt ;
	                        op->magic = MOTD_MAGIC ;
	                    } /* end if (envbegin) */
	                    if (rs < 0) {
	                        motd_maplose(op) ;
		            }
	                } /* end if (mapfind) */
	                if (rs < 0) {
	                    ptm_destroy(op->mxp) ;
		        }
	            } /* end if (ptm_begin) */
	            if (rs < 0) {
		        uc_free(op->pr) ;
		        op->pr = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	    if (rs < 0) {
		motd_dtor(op) ;
	    }
	} /* end if (motd_ctor) */
	return rs ;
}
/* end subroutine (motd_open) */

int motd_close(motd *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = motd_magic(op)) >= 0) {
	    {
	        rs1 = motd_ufindend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = motd_envend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = motd_maplose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->pr) {
	        rs1 = uc_free(op->pr) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    {
		rs1 = motd_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (motd_close) */

int motd_check(motd *op,time_t dt) noex {
	int		rs ;
	if ((rs = motd_magic(op)) >= 0) {
	    rs = motd_checker(op,dt) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (motd_check) */

int motd_process(motd *op,cchar *gn,mainv admins,int fd) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = motd_magic(op,gn)) >= 0) {
	    rs = SR_INVALID ;
	    if (gn[0]) {
	        if (userid id ; (rs = id.start(nullptr,gn)) >= 0) {
		    {
			if_constexpr (f_comment) {
			    rs = motd_ufindlook(op,nullptr,id.uid) ;
			}
			if (rs >= 0) {
	                    rs = motd_processid(op,&id,admins,fd) ;
			}
		    }
		    rs1 = id.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (userid) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (motd_process) */

static int motd_procidx(motd *,userid *,mv,int) noex ;

int motd_processid(motd *op,userid *idp,mainv admins,int fd) noex {
	int		rs ;
	int		wlen = 0 ; /* return-value */
	if ((rs = motd_magic(op,idp)) >= 0) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
		cchar	*groupname = idp->groupname ;
		rs = SR_INVALID ;
	        if (groupname[0]) {
		    rs = motd_procidx(op,idp,admins,fd) ;
		    wlen = rs ;
	        } /* end if (valid) */
	    } /* end if (open) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (motd_processid) */

static int motd_procidx(motd *op,userid *idp,mv admins,int fd) noex {
	cint		n = nelem(envstrs) ;
    	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	{
	    cint	sz = (op->nenv + n + 1) * szof(cchar *) ;
	    if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	        cchar	**ev = (cchar **) p ;
	        if (SP packer ; (rs = packer.start(128)) >= 0) {
	            if ((rs = motd_envadds(op,&packer,ev,idp)) >= 0) {
			cchar *gn = idp->groupname ;
	                rs = motd_processor(op,ev,admins,gn,fd) ;
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
} /* end subroutine (motd_procidx) */


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
	cchar		keys[] = "pen" ;
	cchar		name[] = MOTD_NAME ;
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
	    if ((rs >= 0) && vp) {
		char	kbuf[2] = {} ;
		kbuf[0] = char(kch) ;
		rs = vecstr_envset(slp,kbuf,vp,vl) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (motd_schedload) */

static int motd_checker(motd *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		nchanged = 0 ;
	if (op->nmaps > 0) {
	    ptm *mxp = op->mxp ;
	    if (dt == 0) dt = getustime ;
	    if ((rs = mxp->lockbegin) >= 0) {
	        if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	            rs = mapper_check(&op->mapper,dt) ;
	            nchanged = rs ;
	            op->ti_lastcheck = dt ;
	        } /* end if */
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (positive) */
	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (motd_checker) */

static int motd_envbegin(motd *op) noex {
	cint		es = mkchar(envpre[0]) ;
	cint		envprelen = lenstr(envpre) ;
	int		rs = SR_OK ;
	int		sz ;
	int		n = lenstrarr(environ) ;
	int		c = 0 ; /* return-value */
	sz = (n + 1) * szof(cchar *) ;
	if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	    cchar	**va = ccharpp(p) ;
	    op->envv = va ;
	    for (int i = 0 ; environ[i] ; i += 1) {
	        cchar	*ep = environ[i] ;
	        bool	f = true ;
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
	if (op->envv) {
	    rs1 = uc_free(op->envv) ;
	    if (rs >= 0) rs = rs1 ;
	    op->envv = nullptr ;
	}
	return rs ;
}
/* end subroutine (motd_envend) */

static int mkdigenv(char *ebuf,int elen,cc *pre,cc *key,uint uv) noex {
    	int	rs ;
    	char	digbuf[diglen + 1] ;
	if ((rs = ctdec(digbuf,diglen,uv)) >= 0) {
	    rs = sncpy(ebuf,elen,pre,key,"=",digbuf) ;
	}
	return rs ;
} /* end subroutine (mkdigenv) */

static int motd_envadds(motd *op,strpack *spp,ccharpp ev,userid *idp) noex {
	int		rs = SR_OK ;
	int		n = 0 ; /* return-value */
	mainv		envv = op->envv ;
	cchar		*pre = envpre ;
	char		envbuf[envlen + 1] ;
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
	            rs = sncpy(envbuf,envlen,pre,es,"=",cp) ;
	            el = rs ;
	        }
	        break ;
	    case envstr_groupname:
	        cp = idp->groupname ;
	        if ((cp != nullptr) && (cp[0] != '\0')) {
	            rs = sncpy(envbuf,envlen,pre,es,"=",cp) ;
	            el = rs ;
	        }
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && (envbuf[0] != '\0')) {
	        rs = motd_envstore(op,spp,ev,n,envbuf,el) ;
	        if (rs > 0) n += 1 ;
	    }
	} /* end for */
	{
	    ev[n] = nullptr ; /* very important! */
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (motd_envadds) */

static int motd_envstore(motd *op,strpack *spp,
		ccharpp ev,int n,cc *ep,int el) noex {
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
/* end subroutine (motd_envstore) */

static int motd_processor(motd *op,ccharpp ev,mv adms,cchar *gn,int fd) noex {
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
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		ul = 0 ; /* return-value */
	if (ubuf) {
	    if ((rs = getbufsize(getbufsize_un)) >= 0) {
	        ptm *mxp = op->mxp ;
	        cint	ulen = rs ;
	        if ((rs = mxp->lockbegin) >= 0) {
	            if (! op->open.ufind) {
		        rs = motd_ufindbegin(op) ;
		    }
	            if (rs >= 0) {
	                rs = finduid_lookup(op->ufp,ubuf,ulen,uid) ;
		        ul = rs ;
	            } /* end if */
	            rs1 = mxp->lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex) */
	    } /* end if (getbusize) */
	} /* end if (non-null) */
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (motd_ufindlook) */

static int mapper_start(MR *mmp,time_t dt,cchar *fname) noex {
	int		rs ;
	mainv		evp = mainv(environ) ;
	memclear(mmp) ;
	if ((rs = lockrw_create(&mmp->rwm,0)) >= 0) {
	    if (cc *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
		mmp->fname = cp ;
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
	    if (dt == 0) dt = getustime ;
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

static int mapper_process(MR *mmp,ccharpp ev,mv adms,cc *gn,int fd) noex {
	cint		to_lock = TO_LOCK ;
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
	            rs1 = u_close(rfd) ;
		    if (rs >= 0) rs = rs1 ;
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

static int mapper_processor(MR *mmp,ccharpp ev,mv adms,cc *gn,int fd) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

	void	*vp{} ;
	for (int i = 0 ; vechand_get(&mmp->mapdirs,i,&vp) >= 0 ; i += 1) {
	    MD	*ep = mdp(vp) ;
	    if (vp) {
	        rs = mapdir_process(ep,ev,adms,gn,fd) ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_processor) */

static int mapper_mapload(MR *mmp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != MOTD_MAPMAGIC) return SR_NOTOPEN ;

	if (ustat sb ; (rs = u_stat(mmp->fname,&sb)) >= 0) {
	    paramfile		*pfp = &mmp->dirsfile ;
	    paramfile_ent	pe ;
	    paramfile_cur	cur ;
	    mmp->ti_mtime = sb.st_mtime ;
	    if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	        cint	plen = PBUFLEN ;
	        int	kl, vl ;
		int	fl ;
	        cchar	*kp, *vp ;
	        char	pbuf[PBUFLEN + 1] ;

	        while (rs >= 0) {
	            kl = paramfile_curenum(pfp,&cur,&pe,pbuf,plen) ;
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
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	} /* end if (stat) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mapper_mapload) */

static int mapper_mapadd(MR *mmp,cc *kp,int kl,cc *valp,int vall) noex {
	cint		sz = szof(MD) ;
	int		rs ;

	if ((kp == nullptr) || (valp == nullptr)) return SR_FAULT ;

	if ((kl == 0) || (vall == 0)) return SR_INVALID ;

	if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
	    MD	*ep = (MD *) vp ;
	    if ((rs = mapdir_start(ep,kp,kl,valp,vall)) >= 0) {
		vechand *mp = &mmp->mapdirs ;
	        rs = mp->add(ep) ;
	        if (rs < 0) {
	            mapdir_finish(ep) ;
		}
	    }
	    if (rs < 0) {
	        uc_free(ep) ;
	    }
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

	return rs ;
}
/* end subroutine (mapper_mapfins) */

#if	CF_TESTPROC

static int mapper_lockcheck(MR *mmp,cchar *s) noex {
	cint		to_lock = TO_LOCK ;
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
	    kl = lenstr(kp) ;

	if (vl < 0)
	    vl = lenstr(vp) ;

	{
	    cint	sz = (kl + 1 + vl + 1) ;
	    if (char *p ; (rs = uc_malloc(sz,&p)) >= 0) {
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

static int mapdir_process(MD *ep,ccharpp ev,mv admins,cchar *gn,int fd) noex {
	cint		to_lock = TO_LOCK ;
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
	        unl = intconv(tp - un) ;
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
	            if ((rs = getpwx_name(&pw,pwbuf,pwlen,un)) >= 0) {
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
			    cauto mall = uc_mallocstrw ;
	                    if (cc *cp ; (rs = mall(hbuf,fl,&cp)) >= 0) {
	                        ep->dname = cp ;
			    }
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

static int mapdir_processor(MD *ep,ccharpp ev,cchar *gn,int fd) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	bool		f_continue = true ;
	cchar		*dn = ep->dirname ;
	if (dn[0] == '~') {
	    dn = ep->dname ;
	    f_continue = ((dn != nullptr) && (dn[0] != '\0')) ;
	}
	if (f_continue) {
	    cint	n = lenstrarr(ev) ;
	    cchar	*pre = envpre ;
	    cchar	*post ;
	    cchar	defname[] = MOTD_DEFGROUP ;
	    cchar	allname[] = MOTD_ALLGROUP ;
	    char	env_admin[envlen+1] ;
	    char	env_admindir[envlen+1] ;
	    post = envstrs[envstr_admin] ;
	    strdcpy4(env_admin,envlen,pre,post,"=",ep->admin) ;
	    post = envstrs[envstr_admindir] ;
	    strdcpy4(env_admindir,envlen,pre,post,"=",dn) ;
	    ev[n+0] = env_admin ;
	    ev[n+1] = env_admindir ;
	    ev[n+2] = nullptr ;
	    if ((rs = mapdir_procout(ep,ev,dn,gn,fd)) >= 0) {
	        wlen += rs ;
	    } else if (isNotPresent(rs)) {
	        gn = defname ;
	        if ((rs = mapdir_procout(ep,ev,dn,gn,fd)) >= 0) {
	            wlen += rs ;
		} else if (isNotPresent(rs)) {
	            gn = allname ;
	            if ((rs = mapdir_procout(ep,ev,dn,gn,fd)) >= 0) {
	                wlen += rs ;
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    }
		}
	    }
	    {
	        ev[n] = nullptr ;
	    }
	} /* end if (continued) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_processor) */

static int mapdir_procout(MD *ep,mainv ev,cchar *dn,cchar *gn,int fd) noex {
	cint		clen = MAXNAMELEN ;
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

static int mapdir_procouter(MD *ep,mainv ev,cchar *fname,int ofd) noex {
	cint		of = O_RDONLY ;
	cint		to_open = TO_OPEN ;
	cint		to_read = TO_READ ;
	cint		to_write = TO_WRITE ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (ep && ev && fname) {
	    cint	olen = msgbuflen ;
	    if (char *obuf ; (rs = uc_malloc((olen + 1),&obuf)) >= 0) {
	        if ((rs = uc_openenv(fname,of,0,ev,to_open)) >= 0) {
	            cint	mfd = rs ;
	            if_constexpr (f_writeto) {
		        cint to = to_read ;
	                while ((rs = uc_reade(mfd,obuf,olen,to,0)) > 0) {
	                    rs = writeto(ofd,obuf,rs,to_write) ;
	                    wlen += rs ;
	                    if (rs < 0) break ;
	                } /* end while */
	            } else {
	                rs = uc_writedesc(ofd,mfd,-1) ;
	                wlen += rs ;
	            }
	            rs1 = u_close(mfd) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (open) */
	        rs = rsfree(rs,obuf) ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procouter) */


