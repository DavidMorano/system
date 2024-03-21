/* mkprogenv SUPPORT */
/* lang=C++20 */

/* make program environment */
/* version %I% last-modified %G% */

#define	CF_UINFO	1		/* try to use |uinfo(3uc)| */
#define	CF_DEFPATH	0		/* use our local default PATH */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkprogenv_start

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
	<0		error


	Name:
	mkprogenv_finish

	Synopsis:
	int mkprogenv_finish(mkprogenv *op) noex

	Arguments:
	op		object pointer

	Returns:
	>=0		OK
	<0		error


	Notes:

	1. We use |gethz()| rather than |uc_confsys()| because it
	may be faster.  The |gethz()| function caches its value
	internally without having to go to the kernel. In some
	implementations, |uc_confsys()| also caches some of its
	values, but we can never be sure.

	2. The compile-time CF_DEFPATH allows for grabbing a default
	path from the system LOGIN configuration.  This takes a
	little bit of time so we now feel that it is not a good
	idea.  A better idea is to use some sort of system-call or
	pseudo system-call that stores that (LOGIN configuration)
	value.  Something along these lines of using the system
	call |uc_confsys()| to grab the CS_PATH value, which in
	some systems caches a default path (the CS_PATH) valiable.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<vecstr.h>
#include	<envlist.h>
#include	<uinfo.h>
#include	<userattrdb.h>
#include	<getusername.h>
#include	<getpwd.h>
#include	<gethz.h>
#include	<getourenv.h>
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


/* local defines */

#ifndef	DEFLOGFNAME
#define	DEFLOGFNAME	"/etc/default/login"
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(MAXHOSTNAMELEN + 40)
#endif

#ifndef	VARPATH
#define	VARPATH		"PATH"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARHOME
#define	VARHOME		"HOME"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#define	NENVS		150

#define	DEFPATH		"/usr/preroot/bin:/usr/xpg4/bin:/usr/bin:/usr/extra/bin"


/* external subroutines */

extern "C" {
    extern int	nisdomainname(char *,int) noex ;
    extern int	vecstr_envfile(vecstr *,cchar *) noex ;
    extern int	hasvarpathprefix(cchar *,int) noex ;
    extern int	mkvarpath(char *,cchar *,int) noex ;
}


/* external variables */

extern cchar	**environ ; /* secretly it's 'char **' */


/* forward reference */

static int mkprogenv_mkenv(mkprogenv *,mainv) noex ;
static int mkprogenv_mkenvdef(mkprogenv *,envlist *,mainv) noex ;
static int mkprogenv_mkenvsys(mkprogenv *,envlist *,mainv) noex ;
static int mkprogenv_mkenvextras(mkprogenv *,envlist *,mainv) noex ;
static int mkprogenv_envadd(mkprogenv *,envlist *,cchar *,cchar *,int) noex ;
static int mkprogenv_userinfo(mkprogenv *) noex ;

#if	CF_DEFPATH
static int mkprogenv_mkenvpath(mkprogenv *,envlist *,cc *,cc *) noex ;
#else
static int mkprogenv_cspath(mkprogenv *,envlist *) noex ;
#endif /* CF_DEFPATH */


/* local variables */

static constexpr cpcchar	envbad[] = {
	"_",
	"_A0",
	"_EF",
	"A__z",
	"RANDOM",
	"TERM",
	"TERMDEV",
	"TMOUT",
	"PWD",
	nullptr
} ;

static constexpr cpcchar	envsys[] = {
	"SYSNAME",
	"RELEASE",
	"VERSION",
	"MACHINE",
	"ARCHITECTURE",
	"HZ",
	"NODE",
	"TZ",
	"NISDOMAIN",
	nullptr
} ;

static constexpr cpcchar	envdef[] = {
	"LD_LIBRARY_PATH",
	"LD_RUN_PATH",
	"NISDOMAIN",
	"NODE",
	"DOMAIN",
	"USERNAME",
	"USER",
	"LOGNAME",
	"HOME",
	"TZ",
	"MAIL",
	"MAILDIR",
	"LANG",
	"LC_COLLATE",
	"LC_CTYPE",
	"LC_MESSAGES",
	"LC_MONETARY",
	"LC_NUMERIC",
	"LC_TIME",
	nullptr
} ;

enum envextra {
	extraenv_username,
	extraenv_home,
	extraenv_overlast
} ;

static constexpr cpcchar	envextra[] = {
	"USERNAME",
	"HOME",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int mkprogenv_start(mkprogenv *op,mainv envv) noex {
	mainv		progenviron = mainv(environ) ;
	int		rs ;
	int		opts ;

	if (op == nullptr) return SR_FAULT ;

	memclear(op) ;			/* dangerous */
	op->envv = (envv) ? envv : progenviron ;

	opts = VECHAND_OCOMPACT ;
	if ((rs = vechand_start(&op->env,NENVS,opts)) >= 0) {
	    cint	size = 256 ;
	    if ((rs = strpack_start(&op->stores,size)) >= 0) {
	        rs = mkprogenv_mkenv(op,envv) ;
	        if (rs < 0) {
	            strpack_finish(&op->stores) ;
		}
	    }
	    if (rs < 0) {
	        vechand_finish(&op->env) ;
	    }
	} /* end if (vechand_start) */

	return rs ;
}
/* end subroutine (mkprogenv_start) */

int mkprogenv_finish(mkprogenv *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
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
	    {
	        rs1 = strpack_finish(&op->stores) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vechand_finish(&op->env) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkprogenv_finish) */

int mkprogenv_envset(mkprogenv *op,cchar *kp,cchar *vp,int vl) noex {
	vechand		*elp = &op->env ;
	int		rs ;
	int		rs1 ;
	int		size = 1 ; /* terminating NUL */
	char		*p ;

	if (kp == nullptr) return SR_FAULT ;

	size += strlen(kp) ;
	size += 1 ;			/* for the equals sign character */
	if (vp != nullptr) size += strnlen(vp,vl) ;

	if ((rs = uc_malloc(size,&p)) >= 0) {
	    cchar	*ep ;
	    char	*bp = p ;
	    bp = strwcpy(bp,kp,-1) ;
	    *bp++ = '=' ;
	    if (vp != nullptr) bp = strwcpy(bp,vp,vl) ;
	    if ((rs = strpack_store(&op->stores,p,(bp-p),&ep)) >= 0) {
		vechand_vcmp	vcf = vechand_vcmp(vstrkeycmp) ;
	        rs1 = vechand_search(elp,ep,vcf,nullptr) ;
	        if (rs1 >= 0) vechand_del(elp,rs1) ;
	        rs = vechand_add(elp,ep) ;
	    }
	    uc_free(p) ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (mkprogenv_envset) */

int mkprogenv_getvec(mkprogenv *op,cchar ***eppp) noex {
	vechand		*elp = &op->env ;
	int		rs ;
	if (eppp != nullptr) {
	    rs = vechand_getvec(elp,eppp) ;
	} else {
	    rs = vechand_count(elp) ;
	}
	return rs ;
}
/* end subroutine (mkprogenv_getvec) */


/* private subroutines */

static int mkprogenv_mkenv(mkprogenv *op,mainv envv) noex {
	envlist		envtrack, *etp = &envtrack ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;

	if ((rs = envlist_start(etp,NENVS)) >= 0) {
	    vechand	*elp = &op->env ;
	    int		f_path = FALSE ;
	    cchar	*varpath = VARPATH ;
	    cchar	*varpwd = VARPWD ;
	    cchar	*kp ;

	    if ((rs >= 0) && (envv != nullptr)) {
	        for (int i = 0 ; (rs >= 0) && envv[i] ; i += 1) {
	            kp = envv[i] ;
	            if (matkeystr(envbad,kp,-1) < 0) {
	                if ((! f_path) && (kp[0] == 'P')) {
	                    f_path = (strkeycmp(kp,varpath) == 0) ;
	                }
	                n += 1 ;
	                if ((rs = vechand_add(elp,kp)) >= 0) {
	                    rs = envlist_add(etp,kp,-1) ;
	                }
	            } /* end if (good ENV variable) */
	        } /* end for */
	    } /* end if (ENV was specified) */

	    if ((rs >= 0) && (! f_path)) {
#if	CF_DEFPATH
	        {
	            rs = mkprogenv_mkenvpath(op,etp,varpath,DEFPATH) ;
	            n += rs ;
	        }
#else /* CF_DEFPATH */
	        rs = mkprogenv_cspath(op,etp) ;
	        n += rs ;
#endif /* CF_DEFPATH */
	    } /* end if (PATH) */

/* default environment variables */

	    if ((rs >= 0) && (envv == nullptr)) {
	        rs = mkprogenv_mkenvdef(op,etp,envdef) ;
	        n += rs ;
	    }

/* system environment variables */

	    if (rs >= 0) {
	        if ((rs = mkprogenv_mkenvdef(op,etp,envsys)) >= 0) {
		    cint	ne = int(nelem(envsys)-1) ;
	            n += rs ;
	            if (rs < ne) {
	                rs = mkprogenv_mkenvsys(op,etp,envsys) ;
	                n += rs ;
	            }
	        }
	    } /* end if (system environment variables) */

/* USERNAME and HOME */

	    if (rs >= 0) {
	        rs = mkprogenv_mkenvextras(op,etp,envextra) ;
	        n += rs ;
	    } /* end if (extra environment variables) */

/* PWD */

	    if (rs >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        if ((rs = envlist_present(etp,varpwd,-1,nullptr)) == rsn) {
	            char	pwd[MAXPATHLEN + 1] ;
	            if ((rs = getpwd(pwd,MAXPATHLEN)) > 0) {
	                n += 1 ;
	                rs = mkprogenv_envadd(op,etp,varpwd,pwd,rs) ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            } /* end if */
	        }
	    } /* end if (PWD) */

/* done */

	    rs1 = envlist_finish(etp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (envlist) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_mkenv) */

static int mkprogenv_mkenvdef(mkprogenv *op,envlist *etp,mainv envs) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		n = 0 ;
	for (int i = 0 ; (rs >= 0) && (envs[i] != nullptr) ; i += 1) {
	    cchar	*kp = envs[i] ;
	    if ((rs = envlist_present(etp,kp,-1,nullptr)) == rsn) {
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

static int mkprogenv_mkenvsys(mkprogenv *op,envlist *etp,mainv envs) noex {
	cint		vlen = VBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		vl ;
	int		n = 0 ;
#if	CF_UINFO
	uinfo_names	uid ;
#else
	UTSNAME		un ;
#endif
	cchar	*tp ;
	cchar	*kp ;
	cchar	*vp ;
	char		vbuf[VBUFLEN+1] = { 0 } ;

#if	CF_UINFO
	rs = uinfo_name(&uid) ;
#else
	rs = u_uname(&un) ;
#endif

	for (i = 0 ; (rs >= 0) && (envs[i] != nullptr) ; i += 1) {
	    kp = envs[i] ;

	    if ((rs = envlist_present(etp,kp,-1,nullptr)) == SR_NOTFOUND) {
	        cint	sc = mkchar(kp[0]) ;

	        rs = SR_OK ;
	        vp = nullptr ;
	        vl = -1 ;
	        switch (sc) {
#if	CF_UINFO
	        case 'S':
	        case 'R':
	        case 'V':
	        case 'M':
	        case 'N':
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
	                    rs = nisdomainname(vbuf,vlen) ;
	                    vl = rs ;
	                    vp = vbuf ;
	                } else {
	                    vp = uid.nodename ;
	                    if ((tp = strchr(vp,'.')) != nullptr) {
	                        rs = snwcpy(vbuf,vlen,vp,(tp-vp)) ;
	                        vl = rs ;
	                        vp = vbuf ;
	                    }
	                } /* end if */
	                break ;
	            } /* end switch */
	            break ;
#else /* CF_UINFO */
	        case 'S':
	            vp = un.sysname ;
	            break ;
	        case 'R':
	            vp = un.release ;
	            break ;
	        case 'V':
	            vp = un.version ;
	            break ;
	        case 'M':
	            vp = un.machine ;
	            break ;
	        case 'N':
	            vp = un.nodename ;
	            if ((tp = strchr(vp,'.')) != nullptr) {
	                rs = snwcpy(vbuf,vlen,vp,(tp-vp)) ;
	                vl = rs ;
	                vp = vbuf ;
	            }
	            break ;
#endif /* CF_INFO */
	        case 'A':
	            if ((rs = uc_getarchitecture(vbuf,vlen)) >= 0) {
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
	                userattrdb	ua ;
	                if ((rs = userattrdb_open(&ua,op->un)) >= 0) {
	                    {
	                        rs1 = userattrdb_lookup(&ua,vbuf,vlen,"tz") ;
	                        if (rs1 >= 0) vp = vbuf ;
	                    }
	                    rs1 = userattrdb_close(&ua) ;
	                    if (rs1 >= 0) rs = rs1 ;
	                } /* end if (userattr) */
	                if (isNotPresent(rs) || (rs == SR_NOSYS)) {
	                    rs = SR_OK ;
	                }
	            }
	            break ;
	        } /* end switch */

	        if ((rs >= 0) && (vp != nullptr)) {
	            n += 1 ;
	            rs = mkprogenv_envadd(op,etp,kp,vp,vl) ;
	        } /* end if */

	    } /* end if (environment variable was not already present) */

	} /* end for */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_mkenvsys) */

static int mkprogenv_mkenvextras(mkprogenv *op,envlist *etp,mainv envs) noex {
	cint		nrs = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		i ;
	int		n = 0 ;
	int		f_home = FALSE ;
	int		f_username = FALSE ;
	cchar	*var ;
	cchar	*kp ;

	for (i = 0 ; (rs >= 0) && (envs[i] != nullptr) ; i += 1) {
	    kp = envs[i] ;
	    if ((rs = envlist_present(etp,kp,-1,nullptr)) == nrs) {
	        rs = SR_OK ;
	        switch (i) {
	        case extraenv_username:
	            f_username = TRUE ;
	            break ;
	        case extraenv_home:
	            f_home = TRUE ;
	            break ;
	        } /* end switch */
	    } /* end if (not found) */
	    if (f_username && f_home) break ;
	} /* end for (extra ENVs) */

	if ((rs >= 0) && (f_username || f_home)) {
	    if ((rs = mkprogenv_userinfo(op)) >= 0) {
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

static int mkprogenv_envadd(mkprogenv *op,envlist *etp,cchar *kp,
		cchar *vp,int vl) noex {
	vechand		*elp = &op->env ;
	int		rs ;
	int		kl = strlen(kp) ;
	int		bl = 0 ;
	char		*bp ;

	bl += (kl+1) ;
	if (vp != nullptr) bl += ((vl >= 0) ? vl : strlen(vp)) ;

	if ((rs = uc_malloc((bl+1),&bp)) >= 0) {
	    cchar	*ep ;

	    strdcpy3w(bp,bl,kp,"=",vp,vl) ;

	    if ((rs = strpack_store(&op->stores,bp,bl,&ep)) >= 0) {
	        if ((rs = vechand_add(elp,ep)) >= 0) {
	            rs = envlist_add(etp,ep,kl) ;
	        }
	    } /* end if (store) */

	    uc_free(bp) ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (mkprogenv_envadd) */

#if	CF_DEFPATH
static int mkprogenv_mkenvpath(mkprogenv *op,envlist *etp,
		cchar *varpath,cchar *defpath) noex {
	vecstr		deflogin ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	cchar	*tp = DEFLOGFNAME ;
	cchar	*cp ;

	if ((rs = vecstr_start(&deflogin,10,0)) >= 0) {

	    cp = nullptr ;
	    if ((rs = vecstr_envfile(&deflogin,tp)) >= 0) {

	        rs1 = vecstr_finder(&deflogin,varpath,vstrkeycmp,&cp) ;

	        if ((rs1 >= 0) && ((tp = strchr(cp,'=')) != nullptr)) {
	            cp = (tp + 1) ;
	        }

	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    } /* end if (vecstr_envfile) */

	    if (cp == nullptr) cp = defpath ;

	    if (rs >= 0) {
	        n += 1 ;
	        rs = mkprogenv_envadd(op,etp,varpath,cp,-1) ;
	    } /* end if */

	    rs1 = vecstr_finish(&deflogin) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_mkenvpath) */
#else /* CF_DEFPATH */
static int mkprogenv_cspath(mkprogenv *op,envlist *etp) noex {
	cint		plen = (2*MAXPATHLEN) ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	cchar		*varpath = VARPATH ;
	char		*pbuf{} ;
	if ((rs = uc_malloc((plen+1),&pbuf)) >= 0) {
	    cint	req = _CS_PATH ;
	    if ((rs = uc_confstr(pbuf,plen,req)) >= 0) {
	        rs = mkprogenv_envadd(op,etp,varpath,pbuf,rs) ;
	        n += rs ;
	    } /* end if */
	    rs1 = uc_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mkprogenv_cspath) */
#endif /* CF_DEFPATH */

static int mkprogenv_userinfo(mkprogenv *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->un == nullptr) {
	    if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	        ucentpw	pw ;
	        cint	pwlen = rs ;
	        char	*pwbuf ;
	        if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	            if ((rs = getpwusername(&pw,pwbuf,pwlen,-1)) >= 0) {
			cchar	*un = pw.pw_name ;
			cchar	*uh = pw.pw_dir ;
	                cchar	*cp ;
	                if ((rs = uc_mallocstrw(un,-1,&cp)) >= 0) {
	                    op->un = cp ;
	                    if ((rs = uc_mallocstrw(uh,-1,&cp)) >= 0) {
	                        op->uh = cp ;
			    }
	                }
	            } /* end if (getpwusername) */
	            rs1 = uc_free(pwbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a) */
	    } /* end if (getbufsize) */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (mkprogenv_userinfo) */


