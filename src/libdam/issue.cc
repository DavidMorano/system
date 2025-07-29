/* issue SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to help and manage "issue" messages */
/* version %I% last-modified %G% */

#define	CF_WRITETO	1		/* time out writes */
#define	CF_MAPPERAUDIT	0		/* call |mapper_audit()| */

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
	file-descriptor because we do not know if it is a non-regular
	file that might be flow-controlled.  We do not wait forever
	for those sorts of outputs.  So let us say that the output
	is a terminal that is currently flow-controlled.  We will
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
#include	<cstring>		/* |strchr(3c)| + |strncmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getbufsize.h>
#include	<getusername.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<ids.h>
#include	<writeto.h>
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
#include	<strnul.hh>
#include	<mkchar.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"issue.h"

import libutil ;			/* |loadstrs(3u)| */

/* local defines */

#define	IS		issue
#define	IS_MAP		issue_map
#define	IS_MD		issue_mapdir
#define	IS_DEFGROUP	"default"
#define	IS_ALLGROUP	"all"
#define	IS_NAME		"issue"
#define	IS_DIRSFNAME	"dirs"
#define	IS_MAGIC	ISSUE_MAGIC
#define	IS_DEFKEYNAME	ISSUE_DEFKEYNAME
#define	IS_MAPMAGIC	0x21367425

#define	MR		issue_mr
#define	MR_MAGIC	IS_MAPMAGIC

#define	MD		issue_mapdir

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

#undef	NLPS
#define	NLPS		10		/* number? polls per second */

#ifndef	CF_WRITETO
#define	CF_WRITETO	1
#endif
#ifndef	CF_MAPPERAUDIT
#define	CF_MAPPERAUDIT	0		/* call |mapper_audit()| */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine (template) */
using std::max ;			/* subroutine (template) */


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
} ; /* end struct (issue_mapdir) */

namespace {
    struct vars {
	int	maxpathlen ;
	int	maxnamelen ;
	int	envlen ;
	operator int () noex ;
    } ; /* end struct */
} /* end namespace */


/* forward references */

template<typename ... Args>
static int issue_ctor(issue *op,Args ... args) noex {
    	IS		*hop = op ;
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

static int	mapper_start(IS_MAP *,time_t,cchar *) noex ;
static int	mapper_finish(IS_MAP *) noex ;
static int	mapper_check(IS_MAP *,time_t) noex ;
static int	mapper_process(IS_MAP *,cchar **,mainv,cchar *,int) noex ;
static int	mapper_processor(IS_MAP *,cchar **,mainv,cchar *,int) noex ;
static int	mapper_mapload(IS_MAP *) noex ;
static int	mapper_mapadd(IS_MAP *,cchar *,int,cchar *,int) noex ;
static int	mapper_mapfins(IS_MAP *) noex ;
static int	mapper_audit(IS_MAP *) noex ;

static int	mapdir_start(MD *,cchar *,int,cchar *,int) noex ;
static int	mapdir_finish(MD *) noex ;
static int	mapdir_proc(MD *,cchar **,mainv,cchar *,int) noex ;
static int	mapdir_expand(MD *) noex ;
static int	mapdir_expander(MD *) noex ;
static int	mapdir_procer(MD *,cchar **,cchar *,int) noex ;
static int	mapdir_procout(MD *,cchar **,cchar *,cchar *,int) noex ;
static int	mapdir_procouter(MD *,cchar **,cchar *,int) noex ;
static int	mapdir_procerthem(MD *,cc **,cc *,vecstr *,mv,int) noex ;
static int	mapdir_procerone(MD *,cc **,cc *,vecstr *,cc *,int) noex ;

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

cchar			envpre[] = "ISSUE_" ;	/* environment prefix */

cint			msgbuflen	= MSGBUFLEN ;

cbool			f_writeto	= CF_WRITETO ;
cbool			f_mapperaudit	= CF_MAPPERAUDIT ;


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

int issue_process(IS *op,cchar *gn,mainv adms,int fd) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if ((rs = issue_magic(op,gn)) >= 0) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
	        rs = SR_INVALID ;
	        if (gn[0]) {
		    cint	n = nelem(envstrs) ;
		    int		sz ;
		    if (gn[0] == '-') {
			gn = IS_DEFKEYNAME ;
		    }
	            sz = (op->nenv + n + 1) * szof(cchar *) ;
	            if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	                cchar	**ev = ccharpp(p) ;
	                if (SP packer ; (rs = packer.start(128)) >= 0) {
	                    if ((rs = issue_envadds(op,&packer,ev,gn)) >= 0) {
	                        rs = issue_processor(op,ev,adms,gn,fd) ;
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

int issue_audit(IS *op) noex {
	int		rs ;
	if ((rs = issue_magic(op)) >= 0) {
	    if_constexpr (f_mapperaudit) {
	        if (op->nmaps > 0) {
	            rs = mapper_audit(&op->mapper) ;
	        }
	    } /* end if_constexpr (f_mapperaudit) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (issue_audit) */


/* private subroutines */

static int issue_mapfind(IS *op,time_t dt) noex {
	int		rs ;
	if (char *mbuf ; (rs = malloc_mp(&mbuf)) >= 0) {
	    mbuf[0] = '\0' ;
	    if ((rs = issue_mapfname(op,mbuf)) >= 0) {
	        if (mbuf[0]) {
	            if ((rs = mapper_start(&op->mapper,dt,mbuf)) >= 0) {
	                op->nmaps += 1 ;
		    }
	        }
	    } /* end if (map-fname) */
	    rs = rsfree(rs,mbuf) ;
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
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	fbuf[0] = '\0' ;
	if (vecstr ss ; (rs = ss.start(6,0)) >= 0) {
	    if ((rs = issue_schedload(op,&ss)) >= 0) {
	        cint	flen = var.maxpathlen ;
		cint	am = R_OK ;
		cchar	*fe = op->fe ;
	        if ((rs = permsched(schedmaps,&ss,fbuf,flen,fe,am)) >= 0) {
		    c = 1 ;
		} else if (isNotAccess(rs)) {
	            fbuf[0] = '\0' ;
		}
	    } /* end if (issue-schedload) */
	    rs1 = ss.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (scheds) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (issue_mapfname) */

static int issue_schedload(IS *op,vecstr *slp) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	cchar		keys[] = "pen" ;
	cchar		name[] = IS_NAME ;
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
	        char	kbuf[2] = { } ;
	        kbuf[0] = char(kch) ;
		c += 1 ;
	        rs = slp->envset(kbuf,vp,vl) ;
	    } /* end if (loading value) */
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? c : rs ;
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
	int		n = lenstrarr(environ) ;
	int		c = 0 ; /* return-value */
	sz = (n + 1) * szof(cchar *) ;
	if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	    cchar	**va = ccharpp(p) ;
	    op->envv = va ; /* <- store it */
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
	cint		elen = var.envlen ;
	int		rs ;
	int		n ; /* return-value */
	mainv		envv = op->envv ;
	for (n = 0 ; n < op->nenv ; n += 1) {
	    ev[n] = envv[n] ;
	}
	if (char *ebuf ; (rs = uc_malloc((elen + 1),&ebuf)) >= 0) {
	    cchar	*pre = envpre ;
	    for (int i = 0 ; (rs >= 0) && envstrs[i] ; i += 1) {
	        int	el = -1 ;
		cchar	*es = envstrs[i] ;
	        cchar	*cp ;
	        ebuf[0] = '\0' ;
	        switch (i) {
	        case envstr_keyname:
	            cp = keyname ;
	            if ((cp != nullptr) && (cp[0] != '\0')) {
	                rs = sncpy(ebuf,elen,pre,es,"=",cp) ;
	                el = rs ;
	            }
	            break ;
	        } /* end switch */
	        if ((rs >= 0) && (ebuf[0] != '\0')) {
	            rs = issue_envstore(op,spp,ev,n,ebuf,el) ;
	            if (rs > 0) n += 1 ;
	        }
	    } /* end for */
	    rs = rsfree(rs,ebuf) ;
	} /* end if (m-a-f) */
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

static int mapper_start(IS_MAP *mmp,time_t dt,cchar fname[]) noex {
	int		rs ;
	mainv		evp = environ ;
	memclear(mmp) ;
	if ((rs = lockrw_create(&mmp->rwm,0)) >= 0) {
	    if (cc *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        mmp->fname = cp ;
	        if ((rs = vechand_start(&mmp->mapdirs,4,0)) >= 0) {
	            paramfile	*pfp = &mmp->dirsfile ;
	            cchar	*fn = mmp->fname ;
	            if ((rs = pfp->open(evp,fn)) >= 0) {
	                cint	to = TO_MAPCHECK ;
	                if ((rs = pfp->checkint(to)) >= 0) {
	                    mmp->magic = IS_MAPMAGIC ;
	                    mmp->ti_check = dt ;
	                    rs = mapper_mapload(mmp) ;
	                    if (rs < 0) {
	                        mmp->magic = 0 ;
			    }
	                } /* end if */
	                if (rs < 0) {
	                    pfp->close() ;
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

static int mapper_check(IS_MAP *mmp,time_t dt) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		nchanged = 0 ; /* return-value */
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
	        if ((rs = lockrw_wrlock(&mmp->rwm,to_lock)) >= 0) {
		    paramfile	*pfp = &mmp->dirsfile ;
	            cint	to = TO_MAPCHECK ;
	            if (dt == 0) dt = getustime ;
	            if ((dt - mmp->ti_check) >= to) {
	                if ((rs = pfp->check(dt)) > 0) {
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
		rs = SR_OK ;
	        for (int i = 0 ; mp->get(i,&vp) >= 0 ; i += 1) {
	            MD	*ep = (MD *) vp ;
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

static int mapper_mapload(IS_MAP *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
	        if (ustat sb ; (rs = u_stat(mmp->fname,&sb)) >= 0) {
	            mmp->ti_mtime = sb.st_mtime ;
		    if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) {
			cint	plen = rs ;
	                PF	*pfp = &mmp->dirsfile ;
	                if (PF_CUR cur ; (rs = pfp->curbegin(&cur)) >= 0) {
	                    PF_ENT	pe ;
	                    int		kl ;
	                    int		vl ;
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

static int mapper_mapadd(IS_MAP *mmp,cc *kp,int kl,cc *vap,int val) noex {
	int		rs = SR_FAULT ;
	if (kp && vap) {
	    rs = SR_INVALID ;
	    if ((kl > 0) && (val > 0)) {
	        cint	sz = szof(MD) ;
	        if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
	            MD	*ep = (MD *) vp ;
	            if ((rs = mapdir_start(ep,kp,kl,vap,val)) >= 0) {
	                rs = vechand_add(&mmp->mapdirs,ep) ;
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

static int mapper_mapfins(IS_MAP *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
		vechand		*mlp = &mmp->mapdirs ;
		rs = SR_OK ;
	        if ((rs1 = mlp->count) >= 0) {
	            void *vp{} ;
	            for (int i = 0 ; (rs1 = mlp->get(i,&vp)) >= 0 ; i += 1) {
	                MD	*ep = (MD *) vp ;
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
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_mapfins) */

static int mapper_audit(IS_MAP *mmp) noex {
	int		rs = SR_FAULT ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == IS_MAPMAGIC) {
	        vechand		*mlp = &mmp->mapdirs ;
		rs = vechand_count(mlp) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_audit) */

static int mapdir_start(MD *ep,cc *kp,int kl,cc *vap,int val) noex {
	int		rs = SR_FAULT ;
	if (ep && kp && vap) {
	    rs = SR_INVALID ;
	    memclear(ep) ;
	    if ((kl > 0) && (val > 0)) {
	        if (kl < 0) kl = lenstr(kp) ;
	        if (val < 0) val = lenstr(vap) ;
	        {
	            cint	sz = (kl + 1 + val + 1) ;
	            if (char *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	                char	*bp = p ;
	                ep->admin = bp ;
	                bp = strwcpy(bp,kp,kl) + 1 ;
	                ep->dirname = bp ;
	                bp = strwcpy(bp,vap,val) + 1 ;
	                rs = lockrw_create(&ep->rwm,0) ;
	                if (rs < 0) {
	                    uc_free(ep->admin) ;
	                    ep->admin = nullptr ;
	                }
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

static int mapdir_proc(MD *ep,cchar **ev,mv adms,cc *gn,int fd) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
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
	    		    rs1 = lockrw_unlock(&ep->rwm) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (locked) */
		    } /* end if (acceptable) */
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
	if ((rs = lockrw_wrlock(&ep->rwm,to_lock)) >= 0) {
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

static int mapdir_procerv(MD *,cc **,cc *,mv,int,cc *) noex ;

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
    	        	cint	nstrs = 4 ;
			{
			    cchar	name[] = IS_NAME ;
	                    cchar	defname[] = IS_DEFGROUP ;
	                    cchar	allname[] = IS_ALLGROUP ;
			    cchar       *strs[nstrs + 1] ;
                            loadstrs(strs,nstrs,gn,defname,allname,name) ;
		            rs = mapdir_procerv(mdp,ev,dn,strs,fd,name) ;
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

static int mapdir_procerv(MD *ep,cc **ev,cc *dn,mv strs,int fd,cc *na) noex {
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
                                        for (int i = 0 ; strs[i] ; i += 1) {
					    cchar	*sp = strs[i] ;
                                            f = isBaseMatch(den,sp,digp) ;
                                            if (f) break ;
                                        } /* end for */
                                    }
                                    if (f) {
					tl = intconv(tp - den) ;
                                        rs = nums.add(den,tl) ;
                                    }
                                } /* end if (strcmp) */
                            } /* end if (have an IS file) */
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

static int mapdir_procerthem(MD *mdp,cc **ev,cc *dn,
		VS *blp,mv strs,int fd) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (mdp) {
	    bool	fcont = true ;
	    rs = SR_OK ;
	    for (int i = 0 ; (rs >= 0) && fcont && strs[i] ; i += 1) {
	        cchar	*sp = strs[i] ;
	        if ((rs = mapdir_procerone(mdp,ev,dn,blp,sp,fd)) >= 0) {
	            wlen += rs ;
		    fcont = false ;
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procerthem) */

static int mapdir_procerone(MD *ep,cc **ev,cc *dn,
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
/* end subroutine (mapdir_procerone) */

static int mapdir_procout(MD *ep,cc **ev,cc *dn,cc *gn,int fd) noex {
    	cint		sz = ((var.maxpathlen + 1) + (var.maxnamelen + 1)) ;
	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		wlen = 0 ; /* return-value */
	int		ai = 0 ;
	cchar		name[] = IS_NAME ;
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

static int mapdir_procouter(MD *ep,cc **ev,cc *fn,int ofd) noex {
	cint		to_open = TO_OPEN ;
	cint		to_read = TO_READ ;
	cint		to_write = TO_WRITE ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (ep) {
	    cint	of = O_RDONLY ;
	    cmode	om = 0664 ;
	    if ((rs = uc_openenv(fn,of,om,ev,to_open)) >= 0) {
		cint	mlen = msgbuflen ;
	        cint	mfd = rs ;
	        if_constexpr (f_writeto) {
	            cint sz = (mlen + 1) ;
	            if (char *mbuf ; (rs = uc_malloc(sz,&mbuf)) >= 0) {
		        cint	to = to_read ;
	                while ((rs = uc_reade(mfd,mbuf,mlen,to,0)) > 0) {
	                    rs = writeto(ofd,mbuf,rs,to_write) ;
	                    wlen += rs ;
	                    if (rs < 0) break ;
	                } /* end while */
	                rs = rsfree(rs,mbuf) ;
	            } /* end if (m-a-f) */
	        } else {
	            rs = uc_writedesc(ofd,mfd,-1) ;
	            wlen += rs ;
	        } /* end if_constexpr (f_writeto) */
	        rs1 = u_close(mfd) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (open) */
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
		}
	    }
	}
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

static bool isBaseMatch(cchar *den,cchar *bname,cchar *digp) noex {
	bool		f = false ;
	if (digp) {
	    csize	nsize = size_t(digp - den) ;
	    f = (strncmp(den,bname,nsize) == 0) ;
	} else {
	    cint	bl = lenstr(bname) ;
	    cint	m = nleadstr(den,bname,bl) ;
	    f = (m == bl) && (den[m] == '.') ;
	}
	return f ;
}
/* end subroutine (isBaseMatch) */


