/* sysdialer_entry SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system-dialer storage object */
/* version %I% last-modified %G% */


/* revision history:

	- 2003-11-04, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sysdialer

	Description:
	This object manages what system-dialers (SYSDIALER) have
	been loaded so far.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |getustime(3u)| */
#include	<uclibmem.h>
#include	<getnodedomain.h>
#include	<vecstr.h>
#include	<fsdir.h>
#include	<dirseen.h>
#include	<strn.h>		/* |strnrchr(3uc)| */
#include	<strx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<mkshlibname.h>
#include	<isoneof.h>
#include	<ischarx.h>
#include	<isnot.h>		/* |isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"sysdialer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	SD		sysdialer
#define	SD_FL		sysdialer_fl
#define	SD_ENT		sysdialer_ent
#define	SD_CALLS	sysdialer_calls
#define	SD_INFO		sysdialer_info
#define	SD_ARGS		sysdialer_args
#define	SD_PRC		sysdialer_prc
#define	SD_MOD		sysdialer_mod
#define	SD_MAGIC	SYSDIALER_MAGIC
#define	SD_DNAME	"sysdialers"

#define	DS		dirseen

#define	TO_FILECHECK	3

#define	NEXTS		3		/* number of extensions */

#define	LIBCNAME	"lib"


/* imported namespaces */

using sysdialer_util::sysdialer_calls ;	/* type */
using sysdialer_util::vecstr_loadexts ;	/* subroutine */
using sysdialer_util::var ;		/* variable */
using sysdialer_util::exts ;		/* variable */


/* local typedefs */

typedef SD_ENT		ent ;
typedef SD_MOD		mod ;
typedef SD_PRC		prcache ;
typedef SD_ENT *	entp ;
typedef sysdialer_fext	fext ;
typedef sysdialer_calls *	callptr ;


/* external subroutines */


/* external variables */
 

/* local structures */


/* forward references */


/* local variables */

enum subs {
	sub_open,
	sub_reade,
	sub_recve,
	sub_recvfrome,
	sub_recvmsge,
	sub_write,
	sub_send,
	sub_sendto,
	sub_sendmsg,
	sub_shutdown,
	sub_close,
	sub_overlast
} ; /* end enum (subs) */

constexpr cpcchar	subs[] = {
	"open",
	"reade",
	"recve",
	"recvfrome",
	"recvmsge",
	"write",
	"send",
	"sendto",
	"sendmsg",
	"shutdown",
	"close",
	nullptr
} ; /* end array (subs) */

cchar				soprefix[] = "sd" ;


/* exported variables */


/* exported subroutines */

namespace sysdialer_util {
    int entry_start(ent *ep,cc *name,cc *itype) noex {
	int		rs = SR_FAULT ;
	if (ep && name) ylikely {
	    cint 	csz = szof(sysdialer_calls) ;
	    memclear(ep) ;
	    if (void *vp ; (rs = lm_mall(csz,&vp)) >= 0) {
	        cint	osz = szof(SD_MOD) ;
		ep->callp = vp ;
	        if (SD_MOD *mp ; (rs = lm_mall(osz,&mp)) >= 0) {
	            ep->mp = mp ;
	            memclear(mp) ;
	            if (cchar *cp ; (rs = lm_strw(name,-1,&cp)) >= 0) ylikely {
		        ep->name = cp ;
	                if ((itype != nullptr) && (itype[0] != '\0')) {
	                    if ((rs = lm_strw(itype,-1,&cp)) >= 0) ylikely {
			        ep->itype = cp ;
		            }
	                }
	                if (mp != nullptr) {
	                    mp->count += 1 ;
	                }
	                ep->count = 1 ;
		        if ((rs < 0) && ep->itype) {
		            vp = voidp(ep->itype) ;
		            lm_free(vp) ;
		            ep->itype = nullptr ;
		        } /* end if (error) */
	            } /* end if (memory-allocation) */
		    if (rs < 0) {
		        lm_free(mp) ;
		        ep->mp = nullptr ;
		    } /* end if (error) */
	        } /* end if (memory-allocation) */
	        if (rs < 0) {
		    lm_free(ep->callp) ;
	        } /* end if (error) */
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (entry_start) */
} /* end namespace (sysdialer_util) */

namespace sysdialer_util {
    int entry_finish(ent *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) ylikely {
	    rs = SR_OK ;
	    if (ep->name != nullptr) ylikely {
	        void *vp = voidp(ep->name) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->name = nullptr ;
	    }
	    if (ep->itype != nullptr) ylikely {
	        void *vp = voidp(ep->itype) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->itype = nullptr ;
	    }
	    if (ep->mp != nullptr) ylikely {
	        SD_MOD	*mp = ep->mp ;
	        if (mp->count-- == 1) {
	            if ((mp->dhp != nullptr) && (mp->dhp != RTLD_DEFAULT)) {
	                dlclose(mp->dhp) ;
		    }
	            rs1 = lm_free(mp) ;
		    if (rs >= 0) rs = rs1 ;
		    ep->mp = nullptr ;
	        } /* end if (cleanup) */
	    } /* end if */
	    if (ep->callp) {
		rs1 = lm_free(ep->callp) ;
		if (rs >= 0) rs = rs1 ;
		ep->callp = nullptr ;
	    }
	    memclear(ep) ;
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (entry_finish) */
} /* end namespace (sysdialer_util) */

/* try to load a file with the given name */

namespace {
    struct checkdirer {
	ent	*ep ;		/* argument */
	cchar	*libdname ;	/* argument */
	cchar	*name ;		/* argument */
	char	* dname	;
	char	* dlfname ;
	char	* fname	;
	checkdirer(ent *e,cc *l,cc *n) noex : ep(e), libdname(l) {
	    name = n ;
	} ; /* end ctor */
	operator int () noex ;
	int vecer() noex ;
	int looper(vecstr *) noex ;
    } ; /* end struct (checkdirer) */
} /* end namespace */

namespace sysdialer_util {
    int entry_checkdir(ent *ep,cc *libdname,cc *name) noex {
	checkdirer co(ep,libdname,name) ;
	return co ;
    } /* end subroutine (entry_checkdir) */
} /* end namespace (sysdialer_util) */

checkdirer::operator int () noex {
    	cint		maxpath = var.maxpathlen ;
	cint		sz = ((var.maxpathlen + 1) * 3) ;
	int		rs ;
	int		rs1 ;
	int		fl = 0 ; /* return-value */
	int		ai = 0 ;
	if (char *a ; (rs = lm_mall(sz,&a)) >= 0) {
	    cchar	*sublibdname = SD_DNAME ;
	    dname	= (a + ((maxpath + 1) * ai++)) ;
	    dlfname	= (a + ((maxpath + 1) * ai++)) ;
	    fname	= (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = mkpath(dname,libdname,sublibdname)) >= 0) {
		if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) {
		    rs = SR_NOTDIR ;
		    if (S_ISDIR(sb.st_mode)) {
		    	rs = vecer() ;
			fl = rs ;
		    } /* end if (is-dir) */
		} /* end if (u_stat) */
	    } /* end if (mkpath) */
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fl : rs ;
} /* end method (checkdirer::operator) */

int checkdirer::vecer() noex {
	int		rs ;
	int		rs1 ;
	int		fl = 0 ; /* return-value */
	if (vecstr enames ; (rs = enames.start(3,0)) >= 0) {
	    cint	namelen = lenstr(name) ;
	    if ((rs = vecstr_loadexts(&enames,dname,name,namelen)) > 0) {
		rs = looper(&enames) ;
		fl = rs ;
	    } /* end if (vecstr_loadexts) */
	    rs1 = enames.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return (rs >= 0) ? fl : rs ;
} /* end method (checkdirer::vecer) */

int checkdirer::looper(vecstr *elp) noex {
    	cnullptr	np{} ;
	cint		dlm = (RTLD_LAZY | RTLD_LOCAL) ;
	cint		rsn = SR_NOENT ;
    	int		rs = SR_OK ;
	int		fl = 0 ; /* return-value */
	for (int i = 0 ; (rs >= 0) && exts[i] ; i += 1) {
	    cchar *ext = exts[i] ;
	    if ((rs = elp->findn(ext)) >= 0) {
		if ((rs = mkshlibname(fname,soprefix,name,ext)) >= 0) {
		    if ((rs = mkpath(dlfname,dname,fname)) >= 0) {
	        	fl = rs ;
	        	if (void *dhp ; (dhp = dlopen(dlfname,dlm)) != np) {
			    SD_INFO	*dip = (SD_INFO *) dlsym(dhp,name) ;
	                    if (dip && (strcmp(dip->name,name) == 0)) {
	                        ep->osz = dip->osz ;
	                        ep->flags = dip->flags ;
	                        ep->mp->dhp = dhp ;
	                        break ;
	                    } else {
	                        dlclose(dhp) ;
				fl = 0 ;
		            }
			} else {
			    fl = 0 ;
			} /* end if (dlopen) */
		    } /* end if (mkpath) */
		} /* end if (ok) */
	    } else if (rs == rsn) {
		rs = SR_OK ;
	    } /* end if (tried one) */
	} /* end for (extensions) */
	return (rs >= 0) ? fl : rs ;
} /* end method (checkdirer::looper) */

/* load up the available subroutines from this module */
namespace sysdialer_util {
    int entry_loadcalls(ent *ep,void *dhp) noex {
	sysdialer_calls *callp = callptr(ep->callp) ;
	cnullptr	np{} ;
	int		rs ;
	int		rs1 ;
	int		nl ;
	if (char *nbuf ; (rs = lm_mn(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    int		bl ;
	    char	*bp ;
	    nl = intconv(strwcpy(nbuf,ep->name,nlen) - nbuf) ;
	    bp = charp(nbuf + nl) ;
	    bl = (nlen - nl) ;
	    for (int i = 0 ; (rs >= 0) && subs[i] ; i += 1) {
	        if ((rs = sncpy(bp,bl,"_",subs[i])) >= 0) {
	            if (void *fp ; (fp = dlsym(dhp,nbuf)) != np) {
	                switch (i) {
	                case sub_open:
	                    callp->open		= soopen_f(fp) ;
	                    break ;
	                case sub_reade:
	                    callp->reade	= soreade_f(fp) ;
	                    break ;
	                case sub_recve:
	                    callp->recve	= sorecve_f(fp) ;
	                    break ;
	                case sub_recvfrome:
	                    callp->recvfrome	= sorecvfrome_f(fp) ;
	                    break ;
	                case sub_recvmsge:
	                    callp->recvmsge	= sorecvmsge_f(fp) ;
	                    break ;
	                case sub_write:
	                    callp->write	= sowrite_f(fp) ;
	                    break ;
	                case sub_send:
	                    callp->send		= sosend_f(fp) ;
	                    break ;
	                case sub_sendto:
	                    callp->sendto	= sosendto_f(fp) ;
	                    break ;
	                case sub_sendmsg:
	                    callp->sendmsg	= sosendmsg_f(fp) ;
	                    break ;
	                case sub_shutdown:
	                    callp->shutdown	= soshutdown_f(fp) ;
	                    break ;
	                case sub_close:
	                    callp->close	= soclose_f(fp) ;
	                    break ;
	                } /* end switch */
	            } /* end if (got a symbol) */
	        } /* end if (sncpy) */
	    } /* end for */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	if ((rs >= 0) && (callp->open == nullptr)) {
	    rs = SR_LIBACC ;
	}
	return rs ;
    } /* end subroutine (entry_loadcalls) */
} /* end namespace (sysdialer_util) */

namespace sysdialer_util {
    int entry_hasname(ent *ep,void *dhp,cc *name) noex {
	SD_INFO		*dip = (SD_INFO *) dlsym(dhp,name) ;
	int		rs = SR_NOTFOUND ;
	if ((dip != nullptr) && (strcmp(dip->name,name) == 0)) {
	    ep->osz = dip->osz ;
	    rs = SR_OK ;
	}
	return rs ;
    } /* end subroutine (entry_hasname) */
} /* end namespace (sysdialer_util) */


