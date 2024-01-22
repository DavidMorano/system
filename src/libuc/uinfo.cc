/* uinfo */
/* lang=C++20 */

/* UNIX® information (a cache for |uname(2)| and sisters) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2020-03-26, David A­D­ Morano
	I added (sort of for fun) a use of C++20 |if constexpr|.
	Yes, with only a relatively few used by me of this new
	features, I can see that it is no where near capable of
	eliminating all pre-propcessor symbols. But I try my best
	(since I do not like pre-processor symbols any more than
	the next guy).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	int uinfo_name
	int uinfo_aux

	Description:
	This module serves as a cache (of sorts) for UNIX® information
	that is related to the underlying machine and which does
	not (easily) change during program exection.  Since we are
	basically dealing with global data, we need to make the
	establishment of it multi-thread safe.  We also want fork
	safety.  Yes, we want everything, including cleanup on
	module unloading (since, yes, we could all be inside a
	loadable and unloadble module!).  For these purposes we
	employ the basic (and not so basic) means of accomplishing
	this.  See the code for our various machinations.

	Q. Do these subroutines (the public ones) need to be 
	multi-thread-safe?
	A. What do you think?

	Q. Did these subroutines need to be async-signal-safe?
	A. No.  I do not think that these needed to be async-signal-safe.

	Q. Is it a "waste" to make these subroutines async-signal-safe?
	A. Yes. Probably.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<sysauxinfo.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<timewatch.hh>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"uinfo.h"


/* local defines */

#ifndef	NODENAMELEN
#define	NODENAMELEN	256
#endif

#if	defined(SAI_ARCHITECTURE)
#define	F_ARCHITECTURE		1
#else
#define	F_ARCHITECTURE		0
#define	SAI_ARCHITECTURE	0
#endif

#if	defined(SAI_PLATFORM)
#define	F_PLATFORM		1
#else
#define	F_PLATFORM		0	
#define	SAI_PLATFORM		0
#endif

#if	defined(SAI_HWPROVIDER)
#define	F_HWPROVIDER		1
#else
#define	F_HWPROVIDER		0
#define	SAI_HWPROVIDER		0
#endif

#if	defined(SAI_HWSERIAL)
#define	F_HWSERIAL		1
#else
#define	F_HWSERIAL		0
#define	SAI_HWSERIAL		0
#endif

#if	defined(SAI_SRPCDOMAIN)
#define	F_SRPCDOMAIN		1
#else
#define	F_SRPCDOMAIN		0
#define	SAI_SRPCDOMAIN		0
#endif


/* external subroutines */

extern "C" {
   extern int uc_sysauxinfo(char *,int,int) noex ;
}


/* local structures */

namespace {
    struct uinfo_tmpaux {
	char		architecture[NODENAMELEN+1] ;
	char		platform[NODENAMELEN+1] ;
	char		hwprovider[NODENAMELEN+1] ;
	char		hwserial[NODENAMELEN+1] ;
	char		nisdomain[NODENAMELEN+1] ;
    } ;
    struct uinfo_alloc {
	cchar		*name ;	/* string allocation for "name" */
	cchar		*aux ;	/* string allocation for "aux" */
    } ;
    struct uinfo {
	ptm		mx ;		/* data mutex */
	uinfo_alloc	a ;		/* memory allocations */
	uinfo_infoname	name ;
	uinfo_infoaux	aux ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	int init() noex ;
	int fini() noex ;
	int getname(uinfo_infoname *) noex ;
	int getaux(uinfo_infoaux *) noex ;
	void atforkbefore() noex {
	    mx.lockbegin() ;
	} ;
	void atforkafter() noex {
	    mx.lockend() ;
	} ;
	~uinfo() noex {
	    cint	rs = fini() ;
	    if (rs < 0) {
	        ulogerror("uinfo",rs,"dtor-fini") ;
	    }
	} ; /* end dtor (uinfo) */
    } ; /* end struct (uinfo) */
}


/* forward references */

extern "C" {
    static void	uinfo_atforkbefore() noex ;
    static void	uinfo_atforkafter() noex ;
    static void	uinfo_exit() noex ;
}

static int	uinfo_getaux(uinfo_tmpaux *) noex ;


/* local variables */

static uinfo		uinfo_data ;

constexpr bool		f_architecture = F_ARCHITECTURE	;
constexpr bool		f_platform = F_PLATFORM	;
constexpr bool		f_hwprovider = F_HWPROVIDER ;
constexpr bool		f_hwserial = F_HWSERIAL ;
constexpr bool		f_srpcdomain = F_SRPCDOMAIN ;


/* exported subroutines */

int uinfo_init() noex {
	return uinfo_data.init() ;
}

int uinfo_fini() noex {
	return uinfo_data.fini() ;
}

int uinfo_name(uinfo_infoname *unp) noex {
	return uinfo_data.getname(unp) ;
}

int uinfo_aux(uinfo_infoaux *uxp) noex {
	return uinfo_data.getaux(uxp) ;
}


/* local subroutines */

int uinfo::init() noex {
	int		rs = SR_NXIO ;
	int		f = false ;
	if (!fvoid) {
	    cint	to = utimeout[uto_busy] ;
	    if (! finit.testandset) {
	        if ((rs = mx.create) >= 0) {
	            void_f	b = uinfo_atforkbefore ;
	            void_f	a = uinfo_atforkafter ;
	            if ((rs = uc_atfork(b,a,a)) >= 0) {
	                if ((rs = uc_atexit(uinfo_exit)) >= 0) {
	                    finitdone = true ;
	                    f = true ;
	                }
	                if (rs < 0) {
	                    uc_atforkexpunge(b,a,a) ;
			}
	            } /* end if (uc_atfork) */
	 	    if (rs < 0) {
		        mx.destroy() ;
		    }
	        } /* end if (ptm_create) */
	        if (rs < 0) {
	            finit = false ;
		}
	    } else if (! finitdone) {
	        timewatch	tw(to) ;
	        auto lamb = [this] () -> int {
	            int		rs = SR_OK ;
	            if (!finit) {
		        rs = SR_LOCKLOST ;		/* <- failure */
	            } else if (finitdone) {
		        rs = 1 ;			/* <- OK ready */
	            }
	            return rs ;
	        } ; /* end lambda (lamb) */
	        rs = tw(lamb) ;		/* <- time-watching occurs in there */
	    } /* end if (initialization) */
	} /* end if (not voided) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uinfo::init) */

int uinfo::fini() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (finitdone && (!fvoid.testandset)) {
	    {
	        uinfo_alloc	*uap = &a ;
	        if (uap->name) {
	            cvoid	*p = (cvoid *) uap->name ;
	            rs1 = uc_libfree(p) ;
		    if (rs >= 0) rs = rs1 ;
	            uap->name = nullptr ;
	        }
	        if (uap->aux) {
	            cvoid	*p = (cvoid *) uap->aux ;
	            rs1 = uc_libfree(p) ;
		    if (rs >= 0) rs = rs1 ;
	            uap->aux = nullptr ;
	        }
	    }
	    {
	        void_f	b = uinfo_atforkbefore ;
	        void_f	a = uinfo_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    finit = false ;
	    finitdone = false ;
	} /* end if (was initialized) */
	return rs ;
}
/* end subroutine (uinfo::fini) */

int uinfo::getname(uinfo_infoname *unp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = true ;
	if (unp) {
	    sigblocker	b ;
	    memclear(unp) ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init()) >= 0) {
	            uinfo_alloc		*uap = &a ;
	            if (uap->name) {
	                uinfo_infoname	tmpname ;
	                cint		size = sizeof(UTSNAME) ;
	                cchar		*nnamep = nullptr ;
	                void		*p ;
	                if ((rs = uc_libmalloc(size,&p)) >= 0) {
	                    UTSNAME	*unp = (UTSNAME *) p ;
	                    if ((rs = u_uname(unp)) >= 0) {
	                        cint	nlen = NODENAMELEN ;
	                        int	size = 0 ;
	                        char	*bp ;
	                        size += (strnlen(unp->sysname,nlen) + 1) ;
	                        size += (strnlen(unp->nodename,nlen) + 1) ;
	                        size += (strnlen(unp->release,nlen) + 1) ;
	                        size += (strnlen(unp->version,nlen) + 1) ;
	                        size += (strnlen(unp->machine,nlen) + 1) ;
	                        if ((rs = uc_libmalloc(size,&bp)) >= 0) {
	                            nnamep = bp ;
	                            tmpname.sysname = bp ;
	                            bp = (strwcpy(bp,unp->sysname,nlen) + 1) ;
	                            tmpname.nodename = bp ;
	                            bp = (strwcpy(bp,unp->nodename,nlen) + 1) ;
	                            tmpname.release = bp ;
	                            bp = (strwcpy(bp,unp->release,nlen) + 1) ;
	                            tmpname.version = bp ;
	                            bp = (strwcpy(bp,unp->version,nlen) + 1) ;
	                            tmpname.machine = bp ;
	                            bp = (strwcpy(bp,unp->machine,nlen) + 1) ;
	                        } /* end if (memory-allocation) */
	                    } /* end if (uname) */
	                    if (rs >= 0) {
	                        if ((rs = uc_forklockbegin(-1)) >= 0) {
	                            if ((rs = mx.lockbegin) >= 0) {
	                                if (uap->name) {
	                                    uap->name = nnamep ;
	                                    name = tmpname ;
	                                    nnamep = nullptr ;
	                                    f = false ;
	                                }
	                                rs1 = mx.lockend ;
					if (rs >= 0) rs = rs1 ;
	                            } /* end if (mutex) */
	                            rs1 = uc_forklockend() ;
				    if (rs >= 0) rs = rs1 ;
	                        } /* end if (forklock) */
	                    } /* end if (ok) */
	                    if (nnamep) uc_libfree(nnamep) ;
	                    rs1 = uc_libfree(unp) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (memory-allocation-free) */
	            } /* end if (need to get information) */
	            if (rs >= 0) *unp = name ;
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uinfo::getname) */

int uinfo::getaux(uinfo_infoaux *uxp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = true ;
	if (uxp) {
	    sigblocker	b ;
	    memclear(uxp) ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init()) >= 0) {
	            uinfo_alloc	*uap = &a ;
	            if (uap->aux) {
	                uinfo_infoaux	tmpaux ;
	                cint		size = sizeof(uinfo_tmpaux) ;
	                cchar		*nauxp = nullptr ;
	                void		*p ;
	                if ((rs = uc_libmalloc(size,&p)) >= 0) {
	                    uinfo_tmpaux	*tap = (uinfo_tmpaux *) p ;
	                    if ((rs = uinfo_getaux(tap)) >= 0) {
	                        cint	nlen = NODENAMELEN ;
	                        int	size = 0 ;
			        cchar	*np ;
	                        char	*bp ;
	                        np = tap->architecture ;
	                        size += (strnlen(np,nlen) + 1) ;
	                        size += (strnlen(tap->platform,nlen) + 1) ;
	                        size += (strnlen(tap->hwprovider,nlen) + 1) ;
	                        size += (strnlen(tap->hwserial,nlen) + 1) ;
	                        size += (strnlen(tap->nisdomain,nlen) + 1) ;
	                        if ((rs = uc_libmalloc(size,&bp)) >= 0) {
	                            nauxp = bp ;
	                            tmpaux.architecture = bp ;
	                            np = tap->architecture ;
	                            bp = (strwcpy(bp,np,nlen) + 1) ;
	                            tmpaux.platform = bp ;
	                            np = tap->platform ;
	                            bp = (strwcpy(bp,np,nlen) + 1) ;
	                            tmpaux.hwprovider = bp ;
	                            np = tap->hwprovider ;
	                            bp = (strwcpy(bp,np,nlen) + 1) ;
	                            tmpaux.hwserial = bp ;
	                            np = tap->hwserial ;
	                            bp = (strwcpy(bp,np,nlen) + 1) ;
	                            tmpaux.nisdomain = bp ;
	                            np = tap->nisdomain ;
	                            bp = (strwcpy(bp,np,nlen) + 1) ;
	                        } /* end if (memory-allocation) */
	                    } /* end if (uaux) */
	                    if (rs >= 0) { /* resolve possible race */
	                        if ((rs = uc_forklockbegin(-1)) >= 0) {
	                            if ((rs = mx.lockbegin) >= 0) {
	                                if (uap->aux) {
	                                    uap->aux = nauxp ;
	                                    aux = tmpaux ;
	                                    nauxp = nullptr ;
	                                    f = false ;
				        }
	                                rs1 = mx.lockend ;
					if (rs >= 0) rs = rs1 ;
	                            } /* end if (mutex) */
	                            rs1 = uc_forklockend() ;
				    if (rs >= 0) rs = rs1 ;
	                        } /* end if (forklock) */
	                    } /* end if (ok) */
	                    if (nauxp) uc_libfree(nauxp) ;
	                    rs1 = uc_libfree(tap) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (memory-allocation-free) */
	            } /* end if (need to get information) */
	            if (rs >= 0) *uxp = aux ;
	        } /* end if (init) */
	        rs1 = b.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (uinfo::getaux) */


/* local subroutines */

static int uinfo_getaux(uinfo_tmpaux *tap) noex {
	[[maybe_unused]] char	*nbuf = nullptr ;
	[[maybe_unused]] cint	nlen = NODENAMELEN ;
	int			rs = SR_OK ;
	[[maybe_unused]] int	rs1 ;
	tap->architecture[0] = '\0' ;
	tap->platform[0] = '\0' ;
	tap->hwprovider[0] = '\0' ;
	tap->hwserial[0] = '\0' ;
	tap->nisdomain[0] = '\0' ;
	if constexpr (f_architecture) {
	    cint	req = SAI_ARCHITECTURE ;
	    nbuf = tap->architecture ;
	    rs1 = uc_sysauxinfo(nbuf,nlen,req) ;
	    if (rs1 < 0) nbuf[0] = '\0' ;
	}
	if constexpr (f_platform) {
	    cint	req = SAI_PLATFORM ;
	    nbuf = tap->platform ;
	    rs1 = uc_sysauxinfo(nbuf,nlen,req) ;
	    if (rs1 < 0) nbuf[0] = '\0' ;
	}
	if constexpr (f_hwprovider) {
	    cint	req = SAI_HWPROVIDER ;
	    nbuf = tap->hwprovider ;
	    rs1 = uc_sysauxinfo(nbuf,nlen,req) ;
	    if (rs1 < 0) nbuf[0] = '\0' ;
	}
	if constexpr (f_hwserial) {
	    cint	req = SAI_HWSERIAL ;
	    nbuf = tap->hwserial ;
	    rs1 = uc_sysauxinfo(nbuf,nlen,req) ;
	    if (rs1 < 0) nbuf[0] = '\0' ;
	}
	if constexpr (f_srpcdomain) {
	    cint	req = SAI_SRPCDOMAIN ;
	    nbuf = tap->nisdomain ;
	    rs1 = uc_sysauxinfo(nbuf,nlen,req) ;
	    if (rs1 < 0) nbuf[0] = '\0' ;
	}
	return rs ;
}
/* end subroutine (uinfo_getaux) */

static void uinfo_atforkbefore() noex {
	uinfo_data.atforkbefore() ;
}

static void uinfo_atforkafter() noex {
	uinfo_data.atforkafter() ;
}

static void uinfo_exit() noex {
	int		rs = uinfo_data.fini() ;
	if (rs < 0) {
	    ulogerror("uinfo",rs,"exit-fini") ;
	}
}


