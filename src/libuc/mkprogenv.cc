/* mkprogenv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make program environment */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	mkprogenv

	Description:
	This little honey creates an environment list for launching
	new processes.  It uses both the existing environment (if
	any) and also creates the minimium variables felt needed
	to give a new processes a fair chance.

	Synopsis:
	int mkprogenv_start(mkprogenv *op,mainv envv) noex

	Arguments:
	op		object pointer
	pr		program-root
	envv		environment array

	Returns:
	>=0		OK
	<0		error (system-return)


	Name:
	mkprogenv_finish

	Synopsis:
	int mkprogenv_finish(mkprogenv *op) noex

	Arguments:
	op		object pointer

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	1. We use |gethz()| rather than |uc_sysconfval()| because it
	may be faster.  The |gethz()| function caches its value
	internally without having to go to the kernel.  In some
	implementations, |uc_sysconfval()| also caches some of its
	values, but we can never be sure.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<uinfo.h>
#include	<userattrdb.h>
#include	<getbufsize.h>
#include	<getarchitecture.h>
#include	<getusername.h>
#include	<getpwd.h>
#include	<gethz.h>
#include	<getourenv.h>
#include	<getxname.h>
#include	<getnisdomain.h>	/* |getnisdomain(3uc)| */
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<vecstr.h>
#include	<envlist.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<ctdec.h>
#include	<matxstr.h>
#include	<strkeycmp.h>
#include	<vstrkeycmpx.h>
#include	<mkchar.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"mkprogenv.h"

import libutil ;
import uconstants ;

/* local defines */

#define	NENVS		150
#define	PATHMULT	4		/* number of 'maxpathlen' */
#define	VBUFMULT	40		/* number of 'maxhostlen' */

#define	EL		envlist


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mkvarpath(char *,cchar *,int) noex ;
    extern bool	hasvarpathprefix(cchar *,int) noex ;
}


/* external variables */

extern cchar	**environ ; /* secretly it is really 'char **' */


/* forward reference */

template<typename ... Args>
static int mkprogenv_ctor(mkprogenv *op,Args ... args) noex {
    	MKPROGENV	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->envp = new(nothrow) vechand) != np) ylikely {
	        if ((op->storep = new(nothrow) strpack) != np) ylikely {
		    rs = SR_OK ;
		} /* end if (new_strpack) */
		if (rs < 0) {
		    delete op->envp ;
		    op->envp = nullptr ;
		}
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkprogenv_ctor) */

static int mkprogenv_dtor(mkprogenv *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->storep) {
		delete op->storep ;
		op->storep = nullptr ;
	    }
	    if (op->envp) {
		delete op->envp ;
		op->envp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkprogenv_dtor) */

template<typename ... Args>
static inline int mkprogenv_magic(mkprogenv *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == MKPROGENV_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mkprogenv_magic) */

static int mkprogenv_mkenv(mkprogenv *,mainv) noex ;
static int mkprogenv_mkenvdef(mkprogenv *,EL *,mainv) noex ;
static int mkprogenv_mkenvsys(mkprogenv *,EL *,mainv) noex ;
static int mkprogenv_mkenvextras(mkprogenv *,EL *,mainv) noex ;
static int mkprogenv_envadd(mkprogenv *,EL *,cchar *,cchar *,int) noex ;
static int mkprogenv_userinfo(mkprogenv *) noex ;
static int mkprogenv_cspath(mkprogenv *,EL *) noex ;


/* local variables */

constexpr cpcchar	envbad[] = {
	"_",
	"_A0",
	"_EF",
	"A__z",
	"TMOUT",
	varname.random,
	varname.term,
	varname.termprogram,
	varname.termdev,
	varname.pwd,
	nullptr
} ; /* end array (envbad) */

constexpr cpcchar	envsys[] = {
	varname.sysname,
	varname.release,
	varname.version,
	varname.machine,
	varname.architecture,
	varname.node,
	varname.tz,
	varname.nisdomain,
	nullptr
} ; /* end array (envsys) */

constexpr cpcchar	envdef[] = {
	"LD_LIBRARY_PATH",
	"LD_RUN_PATH",
	varname.nisdomain,
	varname.node,
	varname.domain,
	varname.username,
	varname.user,
	varname.logname,
	varname.logline,
	varname.utmpname,
	varname.utmpline,
	varname.home,
	varname.tz,
	varname.mail,
	varname.maildir,
	"LANG",
	"LC_COLLATE",
	"LC_CTYPE",
	"LC_MESSAGES",
	"LC_MONETARY",
	"LC_NUMERIC",
	"LC_TIME",
	nullptr
} ; /* end array (envdef) */

enum envextra {
	extraenv_username,
	extraenv_home,
	extraenv_overlast
} ; /* end enum (envextra) */

constexpr cpcchar	envextra[] = {
	varname.username,
	varname.home,
	nullptr
} ; /* end array (envextra) */

constexpr cpcchar	defpaths[] = {
	"/usr/preroot/bin",
	"/usr/xpg4/bin",
	"/usr/local/bin",
	"/opt/local/bin",
	"/usr/bin",
	"/bin",
	"/usr/sbin",
	"/sbin"
	"/usr/extra/bin",
	nullptr
} ; /* end array (defpaths) */

static bufsizevar	maxpathlen(getbufsize_mp) ;
static bufsizevar	maxhostlen(getbufsize_hn) ;


/* exported variables */


/* exported subroutines */

int mkprogenv_start(mkprogenv *op,mainv envv) noex {
	mainv		progenviron = mainv(environ) ;
	int		rs ;
	if ((rs = mkprogenv_ctor(op)) >= 0) ylikely {
	    cint	vn = NENVS ;
	    cint	vo = VECHAND_OCOMPACT ;
	    op->envv = (envv) ? envv : progenviron ;
	    if ((rs = vechand_start(op->envp,vn,vo)) >= 0) ylikely {
	        cint	ssz = 256 ;
	        if ((rs = strpack_start(op->storep,ssz)) >= 0) ylikely {
	            rs = mkprogenv_mkenv(op,envv) ;
	            if (rs < 0) {
	                strpack_finish(op->storep) ;
		    }
	        } /* end if (strpack_start) */
	        if (rs < 0) {
	            vechand_finish(op->envp) ;
	        }
	    } /* end if (vechand_start) */
	    if (rs < 0) {
		mkprogenv_dtor(op) ;
	    }
	} /* end if (mkprogenv_ctor) */
	return rs ;
}
/* end subroutine (mkprogenv_start) */

int mkprogenv_finish(mkprogenv *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mkprogenv_magic(op)) >= 0) ylikely {
	    rs = SR_OK ;
	    if (op->uh) {
	        rs1 = uc_free(op->uh) ;
	        if (rs >= 0) rs = rs1 ;
	        op->uh = nullptr ;
	    }
	    if (op->un) {
	        rs1 = uc_free(op->un) ;
	        if (rs >= 0) rs = rs1 ;
	        op->un = nullptr ;
	    }
	    if (op->storep) {
	        rs1 = strpack_finish(op->storep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->envv) {
	        rs1 = vechand_finish(op->envp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = mkprogenv_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkprogenv_finish) */

int mkprogenv_envset(mkprogenv *op,cchar *kp,cchar *valp,int vall) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	if ((rs = mkprogenv_magic(op,kp)) >= 0) ylikely {
	    strpack	*spp = op->storep ;
	    int		sz = 1 ; /* terminating NUL */
	    sz += lenstr(kp) ;
	    sz += 1 ;			/* for the equals sign character */
	    if (valp) {
	        sz += int(strnlen(valp,vall)) ;
	    }
	    if (char *ebuf ; (rs = uc_malloc(sz,&ebuf)) >= 0) ylikely {
	        char	*bp = ebuf ;
	        bp = strwcpy(bp,kp,-1) ;
	        *bp++ = '=' ;
	        if (valp) {
	       	    bp = strwcpy(bp,valp,vall) ;
	        }
		cint	el = intconv(bp - ebuf) ;
	        if (cchar *ep{} ; (rs = spp->store(ebuf,el,&ep)) >= 0) ylikely {
	    	    vechand	*elp = op->envp ;
		    {
		        auto	vcf = vechand_f(vstrkeycmp) ;
	                if ((rs = elp->search(ep,vcf,np)) >= 0) {
	                    rs = elp->del(rs) ;
			} else if (rs == rsn) {
	                    rs = SR_OK ;
			}
		    } /* end block */
		    if (rs >= 0) {
	                rs = vechand_add(elp,ep) ;
		    }
	        } /* end if (strpack_store) */
	        rs1 = uc_free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkprogenv_envset) */

int mkprogenv_getvec(mkprogenv *op,mainv *evp) noex {
	int		rs ;
	if ((rs = mkprogenv_magic(op)) >= 0) ylikely {
	    vechand	*elp = op->envp ;
	    if (evp) {
	        rs = elp->getvec(evp) ;
	    } else {
	        rs = elp->count ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkprogenv_getvec) */


/* private subroutines */

static int mkprogenv_mkenvpwd(mkprogenv *op,EL *etp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		c = 0 ;
	cchar		*varpwd = varname.pwd ;
	if ((rs = etp->present(varpwd)) == rsn) {
	    if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) ylikely {
		if ((rs = getpwd(pbuf,rs)) > 0) ylikely {
	            c = 1 ;
	            rs = mkprogenv_envadd(op,etp,varpwd,pbuf,rs) ;
		} else if (isNotPresent(rs)) {
	            rs = SR_OK ;
		} /* end if */
		rs = rsfree(rs,pbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (envlist_present) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkprogenv_mkenvpwd) */

static int mkprogenv_mkenv(mkprogenv *op,mainv envv) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (envlist et ; (rs = et.start(NENVS)) >= 0) ylikely {
	    vechand	*elp = op->envp ;
	    bool	f_path = false ;
	    cchar	*varpath = varname.path ;
	    if ((rs >= 0) && (envv != nullptr)) {
	        for (int i = 0 ; (rs >= 0) && envv[i] ; i += 1) {
	            cchar	*kp = envv[i] ;
	            if (matkeystr(envbad,kp,-1) < 0) {
	                if ((! f_path) && (kp[0] == 'P')) {
	                    f_path = (strkeycmp(kp,varpath) == 0) ;
	                }
	                n += 1 ;
	                if ((rs = elp->add(kp)) >= 0) {
	                    rs = et.add(kp,-1) ;
	                }
	            } /* end if (good ENV variable) */
	        } /* end for */
	    } /* end if (ENV was specified) */
	    if ((rs >= 0) && (! f_path)) {
	        rs = mkprogenv_cspath(op,&et) ;
	        n += rs ;
	    } /* end if (PATH) */
	    /* default environment variables */
	    if ((rs >= 0) && (envv == nullptr)) {
	        rs = mkprogenv_mkenvdef(op,&et,envdef) ;
	        n += rs ;
	    }
	    /* system environment variables */
	    if (rs >= 0) ylikely {
	        if ((rs = mkprogenv_mkenvdef(op,&et,envsys)) >= 0) ylikely {
		    cint	ne = int(nelem(envsys) - 1) ;
	            n += rs ;
	            if (rs < ne) {
	                rs = mkprogenv_mkenvsys(op,&et,envsys) ;
	                n += rs ;
	            }
	        }
	    } /* end if (system environment variables) */
	    /* USERNAME and HOME */
	    if (rs >= 0) ylikely {
	        rs = mkprogenv_mkenvextras(op,&et,envextra) ;
	        n += rs ;
	    } /* end if (extra environment variables) */
	    /* PWD */
	    if (rs >= 0) ylikely {
		rs = mkprogenv_mkenvpwd(op,&et) ;
		n += rs ;
	    } /* end if */
	    /* done */
	    rs1 = et.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (envlist) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_mkenv) */

static int mkprogenv_mkenvdef(mkprogenv *op,EL *etp,mainv envs) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		n = 0 ;
	for (int i = 0 ; (rs >= 0) && (envs[i] != nullptr) ; i += 1) {
	    cchar	*kp = envs[i] ;
	    if ((rs = etp->present(kp)) == rsn) {
	        rs = SR_OK ;
	        if (cchar *cp ; (cp = getourenv(op->envv,kp)) != nullptr) {
	            n += 1 ;
	            rs = mkprogenv_envadd(op,etp,kp,cp,-1) ;
	        } /* end if */
	    } /* end if (adding a default ENV) */
	} /* end for (defualt ENVs) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_mkenvdef) */

static int mktz(char *vbuf,int vlen,cchar *un) noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (userattrdb ua ; (rs = ua.open(un)) >= 0) ylikely {
	    if ((rs = ua.lookup(vbuf,vlen,"tz")) >= 0) {
		len = rs ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	    rs1 = ua.close ;
	    if (rs1 >= 0) rs = rs1 ;
	} /* end if (userattr) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mktz) */

static int mkprogenv_mkenvsys(mkprogenv *op,EL *etp,mainv envs) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		n = 0 ;
	if ((rs = maxhostlen) >= 0) ylikely {
	    cint	vlen = (VBUFMULT * rs) ;
	    if (char *vbuf ; (rs = uc_malloc((vlen+1),&vbuf)) >= 0) ylikely {
	        if (uinfo_names uid ; (rs = uinfo_name(&uid)) >= 0) ylikely {
	            for (int i = 0 ; (rs >= 0) && envs[i] ; i += 1) {
	                cchar	*kp = envs[i] ;
	                if ((rs = etp->present(kp)) == rsn) {
	                    cint	sc = mkchar(kp[0]) ;
	        	    cchar	*vp = nullptr ;
			    cchar	*tp ;
	        	    int		vl = -1 ;
                            rs = SR_OK ;
                            switch (sc) {
                            case 'S':
                                vp = uid.sysname ;
                                break ;
                            case 'R':
                                vp = uid.release ;
                                break ;
                            case 'V':
                                vp = uid.version ;
                                break ;
                            case 'M':
                                vp = uid.machine ;
                                break ;
                            case 'N':
                                if (kp[1] == 'I') {
                                    rs = getnisdomain(vbuf,vlen) ;
                                    vl = rs ;
                                    vp = vbuf ;
                                } else {
                                    vp = uid.nodename ;
                                    if ((tp = strchr(vp,'.')) != np) {
                                        vl = intconv(tp - vp) ;
                                    }
                                } /* end if */
                                break ;
                            case 'A':
                                if ((rs = getarch(vbuf,vlen)) >= 0) {
                                    vl = rs ;
                                    vp = vbuf ;
                                } else if (isNotPresent(rs)) {
                                    rs = SR_OK ;
                                }
                                break ;
                            case 'H':
                                if ((rs = gethz(0)) >= 0) {
                                    vp = vbuf ;
                                    rs = ctdecl(vbuf,vlen,rs) ;
                                    vl = rs ;
                                }
                                break ;
                            case 'T':
                                if (op->un == nullptr) {
                                    rs = mkprogenv_userinfo(op) ;
                                }
                                if (rs >= 0) {
                                    if ((rs = mktz(vbuf,vlen,op->un)) > 0) {
					vl = rs ;
                                        vp = vbuf ;
                                    }
                                }
                                break ;
                            } /* end switch */
                            if ((rs >= 0) && vp) {
                                n += 1 ;
                                rs = mkprogenv_envadd(op,etp,kp,vp,vl) ;
                            } /* end if */
	    		} /* end if (already present) */
		    } /* end for */
		} /* end if (uinfo) */
		rs = rsfree(rs,vbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (maxhostlen) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_mkenvsys) */

static int mkprogenv_mkenvextras(mkprogenv *op,EL *etp,mainv envs) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		n = 0 ;
	bool		f_home = false ;
	bool		f_username = false ;
	for (int i = 0 ; (rs >= 0) && (envs[i] != nullptr) ; i += 1) {
	    cchar	*kp = envs[i] ;
	    if ((rs = etp->present(kp)) == rsn) {
	        rs = SR_OK ;
	        switch (i) {
	        case extraenv_username:
	            f_username = true ;
	            break ;
	        case extraenv_home:
	            f_home = true ;
	            break ;
	        } /* end switch */
	    } /* end if (not found) */
	    if (f_username && f_home) break ;
	} /* end for (extra ENVs) */
	if ((rs >= 0) && (f_username || f_home)) {
	    if ((rs = mkprogenv_userinfo(op)) >= 0) {
		cchar	*var ;
	        if ((rs >= 0) && f_username) {
	            var = envs[extraenv_username] ;
	            rs = mkprogenv_envadd(op,etp,var,op->un,-1) ;
	            n += rs ;
	        } /* end if */
	        if ((rs >= 0) && f_home) {
	            var = envs[extraenv_home] ;
	            rs = mkprogenv_envadd(op,etp,var,op->uh,-1) ;
	            n += rs ;
	        } /* end if */
	    } /* end if (user-info) */
	} /* end if (needed them) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_mkenvextras) */

static int mkprogenv_envadd(mkprogenv *op,EL *etp,cc *kp,cc *vp,int vl) noex {
	vechand		*elp = op->envp ;
	cint		kl = lenstr(kp) ;
	int		rs ;
	int		bl = 0 ;
	bl += (kl + 1) ;
	if (vp) {
	    bl += ((vl >= 0) ? vl : int(lenstr(vp))) ;
	}
	if (char *bp ; (rs = uc_malloc((bl+1),&bp)) >= 0) {
	    strpack	*spp = op->storep ;
	    strdcpy3w(bp,bl,kp,"=",vp,vl) ;
	    if (cchar *ep{} ; (rs = spp->store(bp,bl,&ep)) >= 0) {
	        if ((rs = elp->add(ep)) >= 0) {
	            rs = etp->add(ep,kl) ;
	        }
	    } /* end if (store) */
	    rs = rsfree(rs,bp) ;
	} /* end if (memory-allocation) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (mkprogenv_envadd) */

static int mkprogenv_cspath(mkprogenv *op,EL *etp) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	plen = (PATHMULT * rs) ;
	    if (char *pbuf ; (rs = uc_malloc((plen + 1),&pbuf)) >= 0) {
	        cint	req = _CS_PATH ;
	        if ((rs = uc_sysconfstr(req,pbuf,plen)) >= 0) {
		    cchar	*varpath = varname.path ;
	            rs = mkprogenv_envadd(op,etp,varpath,pbuf,rs) ;
	            n += rs ;
	        } /* end if */
	        rs1 = uc_free(pbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_cspath) */

static int mkprogenv_userinfo(mkprogenv *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->un == nullptr) {
	    if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
	        ucentpw		pw ;
	        cint		pwlen = rs ;
	        if ((rs = getpwusername(&pw,pwbuf,pwlen,-1)) >= 0) {
		    cchar	*un = pw.pw_name ;
		    cchar	*uh = pw.pw_dir ;
		    if (cchar *cp ; (rs = uc_mallocstrw(un,-1,&cp)) >= 0) {
			op->un = cp ;
			if ((rs = uc_mallocstrw(uh,-1,&cp)) >= 0) {
	                    op->uh = cp ;
			} /* end if (memory-allocation) */
		    } /* end if (memory-allocations) */
	        } /* end if (getpwusername) */
	        rs1 = uc_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (mkprogenv_userinfo) */


