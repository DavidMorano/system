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
	int uc_opendialer(prn,svc,of,om,argv,envv,to)
	cchar	prn[] ;
	cchar	svc[] ;
	int		of ;
	mode_t		om ;
	cchar	**argv[] ;
	cchar	**envv[] ;
	int		to ;

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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucopen.h>
#include	<getnodedomain.h>
#include	<ids.h>
#include	<sncpyx.h>
#include	<mkx.h>
#include	<mkpr.h>
#include	<mkpathx.h>
#include	<permx.h>
#include	<isnot.h>
#include	<localmisc.h>

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	SVCDNAME	"lib/opendialers"
#define	SVCSYMPREFIX	"opendialer_"

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;
typedef mode_t		m_t ;


/* external subroutines */


/* external variables */


/* local structures */

extern "C" {
    typedef int (*subdialer_t)(cc *,cc *,cc *,int,mode_t,mainv,mainv,int) noex ;
}

struct subinfo_flags {
	uint		dummy:1 ;
} ; /* end struct (subinfo_flags) */

struct subinfo {
	ids		id ;
	SI_FL		fl ;
	cchar		*prn ;
	cchar		*svc ;
	char		*dialsym ;	/* memory-allocated */
	mainv		argv ;
	mainv		envv ;
	int		of ;
	int		to ;
	int		fd ;
	mode_t		om ;
} ; /* end struct (subinfo) */


/* forward references */

local int	subinfo_start(SI *,cchar *,cchar *,
			int,mode_t,mainv,mainv,int) noex ;
local int	subinfo_finish(SI *) noex ;
local int	subinfo_search(SI *) noex ;
local int	subinfo_exts(SI *,cchar *,cchar *,char *) noex ;
local int	subinfo_searchlib(SI *,cchar *,cchar *) noex ;
local int	subinfo_searchcall(SI *,cchar *,subdialer_t) noex ;
local int	subinfo_idbegin(SI *) noex ;
local int	subinfo_idend(SI *) noex ;
local int	subinfo_envv(SI *op,mainv) noex ;


/* local variables */

constexpr cpcchar		prns[] = {
	"extra",
	"preroot",
	nullptr
} ; /* end array (prns) */

constexpr cpcchar		soexts[] = {
	"so",
	"o",
	"",
	nullptr
} ; /* end array (soexts) */


/* exported variables */


/* exported subroutines */

int uc_opendialer(cc *prn,cc *svc,int of,m_t om,mv argv,mv envv,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	if (prn && svc) {
	    rs = SR_INVALID ;
	    if (prn[0] && svc[0]) {
	        SI si, *sip = &si ;
	        if ((rs = subinfo_start(&si,prn,svc,of,om,argv,envv,to)) >= 0) {
	            if ((rs = subinfo_search(&si)) > 0) { /* >0 means found */
		        fd = sip->fd ;
	            } else if (rs == 0) {
		        rs = SR_NOENT ;
	            }
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	            if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_opendialer) */


/* local subroutines */

local int subinfo_start(SI *sip,cc *prn,cc *svc,int of,mode_t om,
		mainv argv,mainv ev,int to) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	   if ((rs = memclear(sip)) >= 0) ylikely {
	       if ((rs = subinfo_envv(sip,ev)) >= 0) ylikely {
	           sip->prn = prn ;
	           sip->svc = svc ;
	           sip->argv = argv ;
	           sip->of = of ;
	           sip->om = om ;
	           sip->to = to ;
	           sip->fd = -1 ;
	           {
		       cint	mn = MAXNAMELEN ;
	               cchar	*prefix = SVCSYMPREFIX ;
	               char	dialsym[mn +1] ;
	               if ((rs = sncpy2(dialsym,mn,prefix,sip->prn)) >= 0) {
		           cchar	*sp = dialsym ;
	                   if (cchar *cp ; (rs = lm_strw(sp,rs,&cp)) >= 0) {
	                       sip->dialsym = const_cast<char *>(cp) ;
		           }
	               }
	            } /* end block */
	        } /* end if (memclear) */
	    } /* end if (subifo_envv) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->dialsym) {
	    rs1 = lm_free(sip->dialsym) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dialsym = nullptr ;
	}
	return rs ;
} /* end subroutine (subinfo_finish) */

local int subinfo_search(SI *sip) noex {
	cint	plen = MAXPATHLEN ;
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if ((rs = subinfo_idbegin(sip)) >= 0) {
	    cint	sz = ((plen+1)*3) ;
	    int		ai = 0 ;
	    if (char	*abuf ; (rs = lm_mall(sz,&abuf)) >= 0) {
	        char	dn[MAXHOSTNAMELEN+1] ;
	        char	*pdn = (abuf+(ai++*(plen+1))) ;
	        char	*sdn = (abuf+(ai++*(plen+1))) ;
	        char	*sfn = (abuf+(ai++*(plen+1))) ;
	        if ((rs = getnodedomain(nullptr,dn)) >= 0) {
		    ustat	sb ;
	            for (int i = 0 ; prns[i] != nullptr ; i += 1) {
	                if ((rs = mkpr(pdn,plen,prns[i],dn)) > 0) {
		            if ((rs = mkpath2(sdn,pdn,SVCDNAME)) >= 0) {
	        		if ((rs = u_stat(sdn,&sb)) >= 0) {
				    if (S_ISDIR(sb.st_mode)) {
	                                rs = subinfo_exts(sip,pdn,sdn,sfn) ;
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
	        } /* end if */
	        rs1 = lm_free(abuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ma-a) */
	    rs1 = subinfo_idend(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo-id) */
	if ((rs < 0) && (sip->fd >= 0)) {
	    u_close(sip->fd) ;
	    sip->fd = -1 ;
	}
	return (rs >= 0) ? f : rs ;
} /* end subroutine (subinfo_search) */

local int subinfo_exts(SI *sip,cchar *pr,cchar *sdn,char *sfn) noex {
	ustat		sb ;
	cint		am = (R_OK|X_OK) ;
	int		rs = SR_OK ;
	int		f = false ;
	cchar		*prn = sip->prn ;
	for (int i = 0 ; soexts[i] != nullptr ; i += 1) {
	    if ((rs = mksofname(sfn,sdn,prn,soexts[i])) >= 0) {
	        if ((rs = u_stat(sfn,&sb)) >= 0) {
		    if (S_ISREG(sb.st_mode)) {
	                if ((rs = permids(&sip->id,&sb,am)) >= 0) {
			    rs = subinfo_searchlib(sip,pr,sfn) ;
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
} /* end subroutine (subinfo_exts) */

local int subinfo_searchlib(SI *sip,cchar *pr,cchar *sfn) noex {
	cint		dlmode = RTLD_LAZY ;
	int		rs = SR_OK ;
	int		f = false ;
	void		*sop ;
	if ((sop = dlopen(sfn,dlmode)) != nullptr) {
	    subdialer_t	symp ;
	    if ((symp = (subdialer_t) dlsym(sop,sip->dialsym)) != nullptr) {
		rs = subinfo_searchcall(sip,pr,symp) ;
		f = rs ;
	    } /* end if (dlsym) */
	    dlclose(sop) ;
	} /* end if (dlopen) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (subinfo_searchlib) */

local int subinfo_searchcall(SI *sip,cchar *pr,subdialer_t symp) noex {
	mode_t		om = sip->om ;
	int		rs ;
	int		of = sip->of ;
	int		to = sip->to ;
	int		f = false ;
	cchar	*prn = sip->prn ;
	cchar	*svc = sip->svc ;
	mainv		argv = sip->argv ;
	mainv		envv = sip->envv ;
	if ((rs = (*symp)(pr,prn,svc,of,om,argv,envv,to)) >= 0) {
	    sip->fd = rs ;
	    f = true ;
	} /* end if (call) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (subinfo_searchcall) */

local int subinfo_idbegin(SI *sip) noex {
	int		rs = ids_load(&sip->id) ;
	return rs ;
} /* end subroutine (subinfo_idbegin) */

local int subinfo_idend(SI *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    {
	        rs1 = ids_release(&sip->id) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_idend) */

local int subinfo_envv(subinfo *op,mainv envv) noex {
    	int	rs = SR_OK ;
	if ((op->envv = envv) == nullptr) {
	    if (mainv ev ; (rs = u_getenviron(&ev)) >= 0) {
		op->envv = ev ;
	    }
	}
	return rs ;
} /* end subroutine (subinfo_envv) */


