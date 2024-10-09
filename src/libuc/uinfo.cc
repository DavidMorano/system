/* uinfo SUPPORT */
/* lang=C++20 */

/* UNIX® information (a cache for |uname(2)| and sisters) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2020-03-26, David A­D­ Morano
	I refactored the |uinfo_auxload| subroutine below.  It
	previously used a proprietary interface to get that auxillary
	information, but now it uses a generic interface (suitable
	for most all UNIX®-like systems).

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

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<cstring>
#include	<usystem.h>
#include	<ucsysauxinfo.h>
#include	<getbufsize.h>
#include	<sigblocker.h>
#include	<ptm.h>
#include	<timewatch.hh>
#include	<strwcpy.h>
#include	<localmisc.h>		/* |NODENAMELEN| */

#include	"uinfo.h"


/* local defines */

#ifndef	NODENAMELEN
#define	NODENAMELEN	256
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    constexpr int	tmpaux_nfields = 5 ;
    struct uinfo_tmpaux {
	int		flen ;		/* length of fields (buffers) below */
	char		*a ;
	char		*architecture ;
	char		*platform ;
	char		*hwprovider ;
	char		*hwserial ;
	char		*nisdomain ;
    } ;
    struct setname {
	uinfo_infoname	tmpname ;
	char		*strp = nullptr ;
	~setname() {
	    if (strp) {
	        uc_libfree(strp) ;
	        strp = nullptr ;
	    }
	}
    } ;
    struct setaux {
	uinfo_infoaux	tmpaux ;
	char		*strp = nullptr ;
	~setaux() {
	    if (strp) {
	        uc_libfree(strp) ;
	        strp = nullptr ;
	    }
	}
    } ;
    struct uinfo_alloc {
	cchar		*name ;	/* string allocation for "name" */
	cchar		*aux ;	/* string allocation for "aux" */
    } ;
    struct uinfo {
	ptm		mx ;		/* data mutex */
	uinfo_alloc	ao ;		/* memory allocations */
	uinfo_infoname	name ;
	uinfo_infoaux	aux ;
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	int init() noex ;
	int fini() noex ;
	int getname(uinfo_infoname *) noex ;
	int getname_setup() noex ;
	int getname_load(setname *) noex ;
	int getname_install(setname *) noex ;
	int getaux(uinfo_infoaux *) noex ;
	int getaux_setup() noex ;
	int getaux_load(setaux *) noex ;
	int getaux_install(setaux *) noex ;
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

static int	uinfo_auxbegin(uinfo_tmpaux *) noex ;
static int	uinfo_auxend(uinfo_tmpaux *) noex ;
static int	uinfo_auxload(uinfo_tmpaux *) noex ;


/* local variables */

static uinfo		uinfo_data ;

constexpr int		sais[] = {
	SAI_ARCHITECTURE,
	SAI_PLATFORM,
	SAI_HWPROVIDER,
	SAI_HWSERIAL,
	SAI_RPCDOMAIN
} ;


/* exported variables */


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
	    rs = SR_OK ;
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
	        uinfo_alloc	*uap = &ao ;
	        if (uap->name) {
	            rs1 = uc_libfree(uap->name) ;
		    if (rs >= 0) rs = rs1 ;
	            uap->name = nullptr ;
	        }
	        if (uap->aux) {
	            rs1 = uc_libfree(uap->aux) ;
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
	if (unp) {
	    sigblocker	b ;
	    memclear(unp) ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init()) >= 0) {
		    if ((rs = getname_setup()) >= 0) {
			*unp = name ;
		    }
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uinfo::getname) */

int uinfo::getname_setup() noex {
	int		rs = SR_OK ;
	if (ao.name == nullptr) {
	    setname	tmp ;
	    if ((rs = getname_load(&tmp)) >= 0) {
		rs = getname_install(&tmp) ;
	    } /* end if (ok) */
	} /* end if (setup needed) */
	return rs ;
}
/* end method (uinfo::getname_setup) */

int uinfo::getname_load(setname *setp) noex {
	cint		usz = sizeof(UTSNAME) ;
	void		*vp{} ;
	int		rs ;
	int		rs1 ;
	if ((rs = uc_libmalloc(usz,&vp)) >= 0) {
	    UTSNAME	*utsp = (UTSNAME *) vp ;
            if ((rs = u_uname(utsp)) >= 0) {
                cint    nlen = int(sizeof(utsp->sysname) - 1) ;
                int     sz = 0 ;
                char    *bp ;
                sz += (strnlen(utsp->sysname,nlen) + 1) ;
                sz += (strnlen(utsp->nodename,nlen) + 1) ;
                sz += (strnlen(utsp->release,nlen) + 1) ;
                sz += (strnlen(utsp->version,nlen) + 1) ;
                sz += (strnlen(utsp->machine,nlen) + 1) ;
                if ((rs = uc_libmalloc(sz,&bp)) >= 0) {
                    setp->strp = bp ;
                    setp->tmpname.sysname = bp ;
                    bp = (strwcpy(bp,utsp->sysname,nlen) + 1) ;
                    setp->tmpname.nodename = bp ;
                    bp = (strwcpy(bp,utsp->nodename,nlen) + 1) ;
                    setp->tmpname.release = bp ;
                    bp = (strwcpy(bp,utsp->release,nlen) + 1) ;
                    setp->tmpname.version = bp ;
                    bp = (strwcpy(bp,utsp->version,nlen) + 1) ;
                    setp->tmpname.machine = bp ;
                    bp = (strwcpy(bp,utsp->machine,nlen) + 1) ;
                } /* end if (memory-allocation) */
            } /* end if (uname) */
	    rs1 = uc_libfree(utsp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (uinfo::getname_load) */

int uinfo::getname_install(setname *setp) noex {
	int		rs ;
	int		rs1 ;
        if ((rs = uc_forklockbegin(-1)) >= 0) {
            if ((rs = mx.lockbegin) >= 0) {
                if (ao.name == nullptr) {
                    ao.name = setp->strp ;
                    name = setp->tmpname ;
                    setp->strp = nullptr ;
		} else {
		    uc_libfree(setp->strp) ;
		    setp->strp = nullptr ;
                }
                rs1 = mx.lockend ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (mutex) */
            rs1 = uc_forklockend() ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (forklock) */
	return rs ;
}
/* end method (uinfo::getname_install) */

int uinfo::getaux(uinfo_infoaux *uxp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uxp) {
	    sigblocker	b ;
	    memclear(uxp) ;
	    if ((rs = b.start) >= 0) {
	        if ((rs = init()) >= 0) {
		    if ((rs = getaux_setup()) >= 0) {
			*uxp = aux ;
		    }
	        } /* end if (init) */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uinfo::getaux) */

int uinfo::getaux_setup() noex {
	int		rs = SR_OK ;
	if (ao.name == nullptr) {
	    setaux	tmp ;
	    if ((rs = getaux_load(&tmp)) >= 0) {
		rs = getaux_install(&tmp) ;
	    } /* end if (ok) */
	} /* end if (setup needed) */
	return rs ;
}
/* end method (uinfo::getaux_setup) */

int uinfo::getaux_load(setaux *setp) noex {
	cint		usz = sizeof(uinfo_tmpaux) ;
	void		*vp{} ;
	int		rs ;
	int		rs1 ;
	if ((rs = uc_libmalloc(usz,&vp)) >= 0) {
	    uinfo_tmpaux	*tap = (uinfo_tmpaux *) vp ;
	    if ((rs = uinfo_auxbegin(tap)) >= 0) {
                if ((rs = uinfo_auxload(tap)) >= 0) {
                    cint    nlen = tap->flen ;
                    int     sz = 0 ;
                    sz += (strnlen(tap->architecture,nlen) + 1) ;
                    sz += (strnlen(tap->platform,nlen) + 1) ;
                    sz += (strnlen(tap->hwprovider,nlen) + 1) ;
                    sz += (strnlen(tap->hwserial,nlen) + 1) ;
                    sz += (strnlen(tap->nisdomain,nlen) + 1) ;
                    if (char *bp{} ; (rs = uc_libmalloc(sz,&bp)) >= 0) {
                        cchar   *sp ;
                        setp->strp = bp ;
		        {
                            setp->tmpaux.architecture = bp ;
                            sp = tap->architecture ;
                            bp = (strwcpy(bp,sp,nlen) + 1) ;
		        }
		        {
                            setp->tmpaux.platform = bp ;
                            sp = tap->platform ;
                            bp = (strwcpy(bp,sp,nlen) + 1) ;
		        }
		        {
                            setp->tmpaux.hwprovider = bp ;
                            sp = tap->hwprovider ;
                            bp = (strwcpy(bp,sp,nlen) + 1) ;
		        }
		        {
                            setp->tmpaux.hwserial = bp ;
                            sp = tap->hwserial ;
                            bp = (strwcpy(bp,sp,nlen) + 1) ;
		        }
		        {
                            setp->tmpaux.nisdomain = bp ;
                            sp = tap->nisdomain ;
                            bp = (strwcpy(bp,sp,nlen) + 1) ;
		        }
                    } /* end if (memory-allocation) */
                } /* end if (uinfo_auxload) */
	    	rs1 = uinfo_auxend(tap) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (tmpaux) */
	    rs1 = uc_libfree(tap) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (uinfo::getaux_load) */

int uinfo::getaux_install(setaux *setp) noex {
	int		rs ;
	int		rs1 ;
        if ((rs = uc_forklockbegin(-1)) >= 0) {
            if ((rs = mx.lockbegin) >= 0) {
                if (ao.aux == nullptr) {
                    ao.aux = setp->strp ;
                    aux = setp->tmpaux ;
                    setp->strp = nullptr ;
		} else {
		    uc_libfree(setp->strp) ;
		    setp->strp = nullptr ;
                }
                rs1 = mx.lockend ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (mutex) */
            rs1 = uc_forklockend() ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (forklock) */
	return rs ;
}
/* end method (uinfo::getaux_install) */

static int uinfo_auxbegin(uinfo_tmpaux *tap) noex {
    	int		rs = SR_FAULT ;
	if (tap) {
	    if ((rs = getbufsize(getbufsize_nn)) >= 0) {
		cint	flen = rs ;
		cint	sz = (tmpaux_nfields * (rs + 1)) ;
		if (void *vp{} ; (rs = uc_libmalloc(sz,&vp)) >= 0) {
		    int		ai = 0 ;
		    char	*a = charp(vp) ;
		    tap->flen = flen ;
		    tap->a = charp(vp) ;
		    {
			tap->architecture = (a + ((flen + 1) & ai++)) ;
			tap->platform = (a + ((flen + 1) & ai++)) ;
			tap->hwprovider = (a + ((flen + 1) & ai++)) ;
			tap->hwserial = (a + ((flen + 1) & ai++)) ;
			tap->nisdomain = (a + ((flen + 1) & ai++)) ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if (getbufsize) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uinfo_auxbegin) */

static int uinfo_auxend(uinfo_tmpaux *tap) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (tap) {
	    rs = SR_OK ;
	    if (tap->a) {
	        rs1 = uc_libfree(tap->a) ;
		if (rs >= 0) rs = rs1 ;
		tap->a = nullptr ;
	    }
	}
	return rs ;
}
/* end subroutine (uinfo_auxend) */

static int uinfo_auxload(uinfo_tmpaux *tap) noex {
	cint    	nlen = tap->flen ;
	int		rs = SR_OK ;
	int		sz = 0 ;
	tap->architecture[0] = '\0' ;
	tap->platform[0] = '\0' ;
	tap->hwprovider[0] = '\0' ;
	tap->hwserial[0] = '\0' ;
	tap->nisdomain[0] = '\0' ;
	for (cauto &req : sais) {
	    char	*nbuf = nullptr ;
	    switch (req) {
	    case SAI_ARCHITECTURE:
	        nbuf = tap->architecture ;
		break ;
	    case SAI_PLATFORM:
	        nbuf = tap->platform ;
		break ;
	    case SAI_HWPROVIDER:
	        nbuf = tap->hwprovider ;
		break ;
	    case SAI_HWSERIAL:
	        nbuf = tap->hwserial ;
		break ;
	    case SAI_RPCDOMAIN:
	        nbuf = tap->nisdomain ;
		break ;
	    } /* end switch */
	    if ((req >= 0) && nbuf) {
	        if ((rs = uc_sysauxinfo(nbuf,nlen,req)) >= 0) {
		    sz += (rs + 1) ;
		} else if (rs == SR_NOTFOUND) {
		    rs = SR_OK ;
		    sz += 1 ;		/* for the NUL character */
	            nbuf[0] = '\0' ;
		}
	    } else {
		sz += 1 ;
	    } /* end if */
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (uinfo_auxload) */

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


