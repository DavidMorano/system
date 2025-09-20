/* lockmemalloc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* memory allocation facility w/ locking */
/* version %I% last-modified %G% */


/* revision history:

	= 2015-04-06, David A­D­ Morano
	These subroutines were originally written to get around the
	lack of a mutex lock around the KSH (AST) memory allocation
	subroutines.  I have put this off for a long time now.  I
	had tried to avoid this little scheme for some time.  And,
	YES, I am pissed off that KSH sucks cock meat by not locking
	its memory allocation subroutines!

*/

/* Copyright © 2015 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a hack to get around the fact that the KSH program
	does not have a mutex around its memory allocation subroutines.

	Although the KSH program wants to be sigle-threaded, the
	problem comes when complicated built-in commands are
	dynamically loaded.  These built-ins can call subroutines
	that are multi-threaded.  This usually leads to a program
	crash due to no mutex lock being around the memory allocation
	facility (using the familiar subroutine names that we all
	know and love: |malloc(3c)| and friends) used by KSH.

	The only real (semi-user visible) subroutine of this module is
	|lockmemalloc_set(3uc)|.

	Synopsis:
	int lockmemalloc_set(int cmd) noex

	Arguments:
	cmd		command to facility

	Returns:
	<0		fail
	>=0		OK

	Notes:

	= Commands are one of:
	0		turn locking OFF
	1		turn locking ON

	By default, locking is OFF.  This is done so that when
	software is linked with this module (by mistake, confusion,
	or otherwise), it will not suffer the performance overhead
	of the locking.

	When KSH loads a built-in and does so from a new shared
	library, it looks for a subroutine named |lib_init()| in
	the associated shared library and calls it.  This above
	subroutine (|lockmemalloc_set(3lkcmd)|) is called from the
	|lib_init(3lkcmd)| subroutine to turn locking ON within the
	module that is loaded by KSH (usually the shared-memory
	object that this subroutine is linked with as a bundle).

	= How to use:

	Put this module somewhere so that it interposes itself upon
	the "real" subroutines with these same names in the lower-level
	UNIX® library adaptation layer |libuc(3uc)|.

	= Compilation options:

	+ CF_MALLOCSTRW - This switch requests that the subroutine
	|mem_strw()| be compiled into this module. This
	might be wanted if the module which otherwise contains the
	subroutine |mem_mall()| is linked with the Solaris®
	"symbolic" mode. That mode ("symbolic") makes references
	from a given module be linked to the symbols in that same
	module if those sumbols are present. Since the subroutine
	|mem_strw()| calls |mem_mall()|, this compile-time
	switch might be wanted if the module containing
	|mem_strw()| is linked with the "symbolic" mode.
	We want the subroutine |mem_strw()| (where ever it
	is located) to always call us (this module) if this module
	is linked into a program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ptm.h>
#include	<localmisc.h>

#include	"lockmemalloc.h"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	LOCKMEMALLOC	struct lockmemalloc


/* external subroutines */


/* external variables */


/* local structures */

struct lockmemalloc {
	ptm		mx ;		/* data mutex */
	volatile uint	f_void ;
	volatile uint	f_init ;
	volatile uint	f_initdone ;
	volatile uint	f_lock ;
} ; /* end struct (lockmemalloc) */


/* forward references */

int		lockmemalloc_init() noex ;
int		lockmemalloc_fini() noex ;

int		mem_mall(int,void *) noex ;

static int	lockmemalloc_basemalloc(int,void *) noex ;
static int	lockmemalloc_basevalloc(int,void *) noex ;
static int	lockmemalloc_baserealloc(cvoid *,int,void *) noex ;
static int	lockmemalloc_basefree(cvoid *) noex ;

static int	lockmemalloc_lockmalloc(int,void *) noex ;
static int	lockmemalloc_lockvalloc(int,void *) noex ;
static int	lockmemalloc_lockrealloc(cvoid *,int,void *) noex ;
static int	lockmemalloc_lockfree(cvoid *) noex ;

static void	lockmemalloc_atforkbefore() noex ;
static void	lockmemalloc_atforkafter() noex ;
static void	lockmemalloc_exit() noex ;


/* local vaiables */

static LOCKMEMALLOC	lockmemalloc_data ;


/* exported variables */


/* exported subroutines */

int lockmemalloc_init() noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs = SR_OK ;
	int		f = false ;
	if (! uip->f_init) {
	    uip->f_init = true ;
	    if ((rs = uip->mx.create) >= 0) {
	        void_f	b = lockmemalloc_atforkbefore ;
	        void_f	a = lockmemalloc_atforkafter ;
	        if ((rs = uc_atforkrecord(b,a,a)) >= 0) {
	            if ((rs = uc_atexit(lockmemalloc_exit)) >= 0) {
	                uip->f_initdone = true ;
			f = true ;
	            } /* end if (uc_atexit) */
	            if (rs < 0) {
	                uc_atforkexpunge(b,a,a) ;
		    }
		} /* end if (uc_atfork) */
		if (rs < 0) {
		    uip->mx.destroy() ;
		}
	    } /* end if (ptm) */
	} else {
	    while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
		rs = msleep(1) ;
		if (rs == SR_INTR) break ;
	    }
	    if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (lockmemalloc_init) */

int lockmemalloc_fini() noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone && (! uip->f_void)) {
	    uip->f_void = true ;
	    {
	        void_f	b = lockmemalloc_atforkbefore ;
	        void_f	a = lockmemalloc_atforkafter ;
	        rs1 = uc_atforkexpunge(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	    	rs1 = uip->mx.destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    uip->f_init = false ;
	    uip->f_initdone = false ;
	} /* end if (atexit registered) */
	return rs ;
}
/* end subroutine (lockmemalloc_fini) */

int lockmemalloc_set(int cmd) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	if ((rs = lockmemalloc_init()) >= 0) {
	    switch (cmd) {
	    case lockmemallocset_end:
	        uip->f_lock = false ;
	        break ;
	    case lockmemallocset_begin:
	        uip->f_lock = true ;
	        break ;
	    default:
	        rs = SR_NOTSUP ;
	        break ;
	    } /* end switch */
	} /* end if (init) */
	return rs ;
}
/* end subroutine (lockmemalloc_set) */

int mem_strw(cchar *sp,int sl,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		sz = 0 ;
	if (sp && rpp) {
	    rs = SR_OK ;
	    *rpp = nullptr ;
	    if (sl < 0) sl = lenstr(sp) ;
	    sz = (sl + 1) ;
	    if (char *bp ; (rs = mem_mall(sz,&bp)) >= 0) {
	        *rpp = bp ;
	        strncpy(bp,sp,sl) ;
	        bp[sl] = '\0' ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
} /* end subroutine (mem_strw) */

int mem_mall(int sz,void *vp) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	if (uip->f_lock) {
	    rs = lockmemalloc_lockmalloc(sz,vp) ;
	} else {
	    rs = lockmemalloc_basemalloc(sz,vp) ;
	}
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (mem_mall) */

int mem_call(int nelem,int esize,void *vp) noex {
	cint	sz = (nelem*esize) ;
	return mem_mall(sz,vp) ;
} /* end subroutine (mem_call) */

int mem_vall(int sz,void *vp) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	if (uip->f_lock) {
	    rs = lockmemalloc_lockvalloc(sz,vp) ;
	} else {
	    rs = lockmemalloc_basevalloc(sz,vp) ;
	}
	return (rs >= 0) ? sz : rs ;
} /* end subroutine (mem_vall) */

int mem_rall(cvoid *cp,int sz,void *vp) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	if (uip->f_lock) {
	    rs = lockmemalloc_lockrealloc(cp,sz,vp) ;
	} else {
	    rs = lockmemalloc_baserealloc(cp,sz,vp) ;
	}
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (mem_rall) */

int mem_free(cvoid *vp) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	if (uip->f_lock) {
	    rs = lockmemalloc_lockfree(vp) ;
	} else {
	    rs = lockmemalloc_basefree(vp) ;
	}
	return rs ;
} /* end subroutine (mem_free) */


/* local subroutines */

static int lockmemalloc_basemalloc(int sz,void *vp) noex {
	void		**rpp = (void **) vp ;
	int		rs = SR_FAULT ;
	if (vp) {
	    rs = SR_INVALID ;
	    if (sz > 0) {
	        csize	msize = sz ;
	        int	to_again = utimeout[uto_again] ;
	        bool	f_exit = false ;
	        void	*rp ;
	        repeat {
	            rs = SR_OK ;
	            if ((rp = malloc(msize)) == nullptr) {
		        rs = (- errno) ;
	                switch (rs) {
	                case SR_AGAIN:
	                    if (to_again-- > 0) {
	                        msleep(1000) ;
		            } else {
	                        f_exit = true ;
	                    }
	                    break ;
	                case SR_INTR:
	                    break ;
		        default:
		            f_exit = true ;
		            break ;
	                } /* end switch */
	            } else {
	                *rpp = rp ;
	            }
	        } until ((rs >= 0) || f_exit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (lockmemalloc_basemalloc) */

static int lockmemalloc_basevalloc(int sz,void *vp) noex {
	void		**rpp = (void **) vp ;
	int		rs = SR_FAULT ;
	if (vp) {
	    *rpp = nullptr ;
	    rs = SR_INVALID ;
	    if (sz > 0) {
	       csize	msize = sz ;
	       int	to_again = utimeout[uto_again] ;
	       bool	f_exit = false ;
	       void	*rp ;
	       repeat {
	           rs = SR_OK ;
	           if ((rp = valloc(msize)) == nullptr) {
		       rs = (- errno) ;
	               switch (rs) {
	               case SR_AGAIN:
	                   if (to_again-- > 0) {
	                       msleep(1000) ;
		           } else {
	                       f_exit = true ;
	                   }
	                   break ;
	               case SR_INTR:
	                   break ;
		       default:
		           f_exit = true ;
		           break ;
	               } /* end switch */
	           } else {
	               *rpp = rp ;
	           }
	       } until ((rs >= 0) || f_exit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (lockmemalloc_basevalloc) */

static int lockmemalloc_baserealloc(cvoid *cp,int sz,void *vp) noex {
	void		*argp = (void *) cp ;
	void		**rpp = (void **) vp ;
	int		rs = SR_FAULT ;
	if (cp && vp) {
	    rs = SR_INVALID ;
	    *rpp = nullptr ;
	    if (sz > 0) {
	        csize		msize = sz ;
	        int		to_again = utimeout[uto_again] ;
	        int		f_exit = false ;
	        void		*rp ;
	        repeat {
	            rs = SR_OK ;
	            if ((rp = realloc(argp,msize)) == nullptr) {
		        rs = (- errno) ;
	                switch (rs) {
	                case SR_AGAIN:
	                    if (to_again-- > 0) {
	                        msleep(1000) ;
		            } else {
	                        f_exit = true ;
	                    }
	                    break ;
	                case SR_INTR:
	                    break ;
		        default:
		            f_exit = true ;
		            break ;
	                } /* end switch */
	            } else {
	                *rpp = rp ;
	            }
	        } until ((rs >= 0) || f_exit) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? sz : rs ;
}
/* end subroutine (lockmemalloc_baserealloc) */

static int lockmemalloc_basefree(cvoid *vp) noex {
	int		rs = SR_OK ;
	if (vp) {
	    ulong	v = (ulong) vp ;
	    rs = SR_BADFMT ;
	    if ((v & 3) == 0) {
	        void	*fvp = (void *) vp ;
		rs = SR_OK ;
	        free(fvp) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lockmemalloc_basefree) */

static int lockmemalloc_lockmalloc(int sz,void *vp) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = lockmemalloc_init()) >= 0) {
	    if ((rs = uip->mx.lockbegin) >= 0) {
		{
	            rs = lockmemalloc_basemalloc(sz,vp) ;
		    rv = rs ;
		}
	        rs1 = uip->mx.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (init) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (lockmemalloc_lockmalloc) */

static int lockmemalloc_lockvalloc(int sz,void *vp) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	int		rs1 ;
	if ((rs = lockmemalloc_init()) >= 0) {
	    if ((rs = uip->mx.lockbegin) >= 0) {
		{
	            rs = lockmemalloc_basevalloc(sz,vp) ;
		}
	        rs1 = uip->mx.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (init) */
	return rs ;
}
/* end subroutine (lockmemalloc_lockvalloc) */

static int lockmemalloc_lockrealloc(cvoid *cp,int sz,void *vp) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = lockmemalloc_init()) >= 0) {
	    if ((rs = uip->mx.lockbegin) >= 0) {
		{
	            rs = lockmemalloc_baserealloc(cp,sz,vp) ;
		    rv = rs ;
		}
	        rs1 = uip->mx.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (init) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (lockmemalloc_lockrealloc) */

static int lockmemalloc_lockfree(cvoid *vp) noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = lockmemalloc_init()) >= 0) {
	    if ((rs = uip->mx.lockbegin) >= 0) {
		{
	            rs = lockmemalloc_basefree(vp) ;
		    rv = rs ;
		}
	        rs1 = uip->mx.lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex) */
	} /* end if (init) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (lockmemalloc_lockfree) */

static void lockmemalloc_atforkbefore() noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	uip->mx.lockbegin() ;
}
/* end subroutine (lockmemalloc_atforkbefore) */

static void lockmemalloc_atforkafter() noex {
	LOCKMEMALLOC	*uip = &lockmemalloc_data ;
	uip->mx.lockend() ;
}
/* end subroutine (lockmemalloc_atforkafter) */

static void lockmemalloc_exit() noex {
	return ;
}
/* end subroutine (lockmemalloc_exit) */


