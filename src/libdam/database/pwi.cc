/* pwi SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* PassWord Index manager */
/* version %I% last-modified %G% */

#define	CF_PWCACHE	1		/* use |ucpwcache(3uc)| */
#define	CF_ONLYUNIQ	1		/* only allow unique names on lookup */

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
	some front-end glue for using the IPASSWD object on an
	IPASSWD database.

	Notes:

	= Searching for a PWI DB file:

	If a PWI DB name is passed to us, we only search for that
	DB.  If no PWI DB is passwed, we search first for a DB with
	the same name as our cluster name (if we have one); otherwise
	failing that we search for a DB with our node name.  If no
	DB is present then we either make (a-fresh) the DB given
	to us by name, or we make a DB using our cluster name.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucpwcache.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mkfname.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC |sfbasename(3uc)| */
#include	<snx.h>			/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<mkfnamesuf.h>		/* LIBUC */
#include	<mkgecosname.h>		/* LIBUC */
#include	<realname.h>		/* LIBUC */
#include	<ipasswd.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<prgetclustername.h>	/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* |REALNAMELEN| */

#include	"pwi.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;

/* local defines */

#define	DBDNAME		"var/pwi"

#ifndef	UCENTPW_FNAME
#define	UCENTPW_FNAME	"/etc/passwd"
#endif

#define	TO_FILEMOD	(24 * 3600)

#define	PROG_MKPWI	"mkpwi"

#undef	VARDBNAME
#define	VARDBNAME	"MKPWI_DBNAME"

#undef	VARPRPWI
#define	VARPRPWI	"MKPWI_PROGRAMROOT"

#ifndef	CF_ONLYUNIQ
#define	CF_ONLYUNIQ	1		/* only allow unique names on lookup */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct opener_fl {
	uint		dbname:1 ;	/* allocated */
} ; /* end struct */

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
} /* end namespace */

namespace {
    struct pwdesc {
	ucentpw		*pwp ;
	char		*pwbuf ;
	int		pwlen ;
	pwdesc(ucentpw *p,char *pb,int pl) noex : pwp(p) {
	    pwbuf = pb ;
	    pwlen = pl ;
	} ; /* end ctor */
    } ; /* end struct (pwdesc) */
} /* end namespace */

namespace {
    struct vars {
    	int		maxpathlen ;
	int		maxnamelen ;
	int		nodenamelen ;
	int		usernamelen ;
	int		realnamelen ;
	int mkvars() noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int pwi_ctor(pwi *op,Args ... args) noex {
    	PWI		*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->dbp = new(nt) ipasswd) != np) {
		rs = SR_OK ;
	    } /* end if (new-ipasswd) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pwi_ctor) */

local int pwi_dtor(pwi *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->dbp) ylikely {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pwi_dtor) */

template<typename ... Args>
local inline int pwi_magic(pwi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PWI_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (pwi_magic) */

local int	realname_isextra(realname *,pwdesc *,cchar *) noex ;


/* local variables */

constexpr cpcchar	exports[] = {
	varname.node,
	varname.home,
	varname.username,
	varname.logname,
	varname.tz,
	varname.pwd,
	nullptr
} ; /* end array */

constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ; /* end array */

constexpr char		extras[] = "°¹²³" ;
constexpr char		progmkpwi[] = PROG_MKPWI ;

constexpr bool		f_onlyuniq = CF_ONLYUNIQ ;

static vars		var ;


/* exported variables */


/* exported subroutines */

int pwi_open(pwi *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = pwi_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
	        if (static cint	rsv = var.mkvars() ; (rs = rsv) >= 0) ylikely {
		    opener	so(pr,dbname) ;
		    if ((rs = so.start()) >= 0) ylikely {
	    	        if ((rs = so.mkidxdname()) >= 0) ylikely {
			    if ((rs = so.decide()) >= 0) ylikely {
				cc *sfn = so.idxdname ;
				if ((rs = ipasswd_open(op->dbp,sfn)) >= 0) {
				    rv = rs ;
	    			    op->magval = PWI_MAGIC ;
				}
			    }
	                } /* end if (opener_midname) */
	                rs1 = so.finish() ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (opener) */
	        } /* end if (vars::mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		pwi_dtor(op) ;
	    }
	} /* end if (pwi_ctor) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (pwi_open) */

int opener::decide() noex {
	cint		to = TO_FILEMOD ;
	int		rs ;
	int		rs1 ;
	if (char *fbuf ; (rs = libmem.mp(&fbuf)) >= 0) ylikely {
	    cchar	*suf = IPASSWD_SUF ;
	    cchar	*endstr = ENDIANSTR ;
	    if ((rs = mkfnamesuf(fbuf,idxdname,suf,endstr)) >= 0) {
		if (ustat sb ; (rs = u_stat(fbuf,&sb)) >= 0) {
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
	    rs1 = libmem.free(fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (opener::decide) */

int pwi_close(pwi *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pwi_magic(op)) >= 0) ylikely {
	    {
	        rs1 = ipasswd_close(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = pwi_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (pwi_close) */

namespace {
    struct lookuper {
	pwi		*op ;
	cchar		*name ;
	char		*rbuf ;
	int		rlen ;
	lookuper(pwi *o,char *rb,int rl,cc *n) noex : op(o) {
	    name = n ;
	    rbuf = rb ;
	    rlen = rl ;
	} ; /* end ctor */
	int operator () (char *,int) noex ;
	int proc(cchar *,int) noex ;
    } ; /* end struct (lookuper) */
} /* end namespace */

int pwi_lookup(pwi *op,char *rbuf,int rlen,cchar *name) noex {
    	int		rs ;
	int		rs1 ;
	int		ul = 0 ;
	if ((rs = pwi_magic(op,rbuf,name)) >= 0) ylikely {
	    cint	nl = lenstr(name) ;
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (name[0]) {
		cint	nlen = min(REALNAMELEN,nl) ;
		if (char *nbuf ; (rs = libmem.mall((nlen+1),&nbuf)) >= 0) {
		    lookuper	lo(op,rbuf,rlen,name) ;
		    {
		        rs = lo(nbuf,nlen) ;
		        ul = rs ;
		    }
		    rs1 = libmem.free(nbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? ul : rs ;
} /* end subroutine (pwi_lookup) */

int lookuper::operator () (char *nbuf,int nlen) noex {
    	int		rs = SR_OK ;
	int		ul = 0 ;
	int		sl = -1 ;
	cchar		*sp = name ;
	if (hasuc(name,-1)) {
	    sp = nbuf ;
	    rs = sncpylc(nbuf,nlen,name) ;
	    sl = rs ;
	}
	if (rs >= 0) {
	    rs = proc(sp,sl) ;
	    ul = rs ;
	} /* end if (ok) */
	return (rs >= 0) ? ul : rs ;
} /* end method (lookuper::operator) */

int lookuper::proc(cchar *sp,int sl) noex {
    	int		rs ;
	int		rs1 ;
	int		ul = 0 ;
	int		c = 0 ;
	if (char *pwbuf ; (rs = libmem.pw(&pwbuf)) >= 0) ylikely {
	    ucentpw	pw ;
	    cint	pwlen = rs ;
	    if (realname rn ; (rs = rn.start(sp,sl)) >= 0) ylikely {
		ipasswd		*iop = op->dbp ;
		auto		ip_cb = ipasswd_curbegin ;
		if (ipasswd_cur	cur ; (rs = ip_cb(iop,&cur)) >= 0) ylikely {
		    if (char *un ; (rs = libmem.un(&un)) >= 0) ylikely {
			pwdesc	pd(&pw,pwbuf,pwlen) ;
		        cint	fopts = 0 ;
	                while (rs >= 0) {
	                    rs1 = ipasswd_fetch(op->dbp,&rn,&cur,fopts,un) ;
	                    if (rs1 == SR_NOTFOUND) break ;
			    rs = rs1 ;
			    if (rs >= 0) {
			        if ((rs = realname_isextra(&rn,&pd,un)) == 0) {
	                            c += 1 ;
	                            rs = sncpy1(rbuf,rlen,un) ;
				    ul = rs ;
			        }
			    }
	                } /* end while */
			rs1 = libmem.free(un) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	            rs1 = ipasswd_curend(op->dbp,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	        rs1 = rn.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (realname) */
	    rs1 = libmem.free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	if_constexpr (f_onlyuniq) {
	    if ((rs >= 0) && (c > 1)) {
	        rs = SR_NOTUNIQ ;
	    }
	} /* end if (f_onlyuniq) */
	return (rs >= 0) ? ul : rs ;
} /* end method (lookuper::proc) */


/* private subroutines */

int opener::start() noex {
    	int		rs = SR_FAULT ;
	if (pr && dbname) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end method (opener::start) */

int opener::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (idxdname) ylikely {
	    voidp vp = voidp(idxdname) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    idxdname = nullptr ;
	} /* end if (memory-release) */
	if (fl.dbname && dbname) ylikely {
	    voidp vp = voidp(dbname) ;
	    rs1 = libmem.free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    dbname = nullptr ;
	    fl.dbname = false ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (opener::finish) */

int opener::mkidxdname() noex {
    	cint		nodelen = var.nodenamelen ;
	int		rs ;
	int		rs1 ;
	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    int		ai = 0 ;
	    cint	sz = ((nodelen + 1) * 2) ;
	    if (char *a ; (rs = libmem.mall(sz,&a)) >= 0) ylikely {
	        cint	nlen = nodelen ;
	        cint	clen = nodelen ;
	        char	*nbuf = (a + (ai++ * (nodelen + 1))) ;
	        char	*cbuf = (a + (ai++ * (nodelen + 1))) ;
	        if ((rs = getnodename(nbuf,nlen)) >= 0) ylikely {
	            cint	rsn = SR_NOTFOUND ;
	            cchar	*nn ;
	            if ((rs = prgetclustername(pr,cbuf,clen,nbuf)) >= 0) {
	                nn = cbuf ;
		    } else if (rs == rsn) {
		        rs = SR_OK ;
		        nn = nbuf ;
		    }
		    if (rs >= 0) {
		        if (char *tbuf ; (rs = libmem.mp(&tbuf)) >= 0) {
	                    if ((rs = mkpath(tbuf,pr,DBDNAME,nn)) >= 0) {
	    		        rs = idxload(tbuf,rs) ;
		            }
		    	    rs1 = libmem.free(tbuf) ;
			    if (rs >= 0) rs = rs1 ;
		        } /* end if (m-a-f) */
		    } /* end if (ok) */
	        } /* end if (getnodename) */
	    	rs1 = libmem.free(a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = idxload(dbname) ;
	} /* end if (empty specification) */
	return rs ;
} /* end method (opener::mkidxdname) */

int opener::idxload(cc *dp,int dl) noex {
	int		rs ;
	if (cchar *cp ; (rs = libmem.strw(dp,dl,&cp)) >= 0) {
	    idxdname = cp ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end method (opener::idxload) */

int opener::mk() noex {
        int		rs ;
        int		rs1 ;
        int		rv = 0 ;
	if (char *pbuf ; (rs = libmem.mp(&pbuf)) >= 0) ylikely {
	    pbuf[0] = '\0' ;
            if ((rs = mkbegin(pbuf)) >= 0) ylikely {
    		{
		    rs = mkproc(pbuf) ;
		    rv = rs ;
		} /* end block */
	        rs1 = mkend() ;
	        if (rs >= 0) rs = rs1 ;
            } /* end if (mk) */
	    rs1 = libmem.free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
        return (rs >= 0) ? rv : rs ;
} /* end method (opener::mk) */

int opener::mkbegin(char *pbuf) noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (ids id ; (rs = id.load) >= 0) ylikely {
	    for (int i = 0 ; prbins[i] != nullptr ; i += 1) {
	        if ((rs = mkpath(pbuf,pr,prbins[i],progmkpwi)) >= 0) ylikely {
		    const uid_t		u = id.uid ;
		    const gid_t		g = id.gid ;
		    const gid_t		*gids = id.gids ;
		    len = rs ;		/* save length for return */
	            if ((rs = perm(pbuf,u,g,gids,X_OK)) >= 0) {
			rs = int(true) ; /* <- signal exit from loop */
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
} /* end method (opener::mkbegin) */

int opener::mkend() noex {
    	return SR_OK ;
} /* end method (opener::mkend) */

int opener::mkproc(cchar *pbuf) noex {
	cint		vn = 10 ;
	cint		vo = vecstrm.compact ;
	int		rs ;
	int		rs1 ;
	int		cpid = 0 ; /* return-value */
	if (vecstr envs ; (rs = vecstr_start(&envs,vn,vo)) >= 0) ylikely {
	    if (char *abuf ; (rs = libmem.mn(&abuf)) >= 0) ylikely {
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
		        cpid = rs ;
			rs = mkwait(cpid) ;
		    }
	        } /* end if (mkenv) */
		rs1 = libmem.free(abuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = envs.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? cpid : rs ;
} /* end method (opener::mkproc) */

int opener::mkspawn(cchar *pbuf,mainv av,vecstr *elp) noex {
    	int		rs ;
	int		cpid = 0 ;
	if (mainv ev{} ; (rs = elp->getvec(&ev)) >= 0) ylikely {
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
} /* end method (opener::mkspawn) */

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
} /* end method (opener::mkwait) */

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
	} /* end if */
	return rs ;
} /* end method (opener:mkenv) */

local int realname_isextra(realname *op,pwdesc *pdp,cchar *un) noex {
	int		rs ;
	int		f = false ;
	if (cchar *lp{} ; (rs = realname_getlast(op,&lp)) >= 0) ylikely {
	    cint	ll = rs ;
	    if (strnbrk(lp,ll,extras) == nullptr) {
		ucentpw		*pwp = pdp->pwp ;
		cint		pwlen = pdp->pwlen ;
		char		*pwbuf = pdp->pwbuf ;
		if ((rs = getpwx_name(pwp,pwbuf,pwlen,un)) > 0) ylikely {
		    cchar	*gecos = pwp->pw_gecos ;
		    if (cchar *sp{} ; (rs = getgecosname(gecos,-1,&sp)) > 0) {
			f = (strnbrk(sp,rs,extras) != nullptr) ;
		    }
		} else if (rs == SR_NOTFOUND) {
		    rs = SR_OK ;
		}
	    } /* end if (query does not have special extras) */
	} /* end if (realname_getlast) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (realname_isextra) */

int vars::mkvars() noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	    if ((rs = bufsizeget(bufsize_mn)) >= 0) ylikely {
	        maxnamelen = rs ;
	        if ((rs = bufsizeget(bufsize_nn)) >= 0) ylikely {
		    nodenamelen = rs ;
	            if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
			usernamelen = rs ;
			realnamelen = REALNAMELEN ;
		    }
		}
	    }
	}
	return rs ;
} /* end method (vars::mkvars) */


