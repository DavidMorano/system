/* envmk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make program environment */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	envmk

	Description:
	This little honey creates an environment list for launching
	new processes.  It uses both the existing environment (if
	any) and also creates the minimium variables felt needed
	to give a new processes a fair chance.

	Synopsis:
	int envmk_start(envmk *op,mainv envv) noex

	Arguments:
	op		object pointer
	pr		program-root
	envv		environment array

	Returns:
	>=0		OK
	<0		error (system-return)


	Name:
	envmk_finish

	Synopsis:
	int envmk_finish(envmk *op) noex

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
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |u_getenviron(3u)| */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucsysconf.h>		/* LIBUC */
#include	<ucinfo.h>		/* LIBUC */
#include	<userattrdb.h>		/* LIBUC */
#include	<getarch.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<gethz.h>		/* LIBUC */
#include	<getxname.h>		/* LIBUC */
#include	<getnisdomain.h>	/* LIBUC |getnisdomain(3uc)| */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<envlist.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strdcpy.h>		/* LIBUC */
#include	<ctdec.h>		/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<strkeycmp.h>		/* |strkeycmp(3uc)| */
#include	<vstrkeycmp.h>		/* |vstrkeycmp(3uc)| */
#include	<mkchar.h>		/* LIBU */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"envmk.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;

/* local defines */

#define	NENVS		150
#define	PATHMULT	4		/* number of 'maxpathlen' */
#define	VBUFMULT	40		/* number of 'maxhostlen' */

#define	EL		envlist

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward reference */

template<typename ... Args>
local int envmk_ctor(envmk *op,Args ... args) noex {
    	ENVMK	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->envp = new(nt) vechand) != np) ylikely {
	        if ((op->storep = new(nt) strpack) != np) ylikely {
		    rs = SR_OK ;
		} /* end if (new_strpack) */
		if (rs < 0) {
		    delete op->envp ;
		    op->envp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (envmk_ctor) */

local int envmk_dtor(envmk *op) noex {
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
} /* end subroutine (envmk_dtor) */

template<typename ... Args>
local inline int envmk_magic(envmk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == ENVMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (envmk_magic) */

local int envmk_envv		(envmk *,mainv) noex ;
local int envmk_mkenv		(envmk *) noex ;
local int envmk_mkenvdef	(envmk *,EL *,mainv) noex ;
local int envmk_mkenvsys	(envmk *,EL *,mainv) noex ;
local int envmk_mkenvextras	(envmk *,EL *,mainv) noex ;
local int envmk_envadd		(envmk *,EL *,cchar *,cchar *,int) noex ;
local int envmk_userinfo	(envmk *) noex ;
local int envmk_cspath		(envmk *,EL *) noex ;


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

static bufsizevar	maxpathlen(bufsize_mp) ;
static bufsizevar	maxhostlen(bufsize_hostname) ;

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int envmk_start(envmk *op,mainv ev) noex {
	int		rs ;
	DPRINTF("ent\n") ;
	if ((rs = envmk_ctor(op)) >= 0) ylikely {
	    if ((rs = envmk_envv(op,ev)) >= 0) ylikely {
	        cint	vn = NENVS ;
	        cint	vo = vechandm.compact ;
	        if ((rs = vechand_start(op->envp,vn,vo)) >= 0) ylikely {
	            cint	ssz = 256 ;
	            if ((rs = strpack_start(op->storep,ssz)) >= 0) ylikely {
	                if ((rs = envmk_mkenv(op)) >= 0) {
			    op->magval = ENVMK_MAGIC ;
		        } /* end if */
	                if (rs < 0) {
	                    strpack_finish(op->storep) ;
		        } /* end if (error) */
	            } /* end if (strpack_start) */
	            if (rs < 0) {
	                vechand_finish(op->envp) ;
	            } /* end if (error) */
	        } /* end if (vechand_start) */
	    } /* end if (envmk_envv) */
	    if (rs < 0) {
		envmk_dtor(op) ;
	    } /* end if (error) */
	} /* end if (envmk_ctor) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (envmk_start) */

int envmk_finish(envmk *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = envmk_magic(op)) >= 0) ylikely {
	    rs = SR_OK ;
	    if (op->uh) ylikely {
		void *vp = voidp(op->uh) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->uh = nullptr ;
	    } /* end if (memory-release) */
	    if (op->un) ylikely {
		void *vp = voidp(op->un) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->un = nullptr ;
	    } /* end if (memory-release) */
	    if (op->storep) ylikely {
	        rs1 = strpack_finish(op->storep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->envp) {
	        rs1 = vechand_finish(op->envp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = envmk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (envmk_finish) */

int envmk_envset(envmk *op,cchar *kp,cchar *valp,int vall) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	if ((rs = envmk_magic(op,kp)) >= 0) ylikely {
	    strpack	*spp = op->storep ;
	    int		sz = 1 ; /* terminating NUL */
	    sz += lenstr(kp) ;
	    sz += 1 ;			/* for the equals sign character */
	    if (valp) {
	        sz += lenstr(valp,vall) ;
	    }
	    if (char *ebuf ; (rs = lm_mall(sz,&ebuf)) >= 0) ylikely {
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
	        rs1 = lm_free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-acquire) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (envmk_envset) */

int envmk_getvec(envmk *op,mainv *evp) noex {
	int		rs ;
	if ((rs = envmk_magic(op)) >= 0) ylikely {
	    vechand	*elp = op->envp ;
	    if (evp) {
	        rs = elp->getvec(evp) ;
	    } else {
	        rs = elp->count ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (envmk_getvec) */


/* private subroutines */

local int envmk_envv(envmk *op,mainv ev) noex {
    	int	rs = SR_OK ;
	if ((op->envv = ev) == nullptr) {
	    if ((rs = u_getenviron(&ev)) >= 0) ylikely {
		op->envv = ev ;
	    }
	} /* end if (needed) */
	return rs ;
} /* end subroutine (envmk_envv) */

local int envmk_mkenvpwd(envmk *op,EL *etp) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	cchar		*varpwd = varname.pwd ;
	DPRINTF("ent\n") ;
	if ((rs = etp->present(varpwd)) == rsn) {
	    if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) ylikely {
		if ((rs = getpwd(pbuf,rs)) > 0) ylikely {
	            c = 1 ;
	            rs = envmk_envadd(op,etp,varpwd,pbuf,rs) ;
		} else if (isNotPresent(rs)) {
	            rs = SR_OK ;
		} /* end if */
		rs1 = lm_free(pbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (envlist_present) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (envmk_mkenvpwd) */

local int envmk_mkenv(envmk *op) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (envlist et ; (rs = et.start(NENVS)) >= 0) ylikely {
	    vechand	*elp = op->envp ;
	    bool	f_path = false ;
	    cchar	*varpath = varname.path ;
	    if ((rs >= 0) && (op->envv != nullptr)) {
	        for (int i = 0 ; (rs >= 0) && op->envv[i] ; i += 1) {
	            cchar	*kp = op->envv[i] ;
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
	        rs = envmk_cspath(op,&et) ;
	        n += rs ;
	    } /* end if (PATH) */
	    /* default environment variables */
	    if ((rs >= 0) && (op->envv == nullptr)) {
	        rs = envmk_mkenvdef(op,&et,envdef) ;
	        n += rs ;
	    }
	    /* system environment variables */
	    if (rs >= 0) ylikely {
	        if ((rs = envmk_mkenvdef(op,&et,envsys)) >= 0) ylikely {
		    cint ne = int(nelem(envsys) - 1) ;
	            n += rs ;
	            if (rs < ne) {
	                rs = envmk_mkenvsys(op,&et,envsys) ;
	                n += rs ;
	            } /* end if */
	        } /* end if (envmk_mkenvdef) */
	    } /* end if (system environment variables) */
	    /* USERNAME and HOME */
	    if (rs >= 0) ylikely {
	        rs = envmk_mkenvextras(op,&et,envextra) ;
	        n += rs ;
	    } /* end if (extra environment variables) */
	    /* PWD */
	    if (rs >= 0) ylikely {
		rs = envmk_mkenvpwd(op,&et) ;
		n += rs ;
	    } /* end if */
	    /* done */
	    rs1 = et.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (envlist) */
	DPRINTF("ret rs=%d n=%d\n",rs,n) ;
	return (rs >= 0) ? n : rs ;
} /* end subroutine (envmk_mkenv) */

local int envmk_mkenvdef(envmk *op,EL *etp,mainv envs) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		n = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	for (int i = 0 ; (rs >= 0) && envs[i] ; i += 1) {
	    cchar	*kp = envs[i] ;
	if (kp) DPRINTF("kp=%s\n",kp) ;
	    if ((rs = etp->present(kp)) == rsn) {
	        rs = SR_OK ;
	        if (cchar *cp = getourenv(op->envv,kp) ; cp) {
	            n += 1 ;
	            rs = envmk_envadd(op,etp,kp,cp,-1) ;
	        } /* end if */
	    } /* end if (adding a default ENV) */
	} /* end for (defualt ENVs) */
	DPRINTF("ret rs=%d n=%d\n",rs,n) ;
	return (rs >= 0) ? n : rs ;
} /* end subroutine (envmk_mkenvdef) */

local int mktz(char *vbuf,int vlen,cchar *un) noex {
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
} /* end subroutine (mktz) */

local int envmk_mkenvsys(envmk *op,EL *etp,mainv envs) noex {
    	cnullptr	np{} ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	DPRINTF("ent\n") ;
	if ((rs = maxhostlen) >= 0) ylikely {
	    cint	vlen = (VBUFMULT * rs) ;
	    DPRINTF("maxpathlen=%d\n",rs) ;
	    if (char *vbuf ; (rs = lm_mall((vlen+1),&vbuf)) >= 0) ylikely {
	        DPRINTF("lm rs=%d\n",rs) ;
	        if (ucinfo_names uid ; (rs = ucinfo_name(&uid)) >= 0) ylikely {
	            DPRINTF("ucinfo() rs=%d\n",rs) ;
	            for (int i = 0 ; (rs >= 0) && envs[i] ; i += 1) {
	                cchar	*kp = envs[i] ;
	                DPRINTF("kp=%s -> envlist_present\n",kp) ;
	                if ((rs = etp->present(kp)) == rsn) {
	                    cint	sc = mkchar(kp[0]) ;
	        	    cchar	*vp = nullptr ;
			    cchar	*tp ;
	        	    int		vl = -1 ;
                            rs = SR_OK ;
			    DPRINTF("switch sc=%c\n",sc) ;
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
                                } /* end if (gethz) */
                                break ;
                            case 'T':
                                if (op->un == nullptr) {
                                    rs = envmk_userinfo(op) ;
                                }
				DPRINTF("T-mid rs=%d\n",rs) ;
                                if (rs >= 0) {
                                    if ((rs = mktz(vbuf,vlen,op->un)) > 0) {
					vl = rs ;
                                        vp = vbuf ;
                                    }
                                } /* end if (ok) */
                                break ;
                            } /* end switch */
                            if ((rs >= 0) && vp) {
                                n += 1 ;
                                rs = envmk_envadd(op,etp,kp,vp,vl) ;
                            } /* end if */
	    		} /* end if (already present) */
		    } /* end for */
		} /* end if (ucinfo) */
		rs1 = lm_free(vbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (maxhostlen) */
	DPRINTF("ret rs=%d n=%d\n",rs,n) ;
	return (rs >= 0) ? n : rs ;
} /* end subroutine (envmk_mkenvsys) */

local int envmk_mkenvextras(envmk *op,EL *etp,mainv envs) noex {
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
	    if ((rs = envmk_userinfo(op)) >= 0) {
		cchar	*var ;
	        if ((rs >= 0) && f_username) {
	            var = envs[extraenv_username] ;
	            rs = envmk_envadd(op,etp,var,op->un,-1) ;
	            n += rs ;
	        } /* end if */
	        if ((rs >= 0) && f_home) {
	            var = envs[extraenv_home] ;
	            rs = envmk_envadd(op,etp,var,op->uh,-1) ;
	            n += rs ;
	        } /* end if */
	    } /* end if (user-info) */
	} /* end if (needed them) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (envmk_mkenvextras) */

local int envmk_envadd(envmk *op,EL *etp,cc *kp,cc *vp,int vl) noex {
	vechand		*elp = op->envp ;
	cint		kl = lenstr(kp) ;
	int		rs ;
	int		rs1 ;
	int		bl = 0 ; /* return-value */
	bl += (kl + 1) ;
	if (vp) {
	    bl += ((vl >= 0) ? vl : int(lenstr(vp))) ;
	}
	if (char *bp ; (rs = lm_mall((bl+1),&bp)) >= 0) ylikely {
	    strpack	*spp = op->storep ;
	    strdcpy3w(bp,bl,kp,"=",vp,vl) ;
	    if (cchar *ep{} ; (rs = spp->store(bp,bl,&ep)) >= 0) {
	        if ((rs = elp->add(ep)) >= 0) {
	            rs = etp->add(ep,kl) ;
	        }
	    } /* end if (store) */
	    rs1 = lm_free(bp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-acquire) */
	return (rs >= 0) ? bl : rs ;
} /* end subroutine (envmk_envadd) */

local int envmk_cspath(envmk *op,EL *etp) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = maxpathlen) >= 0) ylikely {
	    cint	plen = (PATHMULT * rs) ;
	    if (char *pbuf ; (rs = lm_mall((plen + 1),&pbuf)) >= 0) ylikely {
	        cint	req = _CS_PATH ;
	        if ((rs = uc_sysconfstr(req,pbuf,plen)) >= 0) ylikely {
		    cchar	*varpath = varname.path ;
	            rs = envmk_envadd(op,etp,varpath,pbuf,rs) ;
	            n += rs ;
	        } /* end if */
	        rs1 = lm_free(pbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (envmk_cspath) */

local int envmk_userinfo(envmk *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	DPRINTF("ent\n") ;
	if (op->un == nullptr) {
	    if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	        ucentpw		pw ;
	        cint		pwlen = rs ;
	        if ((rs = getpwusername(&pw,pwbuf,pwlen,-1)) >= 0) ylikely {
		    cchar	*un = pw.pw_name ;
		    cchar	*uh = pw.pw_dir ;
		    if (cchar *cp ; (rs = lm_strw(un,-1,&cp)) >= 0) ylikely {
			op->un = cp ;
			if ((rs = lm_strw(uh,-1,&cp)) >= 0) ylikely {
	                    op->uh = cp ;
			} /* end if (memory-acquire) */
			if (rs < 0) {
			    void *vp = voidp(op->un) ;
			    lm_free(vp) ;
			    op->un = nullptr ;
			} /* end if (error) */
		    } /* end if (memory-acquire) */
	        } /* end if (getpwusername) */
		DPRINTF("getpwusername-out rs=%d\n",rs) ;
	        rs1 = lm_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (needed) */
	DPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (envmk_userinfo) */


