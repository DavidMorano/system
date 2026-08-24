/* ucopendialer (open-dialer-service) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open a dialer */
/* version %I% last-modified %G% */

#define	CF_GETEXECNAME	1		/* use 'getexecname(3c)' */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_opendialer

	Description:
	Dialer services are represented as files with names of the form:
		<dialer>¥<svc>[­<arg(s)>
	These sorts of file names are often actually stored in the
	filesystem as symbolic links.

	Synopsis:
	int uc_opendialer(cc *prn,cc *svc,int of,m_t om,mv argv,mv envv,int to)

	Arguments:
	prn		facility name
	svc		service name
	of		open-flags
	om		open-mode
	argv		array of arguments
	envv		attay of environment
	to		time-out

	Returns:
	>=0		file-descriptor
	<0		error (system-return)

	Dialer services are implemented with loadable shared-object
	files. Each service has a file of the same name as the
	service name itself. The file is a shared-object with a
	global symbol of a callable subroutine with the name
	'opendialer_<svc>' where <svc> is the service name. The
	subroutine looks like:

	int opendialer_<svc>(pr,prn,of,om,argv,envv,to)
	cchar	*pr ;
	cchar	*prn ;
	int		of ;
	mode_t		om ;
	cchar	*argv[] ;
	cchar	*envv[] ;

	Multiple services can be actually implemented in the same
	shared-object.  But the actual file of that object should
	be linked to other files, each with the filename of a service
	to be implemented. These links are required because this
	code only searches for services by searching for files with
	the names of the services.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<dlfcn.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<mkpr.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<permx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	SVCDNAME	"lib/opendialers"
#define	SVCSYMPREFIX	"opendialer_"


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

typedef mainv		mv ;
typedef mode_t		m_t ;


/* external subroutines */


/* external variables */


/* local structures */

extern "C" {
    typedef int (*subdialer_t)(cc *,cc *,cc *,int,m_t,mv,mv,int) noex ;
} /* end extern (C) */

namespace {
    struct vars {
	int		maxnamelen ;
	int		maxpathlen ;
	int		hostnamelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
    struct subinfo ;
    struct subinfo_flags {
	uint		dummy:1 ;
    } ; /* end struct (subinfo_flags) */
    enum subinfomems {
    	subinfomem_start,
	subinfomem_finish,
	subinfomem_search,
	subinfomem_idbeg,
	subinfomem_idend,
	subinfomem_buf,
	subinfomem_overlast
    } ; /* end enum (subinfomems) */
    struct subinfo_co {
	subinfo		*op = nullptr ;
	int		w = -1 ;
	void operator () (subinfo *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
    } ; /* end struct (subinfo_co) */
    struct subinfo {
	friend		subinfo_co ;
	subinfo_co	start ;
	subinfo_co	finish ;
	subinfo_co	search ;
	subinfo_co	idbeg ;
	subinfo_co	idend ;
	subinfo_co	buf ;
	cchar		*prn ;
	cchar		*svc ;
	char		*dialsym ;	/* memory-allocated */
	mainv		argv ;
	mainv		envv ;
	ids		id ;
	SI_FL		fl ;
	int		of ;
	int		to ;
	int		fd ;
	mode_t		om ;
	subinfo(cc *p,cc *s,int f,m_t m,mainv a,mainv e,int t) noex {
	    start	(this,subinfomem_start) ;
	    finish	(this,subinfomem_finish) ;
	    search	(this,subinfomem_search) ;
	    idbeg	(this,subinfomem_idbeg) ;
	    idend	(this,subinfomem_idend) ;
	    buf		(this,subinfomem_buf) ;
	    prn		= p ;
	    svc		= s ;
	    of		= f ;
	    om		= m ;
	    argv	= a ;
	    envv	= e ;
	    to		= t ;
	    fd		= -1 ;
	    dialsym	= nullptr ;
	} /* end ctor */
	int exts	(cchar *,cchar *,char *) noex ;
	int searchlib	(cchar *,cchar *) noex ;
	int searchcall	(cchar *,subdialer_t) noex ;
	int environ	() noex ;
	int proc	(char *,char *,char *,cc *) noex ;
    private:
	int ºstart	() noex ;
	int ºfinish	() noex ;
	int ºsearch	() noex ;
	int ºidbeg	() noex ;
	int ºidend	() noex ;
	int ºbuf	() noex ;
    } ; /* end struct (subinfo) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr cpcchar	prns[] = {
	"usrlocal",
	"local",
	"extra",
	"preroot",
	nullptr
} ; /* end array (prns) */

constexpr cpcchar	soexts[] = {
	"so",
	"o",
	"dylib",
	"",
	nullptr
} ; /* end array (soexts) */

static vars		var ;


/* exported variables */


/* exported subroutines */

int uc_opendialer(cc *prn,cc *svc,int of,m_t om,mv av,mv ev,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	if (prn && svc) ylikely {
	    rs = SR_INVALID ;
	    if (prn[0] && svc[0]) ylikely {
	        if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		    subinfo si(prn,svc,of,om,av,ev,to) ;
		    if ((rs = si.start) >= 0) {
	                if ((rs = si.search) > 0) { /* >0 means found */
		            fd = si.fd ;
	                } else if (rs == 0) {
		            rs = SR_NOENT ;
	                }
	                rs1 = si.finish ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (subinfo) */
	            if ((rs < 0) && (fd >= 0)) {
			u_close(fd) ;
		    } /* end if (error) */
	        } /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (uc_opendialer) */


/* local subroutines */

int subinfo::ºstart() noex {
	ccharp		prefix = SVCSYMPREFIX ;
	int		rs ;
	int		rs1 ;
	    if ((rs = environ()) >= 0) ylikely {
		if (charp dbuf ; (rs = mem.mn(&dbuf)) >= 0) {
		    cint dlen = rs ;
	            if ((rs = sncpy(dbuf,dlen,prefix,prn)) >= 0) {
		        cchar	*sp = dbuf ;
	                if (cchar *cp ; (rs = mem.strw(sp,rs,&cp)) >= 0) {
	                    dialsym = const_cast<charp>(cp) ;
		        } /* end if (memory-acquire) */
	            } /* end if (sncpy) */
		    rs1 = mem.free(dbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (subifo::environ) */
	return rs ;
} /* end method (subinfo::ºstart) */

int subinfo::ºfinish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (dialsym) {
	    rs1 = mem.free(dialsym) ;
	    if (rs >= 0) rs = rs1 ;
	    dialsym = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (subinfo::ºfinish) */

int subinfo::ºsearch() noex {
    	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if ((rs = idbeg) >= 0) {
	    {
		rs = buf ;
		f = rs ;
	    }
	    rs1 = idend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo-id) */
	if ((rs < 0) && (fd >= 0)) {
	    u_close(fd) ;
	    fd = -1 ;
	} /* end if (error) */
	return (rs >= 0) ? f : rs ;
} /* end method (subinfo::ºsearch) */

int subinfo::ºbuf() noex {
    	cnullptr	np{} ;
	cint		maxpath = var.maxpathlen ;
	cint		sz = ((var.maxpathlen + 1) * 3) ;
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	int		ai = 0 ;
	if (char	*a ; (rs = mem.mall(sz,&a)) >= 0) {
	    char	*pdn = (a + (ai++ * (maxpath + 1))) ;
	    char	*sdn = (a + (ai++ * (maxpath + 1))) ;
	    char	*sfn = (a + (ai++ * (maxpath + 1))) ;
	    if (charp dn ; (rs = mem.hostname(&dn)) >= 0) {
	        if ((rs = getnodedomain(np,dn)) >= 0) {
		    rs = proc(pdn,sdn,sfn,dn) ;
		    f = rs ;
	        } /* end if */
		rs1 = mem.free(dn) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f : rs ;
} /* end method (subinfo::ºbuf) */

int subinfo::proc(char *pdn,char *sdn,char *sfn,cc *dn) noex {
    	cint		plen = var.maxpathlen ;
    	int		rs = SR_OK ;
	int		f = false ;
		    ustat	sb ;
	            for (int i = 0 ; prns[i] ; i += 1) {
	                if ((rs = mkpr(pdn,plen,prns[i],dn)) > 0) {
		            if ((rs = mkpath(sdn,pdn,SVCDNAME)) >= 0) {
	        		if ((rs = u_stat(sdn,&sb)) >= 0) {
				    if (S_ISDIR(sb.st_mode)) {
	                                rs = exts(pdn,sdn,sfn) ;
	                                f = rs ;
				    } /* end if */
				} else if (isNotPresent(rs)) {
		    		    rs = SR_OK ;
				}
			    } /* end if (mkpath) */
	                } /* end if (mkpr) */
			if (f) break ;
			if (rs < 0) break ;
	            } /* end for (prns) */
	return (rs >= 0) ? f : rs ;
} /* end method (subinfo::proc) */

int subinfo::exts(cchar *pr,cchar *sdn,char *sfn) noex {
	cint		am = (R_OK | X_OK) ;
	int		rs = SR_OK ;
	int		f = false ;
	for (int i = 0 ; soexts[i] ; i += 1) {
	    if ((rs = mksofname(sfn,sdn,prn,soexts[i])) >= 0) {
		if (ustat sb ; (rs = u_stat(sfn,&sb)) >= 0) {
		    if (S_ISREG(sb.st_mode)) {
	                if ((rs = permids(&id,&sb,am)) >= 0) {
			    rs = searchlib(pr,sfn) ;
			    f = rs ;
			} else if (rs == SR_ACCESS) {
			    rs = SR_OK ;
			}
		    } /* end if (regular file) */
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
	    } /* end if (mksofname) */
	    if (f) break ;
	    if (rs < 0) break ;
	} /* end for (soexts) */
	return (rs >= 0) ? f : rs ;
} /* end method (subinfo::exts) */

int subinfo::searchlib(cchar *pr,cchar *sfn) noex {
    	cnullptr	np{} ;
	cint		dlmode = RTLD_LAZY ;
	int		rs = SR_OK ;
	int		f = false ; /* return-value */
	if (void *sop = dlopen(sfn,dlmode) ; sop) {
	    subdialer_t	symp ;
	    if ((symp = (subdialer_t) dlsym(sop,dialsym)) != np) {
		rs = searchcall(pr,symp) ;
		f = rs ;
	    } /* end if (dlsym) */
	    dlclose(sop) ;
	} /* end if (dlopen) */
	return (rs >= 0) ? f : rs ;
} /* end method (subinfo::searchlib) */

int subinfo::searchcall(cchar *pr,subdialer_t symp) noex {
	int		rs ;
	int		f = false ;
	if ((rs = (*symp)(pr,prn,svc,of,om,argv,envv,to)) >= 0) {
	    fd = rs ;
	    f = true ;
	} /* end if (call) */
	return (rs >= 0) ? f : rs ;
} /* end method (subinfo::searchcall) */

int subinfo::ºidbeg() noex {
	return id.load() ;
} /* end method (subinfo::ºidbeg) */

int subinfo::ºidend() noex {
    	return id.release() ;
} /* end method (subinfo::ºidend) */

int subinfo::environ() noex {
    	int	rs = SR_OK ;
	if (envv == nullptr) {
	    if (mainv ev ; (rs = u_getenviron(&ev)) >= 0) {
		envv = ev ;
	    } /* end if (u_getenviron) */
	} /* end if (needed) */
	return rs ;
} /* end method (subinfo::environ) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mn)) >= 0) {
	    maxnamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) {
		maxpathlen = rs ;
	        if ((rs = bufsizeget(bufsize_hostname)) >= 0) {
		    hostnamelen = rs ;
	        }
	    } /* end if */
	} /* end if */
	return rs ;
} /* end method (vars::operator) */

subinfo_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case subinfomem_start:
	        rs = op->ºstart() ;
	        break ;
	    case subinfomem_finish:
	        rs = op->ºfinish() ;
	        break ;
	    case subinfomem_search:
	        rs = op->ºsearch() ;
	        break ;
	    case subinfomem_idbeg:
	        rs = op->ºidbeg() ;
	        break ;
	    case subinfomem_idend:
	        rs = op->ºidend() ;
	        break ;
	    case subinfomem_buf:
	        rs = op->ºbuf() ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (subinfo_co::operator) */


