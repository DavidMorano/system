/* pwi SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* PassWord Index manager */
/* version %I% last-modified %G% */

#define	CF_PWCACHE	1		/* use |ucpwcache(3uc)| */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pwi

	Description:
	This is a small hack for use by the USERINFO built-in command
	that is part of the Korn Shell (KSH).  This object provides
	some front-end glue for using the Iucentpw object on an
	Iucentpw database.

	Notes:

	= Searching for a PWI DB file:

	If a PWI DB name is passed to us, we only search for that
	DB.  If no PWI DB is passwed, we search first for a DB with
	the same name as our cluster name (if we have one); otherwise
	failing that we search for a DB with our node name.

	If no DB is present then we either make (a-fresh) the DB
	given to us by name, or we make a DB using our cluster name.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<ucpwcache.h>
#include	<getbufsize.h>
#include	<endian.h>
#include	<mkpathx.h>
#include	<mkfname.h>
#include	<sfx.h>
#include	<snx.h>
#include	<snwcpy.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<mkfnamesuf.h>
#include	<mkgecosname.h>
#include	<realname.h>
#include	<ipasswd.h>
#include	<vecstr.h>
#include	<spawnproc.h>
#include	<getnodename.h>
#include	<prgetclustername.h>
#include	<xperm.h>
#include	<char.h>
#include	<hasx.h>
#include	<localmisc.h>		/* |REALNAMELEN| */

#include	"pwi.h"


/* local defines */

#if	CF_PWCACHE
#define	GETPW_NAME	ucpwcache_name
#define	GETPW_UID	ucpwcache_uid
#else
#define	GETPW_NAME	getpw_name
#define	GETPW_UID	getpw_uid
#endif /* CF_PWCACHE */

#define	DBDNAME		"var/pwi"

#ifndef	UCENTPW_FNAME
#define	UCENTPW_FNAME	"/etc/passwd"
#endif

#define	TO_FILEMOD	(24 * 3600)

#define	PROG_MKPWI	"mkpwi"

#ifndef	VARHZ
#define	VARHZ		"HZ"
#endif

#ifndef	VARNODE
#define	VARNODE		"NODE"
#endif

#ifndef	VARHOMEDNAME
#define	VARHOMEDNAME	"HOME"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARLOGNAME
#define	VARLOGNAME	"LOGNAME"
#endif

#ifndef	VARTZ
#define	VARTZ		"TZ"
#endif

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif

#undef	VARDBNAME
#define	VARDBNAME	"MKPWI_DBNAME"

#undef	VARPRPWI
#define	VARPRPWI	"MKPWI_PROGRAMROOT"

#define	PWDESC		struct pwdesc


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct opener_fl {
	uint		dbname:1 ;
} ;

namespace {
    struct opener {
	cchar		*pr ;
	cchar		*dbname ;
	cchar		*idxdname ;
	opener_fl	fl{} ;
	opener(cchar *apr,cchar *adbn) noex : pr(apr), dbname(adbn) { 
	    idxdname = nullptr ;
	} ;
	int start() noex ;
	int finish() noex ;
	int mkidxdname() noex ;
	int mkpwi() noex ;
    } ; /* end struct (opener) */
}

struct pwdesc {
	ucentpw		*pwp ;
	char		*pwbuf ;
	int		pwlen ;
} ;

namespace {
    struct vars {
    	int		maxpathlen ;
	int		maxnamelen ;
	int		nodenamelen ;
	int		usernamelen ;
	int		realnamelen ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int pwi_ctor(pwi *op,Args ... args) noex {
    	PWI		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->dbp = new(nothrow) ipasswd) != np) {
		rs = SR_OK ;
	    } /* end if (new-ipasswd) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwi_ctor) */

static int pwi_dtor(pwi *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbp) {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwi_dtor) */

template<typename ... Args>
static inline int pwi_magic(pwi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PWI_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (pwi_magic) */

static int	realname_isextra(realname *,PWDESC *,cchar *) noex ;

static int	mkvars() noex ;


/* local variables */

constexpr cpcchar	exports[] = {
	VARHZ,
	VARNODE,
	VARHOMEDNAME,
	VARUSERNAME,
	VARLOGNAME,
	VARTZ,
	VARPWD,
	nullptr
} ;

constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ;

constexpr cchar		extras[] = "°¹²³" ;
constexpr cchar		progmkpwi[] = PROG_MKPWI ;

static vars		var ;


/* exported variables */


/* exported subroutines */

int pwi_open(pwi *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pwi_ctor(op,pr)) >= 0) {
	    static cint		rsv = mkvars() ;
	    if ((rs = rsv) >= 0) {
		opener		si(pr,dbname) ;
		custime		dt = getustime ;
		if ((rs = si.start()) >= 0) {
	    	    if ((rs = si.mkidxdname()) >= 0) {
	        	time_t	ti_pwi ;
	        	cint	to = TO_FILEMOD ;
	        	cchar	*suf = IPASSWD_SUF ;
	        	cchar	*endstr = ENDIANSTR ;
	        	cchar	*midname = si.idxdname ;
	        	char	fname[MAXPATHLEN+1] ;

	        if ((rs = mkfnamesuf2(fname,midname,suf,endstr)) >= 0) {
			USTAT		sb, *sbp = &sb ;
	            rs1 = u_stat(fname,sbp) ;

	            ti_pwi = sbp->st_mtime ;
	            if ((rs1 >= 0) && (ti_pwi == 0)) {
	                rs1 = SR_NOTFOUND ;
	            }

	            if ((rs1 >= 0) && ((dt - ti_pwi) >= to)) {
	                rs1 = SR_NOTFOUND ;
	            }

	            if (rs1 >= 0) {

	                rs1 = u_stat(UCENTPW_FNAME,&sb) ;

	                if ((rs1 >= 0) && (sb.st_mtime > ti_pwi)) {
	                    rs1 = SR_NOTFOUND ;
	                }

	            } /* end if (checking against system ucentpw file) */

	            if ((rs1 == SR_NOTFOUND) || (rs1 == SR_ACCESS)) {
	                rs = si.mkpwi() ;
	            } else {
	                rs = rs1 ;
		    }

	        } /* end if (mkfnamesuf) */

	        if (rs >= 0) {
	            if ((rs = ipasswd_open(op->dbp,midname)) >= 0) {
	                op->magic = PWI_MAGIC ;
	            }
	        }
	    } /* end if (opener_midname) */
	    rs1 = si.finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */
	    } /* end if (mkvars) */
	    if (rs < 0) {
		pwi_dtor(op) ;
	    }
	} /* end if (pwi_ctor) */
	return rs ;
}
/* end subroutine (pwi_open) */

int pwi_close(pwi *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pwi_magic(op)) >= 0) {
	    {
	        rs1 = ipasswd_close(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = pwi_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwi_close) */

int pwi_lookup(pwi *op,char *rbuf,int rlen,cchar *name) noex {
	ipasswd_cur	cur ;
	realname	rn ;
	cint		nlen = REALNAMELEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl ;
	int		c = 0 ;
	int		fopts = 0 ;
	int		ul = 0 ;
	cchar		*sp ;
	char		nbuf[REALNAMELEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;
	if (rbuf == nullptr) return SR_FAULT ;

	if (op->magic != PWI_MAGIC) return SR_NOTOPEN ;

	if (name[0] == '\0') return SR_INVALID ;

	rbuf[0] = '\0' ;
	if ((rlen >= 0) && (rlen < USERNAMELEN))
	    return SR_OVERFLOW ;

/* conditionally convert to lower case as needed */

	sp = name ;
	sl = -1 ;
	if (hasuc(name,-1)) {
	    sp = nbuf ;
	    rs = sncpylc(nbuf,nlen,name) ;
	    sl = rs ;
	}

/* load "name" into realname object for lookup */

	if (rs >= 0) {
	    ucentpw	pw ;
	    cint	pwlen = getbufsize(getbufsize_pw) ;
	    char	*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
		PWDESC	pd ;
		pd.pwp = &pw ;
		pd.pwbuf = pwbuf ;
		pd.pwlen = pwlen ;
	        if ((rs = realname_start(&rn,sp,sl)) >= 0) {
	            if ((rs = ipasswd_curbegin(op->dbp,&cur)) >= 0) {
		        char	un[USERNAMELEN + 1] ;
	                while (rs >= 0) {
	                    rs1 = ipasswd_fetch(op->dbp,&rn,&cur,fopts,un) ;
	                    if (rs1 == SR_NOTFOUND) break ;
			    rs = rs1 ;
			    if (rs >= 0) {
			        if ((rs = realname_isextra(&rn,&pd,un)) == 0) {
	                            ul = rs1 ; /* this must be HERE! */
	                            c += 1 ;
	                            rs = sncpy1(rbuf,rlen,un) ;
			        }
			    }
	                } /* end while */
	                rs1 = ipasswd_curend(op->dbp,&cur) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (cursor) */
	            rs1 = realname_finish(&rn) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (realname) */
		rs1 = uc_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (ok) */

/* if there was more than one match to the name, punt and issue error */

	if (rs >= 0) {
	    if (c <= 0) {
	        rs = SR_NOTFOUND ;
	    } else if (c > 1) {
	        rs = SR_NOTUNIQ ;
	    }
	} /* end if */

	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (pwi_lookup) */


/* private subroutines */

int opener::start() noex {
    	int		rs = SR_FAULT ;
	if (pr && dbname) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end method (opener::start) */

int opener::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	    if (idxdname) {
	        rs1 = uc_free(idxdname) ;
	        if (rs >= 0) rs = rs1 ;
	        idxdname = nullptr ;
	    }
	    if (fl.dbname && dbname) {
	        rs1 = uc_free(dbname) ;
	        if (rs >= 0) rs = rs1 ;
	        dbname = nullptr ;
		fl.dbname = false ;
	    }
	return rs ;
}
/* end method (opener::finish) */

int opener::mkidxdname() noex {
	int		rs = SR_OK ;
	int		dblen = -1 ;
	char		namebuf[MAXNAMELEN + 1] ;

	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    cint	nlen = NODENAMELEN ;
	    char	nbuf[NODENAMELEN + 1] ;
	    char	cbuf[NODENAMELEN + 1] ;
	    if ((rs = getnodename(nbuf,nlen)) >= 0) {
	        cint	rsn = SR_NOTFOUND ;
	        cchar	*nn ;
	        if ((rs = prgetclustername(pr,cbuf,nlen,nbuf)) >= 0) {
	            nn = cbuf ;
		} else if (rs == rsn) {
		    rs = SR_OK ;
		    nn = nbuf ;
		}
		if (rs >= 0) {
	            rs = mkpath3(namebuf,pr,DBDNAME,nn) ;
	            dbname = namebuf ;
	            dblen = rs ;
		}
	    }
	} /* end if (empty specification) */

	if (rs >= 0) {
	    if (cchar *cp{} ; (rs = uc_mallocstrw(dbname,dblen,&cp)) >= 0) {
	        idxdname = cp ;
	    }
	} /* end if */

	return rs ;
}
/* end method (opener::mkidxdname) */

int opener::mkpwi() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*dbname = idxdname ;
	char		progfname[MAXPATHLEN + 1] ;

	if (dbname == nullptr)
	    return SR_FAULT ;

	if (dbname[0] == '\0')
	    return SR_INVALID ;

	for (int i = 0 ; prbins[i] != nullptr ; i += 1) {
	    if ((rs = mkpath3(progfname,pr,prbins[i],progmkpwi)) >= 0) {
	        rs = perm(progfname,-1,-1,nullptr,X_OK) ;
	    }
	    if (rs >= 0) break ;
	} /* end for */

	if (rs >= 0) {
	    pid_t	cpid ;
	    cint	vn = 10 ;
	    cint	vo = VECSTR_OCOMPACT ;
	    int		cstat ;
	    int		cex ;
	    if (vecstr envs ; (rs = vecstr_start(&envs,vn,vo)) >= 0) {
		cint	alen = MAXNAMELEN ;
		int	cl ;
	        int	ai = 0 ;
	        cchar	*av[10] ;
		cchar	*cp{} ;
		cchar	*argz = progmkpwi ;
		char	abuf[MAXNAMELEN+1] ;

		if ((cl = sfbasename(progmkpwi,-1,&cp)) > 0) {
		    argz = abuf ;
		    strwcpyuc(abuf,cp,MIN(cl,alen)) ;
		}

/* setup arguments */

	        av[ai++] = argz ;
	        av[ai++] = nullptr ;

/* setup environment */

	        vecstr_envadd(&envs,VARPRPWI,pr,-1) ;

		if (dbname != nullptr) {
	            vecstr_envadd(&envs,VARDBNAME,dbname,-1) ;
		}
	        for (int i = 0 ; exports[i] != nullptr ; i += 1) {
	            if ((cp = getenv(exports[i])) != nullptr) {
	                rs = vecstr_envadd(&envs,exports[i],cp,-1) ;
		    }
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) {
	            if (mainv ev{} ; (rs = vecstr_getvec(&envs,&ev)) >= 0) {

	    		spawnproc_con	ps{} ;
	            ps.opts |= SPAWNPROC_OIGNINTR ;
	            ps.opts |= SPAWNPROC_OSETPGRP ;
	            for (int i = 0 ; i < 3 ; i += 1) {
			if (i != 2) {
	                    ps.disp[i] = SPAWNPROC_DCLOSE ;
			} else {
	                    ps.disp[i] = SPAWNPROC_DINHERIT ;
			}
	            } /* end for */

	            rs = spawnproc(&ps,progfname,av,ev) ;
	            cpid = rs ;

		    } /* end if (vecstr_getvec) */
	        } /* end if (ok) */
	        rs1 = vecstr_finish(&envs) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */

	    if (rs >= 0) {

	        cstat = 0 ;
	        rs = 0 ;
	        while (rs == 0) {
	            rs = u_waitpid(cpid,&cstat,0) ;
	            if (rs == SR_INTR) rs = 0 ;
	        } /* end while */

	        if (rs >= 0) {
	            cex = 0 ;
	            if (WIFSIGNALED(cstat)) {
	                rs = SR_UNATCH ;	/* protocol not attached */
		    }
	            if ((rs >= 0) && WIFEXITED(cstat)) {

	                cex = WEXITSTATUS(cstat) ;

	                if (cex != 0)
	                    rs = SR_LIBBAD ;

	            } /* end if (wait-exited) */

	        } /* end if (process finished) */

	    } /* end if */

	} /* end if (ok) */

	return rs ;
}
/* end method (opener::mkpwi) */

static int realname_isextra(realname *op,PWDESC *pdp,cchar *un) noex {
	int		rs ;
	int		f = false ;
	if (cchar *lp{} ; (rs = realname_getlast(op,&lp)) >= 0) {
	    cint	ll = rs ;
	    if (strnpbrk(lp,ll,extras) == nullptr) {
		ucentpw		*pwp = pdp->pwp ;
		cint		pwlen = pdp->pwlen ;
		char		*pwbuf = pdp->pwbuf ;
		if ((rs = GETPW_NAME(pwp,pwbuf,pwlen,un)) > 0) {
		    cchar	*gecos = pwp->pw_gecos ;
		    if (cchar *sp{} ; (rs = getgecosname(gecos,-1,&sp)) > 0) {
			f = (strnpbrk(sp,rs,extras) != nullptr) ;
		    }
		} else if (rs == SR_NOTFOUND) {
		    rs = SR_OK ;
		}
	    } /* end if (query does not have special extras) */
	} /* end if (realname_getlast) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (realname_isextra) */

static int mkvars() noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    var.maxpathlen = rs ;
	    if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	        var.maxnamelen = rs ;
	        if ((rs = getbufsize(getbufsize_nn)) >= 0) {
		    var.nodenamelen = rs ;
	            if ((rs = getbufsize(getbufsize_un)) >= 0) {
			var.usernamelen = rs ;
			var.realnamelen = REALNAMELEN ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (realname_isextra) */


