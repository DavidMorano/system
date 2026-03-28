/* statmsg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

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
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>		/* |UINT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |environ| */
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucread.h>
#include	<ucwrite.h>
#include	<getax.h>
#include	<getusername.h>
#include	<getuserhome.h>
#include	<getxid.h>
#include	<getbufsize.h>
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
#include	<strn.h>
#include	<sfx.h>
#include	<snx.h>
#include	<mkx.h>			/* |mkpathuser(3uc)| */
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

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |loadstrs(3u)| */

/* local defines */

#define	SM_MAGIC	STATMSG_MAGIC
#define	SM_DEFGROUP	"default"
#define	SM_ALLGROUP	"all"
#define	SM_NAME		"statmsg"
#define	SM_SUF		"sm"
#define	SM_DIRSFNAME	"dirs"
#define	SM		statmsg

#define	PF		paramfile
#define	PF_ENT		paramfile_ent
#define	PF_CUR		paramfile_cur

#define	MA		statmsg_mapper
#define	MA_MAGIC	0x21367425

#define	MD		statmsg_mapdir

#define	VS		vecstr

#define	SP		strpack

#define	UI		userid

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
#define	CF_WRITETO	1		/* time out writes */
#endif
#ifndef	CF_PARAMFILE
#define	CF_PARAMFILE	1		/* use |paramfile(3dam)| */
#endif


/* imported namespaces */


/* local typedefs */

typedef	mainv		mv ;


/* external subroutines */

extern "C" {
    extern int uc_openenv(cc *,int,mode_t,mainv,int) noex ;
}


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
} ; /* end struct (motd_mapdir) */

typedef statmsg_mapdir *	mapdirp ;

namespace {
    struct vars {
	int	maxnamelen ;
	int	maxpathlen ;
	int	usernamelen ;
	int	envlen ;
	int	parambuflen ;
	operator int () noex ;
    } ; /* end struct */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int statmsg_ctor(statmsg *op,Args ... args) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
    	STATMSG		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mxp = new(nt) ptm) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (statmsg_ctor) */

local int statmsg_dtor(statmsg *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (statmsg_dtor) */

template<typename ... Args>
local inline int statmsg_magic(statmsg *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == STATMSG_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (statmsg_magic) */

local int	statmsg_userbegin(SM *,cchar *) noex ;
local int	statmsg_userend(SM *) noex ;

local int	statmsg_mapfind(SM *,time_t) noex ;
local int	statmsg_maplose(SM *) noex ;
local int	statmsg_mapfname(SM *,char *) noex ;
local int	statmsg_schedload(SM *,vecstr *) noex ;
local int	statmsg_checker(SM *,time_t) noex ;
local int	statmsg_envbegin(SM *) noex ;
local int	statmsg_envend(SM *) noex ;
local int	statmsg_envadds(SM *,strpack *,cc **,userid *,cc *) noex ;
local int	statmsg_envstore(SM *,strpack *,cchar **,int,
			cchar *,int) noex ;
local int 	statmsg_processor(SM *,cc **,mainv,cc *,cc *,int) noex ;

local int	mapper_start(MA *,time_t,cchar *,cchar *,
			cchar *) noex ;
local int	mapper_finish(MA *) noex ;
local int	mapper_check(MA *,time_t) noex ;
local int	mapper_process(MA *,cc **,mainv,cc *,cc *,int) noex ;
local int	mapper_processor(MA *,cc **,mainv,cc *,cc *,int) noex ;
local int	mapper_mapload(MA *) noex ;
local int	mapper_mapadd(MA *,cchar *,int,
			cchar *,int) noex ;
local int	mapper_mapfins(MA *) noex ;

local int	mapdir_start(MD *,cchar *,
			cchar *,cchar *,int,cchar *,int) noex ;
local int	mapdir_finish(MD *) noex ;
local int	mapdir_proc(MD *,cc **,mainv,cc *,cc *,int) noex ;
local int	mapdir_expand(MD *) noex ;
local int	mapdir_expander(MD *) noex ;
local int	mapdir_procer(MD *,cc **,cc *,cc *,int) noex ;
local int	mapdir_procerthem(MD *,cc **,cc *,vecstr *,mainv,int) noex ;
local int	mapdir_procerone(MD *,cchar **,
			cchar *,vecstr *,cchar *,int) noex ;
local int	mapdir_procout(MD *,cchar **,cchar *,
			cchar *,int) noex ;
local int	mapdir_procouter(MD *,cchar **,
			cchar *,int) noex ;

local bool	isBaseMatch(cchar *,cchar *,cchar *) noex ;


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
	envstr_keyname,
	envstr_admin,
	envstr_admindir,
	envstr_overlast
} ; /* end enum (envstrs) */

constexpr cpcchar	envstrs[] = {
	"USERNAME",
	"GROUPNAME",
	"UID",
	"GID",
	"KEYNAME",
	"ADMIN",
	"ADMINDIR",
	nullptr
} ; /* end array (envstrs) */

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
		    ptm *mxp = op->mxp ;
	            op->fe = SM_DIRSFNAME ;
	            if ((rs = mxp->create) >= 0) {
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
		            mxp->destroy() ;
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
		ptm *mxp = op->mxp ;
	        rs1 = mxp->destroy ;
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

local int statmsg_procidx(statmsg *,userid *,mv,int,cc *) noex ;

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

local int statmsg_procidx(statmsg *op,userid *idp,mv adms,int fd,cc *kn) noex {
	cint		n = nelem(envstrs) ;
    	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	char		kbuf[2] = {} ;
	if (kn == nullptr) kn = kbuf ;
	{
	    cint	sz = (op->nenv + n + 1) * szof(cchar *) ;
	    if (void *p ; (rs = lm_mall(sz,&p)) >= 0) {
		cchar	**ev = ccharpp(p) ;
		if (strpack packer ; (rs = packer.start(128)) >= 0) {
	            if ((rs = statmsg_envadds(op,&packer,ev,idp,kn)) >= 0) {
			cchar	*gn = idp->groupname ;
	                rs = statmsg_processor(op,ev,adms,gn,kn,fd) ;
	                wlen = rs ;
	            }
	            rs1 = packer.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (packer) */
	        rs1 = lm_free(p) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory allocation) */
	} /* end block */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (statmsg_procidx) */


/* private subroutines */

local int statmsg_userbegin(SM *op,cchar *username) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && username) ylikely {
	    rs = SR_INVALID ;
	    if (username[0]) ylikely {
		cint maxpath = var.maxpathlen ;
		cint asz = ((var.maxpathlen + 1) + (var.usernamelen + 1)) ;
		int ai = 0 ;
		if (char *a ; (rs = lm_mall(asz,&a)) >= 0) {
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
		            if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
		                op->useralloc = bp ;
		                op->username = bp ;
		                bp = (strwcpy(bp,username,-1) + 1) ;
		                op->userhome = bp ;
		                bp = (strwcpy(bp,hbuf,-1) + 1) ;
		            } /* end if (memory-allocation) */
	                } /* end if (getuserhome) */
	            } /* end if (ok) */
		    rs1 = lm_free(a) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (statmsg_userbegin) */

local int statmsg_userend(SM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->useralloc) {
	    void *vp = voidp(op->useralloc) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    op->useralloc = nullptr ;
	    op->username = nullptr ;
	    op->userhome = nullptr ;
	}
	return rs ;
}
/* end subroutine (statmsg_userend) */

local int statmsg_mapfind(SM *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	if (char *mbuf ; (rs = lm_mp(&mbuf)) >= 0) {
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
	    rs1 = lm_free(mbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (statmsg_mapfind) */

local int statmsg_maplose(SM *op) noex {
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

local int statmsg_mapfname(SM *op,char *fbuf) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	fbuf[0] = '\0' ;
	if (vecstr ss ; (rs = ss.start(6,0)) >= 0) {
	    if ((rs = statmsg_schedload(op,&ss)) >= 0) {
		cint	flen = var.maxpathlen ;
	    	cint	am = R_OK ;
		cchar	*fe = op->fe ;
	        if ((rs = permsched(schedmaps,&ss,fbuf,flen,fe,am)) >= 0) {
		    c = 1 ;
		} else if (isNotAccess(rs)) {
		    rs = SR_OK ;
		    fbuf[0] = '\0' ;
		}
	    } /* end if (statmsg-schedload) */
	    rs1 = ss.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (statmsg_mapfname) */

local int statmsg_schedload(SM *op,vecstr *slp) noex {
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

local int statmsg_checker(SM *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		nchanged = 0 ;/* return-value */
	if (op->nmaps > 0) {
	    ptm	*mxp = op->mxp ;
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

local int statmsg_envbegin(SM *op) noex {
	int		rs = SR_OK ;
	int		n = lenstrarr(environ) ;
	int		c = 0 ; /* return-value */
	{
	    cint	sz = (n + 1) * szof(cchar *) ;
	    if (void *p ; (rs = lm_mall(sz,&p)) >= 0) {
	        cchar	**va = ccharpp(p) ;
	        op->envv = va ;
	        for (int i = 0 ; environ[i] ; i += 1) {
	            cchar	*ep = environ[i] ;
	            bool	f = true ;
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

local int statmsg_envend(SM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->envv) {
	    rs1 = lm_free(op->envv) ;
	    if (rs >= 0) rs = rs1 ;
	    op->envv = nullptr ;
	}
	return rs ;
}
/* end subroutine (statmsg_envend) */

local int mkdigenv(char *ebuf,int elen,cc *pre,cc *key,uint uv) noex {
    	int	rs ;
    	if (char digbuf[diglen + 1] ; (rs = ctdec(digbuf,diglen,uv)) >= 0) {
	    rs = sncpy(ebuf,elen,pre,key,"=",digbuf) ;
	}
	return rs ;
} /* end subroutine (mkdigenv) */

local int statmsg_envadds(SM *op,SP *spp,cc **ev,userid *idp,cc *kn) noex {
    	cint		elen = var.envlen ;
	int		rs ;
	int		rs1 ;
	int		n ; /* return-value */
	mainv		envv = op->envv ;
	for (n = 0 ; n < op->nenv ; n += 1) {
	    ev[n] = envv[n] ;
	}
	if (char *ebuf ; (rs = lm_mall((elen + 1),&ebuf)) >= 0) {
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
	        case envstr_keyname:
	            if ((kn != nullptr) && (kn[0] != '\0')) {
	                rs = sncpy(ebuf,elen,pre,es,"=",kn) ;
	                el = rs ;
	            }
	            break ;
	        } /* end switch */
	        if ((rs >= 0) && (ebuf[0] != '\0')) {
	            rs = statmsg_envstore(op,spp,ev,n,ebuf,el) ;
	            if (rs > 0) n += 1 ;
	        }
	    } /* end for */
	    rs1 = lm_free(ebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	ev[n] = nullptr ; /* very important! */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (statmsg_envadds) */

local int statmsg_envstore(SM *op,SP *spp,cc **ev,int n,cc *ep,int el) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (ep) ylikely {
	        if (cc *cp ; (rs = strpack_store(spp,ep,el,&cp)) >= 0) {
	            ev[n++] = cp ;
	            rs = n ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (statmsg_envstore) */

local int statmsg_processor(SM *op,cc **ev,mv adms,cc *gn,cc *kn,int fd) noex {
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

local int mapper_start(MA *mmp,time_t dt,cc *un,cc *uh,cc *fname) noex {
	cint		to = TO_MAPCHECK ;
	int		rs ;
	mainv		evp = mainv(environ) ;
	memclear(mmp) ;
	mmp->username = un ;
	mmp->userhome = uh ;
	if ((rs = lockrw_create(&mmp->rwm,0)) >= 0) {
	    if (cc *cp ; (rs = lm_strw(fname,-1,&cp)) >= 0) {
	        mmp->fname = cp ;
	        if ((rs = vechand_start(&mmp->mapdirs,4,0)) >= 0) {
		    PF	*dfp = &mmp->dirsfile ;
	            if ((rs = paramfile_open(dfp,evp,cp)) >= 0) {
	                if ((rs = paramfile_checkint(dfp,to)) >= 0) {
	                    mmp->magic = MA_MAGIC ;
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
	            void *vp = voidp(mmp->fname) ;
	    	    lm_free(vp) ;
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

local int mapper_finish(MA *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mmp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MA_MAGIC) ylikely {
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
	            void *vp = voidp(mmp->fname) ;
	    	    rs1 = lm_free(vp) ;
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

local int mapper_check(MA *mmp,time_t dt) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		nchanged = 0 ; /*return-value */
	if (mmp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MA_MAGIC) ylikely {
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

local int mapper_process(MA *mmp,cc **ev,mv adms,cc *gn,cc *kn,int fd) noex {
	cint		to_lock = TO_LOCK ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (mmp) ylikely {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MA_MAGIC) ylikely {
	        lockrw	*lp = &mmp->rwm ;
	        if ((rs = lockrw_rdlock(lp,to_lock)) >= 0) ylikely {
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

local int mapper_processor(MA *mmp,cc **ev,mv adms,cc *gn,cc *kn,int fd) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MA_MAGIC) {
	        vechand		*mlp = &mmp->mapdirs ;
	        void		*vp{} ;
		rs = SR_OK ;
	        for (int i = 0 ; mlp->get(i,&vp) >= 0 ; i += 1) {
		    MD	*ep = mapdirp(vp) ;
	            if (vp) {
	                rs = mapdir_proc(ep,ev,adms,gn,kn,fd) ;
	                wlen += rs ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapper_processor) */

local int mapper_maploadparam(MA *mmp) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (ustat sb ; (rs = u_stat(mmp->fname,&sb)) >= 0) {
	    cint	plen = var.parambuflen ;
	    mmp->ti_mtime = sb.st_mtime ;
	    if (char *pbuf ; (rs = lm_mall((plen + 1),&pbuf)) >= 0) {
	        PF	*pfp = &mmp->dirsfile ;
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
	                vp = pe.val ;
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
		rs1 = lm_free(pbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (mapper_maploadparam) */

local int mapper_maploadfiler(MA *,cchar *,int) noex ;

local int mapper_maploadfile(MA *mmp) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) {
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
	    rs1 = lm_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (mapper_maploadfile) */

local int mapper_maploadfiler(MA *mmp,cchar *sp,int sl) noex {
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

local int mapper_mapload(MA *mmp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (mmp) {
	    rs = SR_NOTOPEN ;
	    if (mmp->magic == MA_MAGIC) {
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

local int mapper_mapadd(MA *mmp,cc *kp,int kl,cc *vap,int val) noex {
	cint		sz = szof(MD) ;
	int		rs = SR_FAULT ;
	if (mmp && kp && vap) {
	    rs = SR_INVALID ;
	    if ((kl > 0) && (val > 0)) {
	        if (void *vp ; (rs = lm_mall(sz,&vp)) >= 0) {
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
	                lm_free(ep) ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_mapadd) */

local int mapper_mapfins(MA *mmp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mmp) {
	    rs =  SR_NOTOPEN ;
	    if (mmp->magic == MA_MAGIC) {
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
	                    rs1 = lm_free(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
	            }
	        } /* end for */
	    } /* end if (magic) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapper_mapfins) */

local int mapdir_start(MD *ep,cc *un,cc *uh,
		cc *kp,int kl,cc *valp,int vall) noex {
	int		rs = SR_FAULT ;
	if (ep && kp && valp) {
	    memclear(ep) ;
	    rs = SR_INVALID ;
	    if ((kl > 0) && (vall > 0)) {
	        ep->username = un ;
	        ep->userhome = uh ;
	        if (kl < 0) kl = lenstr(kp) ;
	        if (vall < 0) vall = lenstr(valp) ;
	        {
	            cint	sz = (kl + 1 + vall + 1) ;
	            if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
	                ep->admin = bp ;
		        bp = strwcpy(bp,kp,kl) + 1 ;
		        ep->dirname = bp ;
		        bp = strwcpy(bp,valp,vall) + 1 ;
		        rs = lockrw_create(&ep->rwm,0) ;
		        if (rs < 0) {
	    	            void *vp = voidp(ep->admin) ;
	    		    lm_free(vp) ;
		            ep->admin = nullptr ;
		        } /* end if (error) */
	            } /* end if (memory-allocation) */
	        } /* end block */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapdir_start) */

local int mapdir_finish(MD *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->dname) {
	        void *vp = voidp(ep->dname) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->dname = nullptr ;
	    }
	    {
	        rs1 = lockrw_destroy(&ep->rwm) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->admin) {
	        void *vp = voidp(ep->admin) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->admin = nullptr ;
	        ep->dirname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapdir_finish) */

local int mapdir_proc(MD *ep,cc **ev,mv adms,cc *gn,cc *kn,int fd) noex {
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
	                        rs = mapdir_procer(ep,ev,gn,kn,fd) ;
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
/* end subroutine (mapdir_proc) */

local int mapdir_expand(MD *ep) noex {
	cint		to_lock = TO_LOCK ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = lockrw_wrlock(&ep->rwm,to_lock)) >= 0) {
	    if ((ep->dirname[0] != '/') && (ep->dname == nullptr)) {
	        rs = mapdir_expander(ep) ;
		rv = rs ;
	    } /* end if */
	    rs1 = lockrw_unlock(&ep->rwm) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (read-write lock) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mapdir_expand) */

local int mapdir_expander(MD *ep) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	int		pl = 0 ; /* return-value */
	if (ep->dirname) {
	    if (char *tbuf ; (rs = lm_mp(&tbuf)) >= 0) {
	        cchar	*pp ;
	        if (ep->dirname[0] == '~') {
	            pp = tbuf ;
	            rs = mkpathuser(tbuf,ep->username,ep->dirname,-1) ;
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
		    if (cchar *cp ; (rs = lm_strw(pp,pl,&cp)) >= 0) {
	                ep->dname = cp ;
		        if (pl < 0) pl = rs ;
		    } /* end if (memory-allocation) */
	        } /* end if (OK) */
	        rs1 = lm_free(tbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (mapdir_expander) */

local int vecstr_envload(VS *,cc *,cc *,cc *) noex ;

local int mapdir_procerv(MD *,cc **,cc *,mv,int,cc *) noex ;

local int mapdir_procer(MD *mdp,cc **ev,cc *gn,cc *kn,int fd) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
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
		            cchar	defname[] = SM_DEFGROUP ;
		            cchar	allname[] = SM_ALLGROUP ;
	                    cchar	*strs[nstrs + 1] ;
	                    loadstrs(strs,nstrs,gn,kn,defname,allname) ;
		            rs = mapdir_procerv(mdp,ev,dn,strs,fd,kn) ;
			    wlen = rs ;
		        }
	    		ev[n] = nullptr ;
		    } /* end if (vecstr_envload) */
	            rs1 = el.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (vecstr) */
	    } else if (isNotAccess(rs)) {
		rs = SR_OK ;
	    } /* end if (u_stat) */
	} /* end if (continued) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procer) */

local int mapdir_procerv(MD *mdp,cc **ev,cc *dn,mv strs,int fd,cc *kn) noex {
    	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	cchar		suf[] = SM_SUF ;
        if (vecstr nums ; (rs = nums.start) >= 0) {
            if (char *nbuf ; (rs = lm_mn(&nbuf)) >= 0) {
                cint	nlen = rs ;
                if (fsdir d ; (rs = d.open(dn)) >= 0) {
                    for (fsdir_ent de ; (rs = d.read(&de,nbuf,nlen)) > 0 ; ) {
                        cchar       *den = nbuf ;
                        if (den[0] != '.') {
                            cchar   *tp = strchr(den,'.') ;
                            if (tp && (strcmp((tp+1),suf) == 0)) {
                                int         tl ;
                                bool        f = true ;
                                cchar       *digp ;
                                tl = intconv(tp - den) ;
                                digp = strnbrk(den,tl,"0123456789") ;
                                if (digp) {
                                    tl = intconv(tp - digp) ;
                                    f = hasalldig(digp,tl) ;
                                }
                                if (f) {
                                    if ((kn[0] != '\0') && (kn[0] != '-')) {
                                        for (int i = 0 ; strs[i] ; i += 1) {
					    cchar	*sp = strs[i] ;
                                            f = isBaseMatch(den,sp,digp) ;
                                            if (f) break ;
                                        } /* end for */
                                    }
                                }
                                if (f) {
                                    tl = intconv(tp - den) ;
                                    rs = nums.add(den,tl) ;
                                }
                            } /* end if (have an SM file) */
                        }
                        if (rs < 0) break ;
                    } /* end while (reading directory entries) */
                    rs1 = d.close ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (fsdir) */
                rs1 = lm_free(nbuf) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (m-a-f) */
            if (rs >= 0) {
                nums.sort() ;
                rs = mapdir_procerthem(mdp,ev,dn,&nums,strs,fd) ;
                wlen += rs ;
            } /* end if */
            rs1 = nums.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (vecstr-nums) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (mapdir_procerv) */

local int mapdir_procerthem(MD *mdp,cc **ev,cc *dn,
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

local int mapdir_procerone(MD *ep,cc **ev,cc *dn,
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

local int mapdir_procout(MD *ep,cchar **ev,cchar *dn,cchar *bn,int fd) noex {
	cint		sz = ((var.maxpathlen + 1) + (var.maxnamelen + 1)) ;
	cint		maxpath = var.maxpathlen ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	int		ai = 0 ;
	cchar		suf[] = SM_SUF ;
	if (char *a ; (rs = lm_mall(sz,&a)) >= 0) {
	    cint	clen = var.maxnamelen ;
	    char	*fbuf = (a + ((maxpath + 1) * ai++)) ;
	    char	*cbuf = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = snsds(cbuf,clen,bn,suf)) >= 0) {
	        if ((rs = mkpath(fbuf,dn,cbuf)) >= 0) {
	            rs = mapdir_procouter(ep,ev,fbuf,fd) ;
	            wlen += rs ;
		}
	    }
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procout) */

local int mapdir_procouter(MD *ep,cchar **ev,cchar *fn,int ofd) noex {
	cint		to_open = TO_OPEN ;
	cint		to_read = TO_READ ;
	cint		to_write = TO_WRITE ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (ep && ev && fn) {
	    cint	olen = msgbuflen ;
	    if (char *obuf ; (rs = lm_mall((olen + 1),&obuf)) >= 0) {
		cint	of = O_RDONLY ;
		cmode	om = 0 ;
	        if ((rs = uc_openenv(fn,of,om,ev,to_open)) >= 0) {
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
	            } /* end if_constexpr (f_writeto) */
	            rs1 = u_close(mfd) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (open) */
		rs1 = lm_free(obuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mapdir_procouter) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = maxpathlen) == 0) {
	    if ((rs = getbufsize(bufsize_mp)) >= 0) {
	        maxpathlen = rs ;
	        if ((rs = getbufsize(bufsize_mn)) >= 0) {
		    maxnamelen = rs ;
		    if ((rs = getbufsize(bufsize_un)) >= 0) {
		        usernamelen = rs ;
			envlen = maxnamelen ;
			parambuflen = maxpathlen ;
		    }
		}
	    }
	} /* end if (needed) */
	return rs ;
} /* end method (vars::operator) */

local int vecstr_envload(VS *op,cc *pre,cc *adm,cc *dn) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *pbuf ; (rs = lm_sn(&pbuf)) >= 0) {
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
            rs1 = lm_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstr_envload) */

local bool isBaseMatch(cchar *den,cchar *bname,cchar *digp) noex {
	bool		f = false ;
	if (digp) {
	    csize nsize = size_t(digp - den) ;
	    f = (strncmp(den,bname,nsize) == 0) ;
	} else {
	    int	bl = lenstr(bname) ;
	    int	m = nleadstr(den,bname,bl) ;
	    f = (m == bl) && (den[m] == '.') ;
	}
	return f ;
}
/* end subroutine (isBaseMatch) */


