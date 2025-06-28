/* issue SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to help and manage "issue" messages */
/* version %I% last-modified %G% */

#define	CF_WRITETO	1		/* time out writes */
#define	CF_ISSUEAUDIT	0		/* call 'issue_audit()' */
#define	CF_MAPPERAUDIT	0		/* call 'mapper_audit()' */
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
	issue

	Description:
	This object module writes the contents of various ISSUEs
	(as specified by the caller) to an open file descriptor
	(also specified by the caller).

	Implementation notes:

	When processing, we time-out writes to the caller-supplied
	file-descriptor because we don't know if it is a non-regular
	file that might be flow-controlled. We don't wait forever
	for those sorts of outputs. So let's say that the output
	is a terminal that is currently flow-controlled. We will
	time-out on our writes and the user will not get this whole
	ISSUE text!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<usystem.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getbufsize.h>
#include	<getusername.h>
#include	<getax.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vechand.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<strpack.h>
#include	<bfile.h>
#include	<fsdir.h>
#include	<ascii.h>
#include	<six.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<mknpathx.h>
#include	<mkpathx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<matstr.h>
#include	<permx.h>
#include	<nleadx.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"issue.h"

import libutil ;			/* |loadstrs(3u)| */

/* local defines */

#define	IS			issue
#define	IS_MAP			issue_map
#define	IS_MD		issue_mapdir
#define	IS_DEFGROUP		"default"
#define	IS_ALLGROUP		"all"
#define	IS_NAME			"issue"
#define	IS_DIRSFNAME		"dirs"
#define	IS_MAGIC		ISSUE_MAGIC
#define	IS_DEFKEYNAME		ISSUE_DEFKEYNAME
#define	IS_MAPMAGIC		0x21367425

#define	PF			paramfile
#define	PF_ENT			paramfile_ent
#define	PF_CUR			paramfile_cur

#define	SP			strpack

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UCPWCACHE */

#ifndef	MAXNAMELEN
#define	MAXNAMELEN	256
#endif

#ifndef	MSGBUFLEN
#define	MSGBUFLEN	2048
#endif

#undef	ENVBUFLEN
#define	ENVBUFLEN	100

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
#define	NLPS		10		/* number? polls per second */


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */


/* external variables */

extern mainv	environ ;


/* local structures */

struct issue_mapdir {
	lockrw		rwm ;
	cchar		*admin ;
	cchar		*dirname ;	/* raw */
	cchar		*dname ;	/* expanded */
} ;

namespace {
    struct vars {
	int	maxpathlen ;
	operator int () noex ;
    } ; /* end struct */
}


/* forward references */

template<typename ... Args>
static int issue_ctor(issue *op,Args ... args) noex {
    	ISSUE		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (issue_ctor) */

static int issue_dtor(issue *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (issue_dtor) */

template<typename ... Args>
static inline int issue_magic(issue *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == IS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (issue_magic) */

static int	issue_mapfind(IS *,time_t) noex ;
static int	issue_maplose(IS *) noex ;
static int	issue_mapfname(IS *,char *) noex ;
static int	issue_schedload(IS *,vecstr *) noex ;
static int	issue_checker(IS *,time_t) noex ;
static int	issue_envbegin(IS *) noex ;
static int	issue_envend(IS *) noex ;
static int	issue_envadds(IS *,SP *,cchar **,cchar *) noex ;
static int	issue_envstore(IS *,SP *,cchar **,int,cchar *,int) noex ;
static int 	issue_processor(IS *,ccharpp,mainv,cchar *,int) noex ;
#if	CF_ISSUEAUDIT
static int	issue_audit(IS *) noex ;
#endif

static int	mapper_start(IS_MAP *,time_t,cchar *) noex ;
static int	mapper_finish(IS_MAP *) noex ;
static int	mapper_check(IS_MAP *,time_t) noex ;
static int	mapper_process(IS_MAP *,cchar **,mainv,cchar *,int) noex ;
static int	mapper_processor(IS_MAP *,cchar **,mainv,cchar *,int) noex ;
static int	mapper_mapload(IS_MAP *) noex ;
static int	mapper_mapadd(IS_MAP *,cchar *,int,cchar *,int) noex ;
static int	mapper_mapfins(IS_MAP *) noex ;
#if	CF_MAPPERAUDIT
static int	mapper_audit(IS_MAP *) noex ;
#endif

static int	mapdir_start(IS_MD *,cchar *,int,cchar *,int) noex ;
static int	mapdir_finish(IS_MD *) noex ;
static int	mapdir_process(IS_MD *,cchar **,mainv,cchar *,int) noex ;
static int	mapdir_expand(IS_MD *) noex ;
static int	mapdir_expander(IS_MD *) noex ;
static int	mapdir_procer(IS_MD *,cchar **,cchar *,int) noex ;
static int	mapdir_procout(IS_MD *,cchar **,cchar *,cchar *,int) noex ;
static int	mapdir_procouter(IS_MD *,cchar **,cchar *,int) noex ;
static int	mapdir_procerthem(IS_MD *,cc **,cc *,vecstr *,cc **,int) noex ;
static int	mapdir_procerone(IS_MD *,cc **,cc *,vecstr *,cc *,int) noex ;

static int	writeto(int,cchar *,int,int) noex ;

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

constexpr cpcchar	envstrs[] = {
	"KEYNAME",
	"ADMIN",
	"ADMINDIR",
	nullptr
} ;

enum envstrs {
	envstr_keyname,
	envstr_admin,
	envstr_admindir,
	envstr_overlast
} ;

static vars		var ;

constexpr char		envpre[] = "IS_" ;	/* environment prefix */


/* exported variables */


/* exported subroutines */

int issue_open(IS *op,cchar *pr) noex {
	int		rs ;
	if ((rs = issue_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		if ((rs = var) >= 0) {
	            op->fe = IS_DIRSFNAME ;
	            if (cchar *cp ; (rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	                op->pr = cp ;
	                if ((rs = ptm_create(&op->mx,nullptr)) >= 0) {
		            custime dt = getustime ;
	                    if ((rs = issue_mapfind(op,dt)) >= 0) {
	                        if ((rs = issue_envbegin(op)) >= 0) {
	                            op->ti_lastcheck = dt ;
	                            op->magic = IS_MAGIC ;
	                        }
	                        if (rs < 0) {
	                            issue_maplose(op) ;
		                }
	                    } /* end if (mapfind) */
	                    if (rs < 0) {
	                        ptm_destroy(&op->mx) ;
		            }
	                } /* end if (ptm_create) */
	                if (rs < 0) {
	                    uc_free(op->pr) ;
	                    op->pr = nullptr ;
	                }
	            } /* end if (memory-allocation) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		issue_dtor(op) ;
	    }
	} /* end if (issue_ctor) */
	return rs ;
}
/* end subroutine (issue_open) */

int issue_close(IS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = issue_magic(op)) >= 0) {
	    {
	        rs1 = issue_envend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = issue_maplose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&op->mx) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->pr) {
	        rs1 = uc_free(op->pr) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    {
		rs1 = issue_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (issue_close) */

int issue_check(IS *op,time_t dt) noex {
	int		rs ;
	if ((rs = issue_magic(op)) >= 0) {
	    rs = issue_checker(op,dt) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (issue_check) */

int issue_process(IS *op,cchar *groupname,mainv adms,int fd) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if ((rs = issue_magic(op,groupname)) >= 0) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
	        rs = SR_INVALID ;
	        if (groupname[0]) {
		    cint	n = nelem(envstrs) ;
		    int		sz ;
		    if (groupname[0] == '-') {
			groupname = IS_DEFKEYNAME ;
		    }
	            sz = (op->nenv + n + 1) * szof(cchar *) ;
	            if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	                cchar	**ev = (cchar **) p ;
	                if (SP packer ; (rs = packer.start(128)) >= 0) {
			    cc *gn = groupname ;
	                    if ((rs = issue_envadds(op,&packer,ev,gn)) >= 0) {
	                        rs = issue_processor(op,ev,adms,groupname,fd) ;
	                        wlen = rs ;
	                    }
	                    rs1 = strpack_finish(&packer) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (packer) */
	                rs1 = uc_free(p) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (memory allocation) */
	        } /* end if (valid groupname) */
	    } /* end if (valid FD) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (issue_process) */


/* private subroutines */

static int issue_mapfind(IS *op,time_t dt) noex {
	int		rs ;
	if (char *mapfname ; (rs = malloc_mp(&mapfname)) >= 0) {
	    mapfname[0] = '\0' ;
	    if ((rs = issue_mapfname(op,mapfname)) >= 0) {
	        if (mapfname[0] != '\0') {
	            if ((rs = mapper_start(&op->mapper,dt,mapfname)) >= 0) {
	                op->nmaps += 1 ;
		    }
	        }
	    } /* end if (map-fname) */
	    rs = rsfree(rs,mapfname) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (issue_mapfind) */

static int issue_maplose(IS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nmaps > 0) {
	    rs1 = mapper_finish(&op->mapper) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nmaps = 0 ;
	}
	return rs ;
}
/* end subroutine (issue_maplose) */

static int issue_mapfname(IS *op,char *fbuf) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	fbuf[0] = '\0' ;
	if (vecstr scheds ; (rs = vecstr_start(&scheds,6,0)) >= 0) {
	    if ((rs = issue_schedload(op,&scheds)) >= 0) {
		cint	am = R_OK ;
	        cint	flen = var.maxpathlen ;
	        rs1 = permsched(schedmaps,&scheds,fbuf,flen,op->fe,am) ;
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
	    } /* end if (issue-schedload) */
	    rs1 = vecstr_finish(&scheds) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (scheds) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (issue_mapfname) */

static int issue_schedload(IS *op,vecstr *slp) noex {
	int		rs = SR_OK ;
	cchar		*keys = "pen" ;
	cchar		*name = IS_NAME ;
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
	        char	kbuf[2] = { } ;
	        kbuf[0] = char(kch) ;
	        rs = vecstr_envset(slp,kbuf,vp,vl) ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (issue_schedload) */

static int issue_checker(IS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		nchanged = 0 ;
	if (op->nmaps > 0) {
	    if ((rs = ptm_lock(&op->mx)) >= 0) {
	        if (dt == 0) dt = getustime ;
	        if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	            rs = mapper_check(&op->mapper,dt) ;
	            nchanged = rs ;
	            op->ti_lastcheck = dt ;
	        } /* end if */
	        rs1 = ptm_unlock(&op->mx) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if */
	} /* end if (positive) */
	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (issue_checker) */

static int issue_envbegin(IS *op) noex {
	cint		es = mkchar(envpre[0]) ;
	cint		envprelen = lenstr(envpre) ;
	int		rs = SR_OK ;
	int		sz ;
	int		i = lenstrarr(environ) ;
	int		c = 0 ; /* return-value */
	sz = (i + 1) * szof(cchar *) ;
	if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	    cchar	**va = (cchar **) p ;
	    op->envv = va ; /* <- store it */
	    for (i = 0 ; environ[i] != nullptr ; i += 1) {
	        cchar	*ep = environ[i] ;
		bool f = true ;
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
/* end subroutine (issue_envbegin) */

static int issue_envend(IS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->envv) {
	    rs1 = uc_free(op->envv) ;
	    if (rs >= 0) rs = rs1 ;
	    op->envv = nullptr ;
	}
	return rs ;
}
/* end subroutine (issue_envend) */

static int issue_envadds(IS *op,SP *spp,cc **ev,cc *keyname) noex {
	cint		envlen = ENVBUFLEN ;
	int		rs = SR_OK ;
	int		n ; /* return-value */
	int		el ;
	mainv		envv = op->envv ;
	cchar		*cp ;
	char		envbuf[ENVBUFLEN + 1] ;
	for (n = 0 ; n < op->nenv ; n += 1) {
	    ev[n] = envv[n] ;
	}
	for (int i = 0 ; (rs >= 0) && (envstrs[i] != nullptr) ; i += 1) {
	    envbuf[0] = '\0' ;
	    el = -1 ;
	    switch (i) {
	    case envstr_keyname:
	        cp = keyname ;
	        if ((cp != nullptr) && (cp[0] != '\0')) {
	            rs = sncpy4(envbuf,envlen,envpre,envstrs[i],"=",cp) ;
	            el = rs ;
	        }
	        break ;
	    } /* end switch */
	    if ((rs >= 0) && (envbuf[0] != '\0')) {
	        rs = issue_envstore(op,spp,ev,n,envbuf,el) ;
	        if (rs > 0) n += 1 ;
	    }
	} /* end for */
	ev[n] = nullptr ; /* very important! */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (issue_envadds) */

static int issue_envstore(IS *op,SP *spp,cc **ev,int n,cc *ep,int el) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (ep) {
	        if (cchar *cp ; (rs = strpack_store(spp,ep,el,&cp)) >= 0) {
	            ev[n++] = cp ;
	            rs = n ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (issue_envstore) */

static int issue_processor(IS *op,cchar **ev,mainv adms,cc *gn,int fd) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = issue_checker(op,0)) >= 0) {
	    if (op->nmaps > 0) {
	        rs = mapper_process(&op->mapper,ev,adms,gn,fd) ;
	        wlen += rs ;
	    }
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (issue_processor) */

#if	CF_ISSUEAUDIT
static int issue_audit(IS *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
#if	CF_MAPPERAUDIT
	    if (op->nmaps > 0) {
	        rs = mapper_audit(&op->mapper) ;
	    }
#endif /* CF_MAPPERAUDIT */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (issue_audit) */
#endif /* CF_ISSUEAUDIT */

static int mapper_start(IS_MAP *mmp,time_t dt,cchar fname[]) noex {
	int		rs ;
	mainv		evp = environ ;
	memclear(mmp) ;
	if ((rs = lockrw_create(&mmp->rwm,0)) >= 0) {
	    if (cc *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        mmp->fname = cp ;
	        if ((rs = vechand_start(&mmp->mapdirs,4,0)) >= 0) {
	            cchar	*fn = mmp->fname ;
	            if ((rs = paramfile_open(&mmp->dirsfile,evp,fn)) >= 0) {
	                cint	to = TO_MAPCHECK ;
	                if ((rs = paramfile_checkint(&mmp->dirsfile,to)) >= 0) {
	                    mmp->magic = IS_MAPMAGIC ;
	                    mmp->ti_check = dt ;
	                    rs = mapper_mapload(mmp) ;
	                    if (rs < 0) {
	                        mmp->magic = 0 ;
			    }
	                } /* end if */
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

static int mapper_finish(IS_MAP *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
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

static int mapper_check(IS_MAP *mmp,time_t dt) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		nchanged = 0 ; /* return-value */
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
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
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? nchanged : rs ;
}
/* end subroutine (mapper_check) */

static int mapper_process(IS_MAP *mmp,cc **ev,mv adms,cc *gn,int fd) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
	        if ((rs = lockrw_rdlock(&mmp->rwm,to_lock)) >= 0) {
	            {
	                rs = mapper_processor(mmp,ev,adms,gn,fd) ;
	                wlen += rs ;
	            }
	            rs1 = lockrw_unlock(&mmp->rwm) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (read-write lock) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_process) */

static int mapper_processor(IS_MAP *mmp,cc **ev,mv adms,cc *gn,int fd) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
		vechand *mp = &mmp->mapdirs ;
	        void *vp{} ;
	        for (int i = 0 ; mp->get(i,&vp) >= 0 ; i += 1) {
	            IS_MD	*ep = (IS_MD *) vp ;
	            if (vp) {
	                rs = mapdir_process(ep,ev,adms,gn,fd) ;
	                wlen += rs ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_processor) */

static int mapper_mapload(IS_MAP *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
	        if (ustat sb ; (rs = u_stat(mmp->fname,&sb)) >= 0) {
	            PF		*pfp = &mmp->dirsfile ;
	            PF_ENT	pe ;
	            PF_CUR	cur ;
	            mmp->ti_mtime = sb.st_mtime ;
	            if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	                cint	plen = PBUFLEN ;
	                int	kl ;
	                int	vl ;
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
	                rs1 = paramfile_curend(pfp,&cur) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (paramfile-cursor) */
	        } else if (isNotAccess(rs)) {
	            rs = SR_OK ;
	        } /* end if (stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mapper_mapload) */

static int mapper_mapadd(IS_MAP *mmp,cchar *kp,int kl,
		cchar *valp,int vall) noex {
	cint		sz = szof(IS_MD) ;
	int		rs ;

	if ((kp == nullptr) || (valp == nullptr)) return SR_FAULT ;
	if ((kl == 0) || (vall == 0)) return SR_INVALID ;

	if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
	    IS_MD	*ep = (IS_MD *) vp ;
	    if ((rs = mapdir_start(ep,kp,kl,valp,vall)) >= 0) {
	        rs = vechand_add(&mmp->mapdirs,ep) ;
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

static int mapper_mapfins(IS_MAP *mmp) noex {
	vechand		*mlp = &mmp->mapdirs ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (mmp == nullptr) return SR_FAULT ;
	if (mmp->magic != IS_MAPMAGIC) return SR_NOTOPEN ;
	if ((rs1 = vechand_count(mlp)) >= 0) {
	    void *vp{} ;
	    for (int i = 0 ; (rs1 = mlp->get(i,&vp)) >= 0 ; i += 1) {
	        IS_MD	*ep = (IS_MD *) vp ;
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
	} /* end if (vechand-count) */
	if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	return rs ;
}
/* end subroutine (mapper_mapfins) */

#if	CF_MAPPERAUDIT
static int mapper_audit(IS_MAP *mmp) noex {
	vechand		*mlp = &mmp->mapdirs ;
	int		rs ;

	if (mmp == nullptr) return SR_FAULT ;

	if (mmp->magic != IS_MAPMAGIC) return SR_NOTOPEN ;

	rs = vechand_count(mlp) ;

	return rs ;
}
/* end subroutine (mapper_audit) */
#endif /* CF_MAPPERAUDIT */

static int mapdir_start(IS_MD *ep,cchar *kp,int kl,
		cchar *vp,int vl) noex {
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
	} /* end block */

	return rs ;
}
/* end subroutine (mapdir_start) */

static int mapdir_finish(IS_MD *ep) noex {
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

static int mapdir_process(IS_MD *ep,cchar **ev,mv adms,cc *gn,int fd) noex {
	cint	to_lock = TO_LOCK ;
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (ep->dirname[0] != '\0') {
	    int	f_continue = true ;
	    if ((adms != nullptr) && (adms[0] != nullptr)) {
	        f_continue = (matstr(adms,ep->admin,-1) >= 0) ;
	    } /* end if (adms) */
	    if (f_continue) {
	        if ((ep->dirname[0] == '~') && (ep->dname == nullptr)) {
	            rs = mapdir_expand(ep) ;
	        }
	        if (rs >= 0) {
	            if ((ep->dirname[0] != '~') || (ep->dname != nullptr)) {
	                if ((rs = lockrw_rdlock(&ep->rwm,to_lock)) >= 0) {
			    cchar	*dn = ep->dirname ;
	                    if ((dn[0] != '~') || (ep->dname != nullptr)) {
	                        rs = mapdir_procer(ep,ev,gn,fd) ;
	        		wlen += rs ;
	    		    } /* end if */
	    		    lockrw_unlock(&ep->rwm) ;
			} /* end if (locked) */
		    } /* end if (acceptable) */
		} /* end if (ok) */
	    } /* end if (continued) */
	} /* end if (non-nul) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_process) */

static int mapdir_expand(IS_MD *ep) noex {
	cint	to_lock = TO_LOCK ;
	int		rs ;

	if ((rs = lockrw_wrlock(&ep->rwm,to_lock)) >= 0) {

	    if ((ep->dirname[0] == '~') && (ep->dname == nullptr)) {
	        rs = mapdir_expander(ep) ;
	    } /* end if */

	    lockrw_unlock(&ep->rwm) ;
	} /* end if (read-write lock) */

	return rs ;
}
/* end subroutine (mapdir_expand) */

static int mapdir_expander(IS_MD *ep) noex {
	int		rs = SR_OK ;
	int		rs1 = SR_OK ;
	int		fl = 0 ;
	if ((ep->dirname != nullptr) && (ep->dirname[0] == '~')) {
	    int		unl = -1 ;
	    cchar	*un = (ep->dirname + 1) ;
	    cchar	*pp = nullptr ;
	    char	ubuf[USERNAMELEN + 1] ;
	    if (cc *tp ; (tp = strchr(un,'/')) != nullptr) {
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
	    if (char *hbuf ; (rs = malloc_mp(&hbuf)) >= 0) {
	        if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
	            cint	pwlen = rs ;
	            ucentpw pw ;
		    if ((rs = getpwx_name(&pw,pwbuf,pwlen,un)) >= 0) {
		        cchar	*uh = pw.pw_dir ;
	                if (pp) {
	                    rs = mkpath2(hbuf,uh,pp) ;
	                    fl = rs ;
	                } else {
	                    rs = mkpath1(hbuf,uh) ;
	                    fl = rs ;
	                }
	                if (rs >= 0) {
			    cauto malls = uc_mallocstrw ;
	                    if (cchar *cp ; (rs = malls(hbuf,fl,&cp)) >= 0) {
	                        ep->dname = cp ;
			    }
	                }
		    } else if (isNotPresent(rs)) {
		        rs = SR_OK ;
	            } /* end if (getpw_name) */
		    rs1 = uc_free(pwbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
		rs1 = uc_free(hbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = SR_INVALID ;
	}

	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (mapdir_expander) */

static int mapdir_procerv(IS_MD *ep,cc **ev,cc *dn,cc **strs,
		int fd,cc *na) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (vecstr nums ; (rs = nums.start) >= 0) {
	    if (char *nbuf ; (rs = malloc_mn(&nbuf)) >= 0) {
		cint	nlen = rs ;
                if (fsdir d ; (rs = d.open(dn)) >= 0) {
                    for (fsdir_ent de ; (rs = d.read(&de,nbuf,nlen)) > 0 ; ) {
                        cchar       *den = nbuf ;
                        if (den[0] != '.') {
                            if (cc *tp ; (tp = strchr(den,'.')) != np) {
                                if (strcmp((tp + 1),na) == 0) {
                                    cchar       *digp ;
                                    int tl = intconv(tp - den) ;
                                    bool        f = true ;
                                    digp = strnbrk(den,tl,"0123456789") ;
                                    if (digp) {
                                        tl = intconv(tp - digp) ;
                                        f = hasalldig(digp,tl) ;
                                    }
                                    if (f) {
                                        for (int i = 0 ; i < 3 ; i += 1) {
                                            f = isBaseMatch(den,strs[i],digp) ;
                                            if (f) break ;
                                        }
                                    }
                                    if (f) {
					tl = intconv(tp - den) ;
                                        rs = nums.add(den,tl) ;
                                    }
                                } /* end if (strcmp) */
                            } /* end if (have an ISSUE file) */
                        } /* end if */
                        if (rs < 0) break ;
                    } /* end while (reading directory entries) */
                    rs1 = d.close ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (fsdir) */
                if (rs >= 0) {
                    nums.sort() ;
                    rs = mapdir_procerthem(ep,ev,dn,&nums,strs,fd) ;
                    wlen += rs ;
                } /* end if */
	        rs = rsfree(rs,nbuf) ;
	    } /* end if (m-a-f) */
            rs1 = nums.finish ;
            if (rs >= 0) rs = rs1 ;
	} /* end if (nums) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (mapdir_procerv) */

static int mapdir_procer(IS_MD *ep,cc **ev,cc *gn,int fd) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	cchar		*dn = ep->dirname ;
	cchar		*defname = IS_DEFGROUP ;
	cchar		*allname = IS_ALLGROUP ;
	cchar		*name = IS_NAME ;
	char		env_admin[ENVBUFLEN+1] ;
	char		env_admindir[ENVBUFLEN+1] ;
	bool		f_continue = true ;
	if (dn[0] == '~') {
	    dn = ep->dname ;
	    f_continue = ((dn != nullptr) && (dn[0] != '\0')) ;
	}
	if (f_continue) {
	    if (ustat sb ; (rs = u_stat(dn,&sb)) >= 0) {
	        cint	envlen = ENVBUFLEN ;
		int	n = lenstrarr(ev) ; /* used-throughout */
	        cchar		*post ;
	        post = envstrs[envstr_admin] ;
	        strdcpy4(env_admin,envlen,envpre,post,"=",ep->admin) ;
	        post = envstrs[envstr_admindir] ;
	        strdcpy4(env_admindir,envlen,envpre,post,"=",dn) ;
	        ev[n+0] = env_admin ;
	        ev[n+1] = env_admindir ;
	        ev[n+2] = nullptr ;
		{
    	            cint	nstrs = 5 ;
                    cchar       *strs[nstrs + 1] ;
                    loadstrs(strs,nstrs,gn,defname,allname,name) ;
		    rs = mapdir_procerv(ep,ev,dn,strs,fd,name) ;
		    wlen += rs ;
	        }
	        {
	            ev[n] = nullptr ;
	        }
	    } else if (isNotAccess(rs)) {
		rs = SR_OK ;
	    } /* end if (u_stat) */
	} /* end if (continued) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procer) */

static int mapdir_procerthem(IS_MD *ep,cchar **ev,cchar *dn,
		vecstr *blp,cchar **strs,int fd) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	cchar		*kn = strs[0] ;

	rs1 = mapdir_procerone(ep,ev,dn,blp,kn,fd) ;
	if (isNotPresent(rs1)) {
	    kn = strs[1] ;
	    rs1 = mapdir_procerone(ep,ev,dn,blp,kn,fd) ;
	    if (! isNotPresent(rs1)) rs = rs1 ;
	} else {
	    rs = rs1 ;
	}
	if (rs > 0) wlen += rs ;

	if (rs >= 0) {
	    kn = strs[2] ;
	    rs1 = mapdir_procerone(ep,ev,dn,blp,kn,fd) ;
	    if (! isNotPresent(rs1)) rs = rs1 ;
	    if (rs > 0) wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procerthem) */

static int mapdir_procerone(IS_MD *ep,cchar **ev,cchar *dn,
		vecstr *blp,cchar *kn,int fd) noex {
	cint	kl = lenstr(kn) ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	int		wlen = 0 ;
	cchar		*bep ;

	for (i = 0 ; vecstr_get(blp,i,&bep) >= 0 ; i += 1) {
	    if (bep != nullptr) {
	        if (strncmp(bep,kn,kl) == 0) {
	            c += 1 ;
	            if ((rs = mapdir_procout(ep,ev,dn,bep,fd)) >= 0) {
	                wlen += rs ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
		    }
	        }
	    }
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && (c == 0)) rs = SR_NOENT ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procerone) */

static int mapdir_procout(IS_MD *ep,cchar **ev,cchar *dn,
		cchar *gn,int fd) noex {
	cint	clen = MAXNAMELEN ;
	int		rs ;
	int		wlen = 0 ;
	cchar		*name = IS_NAME ;
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

static int mapdir_procouter(IS_MD *ep,cc **ev,cc *fname,int ofd) noex {
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
	time_t		dt = getustime ;
	time_t		ti_write ;
	int		rs = SR_FAULT ;
	int		i ;
	int		pt = TO_POLL ;
	int		pto ;
	int		tlen = 0 ;
	if (wbuf) {
	    rs = SR_BADF ;
	    if (wfd >= 0) {
	        if (wlen < 0) wlen = lenstr(wbuf) ;
	        if (pt > wto) pt = wto ;
	        i = 0 ;
	        fds[i].fd = wfd ;
	        fds[i].events = POLLOUT ;
	        i += 1 ;
	        fds[i].fd = -1 ;
	        fds[i].events = 0 ;
	        ti_write = dt ;
	        pto = (pt * POLL_INTMULT) ;
	        while ((rs >= 0) && (tlen < wlen)) {
	            rs = u_poll(fds,1,pto) ;
	            dt = getustime ;
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
	            if (rs == SR_INTR) rs = SR_OK ;
	            if ((dt - ti_write) >= wto) break ;
	        } /* end while */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (writeto) */
#endif /* CF_WRITETO */

static bool isBaseMatch(cchar *den,cchar *bname,cchar *digp) noex {
	bool	f = false ;
	if (digp == nullptr) {
	    int	bl = lenstr(bname) ;
	    int	m = nleadstr(den,bname,bl) ;
	    f = (m == bl) && (den[m] == '.') ;
	} else {
	    f = (strncmp(den,bname,(digp-den)) == 0) ;
	}
	return f ;
}
/* end subroutine (isBaseMatch) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = maxpathlen) == 0) {
	    rs = getbufsize(getbufsize_mp) ;
	    maxpathlen = rs ;
	}
	return rs ;
} /* end method (vars::operator) */


