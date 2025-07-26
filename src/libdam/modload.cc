/* modload SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* module-load management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	object:
	modload

	Description:
	This object module implements an interface (a trivial one) that
	attempts to load another named module.

	Synopsis:
	int modload_open(ML *op,cc *pr,cc *fn,cc *mn,int opts,mv syms) noex

	Arguments:
	op		object pointer
	pr		program-root
	modfn		module file-name
	modname		module name (the name inside the so-file itself)
	opts		options
	syms		array of symbols to check for existence

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strcmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<usysflag.h>
#include	<getbufsize.h>
#include	<getnodedomain.h>	/* |getnodedomain(3uc)| */
#include	<mallocxx.h>
#include	<ids.h>
#include	<dirseen.h>
#include	<snx.h>
#include	<pathclean.h>
#include	<mkpathx.h>
#include	<mkx.h>			/* |mksofname(3uc)| */
#include	<mkpr.h>
#include	<permx.h>
#include	<strx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"modload.h"

import libutil ;

/* local defines */

#define	ML		modload
#define	ML_MI		modload_mi

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	LIBCNAME	"lib"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
#endif
#ifndef	VARDYLIBPATH
#define	VARDYLIBPATH	"DYLD_LIBRARY_PATH"
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		id:1 ;
    	uint		libvar:1 ;
    	uint		libprs:1 ;
    	uint		libsdirs:1 ;
	uint		liball:1 ;
    	uint		avail:1 ;
    	uint		self:1 ; 
} ;

struct subinfo {
	modload		*mlp ;
	cchar		*pr ;
	cchar		*modfn ;	/* module-file-name */
	mainv		syms ;
	ids		id ;
	SI_FL		fl = {} ;
	int		opts ;
	subinfo(modload *aop,cc *apr,cc *afn,int aso,mv asyms) noex {
	    mlp = aop ;
	    pr = apr ;
	    modfn = afn ;
	    opts = aso ;
	    syms = asyms ;
	} ;
	int start() noex ;
	int finish(int f_abort) noex ;
	int setopts() noex ;
	int objload() noex ;
	int objloadbegin() noex ;
	int objloadend() noex ;
	int sofind() noex ;
	int sofindprs(dirseen *dsp,int dlm) noex ;
	int sofindroot(dirseen *dsp,int dlm,cchar *) noex ;
	int sofindsdirs(dirseen *dsp,int dlm) noex ;
	int sofindvar(dirseen *dsp,int dlm) noex ;
	int socheckvarc(dirseen *dsp,cc *ldnp,int ldnl,int dlm) noex ;
	int sochecklib(dirseen *dsp,cchar *ldname,int dlm) noex ;
	int sockliber(dirseen *dsp,cchar *ldnp,int dlm) noex ;
	int sotest(char *tbuf,int tlen) noex ;
	int checksyms() noex ;
} ;

struct vars {
	int		maxpathlen ;
	int		maxhostlen ;
	operator int () noex ;
} ;


/* forward references */

template<typename ... Args>
static int modload_ctor(modload *op,Args ... args) noex {
    	MODLOAD		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (modload_ctor) */

static int modload_dtor(modload *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (modload_dtor) */

template<typename ... Args>
static inline int modload_magic(modload *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MODLOAD_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (modload_magic) */

static int	modload_objloadclose(ML *) noex ;


/* local variables */

constexpr cpcchar	prnames[] = {
	"LOCAL",
	"NCMP",
	"PCS",
	"EXTRA",
	"PREROOT",
	nullptr
} ;

constexpr cpcchar	sysprs[] = {
	"/usr/extra",
	"/usr/preroot",
	nullptr
} ;

constexpr cpcchar	extdirs[] = {
	"sparcv9",
	"sparcv8",
	"sparcv7",
	"sparc",
	"",
	nullptr
} ;

constexpr cpcchar	exts[] = {
	"so",
	"o",
	"dylib",	/* for ("think different") Apple Darwin */
	"",
	nullptr
} ;

static vars		var ;

constexpr cchar		symsuf[] = MODLOAD_SYMSUF ;

constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */

cint	modloadms::libvar	= (1 << modloado_libvar) ;
cint	modloadms::libprs	= (1 << modloado_libprs) ;
cint	modloadms::libsdirs	= (1 << modloado_libsdirs) ;
cint	modloadms::avail	= (1 << modloado_avail) ;
cint	modloadms::self		= (1 << modloado_self) ;

const modloadms		modloadm ;


/* exported subroutines */

int modload_open(ML *op,cc *pr,cc *modfn,cc *modname,int opts,mv syms) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = modload_ctor(op,modfn,modname)) >= 0) {
	    rs = SR_INVALID ;
	    if (modfn[0] && modname[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
	            subinfo si(op,pr,modfn,opts,syms) ;
	            op->modname = modname ;
	            if ((rs = si.start()) >= 0) {
	                if ((rs = si.objload()) >= 0) {
	                    op->magic = MODLOAD_MAGIC ;
	                }
	                rs1 = si.finish((rs < 0)) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if */
		} /* end if (mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		modload_dtor(op) ;
	    }
	} /* end if (modload_ctor) */
	return rs ;
}
/* end subroutine (modload_open) */

int modload_close(ML *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = modload_magic(op)) >= 0) {
	    {
		rs1 = modload_objloadclose(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = modload_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (modload_close) */

int modload_getsym(ML *op,cchar *symname,cvoid **vpp) noex {
	int		rs ;
	cvoid		*rp = nullptr ;
	if ((rs = modload_magic(op,symname)) >= 0) {
	    rs = SR_INVALID ;
	    if (symname[0]) {
		rs = SR_NOTFOUND ;
	        if (op->sop) {
	            if ((rp = dlsym(op->sop,symname)) != nullptr) {
	                rs = SR_OK ;
	            }
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	if (vpp) {
	    *vpp = (rs >= 0) ? rp : nullptr ;
	}
	return rs ;
}
/* end subroutine (modload_getsym) */

int modload_getmv(ML *op,int vi) noex {
	int		rs ;
	int		v = 0 ;
	if ((rs = modload_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (vi >= 0) {
	        if (op->midp != nullptr) {
	            ML_MI	*mip = op->midp ;
		    rs = SR_OK ;
	            v = mip->mv[vi] ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (modload_getmv) */

int modload_getmva(ML *op,int *mva,int mvn) noex {
	int		rs ;
	int		v = 0 ;
	if ((rs = modload_magic(op,mva)) >= 0) {
	    if (op->midp) {
	        modload_mi	*mip = op->midp ;
	        v = mip->mv[0] ;
	        for (int i = 0 ; i < mvn ; i += 1) {
	            mva[i] = mip->mv[i] ;
	        }
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (modload_getmva) */


/* private subroutines */

static int modload_objloadclose(ML *op) noex {
	if (op->sop != nullptr) {
	    if ((op->sop != RTLD_DEFAULT) && (op->sop != RTLD_SELF)) {
	        dlclose(op->sop) ;
	    }
	    op->sop = nullptr ;
	}
	return SR_OK ;
}
/* end subroutine (modload_objloadclose) */

int subinfo::start() noex {
	int		rs ;
	if ((rs = setopts()) >= 0) {
	    rs = id.load ;
	} /* end if (subinfo::setopts) */
	return rs ;
}
/* end subroutine (subinfo::start) */

int subinfo::finish(int f_abort) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (f_abort) {
	    rs1 = modload_objloadclose(mlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = id.release ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

int subinfo::setopts() noex {
    	const modloadms	sdm ;
	cint		so = opts ;
    	int		rs = SR_OK ;
	bool		fall = true ;
	if (so & sdm.libvar)	{
	    fl.libvar = true ;
	    fall = false ;
	}
	if (so & sdm.libprs)	{
	    fl.libprs = true ;
	    fall = false ;
	}
	if (so & sdm.libsdirs)	{
	    fl.libsdirs = true ;
	    fall = false ;
	}
	if (so & sdm.avail)	{
	    fl.avail = true ;
	}
	if (so & sdm.self)	{
	    fl.self = true ;
	}
	if (fall) {
	    fl.liball = true ;
	}
	return rs ;
}
/* end subroutine (subinfo::setopts) */

int subinfo::objload() noex {
	int		rs ;
	if ((rs = objloadbegin()) >= 0) {
	    rs = checksyms() ;
	    if (rs < 0) {
	        objloadend() ;
	    }
	} /* end if (objload-begin) */
	return rs ;
}
/* end subroutine (subinfo::objload) */

int subinfo::objloadbegin() noex {
    	int		rs ;
	if ((rs = sofind()) == 0) {
	    rs = SR_NOTFOUND ;
	}
	return rs ;
}
/* end subroutine (subinfo::objloadbegin) */

int subinfo::objloadend() noex {
	return modload_objloadclose(mlp) ;
}
/* end subroutine (subinfo::objloadend) */

int subinfo::sofind() noex {
	int		dlm = RTLD_LAZY ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	dlm |= (opts & MODLOAD_OAVAIL) ? RTLD_GLOBAL : RTLD_LOCAL ;
	if (dirseen ds ; (rs = dirseen_start(&ds)) >= 0) {
	    rs = 0 ;
	    if (pr) {
	        if ((rs = sofindroot(&ds,dlm,pr)) > 0) {
		    len = rs ;
		}
	    }
	    if ((rs == 0) && (len == 0)) {
	        if ((rs = sofindvar(&ds,dlm)) > 0) {
		    len = rs ;
		} else if (rs == 0) {
		    if ((rs = sofindprs(&ds,dlm)) > 0) {
		        len = rs ;
		    } else if (rs == 0) {
	                if ((rs = sofindsdirs(&ds,dlm)) > 0) {
			    len = rs ;
			}
		    }
		}
	    }
	    rs1 = dirseen_finish(&ds) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dirseen) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo::sofind) */

int subinfo::sofindroot(dirseen *dsp,int dlm,cchar *apr) noex {
	cint		maxpath = var.maxpathlen ;
	cint		sz = ((var.maxpathlen + 1) * 2) ;
	int		rs ;
	int		rs1 ;
	int		ai = 0 ;
	int		len = 0 ;
	if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
	    char	*lbuf = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = mkpath(lbuf,apr,LIBCNAME)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
		len = rs ;
	        if ((rs = dirseen_havename(dsp,lbuf,-1)) == rsn) {
	            if (USTAT sb ; (rs = u_stat(lbuf,&sb)) >= 0) {
		        if (S_ISDIR(sb.st_mode)) {
	    	            if ((rs = dirseen_havedevino(dsp,&sb)) == rsn) {
	    		        if ((rs = sochecklib(dsp,lbuf,dlm)) >= 0) {
				    rs = SR_OK ; /* ok */
				} else if (isNotPresent(rs)) {
	        		    char *pbuf = (a + ((maxpath + 1) * ai++)) ;
				    len = 0 ;
	    		            if ((rs = pathclean(pbuf,lbuf,-1)) >= 0) {
	        		        rs = dirseen_add(dsp,pbuf,rs,&sb) ;
				    }
			        }
			    } else if (rs >= 0) {
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
	            } /* end if (stat) */
	        } else if (rs >= 0) {
	            rs = SR_OK ;
		    len = 0 ;
	        }
	    } /* end if (mkpath) */
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo::sofindpr) */

int subinfo::sofindvar(dirseen *dsp,int dlm) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*vn{} ;
	if_constexpr (f_darwin) {
	    vn = VARDYLIBPATH ;
	} else {
	    vn = VARLIBPATH ;
	}
	if ((fl.liball || fl.libvar) && vn[0]) {
	    static cchar	*sp = getenv(vn) ;
	    if (sp) {
	        int	sl{} ;
	        for (cchar *tp ; (tp = strbrk(sp,":;")) != nullptr ; ) {
	            sl = intconv(tp - sp) ;
	            if (sl > 0) {
	                if ((rs = socheckvarc(dsp,sp,sl,dlm)) > 0) {
			    len = rs ;
			}
	            } /* end if (non-zero length) */
	            sp = (tp + 1) ;
	            if ((rs < 0) || (len > 0)) break ;
	        } /* end for */
		if ((rs >= 0) && (len == 0)) {
	            if (sp[0] != '\0') {
	                if ((rs = socheckvarc(dsp,sp,-1,dlm)) > 0) {
			    len = rs ;
			}
		    }
	        }
	    } /* end if (getenv) */
	} /* end if (selected && valid) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo::sofindvar) */

int subinfo::sofindprs(dirseen *dsp,int dlm) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (fl.liball || fl.libprs) {
	    cint	maxhost = var.maxhostlen ;
	    cint	sz = ((var.maxhostlen + 1) + (var.maxpathlen + 1)) ;
	    int		ai = 0 ;
	    if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
		cint	dl = var.maxhostlen ;
	        char	*dn = (a + ((maxhost + 1) * ai++)) ;
	        if ((rs = getinetdomain(dn,dl)) >= 0) {
	            cint	prlen = var.maxpathlen ;
	            char	*prbuf = (a + ((maxhost + 1) * ai++)) ;
	            for (int i = 0 ; prnames[i] != nullptr ; i += 1) {
	                if ((rs = mkpr(prbuf,prlen,prnames[i],dn)) >= 0) {
	                    if ((rs = sofindroot(dsp,dlm,prbuf)) > 0) {
			        len = rs ;
			    }
	                }
	                if ((rs < 0) || (len > 0)) break ;
	            } /* end for */
	        } /* end if (getnodedomain) */
	        rs1 = uc_free(a) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (selected) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo::sofindprs) */

int subinfo::sofindsdirs(dirseen *dsp,int dlm) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (fl.liball || fl.libsdirs) {
	    for (int i = 0 ; sysprs[i] != nullptr ; i += 1) {
	        cchar	*dirname = sysprs[i] ;
	        if ((rs = sofindroot(dsp,dlm,dirname)) > 0) {
		    len = rs ;
		}
	        if ((rs < 0) || (len > 0)) break ;
	    } /* end for */
	} /* end if (selected) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo::sofindsdirs) */

int subinfo::socheckvarc(dirseen *dsp,cc *ldnp,int ldnl,int dlm) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = pathclean(tbuf,ldnp,ldnl)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        cint	pl = rs ;
	        cchar	*pp = cast_const<cchar *>(tbuf) ;
		len = rs ;
	        if ((rs = dirseen_havename(dsp,pp,pl)) == rsn) {
	            if (USTAT sb ; (rs = u_stat(pp,&sb)) >= 0) {
	                if (S_ISDIR(sb.st_mode)) {
	                    if ((rs = dirseen_havedevino(dsp,&sb)) == rsn) {
	                        if ((rs = sochecklib(dsp,pp,dlm)) >= 0) {
				    rs = SR_OK ; /* ok */
				} else if (isNotPresent(rs)) {
				    len = 0 ;
	                            rs = dirseen_add(dsp,pp,pl,&sb) ;
	                        }
	                    } else if (rs >= 0) {
	                        rs = SR_OK ;
				len = 0 ;
	                    }
	                } else {
	                    rs = SR_OK ;
			    len = 0 ;
	                }
		    } else if (isNotPresent(rs)) {
			rs = 0 ;
			len = 0 ;
	            } /* end if (stat) */
	        } else if (rs >= 0) {
	            rs = SR_OK ;
		    len = 0 ;
	        }
	    } /* end if (pathclean) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo::socheckvarc) */

int subinfo::sochecklib(dirseen *dsp,cchar *ldname,int dlm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dsp) {
	    if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	        cint	am = (R_OK | X_OK) ;
	        cchar	*ldnp ;
	        mlp->sop = nullptr ;
	        for (int i = 0 ; extdirs[i] != nullptr ; i += 1) {
	            ldnp = ldname ;
	            if (extdirs[i][0] != '\0') {
	                ldnp = tbuf ;
	                rs = mkpath(tbuf,ldname,extdirs[i]) ;
	            }
	            if (rs >= 0) {
	                if (USTAT sb ; (rs = u_stat(ldnp,&sb)) >= 0) {
	                    if (S_ISDIR(sb.st_mode)) {
			        ids	*idp = &id ;
			        if ((rs = permid(idp,&sb,am)) >= 0) {
		   	            rs = sockliber(dsp,ldnp,dlm) ;
			        } else if (isNotPresent(rs)) {
	    		            rs = dirseen_add(dsp,ldnp,-1,&sb) ;
			        }
			    } /* end if (is dir) */
		        } else if (isNotPresent(rs)) {
		            rs = SR_OK ;
		        }
	            } /* end if (ok) */
	            if (mlp->sop != nullptr) break ;
	            if (rs < 0) break ;
	        } /* end for (extdirs) */
		rs1 = uc_free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    if ((rs >= 0) && (mlp->sop == nullptr)) {
		rs = SR_LIBACC ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo::sochecklib) */

int subinfo::sockliber(dirseen *dsp,cchar *ldnp,int dlm) noex {
	int		rs ;
	int		rs1 ;
	if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	    ids		*idp = &id ;
	    cint	tlen = rs ;
	    cint	am = (X_OK | R_OK) ;
	    cchar	*mfn = modfn ;
	    (void) dsp ;
	    for (int j = 0 ; exts[j] != nullptr ; j += 1) {
	        if ((rs = mksofname(tbuf,ldnp,mfn,exts[j])) >= 0) {
	            if (USTAT sb ; (rs = u_stat(tbuf,&sb)) >= 0) {
		        if (S_ISREG(sb.st_mode)) {
			    if ((rs = permid(idp,&sb,am)) >= 0) {
			        cnullptr	np{} ;
				void		*sop ;
			        if ((sop = dlopen(tbuf,dlm)) != np) {
				    mlp->sop = sop ;
				    rs = sotest(tbuf,tlen) ;
				    if (rs < 0) {
				        dlclose(sop) ;
				        mlp->sop = nullptr ;
				        if (isNotPresent(rs)) {
					    rs = SR_OK ;
				        }
				    } /* end if (error) */
			        } /* end if (dlopen) */
			    } else if (isNotPresent(rs)) {
			        rs = SR_OK ;
			    } /* end if (permid) */
		        } /* end if (regular file) */
		    } else if (isNotPresent(rs)) {
		        rs = SR_OK ;
		    } /* end if (u_stat) */
	        } /* end if (mksofname) */
	        if (mlp->sop != nullptr) break ;
	        if (rs < 0) break ;
	    } /* end for */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (subinfo::sockliber) */

int subinfo::sotest(char *tbuf,int tlen) noex {
	int		rs = SR_FAULT ;
	if (mlp->sop) {
	    if ((rs = sncpy(tbuf,tlen,mlp->modname,"_",symsuf)) >= 0) {
		cnullptr	np{} ;
	        rs = SR_NOTFOUND ;
	        if (void *vp ; (vp = dlsym(mlp->sop,tbuf)) != np) {
	            modload_mi	*mip = (modload_mi *) vp ;
	            mlp->midp = mip ;
	            if (strcmp(mip->name,mlp->modname) == 0) {
		        rs = SR_OK ;
	            }
	        } /* end if (dlsym) */
	    } /* end if (sncpy) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo::sotest) */

int subinfo::checksyms() noex {
	int		rs = SR_OK ;
	if (syms != nullptr) {
	    void	*p = nullptr ;
	    for (int i = 0 ; syms[i] ; i += 1) {
	        p = dlsym(mlp->sop,syms[i]) ;
	        if (p == nullptr) break ;
	    } /* end for */
	    if (p == nullptr) rs = SR_NOENT ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo::checksyms) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    var.maxpathlen = rs ;
	    if ((rs = getbufsize(getbufsize_hn)) >= 0) {
		var.maxhostlen = rs ;
	    }
	}
	return rs ;
}
/* end method (vars::operator) */

int modload::open(cc *apr,cc *afn,cc *amn,int mo,mv ms) noex {
	return modload_open(this,apr,afn,amn,mo,ms) ;
}

int modload::getmv(int vi) noex {
	return modload_getmv(this,vi) ;
}

int modload::getmva(int *mva,int mvn) noex {
	return modload_getmva(this,mva,mvn) ;
}

int modload::getsym(cchar *symname,cvoid **vpp) noex {
	return modload_getsym(this,symname,vpp) ;
}

void modload::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("modload",rs,"fini-close") ;
	}
}

modload_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case modloadmem_close:
	        rs = modload_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (modload_co::operator) */


