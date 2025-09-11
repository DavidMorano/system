/* ucopenxsvc (open-facility-service) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open a facility-service */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_openxsvc

	Description:
	This subroutine opens what is referred to as a "facility-service."
	Facility services are services provided by "facilities."
	Facilities are normally or usually really a software
	distribution that has its own source-root or program-root.
	Examples of software distributions within AT&T are:

	NCMP
	PCS (Personal Communication Services)
	EXPTOOLS (Experimental Tools)
	LOCAL
	REXEC (Remote Execution)
	POST
	EXTRA
	AST (Advanced Software Technologies)
	GNU

	Facilities are stored in the filesystem hierarchy rooted
	at a certain place.  Facilities are usually rooted at one
	of the following locations in the system filesystem hierarchy:

	/usr
	/usr/add-on
	/opt

	Facility services are represented in the filesystem as files
	with names of the form:

	<facility>§<svc>[­<arg(s)>

	These sorts of file names are often actually stored in the
	filesystem as symbolic links.

	Synopsis:
	int uc_openxsvc(cc *pr,cc *prn,cc *svc,int of,mode_t om,
		mainv argv,mainv envv,int to) noex

	Arguments:
	pr		program-root
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

	Notes:
	Facility services are implemented with loadable shared-object
	files.  Each service has a file of the same name as the
	service name itself.  The file is a shared-object with a
	global symbol of a callable subroutine with the name
	'opensvc_<svc>' where <svc> is the service name.  The
	subroutine looks like:

	int opensvc_<svc>(cc *pr,cc *prn,int of,mode_t om,
		mainv argv,mainv envv,int to) noex

	Multiple services can be actually implemented in the same
	shared-object.  But the actual file of that object should
	be linked to other files, each with the filename of a service
	to be implemented.  These links are required because this
	code only searches for services by searching for files with
	the names of the services.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<libmallocxx.h>
#include	<ids.h>
#include	<sncpyx.h>
#include	<mkx.h>			/* |mksofname(3uc)| */
#include	<mkpathx.h>
#include	<permx.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

import openxsvc ;

/* local defines */

#define	SI		subinfo
#define	SI_FL		subinfo_flags


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*subsvc_f)(cc *,cc *,int,mode_t,mainv,mainv,int) noex ;
}

/* external subroutines */


/* external variables */

extern cchar	**environ ;


/* local structures */

struct subinfo_flags {
	uint		dummy:1 ;
} ;

namespace {
    struct subinfo : openxsvc {
	subinfo(const openxsvc &so) noex : openxsvc(so) { } ;
	subinfo(const openxsvc *sop) noex : openxsvc(sop) { } ;
	char		*basename ;	/* memory-allocated */
	char		*svcdname ;	/* memory-allocated */
	char		*dialsym ;	/* memory-allocated */
	ids		id ;
	SI_FL		fl ;
	int		fd ;
	int start() noex ;
	int starts() noex ;
	int startdir() noex ;
	int startsym() noex ;
	int search() noex ;
	int exts(char *) noex ;
	int finish() noex ;
	int searchlib(cchar *) noex ;
	int searchcall(subsvc_f) noex ;
    } ; /* end struct (subinfo) */
} /* end namespace */


/* forward references */

static bool	isNoAcc(int) noex ;


/* local variables */

static cpcchar		soexts[] = {
	"so",
	"o",
	"dyld",
	""
} ;

static cint		rsnoacc[] = {
	SR_NOENT,
	SR_ACCESS,
	0
} ;


/* exported variables */


/* exported subroutines */

int uc_openxsvc(const openxsvc *oscp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	if (oscp) {
	    if (oscp->pr && oscp->prn && oscp->svc) {
		rs = SR_INVALID ;
	        if (oscp->pr[0] && oscp->prn[0] && oscp->svc[0]) {
                    if (SI si(oscp) ; (rs = si.start()) >= 0) {
                        if ((rs = si.search()) > 0) {
                            fd = si.fd ;
                        } else if (rs == 0) {
                            rs = SR_NOENT ;
                        }
                        rs1 = si.finish() ;
                        if (rs >= 0) rs = rs1 ;
                        if ((rs < 0) && (fd >= 0)) {
                            u_close(fd) ;
			    fd = -1 ;
                        }
                    } /* end if (subinfo) */
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openxsvc) */


/* local subroutines */

int subinfo::start() noex {
	int		rs = SR_FAULT ;
	if (prefix) {
	    rs = SR_INVALID ;
	    if (prefix[0]) {
		basename = nullptr ;
	        svcdname = nullptr ;
	        dialsym = nullptr ;
	        if (envv == nullptr) {
		    envv = mainv(environ) ;
	        }
	        fd = -1 ;
	        rs = starts() ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

int subinfo::starts() noex {
    	int		rs ;
	if (char *bbuf ; (rs = libmalloc_mn(&bbuf)) >= 0) { /* max-name */
	    if ((rs = sncpy(bbuf,rs,prefix,"s")) >= 0) {
		auto malls = lm_strw ;
	        if (cchar *cp ; (rs = malls(bbuf,rs,&cp)) >= 0) {
		    basename = cast_const<charp>(cp) ;
		    rs = startdir() ;
		    if (rs < 0) {
			lm_free(basename) ;
			basename = nullptr ;
		    } /* end if (error) */
		} /* end if (memory-allocation) */
	    } /* end if (sncpy) */
	    rs = lm_rsfree(rs,bbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (subinfo::starts) */

int subinfo::startdir() noex {
    	int		rs ;
	if (char *dbuf ; (rs = libmalloc_mp(&dbuf)) >= 0) {
	    if ((rs = mkpath(dbuf,pr,"lib",basename)) >= 0) {
	        cint	pl = rs ;
		cchar	*pp = dbuf ;
	        if (USTAT sb ; (rs = u_stat(dbuf,&sb)) >= 0) {
		    if (S_ISDIR(sb.st_mode)) {
			auto malls = lm_strw ;
		        if (cc *cp ; (rs = malls(pp,pl,&cp)) >= 0) {
			    svcdname = cast_const<charp>(cp) ;
			    rs = startsym() ;
			    if (rs < 0) {
				lm_free(svcdname) ;
				svcdname = nullptr ;
			    } /* end if (error) */
		        } /* end if (memory-allocation) */
		    } else {
	    	        rs = SR_NOTDIR ;
		    }
	        } /* end if (stat) */
	    } /* end if (mkpath) */
	    rs = lm_rsfree(rs,dbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (subinfo::startdir) */

int subinfo::startsym() noex {
    	int		rs ;
	if (char *sbuf ; (rs = libmalloc_sn(&sbuf)) >= 0) {
	    if ((rs = sncpy(sbuf,rs,prefix,"_",svc)) >= 0) {
		auto malls = lm_strw ;
		if (cc *cp ; (rs = malls(sbuf,rs,&cp)) >= 0) {
		    dialsym = cast_const<charp>(cp) ;
		} /* end if (memory-allocation) */
	    } /* end if (sncpy) */
	    rs = lm_rsfree(rs,sbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (subinfo::startsym) */

int subinfo::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (dialsym) {
	    rs1 = lm_free(dialsym) ;
	    if (rs >= 0) rs = rs1 ;
	    dialsym = nullptr ;
	}
	if (svcdname) {
	    rs1 = lm_free(svcdname) ;
	    if (rs >= 0) rs = rs1 ;
	    svcdname = nullptr ;
	}
	if (basename) {
	    rs1 = lm_free(basename) ;
	    if (rs >= 0) rs = rs1 ;
	    basename = nullptr ;
	}
	return rs ;
}
/* end method (subinfo::finish) */

int subinfo::search() noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = id.load) >= 0) {
	    if (char *fbuf ; (rs = libmalloc_mp(&fbuf)) >= 0) {
		{
		    rs = exts(fbuf) ;
		    f = rs ;
		} /* end block */
		rs = lm_rsfree(rs,fbuf) ;
	    } /* end if (m-a-f) */
	    rs1 = id.release ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo-id) */
	if ((rs < 0) && (fd >= 0)) {
	    u_close(fd) ;
	    fd = -1 ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end method (subinfo::search) */

int subinfo::exts(char *fbuf) noex {
	cint		am = (R_OK|X_OK) ;
	int		rs = SR_OK ;
	int		f = false ;
	cchar		*sdn = svcdname ;
	for (cauto &e : soexts) {
	    if ((rs = mksofname(fbuf,sdn,svc,e)) >= 0) {
	        if (USTAT sb ; (rs = u_stat(fbuf,&sb)) >= 0) {
		    if (S_ISREG(sb.st_mode)) {
	                if ((rs = permid(&id,&sb,am)) >= 0) {
			    rs = searchlib(fbuf) ;
			    f = rs ;
			} else if (isNoAcc(rs)) {
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
}
/* end method (subinfo::exts) */

int subinfo::searchlib(cchar *sfn) noex {
    	cnullptr	np{} ;
	cint		dlmode = RTLD_LAZY ;
	int		rs = SR_OK ;
	int		f = false ;
	if (void *sop ; (sop = dlopen(sfn,dlmode)) != np) {
	    if (subsvc_f symp ; (symp = (subsvc_f) dlsym(sop,dialsym)) != np) {
		rs = searchcall(symp) ;
		f = rs ;
	    } /* end if (dlsym) */
	    dlclose(sop) ;
	} /* end if (dlopen) */
	return (rs >= 0) ? f : rs ;
}
/* end method (subinfo::searchlib) */

int subinfo::searchcall(subsvc_f symp) noex {
	int		rs ;
	int		f = false ;
	if ((rs = (*symp)(pr,prn,of,om,argv,envv,to)) >= 0) {
	    fd = rs ;
	    f = true ;
	} /* end if (call) */
	return (rs >= 0) ? f : rs ;
}
/* end method (subinfo::searchcall) */

static bool isNoAcc(int rs) noex {
	return isOneOf(rsnoacc,rs) ;
}
/* end subroutine (isNoAcc) */


