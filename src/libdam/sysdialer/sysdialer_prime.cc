/* sysdialer_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system-dialer storage object */
/* version %I% last-modified %G% */

#define	CF_SAMEMODULE	0		/* same-module */

/* revision history:

	- 2003-11-04, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sysdialer

	Description:
	This object manages what system-dialers (SYSDIALER) have
	been loaded so far.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |getustime(3u)| */
#include	<uclibmem.h>
#include	<getbufsize.h>
#include	<getnodedomain.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<fsdir.h>
#include	<ids.h>
#include	<dirseen.h>
#include	<strn.h>		/* |strnrchr(3uc)| */
#include	<strx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<mkfname.h>
#include	<pathclean.h>
#include	<isoneof.h>
#include	<ischarx.h>
#include	<isnot.h>		/* |isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"sysdialer.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |varname(3u)| */

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

#ifndef	CF_SAMEMODULE
#define	CF_SAMEMODULE	0		/* same-module */
#endif


/* imported namespaces */

using sysdialer_util::prcache_start ;	/* subroutine */
using sysdialer_util::prcache_lookup ;	/* subroutine */
using sysdialer_util::prcache_finish ;	/* subroutine */
using sysdialer_util::entry_start ;	/* subroutine */
using sysdialer_util::entry_checkdir ;	/* subroutine */
using sysdialer_util::entry_loadcalls ;	/* subroutine */
using sysdialer_util::entry_hasname ;	/* subroutine */
using sysdialer_util::entry_finish ;	/* subroutine */
using sysdialer_util::var ;		/* variable */
using sysdialer_util::exts ;		/* variable */
using sysdialer_util::prnames ;		/* variable */


/* local typedefs */

typedef SD_ENT		ent ;
typedef SD_MOD		mod ;
typedef SD_PRC		prcache ;
typedef SD_ENT *	entp ;
typedef sysdialer_fext	fext ;


/* external subroutines */


/* external variables */
 

/* local structures */


/* forward references */

template<typename ... Args>
local int sysdialer_ctor(SD *op,Args ... args) noex {
    	SYSDIALER	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->elp = new(nt) vecobj) != np) ylikely {
	        if ((op->plp = new(nt) vecstr) != np) ylikely {
	            if ((op->dlp = new(nt) vecstr) != np) ylikely {
			rs = SR_OK ;
		    } /* end if (new-vecstr) */
		    if (rs < 0) {
		        delete op->plp ;
		        op->plp = np ;
		    } /* end if (error) */
		} /* end if (new-vecstr) */
		if (rs < 0) {
		    delete op->elp ;
		    op->elp = np ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sysdialer_ctor) */

local int sysdialer_dtor(SD *op) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->dlp) ylikely {
		delete op->dlp ;
		op->dlp = np ;
	    }
	    if (op->plp) ylikely {
		delete op->plp ;
		op->plp = np ;
	    }
	    if (op->elp) ylikely {
		delete op->elp ;
		op->elp = np ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (sysdialer_dtor) */

template<typename ... Args>
local inline int sysdialer_magic(SD *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == SYSDIALER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (sysdialer_magic) */

local int sysdialer_sofind(SD *,cc *,cc *,ent *) noex ;
local int sysdialer_socheckvarc(SD *,ids *,DS *,cc *,int, cc *,ent *) noex ;
local int sysdialer_sochecklib(SD *,ids *,DS *,cc *,cc *,ent *) noex ;

#ifdef	COMMENT
local int	sysdialer_sotest(SD *,cchar *) noex ;
#endif

extern "C" {
    local int	vcmpname(cvoid **,cvoid **) noex ;
}

local bool	isNotConn(int) noex ;


/* local variables */

constexpr cpcchar	de64[] = {
	"sparcv9",
	"sparc",
	"",
	nullptr
} ; /* end array */

constexpr cpcchar	de32[] = {
	"sparcv8",
	"sparcv7",
	"sparc",
	"",
	nullptr
} ; /* end array */

constexpr cpcchar	prdirs[] = {
	"syssysdialer",
	"sysdialers",
	nullptr
} ; /* end array (prdirs) */

constexpr int		rsnotconnect[] = {
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
} ; /* end array */


/* exported variables */

constexpr sysdialerms	sysdialerm ;


/* exported subroutines */

local int sysdialer_starter(SD *,mainv,mainv) noex ;

int sysdialer_start(SD *op,cchar *pr,mainv prs,mainv dirs) noex {
	int		rs ;
	if ((rs = sysdialer_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) ylikely {
	            if (cchar *cp ; (rs = lm_strw(pr,-1,&cp)) >= 0) {
	                op->pr = cp ;
		        rs = sysdialer_starter(op,prs,dirs) ;
			if ((rs < 0) && op->pr) {
			    void *vp = voidp(op->pr) ;
			    lm_free(vp) ;
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

local int sysdialer_startprs(SD *,mainv) noex ;
local int sysdialer_startdirs(SD *,mainv) noex ;
local int sysdialer_startents(SD *) noex ;

local int sysdialer_starter(SD *op,mainv prs,mainv dirs) noex {
    	int		rs ;
	if ((rs = sysdialer_startprs(op,prs)) >= 0) {
	    if ((rs = sysdialer_startdirs(op,dirs)) >= 0) {
		rs = sysdialer_startents(op) ;
	    }
            if (rs < 0) {
                sysdialer_finish(op) ;
            }
	}
	return rs ;
}
/* end subroutine (sysdialer_starter) */

local int sysdialer_startprs(SD *op,mainv prs) noex {
    	int		rs = SR_OK ;
        if (prs) {
            if (vecstr *plp = op->plp ; (rs = plp->start(5,0)) >= 0) {
                op->fl.vsprs = true ;
                for (int i = 0 ; prs[i] != nullptr ; i += 1) {
		    cchar *pr = prs[i] ;
                    if (strcmp(pr,op->pr) != 0) {
                        rs = plp->add(pr,-1) ;
                    }
                    if (rs < 0) break ;
                } /* end for */
                if (rs < 0) {
                    if (op->fl.vsprs) {
                        plp->finish() ;
                        op->fl.vsprs = false ;
                    }
                } /* end if (error) */
            } /* end if (vecstr_start) */
        } /* end if (had program roots) */
	return rs ;
}
/* end subroutine (sysdialer_startprs) */

local int sysdialer_startdirs(SD *op,mainv dirs) noex {
    	int		rs = SR_OK ;
        if (dirs) {
	    cint	vn = 10 ;
	    cint	vo = 0 ;
            if (vecstr *dlp = op->dlp ; (rs = dlp->start(vn,vo)) >= 0) {
                op->fl.vsdirs = true ;
                for (int i = 0 ; (rs >= 0) && dirs[i] ; i += 1) {
                    rs = dlp->add(dirs[i]) ;
                    if (rs < 0) break ;
                } /* end for */
                if (rs >= 0) {
                    if (mainv dp ; (rs = dlp->getvec(&dp)) >= 0) {
                        op->dirs = dp ;
                    }
                } /* end if (ok) */
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
/* end subroutine (sysdialer_startdirs) */

local int sysdialer_startents(SD *op) noex {
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
            } /* end if (error) */
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
	    if (op->pr) {
		void *vp = voidp(op->pr) ;
		rs1 = lm_free(vp) ;
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
    	cnullptr	np{} ;
    	cint		rsn = SR_NOENT ;
	int		rs ;
	if ((rs = sysdialer_magic(op,name,depp)) >= 0) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		vecobj	*elp = op->elp ;
	        ent	se{} ;
	        se.name = name ;
		void *vp ;
	        if ((rs = elp->search(&se,vcmpname,&vp)) == rsn) {
		    if (ent e ; (rs = entry_start(&e,name,np)) >= 0) {
			cchar *pr = op->pr ;
			cauto sd_find = sysdialer_sofind ;
			if ((rs = sd_find(op,pr,name,&e)) >= 0) {
			    void	*dhp = e.mp->dhp ;
			    if ((rs = entry_loadcalls(&e,dhp)) >= 0) {
				if ((rs = elp->add(&e)) >= 0) {
				    cint i = rs ;
				    if (depp) {
					elp->get(i,&vp) ;
					*depp = entp(vp) ;
				    }
				} /* end if (ok) */
			    } /* end if (entry_loadcalls) */
			} /* end if (sysdialer_sofind) */
			if (rs < 0) {
			    entry_finish(&e) ;
			}
		    } /* end if (entry_start) */
	        } else if (rs >= 0) {
		    *depp = entp(vp) ;
	            (*depp)->count += 1 ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
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
	        if (void *vp ; (rs = elp->search(&te,vcmpname,&vp)) >= 0) {
	            cint	ei = rs ;
	            ent		*dep = entp(vp) ;
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
	int		llen ;
	sofind(cc *aso,ent *aep) noex : so(aso), ep(aep) { } ;
	int operator () (SD *,cchar *) noex ;
	int socheck(SD *,ustat *) noex ;
	int sofindpr(SD *,cc *) noex ;
	int sofindprs(SD *) noex ;
	int sofindvar(SD *) noex ;
    } ; /* end struct (sofind) */
} /* end namespace */

local int sysdialer_sofind(SD *op,cc *pr,cc *so,ent *ep) noex {
    	sofind sf(so,ep) ;
	return sf(op,pr) ;
} /* end subruotine (sysdialer_sofind) */

int sofind::operator () (SD *op,cchar *pr) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = ids_load(&id)) >= 0) {
	    if ((rs = ds.start) >= 0) {
    		cint	maxpath = var.maxpathlen ;
		cint	asz = ((var.maxpathlen + 1) * 2) ;
		int	ai = 0 ;
		llen = var.maxpathlen ;
	        if (char *a ; (rs = lm_mall(asz,&a)) >= 0) {
	            lbuf = (a + ((maxpath + 1) * ai++)) ;
	            pbuf = (a + ((maxpath + 1) * ai++)) ;
		    {
	                if ((rs = sofindpr(op,pr)) == 0) {
	                    if ((rs = sofindprs(op)) == 0) {
	                        rs = sofindvar(op) ;
		            } /* end if (sofindvar) */
		        } /* end if (sofindprs) */
		        rv = rs ;
		    } /* end block */
		    rs1 = lm_free(a) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	        rs1 = ds.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (dirseen) */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (sofind::operator) */

int sofind::sofindpr(SD *op,cc *pr) noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = mkpath(lbuf,pr,LIBCNAME)) >= 0) {
	    len = rs ;
	    if ((rs = ds.havename(lbuf,len)) >= 0) {
		if (ustat sb ; (rs = u_stat(lbuf,&sb)) >= 0) {
		    if (S_ISDIR(sb.st_mode)) {
			if ((rs = ds.havedevino(&sb)) >= 0) {
			    rs = socheck(op,&sb) ;
			    len = rs ;
			} else if (isNotConn(rs)) {
			    rs = SR_OK ;
			    len = 0 ;
			}
		    } else {
			rs = SR_OK ;
			len = 0 ;
		    }
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		    len = 0 ;
		}
	    } else if (isNotConn(rs)) {
		rs = SR_OK ;
		len = 0 ;
	    }
	} /* end if (mkpath) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (sysdialer::sofindpr) */

int sofind::sofindprs(SD *op) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	for (int i = 0 ; prnames[i] ; i += 1) {
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
	static cchar	*sp = getenv(varname.libpath) ;
	cnullptr	np{} ;
	int		rs = SR_NOENT ;
	if (sp) {
	    cauto sd_socheck = sysdialer_socheckvarc ;
	    for (cchar *tp ; (tp = strbrk(sp,":;")) != np ; ) {
		if (cint tl = intconv(tp - sp) ; tl > 0) {
	             if ((rs = sd_socheck(op,&id,&ds,sp,tl,so,ep)) >= 0) {
			 break ;
		     } else if (rs == SR_NOMEM) {
		         break ;
		     }
		} /* end if (non-zero positive) */
	        sp = (tp + 1) ;
		if (rs < 0) break ;
	    } /* end for */
	    if ((rs < 0) && (rs != SR_NOMEM) && (sp[0] != '\0')) {
	        rs = sysdialer_socheckvarc(op,&id,&ds,sp,-1,so,ep) ;
	    } /* end if (erro) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sofind::sofindvar) */

int sofind::socheck(SD *op,ustat *sbp) noex {
    	int		rs ;
	int		len = 0 ; /* return-value */
	if ((rs = sysdialer_sochecklib(op,&id,&ds,lbuf,so,ep)) > 0) {
	    len = rs ;
	} else if (rs == 0) {
	    if ((rs = pathclean(pbuf,lbuf,-1)) >= 0) {
	        rs = ds.add(pbuf,rs,sbp) ;
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subriutine (sofind::socheck) */

local int sysdialer_socheckvarc(SD *op,ids *idp,DS *dsp,cc *ldnp,
		int ldnl,cc *soname,ent *ep) noex {
    	cint		rsn = SR_NOENT ;
	int		rs ;
	int		rs1 ;
	if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) {
	    if ((rs = pathclean(pbuf,ldnp,ldnl)) >= 0) {
		cchar	*pp = pbuf ;
		cint	pl = rs ;
		if ((rs = dsp->havename(pp,pl)) == rsn) {
		    if (ustat sb ; (rs = u_stat(pp,&sb)) >= 0) {
			rs = SR_NOTDIR ;
			if (S_ISDIR(sb.st_mode)) {
	    		    if ((rs = dsp->havedevino(&sb)) == rsn) {
			    	cc *so = soname ;
			    	cauto sd_sock = sysdialer_sochecklib ;
	    			rs = sd_sock(op,idp,dsp,pp,so,ep) ;
				if (rs == rsn) {
	     			    dsp->add(pp,pl,&sb) ;
				}
			    } /* end if (dirseen_havedevino) */
		        } /* end if (is-directory) */
		    } /* end if (u_stat) */
		} /* end if (dirseen_havename) */
	    } /* end if (pathclean) */
	    rs1 = lm_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (sysdialer_socheckvarc) */

local int sysdialer_sochecklib(SD *op,ids *idp,DS *dsp,cc *libdname,
		cc *soname,ent *ep) noex {
	cint		asz = szof(caddr_t) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	(void) op ;
	(void) idp ;
	if (char *subdname ; (rs = lm_mp(&subdname)) >= 0) {
	    cpcchar	*dirs = (asz == 8) ? de64 : de32 ;
	    for (int i = 0 ; (rs >= 0) && (len == 0) && dirs[i] ; i += 1) {
	        cchar *ldnp = libdname ;
	        if (dirs[i][0] != '\0') {
		    ldnp = subdname ;
	            rs = mkpath(subdname,libdname,dirs[i]) ;
	        }
	        if (rs >= 0) {
		    if (ustat sb ; (rs = u_stat(ldnp,&sb)) >= 0) {
		        if (S_ISDIR(sb.st_mode)) {
		    	    if ((rs = dsp->add(ldnp,-1,&sb)) >= 0) {
		    		rs = entry_checkdir(ep,ldnp,soname) ;
		    		len = rs ;
			    }
			} else {
		            rs = SR_NOTDIR ;
			}
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
			len = 0 ;
		    }
	        } /* end if (ok) */
	    } /* end for (dirs) */
	    rs1 = lm_free(subdname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (sysdialer_sochecklib) */

#ifdef	COMMENT
local int sysdialer_sotest(SD *op,cc *soname) noex {
	SD_INFO	*mip ;
	cnullptr	np{} ;
	int		rs = SR_NOTFOUND ;
	if ((mip = (SD_INFO *) dlsym(op->sop,soname)) != np) {
	    if (strcmp(mip->name,soname) == 0) {
	        ep->osz = dip->osz ;
	        ep->flags = dip->flags ;
	        ep->mp->dhp = dhp ;
		rs = SR_OK ;
	    }
	} /* end if (dlsym) */
	return rs ;
}
/* end subroutine (sysdialer_sotest) */
#endif /* COMMENT */

/* compare the whole entries (including the netgroup) */
local int vcmpname(cvoid **v1pp,cvoid **v2pp) noex {
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

local bool isNotConn(int rs) noex {
    	return isOneOf(rsnotconnect,rs) ;
} /* end subroutine (isNotConn) */


/* comments */

#if	CF_SAMEMODULE
#ifdef	COMMENT
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
#endif /* COMMENT */
#endif /* CF_SAMEMODULE */


