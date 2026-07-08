/* pimkrand SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make some light random data */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_ENVIRON	0		/* use environment */
#define	CF_HEAVY	0		/* use heavy (strong) randomness */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pimkrand

	Description:
	This subroutine creates a unique (?) string no more than
	MSGBOUND characters.

	Synopsis:
	int pimkrand(PROGINFO *pip) noex

	Arguments:
	pip		pointer to program information

	Returns:
	>=0		length of result
	<0		error (system-return)

	Notes:
	1. The initial buffer size estimate given to the BUFFER
	object des not really matter.  That object dynamically
	increases its internal buffer size as needed automatically.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/time.h>		/* POSIX |gethrtime(3c)| */
#include	<unistd.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC */
#include	<ucgetx.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<hash.h>		/* LIBUC |hashmunch(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"proginfo.hh"
#include	"pimkrand.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	PI
#define	PI		proginfo
#endif

#define	BSZMULT		2

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_ENVIRON
#define	CF_ENVIRON	0		/* use environment */
#endif
#ifndef	CF_HEAVY
#define	CF_HEAVY	0		/* use heavy (strong) randomness */
#endif


/* external subroutines */


/* local structures */


/* forward references */

local int	mkrand_light(PI *) noex ;
local int	mkrand_heavy(PI *) noex ;


/* local variables */

cbool		f_comment	= false ;
cbool		f_debug		= CF_DEBUG ;
cbool		f_environ	= CF_ENVIRON ;
cbool		f_heavy		= CF_HEAVY ;


/* exported variables */


/* exported subroutines */

int pimkrand(PI *pip) noex {
	int		rs = SR_OK ;
	if (pip->randvar == 0) {
	    rs = mkrand_light(pip) ;
	    if_constexpr (f_heavy) {
	        if (rs >= 0) {
	            rs = mkrand_heavy(pip) ;
	        }
	    } /* end if_constexpr (f_heavy) */
	} /* end if */
	return rs ;
} /* end subroutine (pimkrand) */


/* local subroutines */

local int mkrand_light(PI *pip) noex {
    	cnullptr	np{} ;
	const pid_t	pid = getppid() ;
	ulong		rv = 0 ;
	ulong		v ;
	int		rs = SR_OK ;
	int		rs1 ;
	if_constexpr (f_comment) {
	    v = gethostid() ;
	    rv ^= (v << 32) ;
	} /* end if_constexpr (f_comment) */
	{
	    v = pip->uid ;
	    rv ^= (v << 32) ;
	}
	{
	    v = ulong(pid) ;
	    rv += (v << 16) ;
	}
	{
	    v = pip->pid ;
	    rv += v ;
	}
	if (TIMEVAL tod ; (rs1 = uc_gettimeofday(&tod,np)) >= 0) {
	    v = tod.tv_sec ;
	    rv ^= (v << 32) ;
	    rv ^= tod.tv_usec ;
	    rv ^= rs1 ;
	} else {
	    v = pip->daytime ;
	    rv ^= (v << 32) ;
	}
	{
	   v = pip->serial ;
	   rv += v ;
	}
	if (rs >= 0) {
	    pip->randvar += rv ;
	}
	return rs ;
} /* end subroutine (mkrand_light) */

local int mkrand_heavy(PI *pip) noex {
    	cnullptr	np{} ;
	ulong		rv = 0 ;
	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) {
	    cint bsz = (BSZMULT * rs) ;
	    if (buffer hb ; (rs = buffer_start(&hb,bsz)) >= 0) {
	        int	bl ;
		cchar	*cp ;
	        if (pip->pwd) {
	            hb.strw(pip->pwd) ;
	        }
	        if (pip->progename) {
	            hb.strw(pip->progename) ;
	        }
	        if (pip->username) {
	            hb.strw(pip->username) ;
	        }
	        if (pip->homedname) {
	            hb.strw(pip->homedname) ;
	        }
	        if (pip->nodename) {
	            hb.strw(pip->nodename) ;
	        }
	        if (pip->domainname) {
	            hb.strw(pip->domainname) ;
	        }
	        if (pip->org) {
	            hb.strw(pip->org) ;
	        }
	        if (pip->name) {
	            hb.strw(pip->name) ;
	        }
	        if (pip->fullname) {
	            hb.strw(pip->fullname) ;
	        }
	        if ((cp = getenv(varname.random)) != np) {
	            hb.strw(cp) ;
	        }
	        if ((cp = getenv(varname.seconds)) != np) {
	            hb.strw(cp) ;
	        }
	        /* get this stuff so far */
	        if (cchar *buf{} ; (rs = hb.get(&buf)) >= 0) {
	            rs = hashmunch(buf,rs,&rv) ;
	        }
	        /* I think we are done with the buffer */
	        bl = hb.finish ;
	        if (rs >= 0) rs = bl ;
	    } /* end if (buffer) */
	    /* pop in our environment also! */
	    if_constexpr (f_environ) {
	       if (rs >= 0) {
		   ulong hv = 0 ;
	           for (int i = 0 ; pip->envv[i] ; i += 1) {
		       cchar *evar = pip->envv[i] ;
	               hv = hash_elf(evar,-1) ;
	               rv ^= (hv << ((i & 1) ? 32 : 0)) ;
	           } /* end for */
	       } /* end if (ok) */
	    } /* end if_constexpr (f_environ) */
	    if (rs >= 0) {
	        pip->randvar += rv ;
	    }
	} /* end if (bufsizeget) */
	return rs ;
} /* end subroutine (mkrand_heavy) */


