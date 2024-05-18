/* modload SUJPPORT */
/* lang=C++20 */

/* module-load management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	modload_open

	Description:
	This module implements an interface (a trivial one) that
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
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getnodename.h>		/* |getnodedomain(3uc)| */
#include	<ids.h>
#include	<dirseen.h>
#include	<pathclean.h>
#include	<mkpathx.h>
#include	<mkx.h>			/* |mksofname(3uc)| */
#include	<mkpr.h>
#include	<xperm.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"modload.h"


/* local defines */

#define	ML		modload
#define	ML_MI		modload_mi

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#define	SI		SUBINFO

#define	LIBCNAME	"lib"

#ifndef	VARLIBPATH
#define	VARLIBPATH	"LD_LIBRARY_PATH"
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
} ;

struct subinfo {
	void		*op ;
	cchar		*pr ;
	cchar		*modfn ;
	mainv		syms ;
	ids		id ;
	SUBINFO_FL	f ;
	int		opts ;
} ;


/* forward references */

template<typename ... Args>
static int modload_ctor(modload *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(op) ;
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

static int	subinfo_start(SI *,ML *,cc *,cc *,int,mainv) noex ;
static int	subinfo_modload(SI *) noex ;
static int	subinfo_finish(SI *,int) noex ;

static int	subinfo_objloadbegin(SI *) noex ;
static int	subinfo_objloadend(SI *) noex ;
static int	subinfo_sofind(SI *) noex ;
static int	subinfo_sofindprs(SI *,dirseen *,int) noex ;
static int	subinfo_sofindsdirs(SI *,dirseen *,int) noex ;
static int	subinfo_sofindpr(SI *,dirseen *,int,cchar *) noex ;
static int	subinfo_sofindvar(SI *,dirseen *,int) noex ;
static int	subinfo_socheckvarc(SI *,dirseen *,cchar *,int,int) noex ;
static int	subinfo_sochecklib(SI *,dirseen *,cchar *,int) noex ;
static int	subinfo_socheckliber(SI *,dirseen *,cchar *,int) noex ;
static int	subinfo_sotest(SI *) noex ;
static int	subinfo_checksyms(SI *) noex ;


/* local variables */

static constexpr cpcchar	prnames[] = {
	"LOCAL",
	"NCMP",
	"PCS",
	"EXTRA",
	"PREROOT",
	nullptr
} ;

static constexpr cpcchar	sysprs[] = {
	"/usr/extra",
	"/usr/preroot",
	nullptr
} ;

static constexpr cpcchar	exts[] = {
	"so",
	"o",
	"",
	nullptr
} ;

static constexpr cpcchar	extdirs[] = {
	"sparcv9",
	"sparcv8",
	"sparcv7",
	"sparc",
	"",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int modload_open(ML *op,cc *pr,cc *modfn,cc *modname,int opts,mv syms) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = modload_ctor(op,modfn,modname)) >= 0) {
	    rs = SR_INVALID ;
	    if (modfn[0] && modname[0]) {
	        SUBINFO		si ;
	        op->modname = modname ;
	        if ((rs = subinfo_start(&si,op,pr,modfn,opts,syms)) >= 0) {
	            if ((rs = subinfo_modload(&si)) >= 0) {
	                op->magic = MODLOAD_MAGIC ;
	            }
	            rs1 = subinfo_finish(&si,(rs<0)) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if */
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
	int		rs = SR_OK ;
	cvoid		*rp = nullptr ;

	if (op == nullptr) return SR_FAULT ;
	if (symname == nullptr) return SR_FAULT ;

	if (op->magic != MODLOAD_MAGIC) return SR_NOTOPEN ;

	if (symname[0] == '\0') return SR_INVALID ;

	if (op->sop) {
	    if ((rp = dlsym(op->sop,symname)) == nullptr) {
	        rs = SR_NOTFOUND ;
	    }
	} else {
	    rs = SR_NOTFOUND ;
	}

	if (vpp) {
	    *vpp = (rs >= 0) ? rp : nullptr ;
	}

	return rs ;
}
/* end subroutine (modload_getsym) */

int modload_getmv(ML *op,int vi) noex {
	int		rs = SR_OK ;
	int		v = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MODLOAD_MAGIC) return SR_NOTOPEN ;

	if (vi < 0) return SR_INVALID ;

	if (op->midp != nullptr) {
	    ML_MI	*mip = op->midp ;
	    v = mip->mv[vi] ;
	} else {
	    rs = SR_NOTFOUND ;
	}

	return (rs >= 0) ? v : rs ;
}
/* end subroutine (modload_getmv) */

int modload_getmva(ML *op,int *mva,int mvn) noex {
	int		rs = SR_OK ;
	int		v = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (mva == nullptr) return SR_FAULT ;

	if (op->magic != MODLOAD_MAGIC) return SR_NOTOPEN ;

	if (op->midp != nullptr) {
	    MODLOAD_MI	*mip = op->midp ;
	    v = mip->mv[0] ;
	    for (int i = 0 ; i < mvn ; i += 1) {
	        mva[i] = mip->mv[i] ;
	    }
	} else {
	    rs = SR_NOTFOUND ;
	}

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

static int subinfo_start(SI *sip,ML *op,cc *pr,cc *modfn,
		int opts,mv syms) noex {
	int		rs ;
	memclear(sip) ;
	sip->op = op ;
	sip->pr = pr ;
	sip->modfn = modfn ;
	sip->opts = opts ;
	sip->syms = syms ;
	rs = ids_load(&sip->id) ;
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip,int f_abort) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (f_abort) {
	    modload	*op = (ML *) sip->op ;
	    rs1 = modload_objloadclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_modload(SI *sip) noex {
	int		rs ;
	if ((rs = subinfo_objloadbegin(sip)) >= 0) {
	    rs = subinfo_checksyms(sip) ;
	    if (rs < 0) {
	        subinfo_objloadend(sip) ;
	    }
	} /* end if (objload-begin) */
	return rs ;
}
/* end subroutine (subinfo_modload) */

static int subinfo_objloadbegin(SI *sip) noex {
	int		rs ;
	{
	    rs = subinfo_sofind(sip) ;
	}
	return rs ;
}
/* end subroutine (subinfo_objloadbegin) */

static int subinfo_objloadend(SI *sip) noex {
	modload		*op = (ML *) sip->op ;
	int		rs ;
	{
	    rs = modload_objloadclose(op) ;
	}
	return rs ;
}
/* end subroutine (subinfo_objloadend) */

static int subinfo_sofind(SI *sip) noex {
	dirseen		ds ;
	int		dlm = RTLD_LAZY ;
	int		rs ;
	int		rs1 ;
	dlm |= (sip->opts & MODLOAD_OAVAIL) ? RTLD_GLOBAL : RTLD_LOCAL ;
	    if ((rs = dirseen_start(&ds)) >= 0) {
	        if (sip->pr != nullptr) {
	            rs = subinfo_sofindpr(sip,&ds,dlm,sip->pr) ;
		}
	        if ((rs < 0) && isNotPresent(rs)) {
	            if (sip->opts & MODLOAD_OLIBVAR) {
	                rs = subinfo_sofindvar(sip,&ds,dlm) ;
		    }
	        }
	        if ((rs < 0) && isNotPresent(rs)) {
	            if (sip->opts & MODLOAD_OPRS) {
	                rs = subinfo_sofindprs(sip,&ds,dlm) ;
		    }
	        }
	        if ((rs < 0) && isNotPresent(rs)) {
	            if (sip->opts & MODLOAD_OSDIRS) {
	                rs = subinfo_sofindsdirs(sip,&ds,dlm) ;
		    }
	        }
	        rs1 = dirseen_finish(&ds) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (dirseen) */
	return rs ;
}
/* end subroutine (subinfo_sofind) */

static int subinfo_sofindprs(SI *sip,dirseen *dsp,int dlm) noex {
	int		rs ;
	char		dn[MAXHOSTNAMELEN + 1] ;

	if ((rs = getnodedomain(nullptr,dn)) >= 0) {
	    cint	prlen = MAXPATHLEN ;
	    int		i ;
	    char	prbuf[MAXPATHLEN + 1] ;
	    for (i = 0 ; prnames[i] != nullptr ; i += 1) {
	        if ((rs = mkpr(prbuf,prlen,prnames[i],dn)) >= 0) {
	            rs = subinfo_sofindpr(sip,dsp,dlm,prbuf) ;
	        }
	        if ((rs >= 0) || (! isNotPresent(rs))) break ;
	    } /* end for */
	} /* end if (getnodedomain) */

	return rs ;
}
/* end subroutine (subinfo_sofindprs) */

static int subinfo_sofindpr(SI *sip,dirseen *dsp,int dlm,cchar *pr) noex {
	int		rs ;
	char		libdname[MAXPATHLEN + 1] ;

	if ((rs = mkpath2(libdname,pr,LIBCNAME)) >= 0) {
	    cint	rsn = SR_NOTFOUND ;
	    if ((rs = dirseen_havename(dsp,libdname,-1)) == rsn) {
	        USTAT	sb ;
		if ((rs = u_stat(libdname,&sb)) >= 0) {
		    if (S_ISDIR(sb.st_mode)) {
	    	        if ((rs = dirseen_havedevino(dsp,&sb)) == rsn) {
	    		    rs = subinfo_sochecklib(sip,dsp,libdname,dlm) ;
			    if ((rs < 0) && isNotPresent(rs)) {
	        		char	pbuf[MAXPATHLEN + 1] ;
	    		        int pl = pathclean(pbuf,libdname,-1) ;
	    		        if (pl >= 0)
	        		    dirseen_add(dsp,pbuf,pl,&sb) ;
			    }
			} else if (rs >= 0) {
	        	    rs = SR_NOENT ;
			}
		    } else {
	    		rs = SR_NOTDIR ;
		    }
	       } /* end if (stat) */
	    } else if (rs >= 0) {
	        rs = SR_NOENT ;
	    }
	} /* end if (mkpath) */

	return rs ;
}
/* end subroutine (subinfo_sofindpr) */

static int subinfo_sofindsdirs(SI *sip,dirseen *dsp,int dlm) noex {
	int		rs = SR_NOENT ;
	cchar		*dirname ;
	for (int i = 0 ; sysprs[i] != nullptr ; i += 1) {
	    dirname = sysprs[i] ;
	    rs = subinfo_sofindpr(sip,dsp,dlm,dirname) ;
	    if ((rs >= 0) || (! isNotPresent(rs))) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (subinfo_sofindsdirs) */

static int subinfo_sofindvar(SI *sip,dirseen *dsp,int dlm) noex {
	int		rs = SR_NOENT ;
	int		sl ;
	cchar		*sp ;

	if ((sp = getenv(VARLIBPATH)) != nullptr) {
	    cchar	*tp ;
	    while ((tp = strpbrk(sp,":;")) != nullptr) {
	        sl = (tp-sp) ;
	        if (sl > 0) {
	            rs = subinfo_socheckvarc(sip,dsp,sp,sl,dlm) ;
	        } /* end if (non-zero length) */
	        sp = (tp + 1) ;
	        if ((rs >= 0) || (! isNotPresent(rs))) break ;
	    } /* end for */
	    if ((rs < 0) && isNotPresent(rs)) {
	        if (sp[0] != '\0') {
	            rs = subinfo_socheckvarc(sip,dsp,sp,-1,dlm) ;
		}
	    }
	} /* end if (getenv) */

	return rs ;
}
/* end subroutine (subinfo_sofindvar) */

static int subinfo_socheckvarc(SI *sip,dirseen *dsp,
		cchar *ldnp,int ldnl,int dlm) noex {
	int		rs ;
	cchar		*pp ;
	char		pathbuf[MAXPATHLEN + 1] ;

	pp = (cchar *) pathbuf ;
	if ((rs = pathclean(pathbuf,ldnp,ldnl)) >= 0) {
	    cint	rsn = SR_NOTFOUND ;
	    cint	pl = rs ;
	    if ((rs = dirseen_havename(dsp,pp,pl)) == rsn) {
	        USTAT		sb ;
	        if ((rs = u_stat(pp,&sb)) >= 0) {
	            if (S_ISDIR(sb.st_mode)) {
	                if ((rs = dirseen_havedevino(dsp,&sb)) == rsn) {
	                    rs = subinfo_sochecklib(sip,dsp,pp,dlm) ;
	                    if ((rs < 0) && isNotPresent(rs)) {
	                        dirseen_add(dsp,pp,pl,&sb) ;
	                    }
	                } else if (rs >= 0) {
	                    rs = SR_NOENT ;
	                }
	            } else {
	                rs = SR_NOTDIR ;
	            }
	        } /* end if (stat) */
	    } else if (rs >= 0) {
	        rs = SR_NOENT ;
	    }
	} /* end if (pathclean) */

	return rs ;
}
/* end subroutine (subinfo_sofindvarc) */

static int subinfo_sochecklib(SI *sip,dirseen *dsp,cchar *ldname,int dlm) noex {
	int		rs = SR_FAULT ;
	if (dsp) {
	    USTAT		sb ;
	    modload		*op = (ML *) sip->op ;
	    cint		am = (R_OK | X_OK) ;
	    cchar		*ldnp ;
	    char		subdname[MAXPATHLEN + 1] ;
	    rs = SR_OK ;
	    op->sop = nullptr ;
	    for (int i = 0 ; extdirs[i] != nullptr ; i += 1) {
	        ldnp = ldname ;
	        if (extdirs[i][0] != '\0') {
	            ldnp = subdname ;
	            rs = mkpath2(subdname,ldname,extdirs[i]) ;
	        }
	        if (rs >= 0) {
	            if ((rs = u_stat(ldnp,&sb)) >= 0) {
	                if (S_ISDIR(sb.st_mode)) {
			    ids		*idp = &sip->id ;
			    if ((rs = sperm(idp,&sb,am)) >= 0) {
		   	        rs = subinfo_socheckliber(sip,dsp,ldnp,dlm) ;
			    } else if (isNotPresent(rs)) {
	    		        dirseen_add(dsp,ldnp,-1,&sb) ;
			        rs = SR_OK ;
			    }
		        } else {
			    rs = SR_NOTDIR ;
			}
		    } else if (isNotPresent(rs)) {
		        rs = SR_OK ;
		    }
	        } /* end if (ok) */
	        if (op->sop != nullptr) break ;
	        if (rs < 0) break ;
	    } /* end for (extdirs) */
	    if ((rs >= 0) && (op->sop == nullptr)) rs = SR_LIBACC ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_sochecklib) */

static int subinfo_socheckliber(SI *sip,dirseen *dsp,cchar *ldnp,int dlm) noex {
	modload		*op = (ML *) sip->op ;
	ids		*idp = &sip->id ;
	cint		am = (X_OK|R_OK) ;
	int		rs = SR_OK ;
	cchar		*mfn = sip->modfn ;
	char		sfn[MAXPATHLEN + 1] ;
	(void) dsp ;
	for (int j = 0 ; exts[j] != nullptr ; j += 1) {
	    if ((rs = mksofname(sfn,ldnp,mfn,exts[j])) >= 0) {
	        USTAT		sb ;
	        if ((rs = u_stat(sfn,&sb)) >= 0) {
		    if (S_ISREG(sb.st_mode)) {
			if ((rs = sperm(idp,&sb,am)) >= 0) {
			    void	*sop ;
			    if ((sop = dlopen(sfn,dlm)) != nullptr) {
				op->sop = sop ;
				rs = subinfo_sotest(sip) ;
				if (rs < 0) {
				    dlclose(sop) ;
				    op->sop = nullptr ;
				    if (isNotPresent(rs)) {
					rs = SR_OK ;
				    }
				} /* end if (error) */
			    } /* end if (dlopen) */
			} else if (isNotPresent(rs)) {
			    rs = SR_OK ;
			} /* end if (sperm) */
		    } else {
			rs = SR_ISDIR ;
		    }
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		} /* end if (u_stat) */
	    } /* end if (mksofname) */
	    if (op->sop != nullptr) break ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (subinfo_socheckliber) */

static int subinfo_sotest(SI *sip) noex {
	modload		*op = (ML *) sip->op ;
	int		rs = SR_FAULT ;
	if (op->sop) {
	    rs = SR_NOTFOUND ;
	    if (void *vp ; (vp = dlsym(op->sop,op->modname)) != nullptr) {
	        MODLOAD_MI	*mip = (MODLOAD_MI *) vp ;
	        op->midp = mip ;
	        if (strcmp(mip->name,op->modname) == 0) {
		    rs = SR_OK ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_sotest) */

static int subinfo_checksyms(SI *sip) noex {
	modload		*op = (ML *) sip->op ;
	int		rs = SR_OK ;
	if (sip->syms != nullptr) {
	    void	*p = nullptr ;
	    for (int i = 0 ; sip->syms[i] ; i += 1) {
	        p = dlsym(op->sop,sip->syms[i]) ;
	        if (p == nullptr) break ;
	    } /* end for */
	    if (p == nullptr) rs = SR_NOENT ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_checksyms) */


