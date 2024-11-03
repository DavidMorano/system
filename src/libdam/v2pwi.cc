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
#include	<mallocxx.h>
#include	<endian.h>
#include	<ids.h>
#include	<mkpathx.h>
#include	<mkfname.h>
#include	<sfx.h>			/* |sfbasename(3uc)| */
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
#include	<isnot.h>
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
	uint		dbname:1 ;	/* allocated */
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
	int dbnameload(cc *) noex ;
	int idxload(cc *,int = -1) noex ;
	int mk() noex ;
	int mkbegin(char *) noex ;
	int mkend() noex ;
	int decide() noex ;
	int mkproc(cchar *) noex ;
	int mkenv(vecstr *) noex ;
	int mkspawn(cchar *,mainv,vecstr *) noex ;
	int mkwait(int) noex ;
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
	int		rv = 0 ;
	if ((rs = pwi_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        static cint	rsv = mkvars() ;
	        if ((rs = rsv) >= 0) {
		    opener	so(pr,dbname) ;
		    if ((rs = so.start()) >= 0) {
	    	        if ((rs = so.mkidxdname()) >= 0) {
			    if ((rs = so.decide()) >= 0) {
				cc *sfn = so.idxdname ;
				if ((rs = ipasswd_open(op->dbp,sfn)) >= 0) {
				    rv = rs ;
	    			    op->magic = PWI_MAGIC ;
				}
			    }
	                } /* end if (opener_midname) */
	                rs1 = so.finish() ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (opener) */
	        } /* end if (mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		pwi_dtor(op) ;
	    }
	} /* end if (pwi_ctor) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (pwi_open) */

int opener::decide() noex {
	cint		to = TO_FILEMOD ;
	int		rs ;
	if (char *fbuf{} ; (rs = malloc_mp(&fbuf)) >= 0) {
	    cchar	*suf = IPASSWD_SUF ;
	    cchar	*endstr = ENDIANSTR ;
	    if ((rs = mkfnamesuf(fbuf,idxdname,suf,endstr)) >= 0) {
		if (USTAT sb ; (rs = u_stat(fbuf,&sb)) >= 0) {
		    custime	dt = getustime ;
		    time_t	ti_pwi = sb.st_mtime ;
		    bool	fmk = false ;
		    fmk = fmk || ((dt - ti_pwi) >= to) ;
	            if ((! fmk) && ((rs = u_stat(UCENTPW_FNAME,&sb)) >= 0)) {
			fmk = (sb.st_mtime > ti_pwi) ;
	            } /* end if (checking against system PASSWD file) */
		    if ((rs >= 0) && fmk) {
	                rs = mk() ;
		    }
	        } else if (isNotPresent(rs)) {
	            rs = mk() ;
		}
	    } /* end if (mkfnamesuf) */
	    rs = rsfree(rs,fbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (opener::decide) */

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
    	cint		nodelen = var.nodenamelen ;
	int		rs ;
	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    int		ai = 0 ;
	    cint	sz = ((nodelen + 1) * 2) ;
	    if (char *a{} ; (rs = uc_malloc(sz,&a)) >= 0) {
	        cint	nlen = nodelen ;
	        cint	clen = nodelen ;
	        char	*nbuf = (a + (ai++ * (nodelen + 1))) ;
	        char	*cbuf = (a + (ai++ * (nodelen + 1))) ;
	        if ((rs = getnodename(nbuf,nlen)) >= 0) {
	            cint	rsn = SR_NOTFOUND ;
	            cchar	*nn ;
	            if ((rs = prgetclustername(pr,cbuf,clen,nbuf)) >= 0) {
	                nn = cbuf ;
		    } else if (rs == rsn) {
		        rs = SR_OK ;
		        nn = nbuf ;
		    }
		    if (rs >= 0) {
		        if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	                    if ((rs = mkpath(tbuf,pr,DBDNAME,nn)) >= 0) {
	    		        rs = idxload(tbuf,rs) ;
		            }
		    	    rs = rsfree(rs,tbuf) ;
		        } /* end if (m-a-f) */
		    } /* end if (ok) */
	        } /* end if (getnodename) */
	    	rs = rsfree(rs,a) ;
	    } /* end if (m-a-f) */
	} else {
	    rs = idxload(dbname) ;
	} /* end if (empty specification) */
	return rs ;
}
/* end method (opener::mkidxdname) */

int opener::idxload(cc *dp,int dl) noex {
	int		rs ;
	if (cchar *cp{} ; (rs = uc_mallocstrw(dp,dl,&cp)) >= 0) {
	    idxdname = cp ;
	}
	return rs ;
}
/* end method (opener::idxload) */

int opener::mk() noex {
        int		rs ;
        int		rs1 ;
        int		rv = 0 ;
	if (char *pbuf{} ; (rs = malloc_mp(&pbuf)) >= 0) {
	    pbuf[0] = '\0' ;
            if ((rs = mkbegin(pbuf)) >= 0) {
    		{
		    rs = mkproc(pbuf) ;
		    rv = rs ;
		} /* end block */
	        rs1 = mkend() ;
	        if (rs >= 0) rs = rs1 ;
            } /* end if (mk) */
	    rs = rsfree(rs,pbuf) ;
	} /* end if (m-a-f) */
        return (rs >= 0) ? rv : rs ;
}
/* end method (opener::mk) */

int opener::mkbegin(char *pbuf) noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (ids id ; (rs = id.load) >= 0) {
	    for (int i = 0 ; prbins[i] != nullptr ; i += 1) {
	        if ((rs = mkpath(pbuf,pr,prbins[i],progmkpwi)) >= 0) {
		    const uid_t		u = id.uid ;
		    const gid_t		g = id.gid ;
		    const gid_t		*gids = id.gids ;
	            if ((rs = perm(pbuf,u,g,gids,X_OK)) >= 0) {
			len = rs ;
		    } else if (isNotPresent(rs)) {
			rs = SR_OK ;
		    }
	        } /* end if (mkpath) */
	        if (rs != 0) break ;
	    } /* end for */
	    rs1 = id.release ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? len : rs ;
}
/* end method (opener::mkbegin) */

int opener::mkend() noex {
    	return SR_OK ;
}
/* end method (opener::mkend) */

int opener::mkproc(cchar *pbuf) noex {
	cint		vn = 10 ;
	cint		vo = VECSTR_OCOMPACT ;
	int		rs ;
	int		rs1 ;
	int		cpid = 0 ;
	if (vecstr envs ; (rs = vecstr_start(&envs,vn,vo)) >= 0) {
	    if (char *abuf{} ; (rs = malloc_mn(&abuf)) >= 0) {
		cint	alen = rs ;
	        int	ai = 0 ;
	        cchar	*av[10] ;
		cchar	*cp{} ;
		cchar	*argz = progmkpwi ;
		if (int cl ; (cl = sfbasename(progmkpwi,-1,&cp)) > 0) {
		    argz = abuf ;
		    strwcpyuc(abuf,cp,min(cl,alen)) ;
		}
		/* setup arguments */
	        av[ai++] = argz ;
	        av[ai++] = nullptr ;
		/* setup environment */
		if ((rs = mkenv(&envs)) >= 0) {
		    if ((rs = mkspawn(pbuf,av,&envs)) >= 0) {
		        cint	cpid = rs ;
			rs = mkwait(cpid) ;
		    }
	        } /* end if (mkenv) */
		rs = rsfree(rs,abuf) ;
	    } /* end if (m-a-f) */
	    rs1 = envs.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? cpid : rs ;
}
/* end method (opener::mkproc) */

int opener::mkspawn(cchar *pbuf,mainv av,vecstr *elp) noex {
    	int		rs ;
	int		cpid = 0 ;
	if (mainv ev{} ; (rs = elp->getvec(&ev)) >= 0) {
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
	    rs = spawnproc(&ps,pbuf,av,ev) ;
	    cpid = rs ;
	} /* end if (vecstr_getvec) */
	return (rs >= 0) ? cpid : rs ;
}
/* end method (opener::mkspawn) */

int opener::mkwait(int cpid) noex {
    	int		rs = SR_OK ;
	int		cstat = 0 ;
	while (rs == 0) {
	     rs = u_waitpid(cpid,&cstat,0) ;
	     if (rs == SR_INTR) rs = 0 ;
	} /* end while */
	if (rs >= 0) {
	    if (WIFSIGNALED(cstat)) {
	        rs = SR_UNATCH ;	/* protocol not attached */
	    } else if (WIFEXITED(cstat)) {
		if (int cex = WEXITSTATUS(cstat) ; cex != 0) {
	      	    rs = SR_LIBBAD ;
	        } /* end if (wait-exited) */
	    } /* end if (process finished) */
	} /* end if (ok) */
	return rs ;
}
/* end method (opener::mkwait) */

int opener::mkenv(vecstr *elp) noex {
        int		rs ;
        cchar		*vn = VARPRPWI ;
        if ((rs = elp->envadd(vn,pr)) >= 0) {
	    if (idxdname) {
	        rs = elp->envadd(VARDBNAME,idxdname) ;
	    }
	    for (int i = 0 ; (rs >= 0) && exports[i] ; i += 1) {
	        if (cc *valp ; (valp = getenv(exports[i])) != nullptr) {
	            rs = elp->envadd(exports[i],valp) ;
		}
	    } /* end for */
	}
	return rs ;
}
/* end method (opener:mkenv) */

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
/* end subroutine (mkvars) */


