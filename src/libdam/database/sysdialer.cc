/* sysdialer SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system-dialer storage object */
/* version %I% last-modified %G% */

#define	CF_SAMEMODULE	1		/* try to use same module */
#define	CF_PRCACHE	1		/* PR-cache */

/* revision history:

	- 2003-11-04, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sysdialer

	Description:
	This object manages what system-dialers (SYSDIALERS) have
	been loaded so far.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strnlen(3c)| |strnlen(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<fsdir.h>
#include	<ids.h>
#include	<dirseen.h>
#include	<strn.h>		/* |strnrchr(3uc)| */
#include	<strwcpy.h>
#include	<strx.h>
#include	<mkpathx.h>
#include	<mkfname.h>
#include	<pathclean.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"sysdialer.h"


/* local defines */

#define	SD		sysdialer
#define	SD_FL		sysdialer_fl
#define	SD_ENT		sysdialer_ent
#define	SD_CALLS	sysdialer_calls
#define	SD_INFO		sysdialer_info
#define	SD_ARGS		sysdialer_args
#define	SD_PRC		sysdialer_prc
#define	SD_MOD		sysdialer_mod
#define	SD_MAGIC	SYSDIALER_MAGIC
#define	SD_DNAME	"sysdialers"

#define	DS		dirseen

#define	TO_FILECHECK	3

#define	NEXTS		3		/* number of extensions */

#define	LIBCNAME	"lib"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif

#ifndef	CF_SAMEMODULE
#define	CF_SAMEMODULE	1		/* try to use same module */
#endif
#ifndef	CF_PRCACHE
#define	CF_PRCACHE	1		/* PR-cache */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef SD_ENT		ent ;
typedef SD_MOD		mod ;
typedef SD_PRC		prcache ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct sysdialer_calls {
	int		(*open)() noex ;
	int		(*reade)() noex ;
	int		(*recve)() noex ;
	int		(*recvfrome)() noex ;
	int		(*recvmsge)() noex ;
	int		(*write)() noex ;
	int		(*send)() noex ;
	int		(*sendto)() noex ;
	int		(*sendmsg)() noex ;
	int		(*shutdown)() noex ;
	int		(*close)() noex ;
    } ; /* end struct (sysdialer_calls) */
}

struct fext {
	cchar		*exp ;
	int		exl ;
} ;

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int sysdialer_ctor(SD *op,Args ... args) noex {
    	SYSDIALER	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->elp = new(nothrow) vecobj) != np) {
	        if ((op->plp = new(nothrow) vecstr) != np) {
	            if ((op->dlp = new(nothrow) vecstr) != np) {
			rs = SR_OK ;
		    } /* end if (new-vecstr) */
		    if (rs < 0) {
		        delete op->plp ;
		        op->plp = np ;
		    }
		} /* end if (new-vecstr) */
		if (rs < 0) {
		    delete op->elp ;
		    op->elp = np ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysdialer_ctor) */

static int sysdialer_dtor(SD *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cnullptr	np{} ;
	    rs = SR_OK ;
	    if (op->dlp) {
		delete op->dlp ;
		op->dlp = np ;
	    }
	    if (op->plp) {
		delete op->plp ;
		op->plp = np ;
	    }
	    if (op->elp) {
		delete op->elp ;
		op->elp = np ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysdialer_dtor) */

template<typename ... Args>
static inline int sysdialer_magic(SD *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSDIALER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sysdialer_magic) */

static int sysdialer_sofind(SD *,cc *,cc *,ent **) noex ;
static int sysdialer_sofindpr(SD *,ids *,DS **,cc *,cc *,ent **) noex ;
static int sysdialer_sofindprs(SD *,ids *,DS **,cc *,ent **) noex ;
static int sysdialer_sofindvar(SD *,ids *,DS **,cc *,ent **) noex ;
static int sysdialer_socheckvarc(SD *,ids *,DS **,cc *,int, cc *,ent **) noex ;
static int sysdialer_sochecklib(SD *,ids *,DS **,cc *,cc *,ent **) noex ;

#ifdef	COMMENT
static int	sysdialer_sotest(SD *,cchar *) noex ;
#endif

static int	prcache_start(prcache *) noex ;
static int	prcache_lookup(prcache *,int,cchar **) noex ;
static int	prcache_finish(prcache *) noex ;

static int	entry_start(ent **,cchar *,cchar *,SD_MOD *) noex ;
static int	entry_checkdir(ent **,cchar *,cchar *) noex ;
static int	entry_loadcalls(ent **,void *) noex ;
static int	entry_hasname(ent **,void *,cchar *) noex ;
static int	entry_finish(ent *) noex ;

static int	vecstr_loadexts(vecstr *,cchar *,cchar *,int) noex ;

extern "C" {
    static int	vcmpname(cvoid **,cvoid **) noex ;
}

static int	getext(fext *,cchar *,int) noex ;
static bool	isNotConn(int) noex ;


/* local variables */

constexpr cpcchar	prnames[] = {
	"LOCAL",
	"PCS",
	"NCMP",
	"EXTRA",
	nullptr
} ;

constexpr cpcchar	exts[] = {
	"so",
	"o",
	"",
	nullptr
} ;

constexpr cpcchar	de64[] = {
	"sparcv9",
	"sparc",
	"",
	nullptr
} ;

constexpr cpcchar	de32[] = {
	"sparcv8",
	"sparcv7",
	"sparc",
	"",
	nullptr
} ;

constexpr cpcchar	prdirs[] = {
	"syssysdialer",
	"sysdialers",
	nullptr
} ; /* end array (prdirs) */

enum subs {
	sub_open,
	sub_reade,
	sub_recve,
	sub_recvfrome,
	sub_recvmsge,
	sub_write,
	sub_send,
	sub_sendto,
	sub_sendmsg,
	sub_shutdown,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"reade",
	"recve",
	"recvfrome",
	"recvmsge",
	"write",
	"send",
	"sendto",
	"sendmsg",
	"shutdown",
	"close",
	nullptr
} ;

constexpr int		rsnotconnected[] = {
	SR_PFNOSUPPORT,
	SR_AFNOSUPPORT,
	SR_NETUNREACH,
	SR_NETDOWN,
	SR_HOSTUNREACH,
	SR_HOSTDOWN,
	SR_TIMEDOUT,
	SR_CONNREFUSED,
	SR_NETRESET,
	SR_CONNABORTED,
	SR_CONNRESET,
	SR_NOENT,
	SR_COMM,
	SR_PROTO,
	0
} ;

static vars		var ;

cbool			f_samemodule 	= CF_SAMEMODULE ;
cbool			f_prcache	= CF_PRCACHE ;


/* exported variables */

cint	sysdialerms::full 	= (1 << sysdialero_full) ;
cint	sysdialerms::halfout	= (1 << sysdialero_halfout) ;
cint	sysdialerms::halfin 	= (1 << sysdialero_halfin) ;
cint	sysdialerms::cor 	= (1 << sysdialero_cor) ;
cint	sysdialerms::co 	= (1 << sysdialero_co) ;
cint	sysdialerms::cl 	= (1 << sysdialero_cl) ;
cint	sysdialerms::nargs	= (1 << sysdialero_nargs) ;

sysdualerms		sysdialerm ;


/* exported subroutines */

static int sysdiar_starter(SD *,mainv,mainv) noex ;

int sysdialer_start(SD *op,cchar *pr,mainv prs,mainv dirs) noex {
	int		rs ;
	if ((rs = sysdialer_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
	            if (cchar *cp ; (rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	                op->pr = cp ;
		        rs = sysdiar_starter(op,prs,dirs) ;
			if ((rs < 0) && op->pr) {
			    uc_free(op->pr) ;
			    op->pr = nullptr ;
			} /* end if (error) */
		    } /* end if (memory-allocation) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		sysdialer_dtor(op) ;
	    }
	} /* end if (sysdialer_ctor) */
	return rs ;
}
/* end subroutine (sysdialer_start) */

static int sysdiar_startprs(SD *,mainv) noex ;
static int sysdiar_startdirs(SD *,mainv) noex ;
static int sysdiar_startents(SD *) noex ;

static int sysdiar_starter(SD *op,mainv prs,mainv dirs) noex {
    	int		rs = SR_OK ;
	if ((rs = sysdiar_startprs(op,prs)) >= 0) {
	    if ((rs = sysdiar_startdirs(op,dirs)) >= 0) {
		rs = sysdiar_startents(op) ;
	    }
            if (rs < 0) {
                sysdialer_finish(op) ;
            }
	}
	return rs ;
}
/* end subroutine (sysdialer_starter) */

static int sysdiar_startprs(SD *op,mainv prs) noex {
    	int		rs = SR_OK ;
        if (prs) {
            vecstr  *plp = op->plp ;
            if ((rs = plp->start(5,0)) >= 0) {
                op->fl.vsprs = true ;
                for (int i = 0 ; prs[i] != nullptr ; i += 1) {
                    if (strcmp(prs[i],op->pr) != 0) {
                        rs = plp->add(prs[i],-1) ;
                    }
                    if (rs < 0) break ;
                } /* end if */
                if (rs < 0) {
                    if (op->fl.vsprs) {
                        plp->finish() ;
                        op->fl.vsprs = false ;
                    }
                }
            } /* end if (vecstr_start) */
        } /* end if (had program roots) */
	return rs ;
}
/* end subroutine (sysdiar_startprs) */

static int sysdiar_startdirs(SD *op,mainv dirs) noex {
    	int		rs = SR_OK ;
        if (dirs) {
            vecstr	*dlp = op->dlp ;
	    cint	vn = 10 ;
	    cint	vo = 0 ;
            if ((rs = dlp->start(vn,vo)) >= 0) {
                op->fl.vsdirs = true ;
                for (int i = 0 ; (rs >= 0) && dirs[i] ; i += 1) {
                    rs = dlp->add(dirs[i]) ;
                    if (rs < 0) break ;
                } /* end for */
                if (rs >= 0) {
                    if (mainv dp ; (rs = dlp->getvec(&dp)) >= 0) {
                        op->dirs = dp ;
                    }
                }
                if (rs < 0) {
                    if (op->fl.vsdirs) {
                        dlp->finish() ;
                        op->fl.vsdirs = false ;
                    }
                } /* end if (errors) */
            } /* end if (vecstr_start) */
        } else {
            op->dirs = prdirs ;
        } /* end if */
	return rs ;
}
/* end subroutine (sysdiar_startdirs) */

static int sysdiar_startents(SD *op) noex {
        vecobj  	*elp = op->elp ;
        cint    	vsz = szof(ent) ;
        cint    	vn = 5 ;
        cint    	vo = VECOBJ_OSORTED ;
    	int		rs ;
        if ((rs = elp->start(vsz,vn,vo)) >= 0) {
            op->fl.voents = true ;
            if ((rs = prcache_start(&op->pc)) >= 0){
                op->fl.prcache = true ;
                op->magic = SD_MAGIC ;
            }
            if (rs < 0) {
                elp->finish() ;
                op->fl.voents = false ;
            }
        } /* end if (vecobj_start) */
	return rs ;
}
/* end subroutine (sysdialer_startents) */

int sysdialer_finish(SD *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysdialer_magic(op)) >= 0) {
	    if (op->fl.prcache) {
	        rs1 = prcache_finish(&op->pc) ;
	        if (rs >= 0) rs = rs1 ;
		op->fl.prcache = false ;
	    }
	    if (op->elp && op->fl.voents) {
		vecobj	*elp = op->elp ;
	        void	*vp{} ;
	        for (int i = 0 ; elp->get(i,&vp) >= 0 ; i += 1) {
	            ent	*ep = (SD_ENT *) vp ;
	            if (vp) {
	                rs1 = entry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        } /* end for */
	        {
	            rs1 = elp->finish ;
	            if (rs >= 0) rs = rs1 ;
		    op->fl.voents = false ;
	        }
	    } /* end if (op->elp) */
	    if (op->dlp && op->fl.vsdirs) {
	        rs1 = vecstr_finish(op->dlp) ;
	        if (rs >= 0) rs = rs1 ;
		op->fl.vsdirs = false ;
	    }
	    if (op->plp && op->fl.vsprs) {
	        rs1 = vecstr_finish(op->plp) ;
	        if (rs >= 0) rs = rs1 ;
		op->fl.vsprs = false ;
	    }
	    if (op->pr != nullptr) {
	        rs1 = uc_free(op->pr) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    {
		rs1 = sysdialer_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysdialer_finish) */

/* load a dialer */
int sysdialer_loadin(SD *op,cchar *name,ent **depp) noex {
	SD_MOD	*mp ;
	ent		se, e, *dep ;
	int		rs ;
	int		osize ;
	int		f_alloc = false ;
	void		*dhp ;

	if ((name == nullptr) || (name[0] == '\0'))
	    return SR_INVALID ;

	se.name = name ;
	rs = vecobj_search(op->elp,&se,vcmpname,depp) ;

	if (rs >= 0) {
	    (*depp)->count += 1 ;
	    goto ret0 ;
	}

/* search for it in any modules that we have already loaded */

#if	CF_SAMEMODULE
	vecobj	*elp = op->elp ;
	void	*vp{} ;
	for (int i = 0 ; (rs = elp->get(i,&vp)) >= 0 ; i += 1) {
	    ent	*dep = (SD_ENT *) vp ;
	    if (vp) {
	        mp = dep->mp ;
	        dhp = mp->dhp ;
	        rs = entry_hasname(dep,dhp,name) ;
	    }
	    if (rs >= 0) break ;
	} /* end for */

	if (rs >= 0) {

	    rs = entry_start(&e,name,nullptr,mp) ;
	    if (rs < 0)
	        goto ret0 ;

	    rs = entry_loadcalls(&e,dhp) ;
	    if (rs < 0)
	        entry_finish(&e) ;

	    goto ret1 ;
	}
#endif /* CF_SAMEMODULE */

/* create a new load module descriptor */

	osize = szof(SD_MOD) ;
	rs = uc_malloc(osize,&mp) ;

	if (rs < 0)
	    goto bad0 ;

	f_alloc = true ;
	memset(mp,0,osize) ;

/* initialize a SD entry */

	rs = entry_start(&e,name,nullptr,mp) ;
	if (rs < 0)
	    goto bad1 ;

/* search for it in the filesystem */

	rs = sysdialer_sofind(op,op->pr,name,&e) ;
	if (rs < 0)
	    goto bad2 ;

	dhp = e.mp->dhp ;

/* got it */

	rs = entry_loadcalls(&e,dhp) ;
	if (rs < 0)
	    goto bad2 ;

/* save this entry */
ret1:
	rs = vecobj_add(op->elp,&e) ;
	i = rs ;
	if (rs < 0)
	    goto bad2 ;

/* return point-to-entry to our caller */

	if (depp != nullptr) {
	    vecobj_get(op->elp,i,depp) ;
	}

ret0:

	return rs ;

/* bad stuff */
bad2:
	entry_finish(&e) ;

bad1:
	if (f_alloc)
	    uc_free(mp) ;

bad0:
	goto ret0 ;
}
/* end subroutine (sysdialer_loadin) */

int sysdialer_loadout(SD *op,cchar *name) noex {
	int		rs ;
	if ((rs = sysdialer_magic(op,name)) >= 0) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		vecobj	*elp = op->elp ;
	        ent	te{} ;
	        te.name = name ;
	        void	*vp{} ;
	        if ((rs = elp->search(&te,vcmpname,&vp)) >= 0) {
	            cint	ei = rs ;
	            ent		*dep = (SD_ENT *) vp ;
	            if (vp) {
	               if (dep->count <= 1) {
	                   entry_finish(dep) ;
	                   elp->del(ei) ;
	               } else {
	                   dep->count -= 1 ;
	               }
	            }
	        } /* end if (vecobj_search) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysdialer_loadout) */

int sysdialer_check(SD *op,time_t dt) noex {
	int		rs ;
	if ((rs = sysdialer_magic(op)) >= 0) {
	    if (dt == 0) dt = getustime ;
	    op->ti_lastcheck = dt ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (sysdialer_check) */


/* private subroutines */

namespace {
    struct sofind {
	ids		id ;
	dirseen		ds ;
	cchar		*so ;
	ent		*ep ;
	char		*lbuf ;
	char		*pbuf ;
	sofind(cc *aso,ent **aep) noex : so(aso), ep(aep) { } ;
	int operator () (cchar *) noex ;
	int socheck(SD *,USTAT *) noex ;
	int sofindpr(SD *,cc *) noex ;
	int sofindprs(SD *) noex ;
    } ; /* end struct (sofind) */
}

static int sysdialer_sofind(SD *op,cc *pr,cc *so,ent *ep) noex {
    	sofind		sf(so,ep) ;
	return sf(op,pr) ;
}

int sofind::operator () (SD *op,cchar *pr) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ids_load(&id)) >= 0) {
	    if ((rs = dirseen_start(&ds)) >= 0) {
    		cint	maxpath = var.maxpathlen ;
		cint	asz = (var.maxpathlen * 2) ;
		int	ai = 0 ;
	        if (char *a ; (rs = uc_malloc(asz,&a)) >= 0) {
	            lbuf = (a + (maxpath * ai++)) ;
	            pbuf = (a + (maxpath * ai++)) ;
	            if ((rs = sofindpr(op,pr)) == 0) {
	                if ((rs = sofindprs(op)) == 0) {
	                    rs = sofindvar(op) ;
		        }
		    }
		    rs = rsfree(rs,a) ;
	        } /* end if (m-a-f) */
	        rs1 = dirseen_finish(&ds) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (dirseen) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return rs ;
}
/* end subroutine (sofind::operator) */

int sofind::sofindpr(SD *op,cc *pr) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	    if ((rs = mkpath(lbuf,pr,LIBCNAME)) >= 0) {
		len = rs ;
		if ((rs = dirseen_havename(&ds,lbuf,-1)) >= 0) {
		    if (USTAT sb ; (rs = u_stat(lbuf,&sb)) >= 0) {
	    		if (S_ISDIR(sb.st_mode)) {
			    if ((rs = dirseen_havedevino(&ds,&sb)) >= 0) {
				rs = socheck(op) ;
			    } else if (isNotConn(rs)) {
				rs = SR_OK ;
			    }
			} else {
			    rs = SR_OK ;
			}
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    }
		} else if (isNotConn(rs)) {
		    rs = SR_OK ;
		}
	    } /* end if (mkpath) */
	return rs ;
}
/* end subroutine (sysdialer::sofindpr) */

int sofind::socheck(SD *op,USTAT *sbp) noex {
    	int		rs ;
	int		len = 0 ;
	if ((rs = sysdialer_sochecklib(op,&id,&ds,lbuf,so,ep)) > 0) {
	    len = rs ;
	} else if (rs == 0) {
	    if ((rs = pathclean(pbuf,lbuf)) >= 0) {
	        rs = dirseen_add(dsp,pbuf,rs,sbp) ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subriutine (sofind::socheck) */

int sofind::sofindprs(SD *op) noex {
	int		rs = SR_NOENT ;
	int		rs1 ;
	int		len = 0 ;
	for (int i = 0 ; prnames[i] != nullptr ; i += 1) {
	    if (cchar *prp ; (rs = prcache_lookup(&op->pc,i,&prp)) >= 0) {
	        rs = sofindpr(op,prp) ;
		len = rs ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	    if (rs >= 0) break ;
	} /* end for */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sofind::sofindprs) */

int sofind::sofindvar(SD *op) noex {
	static cchar	*sp = getenv(VARLIBPATH) ;
	int		rs = SR_NOENT ;
	cchar	*tp ;

	if (sp != nullptr) {

	while ((tp = strbrk(sp,":;")) != nullptr) {

	    if ((tp - sp) > 0) {

	        rs = sysdialer_socheckvarc(op,idp,dsp,sp,(tp - sp),
	            soname,ep) ;

	        if ((rs >= 0) || (rs == SR_NOMEM))
		    break ;

	    } /* end if (non-zero length) */

	    sp = (tp + 1) ;

	} /* end for */

	if ((rs < 0) && (rs != SR_NOMEM) && (sp[0] != '\0')) {
	    rs = sysdialer_socheckvarc(op,idp,dsp,sp,-1,soname,ep) ;
	}

	} /* end if (non-null) */

	return rs ;
}
/* end subroutine (sysdialer_sofindvar) */

static int sysdialer_socheckvarc(SD *op,ids *idp,DS *dsp,cc *ldnp,
		int ldnl,cc *soname,ent *ep) noex {
	USTAT		sb ;
	int		rs ;
	int		rs1 ;
	int		pl ;
	cchar	*pp ;
	char		pathbuf[MAXPATHLEN + 1] ;

	pp = (cchar *) pathbuf ;
	rs = pathclean(pathbuf,ldnp,ldnl) ;
	pl = rs ;
	if (rs < 0)
	    goto ret0 ;

	rs = SR_NOENT ;
	rs1 = dirseen_havename(dsp,pp,pl) ;
	if (rs1 >= 0)
	    goto ret0 ;

	rs = u_stat(pp,&sb) ;
	if ((rs >= 0) && (! S_ISDIR(sb.st_mode)))
	    rs = SR_NOTDIR ;

	if (rs >= 0) {
	    rs1 = dirseen_havedevino(dsp,&sb) ;
	    if (rs1 >= 0) {
		rs = SR_NOENT ;
		goto ret0 ;
	    }
	}

	if (rs >= 0)
	    rs = sysdialer_sochecklib(op,idp,dsp,pp,soname,ep) ;

	if ((rs < 0) && (rs != SR_NOMEM))
	     dirseen_add(dsp,pp,pl,&sb) ;

ret0:
	return rs ;
}
/* end subroutine (sysdialer_sofindvarc) */

static int sysdialer_sochecklib(SD *op,ids *idp,DS *dsp,cc *libdname,
		cc *soname,ent *ep) noex {
	USTAT		sb ;
	int		rs = SR_NOENT ;
	int		rs1 ;
	int		i ;
	int		dsize ;
	cchar	**dirs ;
	cchar	*ldnp ;
	char		subdname[MAXPATHLEN + 1] ;

	dsize = szof(caddr_t) ;
	dirs = (dsize == 8) ? de64 : de32 ;

	for (i = 0 ; dirs[i] != nullptr ; i += 1) {

	    rs1 = SR_OK ;
	    ldnp = libdname ;
	    if (dirs[i][0] != '\0') {
		ldnp = subdname ;
	        rs1 = mkpath(subdname,libdname,dirs[i]) ;
	    }

	    if (rs1 >= 0) {
		rs1 = u_stat(ldnp,&sb) ;
		if ((rs1 >= 0) && (! S_ISDIR(sb.st_mode)))
		    rs1 = SR_NOTDIR ;
	    }

	    if ((rs1 < 0) && (rs1 != SR_NOMEM)) {
		dirseen_add(dsp,ldnp,-1,&sb) ;
	    }

	    if (rs1 >= 0) {
		rs = entry_checkdir(ep,ldnp,soname) ;
	    }

	    if ((rs >= 0) || (! isNotPresent(rs))) break ;
	} /* end for (dirs) */

	return rs ;
}
/* end subroutine (sysdialer_sochecklib) */

#ifdef	COMMENT
static int sysdialer_sotest(SD *op,cc *soname) noex {
	SD_INFO	*mip ;
	cnullptr	np{} ;
	int		rs = SR_NOTFOUND ;
	if ((mip = (SD_INFO *) dlsym(op->sop,soname)) != np) {
	    if (strcmp(mip->name,soname) == 0) {
	        ep->osize = dip->osize ;
	        ep->flags = dip->flags ;
	        ep->mp->dhp = dhp ;
		rs = SR_OK ;
	    }
	}
	return rs ;
}
/* end subroutine (sysdialer_sotest) */
#endif /* COMMENT */

#if	CF_PRCACHE

static int prcache_start(prcache *pcp) noex {
	int		rs ;
	int		osize ;
	pcp->domainname = nullptr ;
	osize = (nelem(prnames) + 1) * szof(char *) ;
	if ((rs = uc_malloc(osize,&pcp->prs)) >= 0) {
	    memset(pcp->prs,0,osize) ;
	}
	return rs ;
}
/* end subroutine (prcache_start) */

static int prcache_finish(prcache *pcp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pcp->domainname != nullptr) {
	    rs1 = uc_free(pcp->domainname) ;
	    if (rs >= 0) rs = rs1 ;
	    pcp->domainname = nullptr ;
	}
	if (pcp->prs != nullptr) {
	    for (int i = 0 ; i < nelem(prnames) ; i += 1) {
		if (pcp->prs[i] != nullptr) {
		    rs1 = uc_free(pcp->prs[i]) ;
	    	    if (rs >= 0) rs = rs1 ;
		}
	    } /* end for */
	    rs1 = uc_free(pcp->prs) ;
	    if (rs >= 0) rs = rs1 ;
	    pcp->prs = nullptr ;
	}
	return rs ;
}
/* end subroutine (prcache_finish) */

static int prcache_lookup(prcache *pcp,int i,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		osize ;
	int		len = 0 ;
	char		pr[MAXPATHLEN + 1] ;

	*rpp = nullptr ;
	if (i >= (nelem(prnames) - 1))
	    return 0 ;

	if (pcp->domainname == nullptr) {
	    char	dn[MAXHOSTNAMELEN + 1] ;
	    if ((rs = getnodedomain(nullptr,dn)) >= 0) {
		if (cchar *cp ; (rs = uc_mallocstrw(dn,-1,&cp)) >= 0) {
		    pcp->domainname = cp ;
		}
	    }
	} /* end if */

	if (rs < 0)
	    goto ret0 ;

	if (pcp->prs[i] == nullptr) {

	    rs1 = mkpr(pr,MAXPATHLEN,prnames[i],pcp->domainname) ;
	    osize = 0 ;
	    if (rs1 >= 0)
		osize = rs1 ;

	    rs = uc_mallocstrw(pr,osize,(pcp->prs + i)) ;

	} /* end if */

	if ((rs >= 0) && (pcp->prs[i][0] != '\0')) {
	    *rpp = pcp->prs[i] ;
	    if ((*rpp)[0] != '\0')
		len = clenstr(*rpp) ;
	}

ret0:
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (prcache_lookup) */

#endif /* CF_PRCACHE */

static int entry_start(ent *ep,cc *name,cc *itype,mod *mp) noex {
	int		rs = SR_FAULT ;
	if (ep && name) {
	    memclear(ep) ;
	    if (cchar *cp ; (rs = uc_mallocstrw(name,-1,&cp)) >= 0) {
		ep->name = cp ;
	        if ((itype != nullptr) && (itype[0] != '\0')) {
	            if ((rs = uc_mallocstrw(itype,-1,&cp)) >= 0) {
			ep->itype = cp ;
		    }
	        }
	        if (mp != nullptr) {
	            mp->count += 1 ;
	            ep->mp = mp ;
	        }
	        ep->count = 1 ;
		if ((rs < 0) && ep->itype) {
		    uc_free(ep->itype) ;
		    ep->itype = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ent *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->name != nullptr) {
	        rs1 = uc_free(ep->name) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->name = nullptr ;
	    }
	    if (ep->itype != nullptr) {
	        rs1 = uc_free(ep->itype) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->itype = nullptr ;
	    }
	    if (ep->mp != nullptr) {
	        SD_MOD	*mp = ep->mp ;
	        if (mp->count <= 1) {
	            if ((mp->dhp != nullptr) && (mp->dhp != RTLD_DEFAULT)) {
	                dlclose(mp->dhp) ;
		    }
	            rs1 = uc_free(mp) ;
		    if (rs >= 0) rs = rs1 ;
		    ep->mp = nullptr ;
	        } else {
	            mp->count -= 1 ;
	        }
	    } /* end if */
	    memclear(ep) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

/* try to load a file with the given name */
static int entry_checkdir(ent *ep,cc *libdname,cc *name) noex {
	USTAT		sb ;
	SD_INFO	*dip ;
	vecstr		enames ;
	int		rs ;
	int		rs1 ;
	int		dlmode ;
	int		namelen ;
	int		c ;
	int		fl = 0 ;
	void		*dhp = nullptr ;
	cchar		*fn ;
	cchar		*sublibdname = SD_DNAME ;
	char		dname[MAXPATHLEN + 1] ;
	char		dlfname[MAXPATHLEN + 1] ;
	char		fname[MAXNAMELEN + 1] ;

	rs = mkpath(dname,libdname,sublibdname) ;
	if (rs < 0)
	    goto ret0 ;

/* test for directory existing */

	rs = u_stat(dname,&sb) ;

	if ((rs >= 0) && (! S_ISDIR(sb.st_mode)))
	    rs = SR_NOTDIR ;

	if (rs < 0)
	    goto ret0 ;

	rs = vecstr_start(&enames,3,0) ;
	if (rs < 0)
	    goto ret0 ;

/* read the directory looking for the prefix name parts */

	namelen = clenstr(name) ;

	rs = vecstr_loadexts(&enames,dname,name,namelen) ;
	c = rs ;
	if (rs < 0)
	    goto ret1 ;

/* load them in turn */

	rs = SR_NOTFOUND ;
	if (c == 0)
	    goto ret1 ;

	for (int i = 0 ; exts[i] != nullptr ; i += 1) {

	    if ((rs1 = vecstr_findn(&enames,exts[i],-1)) >= 0) {

	        rs = SR_INVALID ;
		fn = name ;
		if (exts[i][0] != '\0') {
		    fn = fname ;
	            mkfnamesuf1(fname,name,exts[i]) ;
		}

	        fl = mkpath(dlfname,dname,fn) ;

	        dlmode = (RTLD_LAZY | RTLD_LOCAL) ;
	        if ((dhp = dlopen(dlfname,dlmode)) != nullptr) {

	            dip = (SD_INFO *) dlsym(dhp,name) ;

	            if ((dip != nullptr) && (strcmp(dip->name,name) == 0)) {
	                ep->osize = dip->osize ;
	                ep->flags = dip->flags ;
	                ep->mp->dhp = dhp ;
	                rs = SR_OK ;
	                break ;
	            } else {
	                dlclose(dhp) ;
		    }

		} else {
		    rs = SR_LIBACC ;
	        } /* end if */

	    } /* end if (tried one) */

	} /* end for (extensions) */

ret1:
	vecstr_finish(&enames) ;

ret0:

	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (entry_checkdir) */

/* load up the available subroutines from this module */
static int entry_loadcalls(ent *ep,void *dhp) noex {
	int		rs ;
	int		rs1 ;
	int		nl ;
	if (char *nbuf ; (rs = malloc_mn(&nbuf)) >= 0) {
	    cnullptr	np{} ;
	    cint	nlen = rs ;
	    int		bl ;
	    char	*bp ;
	    nl = intconv(strwcpy(nbuf,ep->name,nlen) - nbuf) ;
	    bp = charp(nbuf + nl) ;
	    bl = (nlen - nl) ;
	    for (int i = 0 ; (rs >= 0) && subs[i] ; i += 1) {
	        if ((rs = sncpy(bp,bl,"_",subs[i])) >= 0) {
		    int		(*fp)() ;
	            if ((fp = (int (*)()) dlsym(dhp,nbuf)) != np) {
	                switch (i) {
	                case sub_open:
	                    ep->c.open = fp ;
	                    break ;
	                case sub_reade:
	                    ep->c.reade = fp ;
	                    break ;
	                case sub_recve:
	                    ep->c.recve = fp ;
	                    break ;
	                case sub_recvfrome:
	                    ep->c.recvfrome = fp ;
	                    break ;
	                case sub_recvmsge:
	                    ep->c.recvmsge = fp ;
	                    break ;
	                case sub_write:
	                    ep->c.write = fp ;
	                    break ;
	                case sub_send:
	                    ep->c.send = fp ;
	                    break ;
	                case sub_sendto:
	                    ep->c.sendto = fp ;
	                    break ;
	                case sub_sendmsg:
	                    ep->c.sendmsg = fp ;
	                    break ;
	                case sub_shutdown:
	                    ep->c.shutdown = fp ;
	                    break ;
	                case sub_close:
	                    ep->c.close = fp ;
	                    break ;
	                } /* end switch */
	            } /* end if (got a symbol) */
	        } /* end if (sncpy) */
	    } /* end for */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	if ((rs >= 0) && (ep->c.open == nullptr)) {
	    rs = SR_LIBACC ;
	}
	return rs ;
}
/* end subroutine (entry_loadcalls) */

static int entry_hasname(ent *ep,void *dhp,cc *name) noex {
	SD_INFO		*dip = (SD_INFO *) dlsym(dhp,name) ;
	int		rs = SR_NOTFOUND ;
	if ((dip != nullptr) && (strcmp(dip->name,name) == 0)) {
	    ep->osize = dip->osize ;
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (entry_hasname) */

static int vecstr_loadexts(vecstr *lp,cc *dname,cc *name,int namelen) noex {
	int		rs ;
	int		rs1 ;
	int		nl ;
	int		c = 0 ;
	if ((cchar *nbuf ; (rs = malloc_mp(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    if (fsdir dir ; (rs = fsdir_open(&dir,dname)) >= 0) {
	        fsdir_ent	slot ;
	        while ((dnl = fsdir_read(&dir,&slot,nbuf,nlen)) > 0) {
		    fext	e ;
		    cint	dnl = rs ;
	            cchar	*dnp = slot.name ;
	            if (dnl < namelen) continue ;
		    nl = getext(&e,dnp,dnl) ;
	            if (nl != namelen) continue ;
 		    if (strncmp(dnp,name,namelen) != 0) continue ;
	            if ((e.exl == 0) || (matstr(exts,e.exp,e.exl) >= 0)) {
	                c += 1 ;
	                if ((rs = vecstr_add(lp,e.exp,e.exl)) >= 0) {
	                    if (c >= NEXTS) break ;
			}
	            } /* end if (got a match) */
		    if (rs < 0) break ;
	        } /* end while (directory entries) */
	        rs1 = fsdir_close(&dir) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_loadexts) */

/* compare the whole entries (including the netgroup) */
static int vcmpname(cvoid **v1pp,cvoid **v2pp) noex {
	ent		**e1pp = (ent **) v1pp ;
	ent		**e2pp = (ent **) v2pp ;
	int		rc = 0 ;
	{
	    ent		*e1p = *e1pp ;
	    ent		*e2p = *e2pp ;
	    if (e1p || e2p) {
	        rc = +1 ;
	        if (e1p) {
		    rc = -1 ;
	            if (e2p) {
		        rc = strcmp(e1p->name,e2p->name) ;
		    }
	        }
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpname) */

static int getext(fext *ep,cchar *namep,int namel) noex {
	int		mnl = cstrnlen(namep,namel) ;
	ep->exp = (namep + mnl) ;
	ep->exl = 0 ;
	if (cchar *tp ; (tp = strnrchr(namep,mnl,'.')) != nullptr) {
	    ep->exp = (tp + 1) ;
	    ep->exl = intconv(namep + mnl - (tp + 1)) ;
	    mnl = intconv(tp - namep) ;
	}
	return mnl ;
}
/* end subroutine (getext) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	}
	return rs ;
}
/* end method (vars::operator) */

static bool isNotConn(int rs) noex ; {
    	return isOneOf(rsnotconnect,rs) ;
}


