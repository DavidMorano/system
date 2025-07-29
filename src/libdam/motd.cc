/* motd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to help (manage) MOTD messages */
/* version %I% last-modified %G% */

#define	CF_WRITETO	1		/* time writes */
#define	CF_TESTPROC	0		/* test using |uc_openfsvc(3uc)| */

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

#define	MR		motd_mr
#define	MR_MAGIC	MOTD_MAPMAGIC

#define	MD		motd_mapdir

#define	PF		paramfile
#define	PF_ENT		paramfile_ent
#define	PF_CUR		paramfile_cur

#define	VS		vecstr

#define	SP		strpack

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	2048
#endif

#define	TO_POLL		5
#define	TO_LOCK		30
#define	TO_OPEN		10
#define	TO_READ		20
#define	TO_WRITE	50
#define	TO_CHECK	5		/* object checking */
#define	TO_MAPCHECK	10		/* mapper checking */
#define	TO_FILEAGE	5		/* directory map-file age */

#ifndef	CF_WRITETO
#define	CF_WRITETO	1
#endif
#ifndef	CF_TESTPROC
#define	CF_TESTPROC	0		/* test using |uc_openfsvc(3uc)| */
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
} ; /* end struct (motd_mapdir) */

typedef MD *		mapdirp ;

namespace {
    struct vars {
	int	maxnamelen ;
	int	maxpathlen ;
	int	envlen ;
	int	parambuflen ;
	operator int () noex ;
    } ; /* end struct */
}


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

static int	mapdir_start(MD *,cchar *,int,cchar *,int) noex ;
static int	mapdir_finish(MD *) noex ;
static int	mapdir_proc(MD *,ccharpp,mainv,cc *,int) noex ;
static int	mapdir_expand(MD *) noex ;
static int	mapdir_expander(MD *) noex ;
static int	mapdir_procer(MD *,ccharpp ,cchar *,int) noex ;
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
} ; /* end array (schedmaps) */

constexpr cpcchar	envbad[] = {
	"TMOUT",
	"A__z",
	nullptr
} ; /* end array (envbad) */

enum envstrs {
	envstr_username,
	envstr_groupname,
	envstr_uid,
	envstr_gid,
	envstr_admin,
	envstr_admindir,
	envstr_overlast
} ; /* end enum (envstrs) */

constexpr cpcchar	envstrs[] = {
	"USERNAME",
	"GROUPNAME",
	"UID",
	"GID",
	"ADMIN",
	"ADMINDIR",
	nullptr
} ; /* end array (envstrs) */

static vars		var ;

constexpr uid_t		uidend = -1 ;
constexpr gid_t		gidend = -1 ;

cchar			envpre[] = "MOTD_" ;	/* environment prefix */

cint			msgbuflen	= MSGBUFLEN ;
cint			diglen		= DECBUFLEN ;

cbool			f_comment	= false ;
cbool			f_writeto	= CF_WRITETO ;
cbool			f_testproc	= CF_TESTPROC ;


/* exported variables */


/* exported subroutines */

int motd_open(motd *op,cchar *pr) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = motd_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        op->fe = MOTD_DIRSFNAME ;
		if ((rs = var) >= 0) {
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
		} /* end if (vars) */
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
	        cchar	**ev = ccharpp(p) ;
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
	if (char *mbuf ; (rs = malloc_mp(&mbuf)) >= 0) {
	    mbuf[0] = '\0' ;
	    if ((rs = motd_mapfname(op,mbuf)) >= 0) {
	        if (mbuf[0] != '\0') {
	            if ((rs = mapper_start(&op->mapper,dt,mbuf)) >= 0) {
	                op->nmaps += 1 ;
		    }
	        }
	    }
	    rs = rsfree(rs,mbuf) ;
	} /* end if (m-a-f) */
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
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	fbuf[0] = '\0' ;
	if (vecstr ss ; (rs = ss.start(6,0)) >= 0) {
	    if ((rs = motd_schedload(op,&ss)) >= 0) {
	        cint	flen = var.maxpathlen ;
		cint	am = R_OK ;
		cchar	*fe = op->fe ;
	        if ((rs = permsched(schedmaps,&ss,fbuf,flen,fe,am)) >= 0) {
		    c = 1 ;
		} else if (isNotAccess(rs)) {
		    fbuf[0] = '\0' ;
		}
	    } /* end if (motd-schedload) */
	    rs1 = ss.finish ;
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
	cint		ech = mkchar(envpre[0]) ;
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
	        if (f && (ep[0] == ech)) {
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
    	int		rs ;
    	if (char digbuf[diglen + 1] ; (rs = ctdec(digbuf,diglen,uv)) >= 0) {
	    rs = sncpy(ebuf,elen,pre,key,"=",digbuf) ;
	}
	return rs ;
} /* end subroutine (mkdigenv) */

static int motd_envadds(motd *op,strpack *spp,ccharpp ev,userid *idp) noex {
    	cint		elen = var.envlen ;
	int		rs ;
	int		n = 0 ; /* return-value */
	mainv		envv = op->envv ;
	for (n = 0 ; n < op->nenv ; n += 1) {
	    ev[n] = envv[n] ;
	}
	if (char *ebuf ; (rs = uc_malloc((elen + 1),&ebuf)) >= 0) {
	    cchar	*pre = envpre ;
	    for (int i = 0 ; (rs >= 0) && envstrs[i] ; i += 1) {
	        uint	uv = UINT_MAX ;
	        int	el = -1 ;
	        cchar	*es = envstrs[i] ;
	        cchar	*cp ;
	        ebuf[0] = '\0' ;
	        switch (i) {
	        case envstr_uid:
	            if ((uv = idp->uid) != uidend) {
		        rs = mkdigenv(ebuf,elen,pre,es,uv) ;
		        el = rs ;
	            }
	            break ;
	        case envstr_gid:
	            if ((uv = idp->gid) != gidend) {
		        rs = mkdigenv(ebuf,elen,pre,es,uv) ;
		        el = rs ;
	            }
	            break ;
	        case envstr_username:
	            cp = idp->username ;
	            if ((cp != nullptr) && (cp[0] != '\0')) {
	                rs = sncpy(ebuf,elen,pre,es,"=",cp) ;
	                el = rs ;
	            }
	            break ;
	        case envstr_groupname:
	            cp = idp->groupname ;
	            if ((cp != nullptr) && (cp[0] != '\0')) {
	                rs = sncpy(ebuf,elen,pre,es,"=",cp) ;
	                el = rs ;
	            }
	            break ;
	        } /* end switch */
	        if ((rs >= 0) && (ebuf[0] != '\0')) {
	            rs = motd_envstore(op,spp,ev,n,ebuf,el) ;
	            if (rs > 0) n += 1 ;
	        }
	    } /* end for */
	    rs = rsfree(rs,ebuf) ;
	} /* end if (m-a-f) */
	ev[n] = nullptr ; /* very important! */
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
	cint		ttl = (1 * 60 * 60) ;
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
	        ptm	*mxp = op->mxp ;
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
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MOTD_MAPMAGIC) {
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
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_finish) */

static int mapper_check(MR *mmp,time_t dt) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		nchanged = 0 ; /* return-value */
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MOTD_MAPMAGIC) {
		lockrw	*lp = &mmp->rwm ;
	        if ((rs = lockrw_wrlock(lp,to_lock)) >= 0) {
		    cint	to = TO_MAPCHECK ;
		    if (dt == 0) dt = getustime ;
	            if ((dt - mmp->ti_check) >= to) {
			PF	*pfp = &mmp->dirsfile ;
	                if ((rs = pfp->check(dt)) > 0) {
	                    {
	                        mapper_mapfins(mmp) ;
	                        vechand_delall(&mmp->mapdirs) ;
	                    }
	                    rs = mapper_mapload(mmp) ;
	                    nchanged = rs ;
	                } /* end if */
	            } /* end if (map-object check) */
	            rs1 = lockrw_unlock(lp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (read-write lock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (mapper_check) */

static int mapper_proctest(MR *,cc **,mv,cc *,int) noex ;

static int mapper_process(MR *mmp,ccharpp ev,mv adms,cc *gn,int fd) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (mmp && ev && adms && gn) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MOTD_MAPMAGIC) {
	        if ((rs = lockrw_rdlock(&mmp->rwm,to_lock)) >= 0) {
	            if_constexpr (f_testproc) {
			rs = mapper_proctest(mmp,ev,adms,gn,fd) ;
	                wlen += rs ;
	            } else {
	                rs = mapper_processor(mmp,ev,adms,gn,fd) ;
	                wlen += rs ;
	            } /* end if_constexpr (f_testproc) */
	            rs1 = lockrw_unlock(&mmp->rwm) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (read-write lock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_process) */

static int mapper_proctest(MR *mmp,ccharpp ev,mv adms,cc *gn,int fd) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	(void) adms ;
	(void) gn ;
	if (mmp) {
	    if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	        cint	llen = rs ;
                cint   	of = O_RDONLY ;
                cint   	to = 5 ;
                cmode  	om = 0666 ;
                cchar	pr[] = "/home/genserv" ;
                cchar	prn[] = "genserv" ;
                cchar	svc[] = "hello" ;
                cchar	*argv[2] ;
                argv[0] = svc ;
                argv[1] = nullptr ;
                if ((rs = uc_openfsvc(pr,prn,svc,of,om,argv,ev,to)) >= 0) {
                    cint        rfd = rs ;
                    while ((rs = uc_reade(rfd,lbuf,llen,5,0)) > 0) {
                        rs = writeto(fd,lbuf,rs,10) ;
                        wlen += rs ;
                        if (rs < 0) break ;
                    } /* end while */
                    rs1 = u_close(rfd) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (uc_openfsvc) */
	        rs = rsfree(rs,lbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (mapper_proctest) */

static int mapper_processor(MR *mmp,ccharpp ev,mv adms,cc *gn,int fd) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MOTD_MAPMAGIC) {
		vechand	*dlp = &mmp->mapdirs ;
	        void	*vp{} ;
	        for (int i = 0 ; dlp->get(i,&vp) >= 0 ; i += 1) {
	            MD	*ep = mapdirp(vp) ;
	            if (vp) {
	                rs = mapdir_proc(ep,ev,adms,gn,fd) ;
	                wlen += rs ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_processor) */

static int mapper_mapload(MR *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MOTD_MAPMAGIC) {
	        if (ustat sb ; (rs = u_stat(mmp->fname,&sb)) >= 0) {
		    cint	plen = var.parambuflen ;
		    cint	psz = (var.parambuflen + 1) ;
		    if (char *pbuf ; (rs = uc_malloc(psz,&pbuf)) >= 0) {
	                PF	*pfp = &mmp->dirsfile ;
	                mmp->ti_mtime = sb.st_mtime ;
	                if (PF_CUR cur ; (rs = pfp->curbegin(&cur)) >= 0) {
	                    PF_ENT	pe ;
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
	        } else if (isNotAccess(rs)) {
	            rs = SR_OK ;
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mapper_mapload) */

static int mapper_mapadd(MR *mmp,cc *kp,int kl,cc *valp,int vall) noex {
	cint		sz = szof(MD) ;
	int		rs = SR_FAULT ;
	if (kp && valp) {
	    rs = SR_INVALID ;
	    if ((kl > 0) && (vall > 0)) {
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
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_mapadd) */

static int mapper_mapfins(MR *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MOTD_MAPMAGIC) {
	        vechand		*mlp = &mmp->mapdirs ;
	        void		*vp{} ;
		rs = SR_OK ;
	        for (int i = 0 ; mlp->get(i,&vp) >= 0 ; i += 1) {
	            MD	*ep = mapdirp(vp) ;
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
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_mapfins) */

static int mapdir_start(MD *ep,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (ep && kp && vp) {
	    rs = SR_INVALID ;
	    if ((kl > 0) && (vl > 0)) {
	        memclear(ep) ;
	        if (kl < 0) kl = lenstr(kp) ;
	        if (vl < 0) vl = lenstr(vp) ;
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
	                } /* end if (error) */
	            } /* end if (memory-allocation) */
	        } /* end if (memory-allocation) */
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

static int mapdir_proc(MD *ep,ccharpp ev,mv admins,cchar *gn,int fd) noex {
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
			lockrw	*lp = &ep->rwm ;
	                if ((rs = lockrw_rdlock(lp,to_lock)) >= 0) {
			    cchar	*dn = ep->dirname ;
	                    if ((dn[0] != '~') || (ep->dname != nullptr)) {
	            		rs = mapdir_procer(ep,ev,gn,fd) ;
	            		wlen += rs ;
	    		    } /* end if */
	    		    rs1 = lockrw_unlock(lp) ;
	    		    if (rs >= 0) rs = rs1 ;
			} /* end if (locked) */
		    } /* end if (ready) */
		} /* end if (ok) */
	    } /* end if (continued) */
	} /* end if (non-nul) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_proc) */

static int mapdir_expand(MD *ep) noex {
	cint		to_lock = TO_LOCK ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs =  lockrw_wrlock(&ep->rwm,to_lock)) >= 0) {
	    if ((ep->dirname[0] == '~') && (ep->dname == nullptr)) {
	        rs = mapdir_expander(ep) ;
		rv = rs ;
	    } /* end if */
	    rs1 = lockrw_unlock(&ep->rwm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (read-write lock) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mapdir_expand) */

static int mapdir_expanders(MD *ep,cc *,cc *) noex ;

static int mapdir_expander(MD *ep) noex {
	int		rs = SR_OK ;
	int		fl = 0 ; /* return-value */
	if ((ep->dirname != nullptr) && (ep->dirname[0] == '~')) {
	    strnul	ns ;
	    int		unl = -1 ;
	    cchar	*un = (ep->dirname + 1) ;
	    cchar	*pp = nullptr ;
	    if (cc *tp ; (tp = strchr(un,'/')) != nullptr) {
	        unl = intconv(tp - un) ;
	        pp = (tp + 1) ;
	    }
	    if ((unl == 0) || (un[0] == '\0')) {
	        un = ep->admin ;
	        unl = -1 ;
	    }
	    if (unl >= 0) {
		ns(un,unl) ;
		un = ns ;
	    }
	    rs = mapdir_expanders(ep,un,pp) ;
	    fl = rs ;
	} else {
	    rs = SR_INVALID ;
	}
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (mapdir_expander) */

static int mapdir_expanders(MD *ep,cc *un,cc *pp) noex {
    	int		rs ;
	int		rs1 ;
	int		fl = 0 ; /* return-value */
        if (char *hbuf ; (rs = malloc_mp(&hbuf)) >= 0) {
            if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
                cint        pwlen = rs ;
                if (ucentpwx pw ; (rs = pw.nam(pwbuf,pwlen,un)) >= 0) {
                    cchar   *uh = pw.pw_dir ;
                    if (pp) {
                        rs = mkpath(hbuf,uh,pp) ;
                        fl = rs ;
                    } else {
                        rs = mkpath(hbuf,uh) ;
                        fl = rs ;
                    }
                    if (rs >= 0) {
                        cauto malls = uc_mallocstrw ;
                        if (cchar *cp ; (rs = malls(hbuf,fl,&cp)) >= 0) {
                            ep->dname = cp ;
                        } /* end if (memory-allocation) */
                    } /* end if (OK) */
                } else if (isNotPresent(rs)) {
                    rs = SR_OK ;
                } /* end if (getpw_name) */
                rs1 = uc_free(pwbuf) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (m-a-f) */
            rs1 = uc_free(hbuf) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (m-a-f) */
	return (rs >= 0) ? fl : rs ;
} /* end subroutine (mapdir_expanders) */

static int vecstr_envload(VS *,cc *,cc *,cc *) noex ;

static int mapdir_procthem(MD *,mv,cc *,mv,int) noex ;

static int mapdir_procer(MD *mdp,ccharpp ev,cchar *gn,int fd) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	bool		f_continue = true ;
	cchar		*dn = mdp->dirname ;
	if (dn[0] == '~') {
	    dn = mdp->dname ;
	    f_continue = ((dn != nullptr) && (dn[0] != '\0')) ;
	}
	if (f_continue) {
	    if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
	        if (VS el ; (rs = el.start) >= 0) {
	            cchar	*pre = envpre ;
		    if ((rs = vecstr_envload(&el,pre,mdp->admin,dn)) >= 0) {
	        	cint	n = lenstrarr(ev) ;
		        int	c = 0 ;
		        cchar	*es ;
		        for (int i = 0 ; el.get(i,&es) >= 0 ; i += 1, c += 1) {
	    	            ev[n+i] = es ;
		        }
	                ev[n+c] = nullptr ;
	                cint nstrs = 3 ;
	                {
	                    cchar	defname[] = MOTD_DEFGROUP ;
	                    cchar	allname[] = MOTD_ALLGROUP ;
		            cchar	*strs[nstrs + 1] ;
		            loadstrs(strs,nstrs,gn,defname,allname) ;
	                    rs = mapdir_procthem(mdp,ev,dn,strs,fd) ;
			    wlen = rs ;
	                }
	                ev[n] = nullptr ;
		    } /* end if (vecstr_envload) */
	            rs1 = el.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (vecstr) */
	    } else if (isNotAccess(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (continued) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (mapdir_procer) */

static int mapdir_procthem(MD *mdp,mv ev,cc *dn,mv strs,int fd) noex {
    	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (mdp) {
	    bool	fcont = true ;
	    rs = SR_OK ;
	    for (int i = 0 ; (rs >= 0) && fcont && strs[i] ; i += 1) {
	        cchar	*sp = strs[i++] ;
	        if ((rs = mapdir_procout(mdp,ev,dn,sp,fd)) >= 0) {
		    wlen += rs ;
		    fcont = false ;
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (mapdir_procthem) */

static int mapdir_procout(MD *ep,mainv ev,cchar *dn,cchar *gn,int fd) noex {
    	cint		sz = ((var.maxpathlen + 1) + (var.maxnamelen + 1)) ;
    	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		wlen = 0 ; /* return-value */
	int		ai = 0 ;
	cchar		name[] = MOTD_NAME ;
	if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
	    cint	clen = var.maxnamelen ;
	    char	*fbuf = (a + ((maxpath + 1) * ai++)) ;
	    char	*cbuf = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = snsds(cbuf,clen,gn,name)) >= 0) {
	        if ((rs = mkpath(fbuf,dn,cbuf)) >= 0) {
	           rs = mapdir_procouter(ep,ev,fbuf,fd) ;
	           wlen += rs ;
	       }
	   }
	   rs = rsfree(rs,a) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procout) */

static int mapdir_procouter(MD *ep,mainv ev,cchar *fname,int ofd) noex {
	cint		to_open = TO_OPEN ;
	cint		to_read = TO_READ ;
	cint		to_write = TO_WRITE ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (ep && ev && fname) {
	    cint	olen = msgbuflen ;
	    if (char *obuf ; (rs = uc_malloc((olen + 1),&obuf)) >= 0) {
		cint	of = O_RDONLY ;
		cmode	om = 0 ;
	        if ((rs = uc_openenv(fname,of,om,ev,to_open)) >= 0) {
	            cint	mfd = rs ;
	            if_constexpr (f_writeto) {
		        cint	to = to_read ;
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

vars::operator int () noex {
    	int		rs ;
	if ((rs = maxpathlen) == 0) {
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	        maxpathlen = rs ;
	        if ((rs = getbufsize(getbufsize_mn)) >= 0) {
		    maxnamelen = rs ;
		    envlen = maxnamelen ;
		    parambuflen = maxpathlen ;
		}
	    }
	} /* end if (needed) */
	return rs ;
} /* end method (vars::operator) */

static int vecstr_envload(VS *op,cc *pre,cc *adm,cc *dn) noex {
    	int		rs ;
	int		c = 0 ;
	if (char *pbuf ; (rs = malloc_sn(&pbuf)) >= 0) {
	    cint	plen = rs ;
            for (int i = 0 ; (rs >= 0) && (i < 2) ; i += 1) {
                cchar       *post{} ;
                cchar       *valp{} ;
                switch (i) {
                case 0:
                    post = envstrs[envstr_admin] ;
                    valp = adm ;
                    break ;
                case 1:
                    post = envstrs[envstr_admindir] ;
                    valp = dn ;
                    break ;
                } /* end switch */
                if (post) {
                    if ((rs = sncpy(pbuf,plen,pre,post)) >= 0) {
                        rs = op->envadd(pbuf,valp) ;
			c += 1 ;
                    }
                }
            } /* end for */
            rs = rsfree(rs,pbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstr_envload) */


