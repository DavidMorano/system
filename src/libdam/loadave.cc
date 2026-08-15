/* loadave SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* load-average gathering and preparation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_GETLOADAVG	1		/* use |u_getloadavg(2)| */

/* revision history:

	= 1999-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	loadave

	Description:
	This object module provides a convenient way to get the
	load average numbers our of the kernel. This object only
	works for those OSes that support the KSTAT system kernel
	interface API.  OSes that supprt the KSTAT interface are
	usually newer SVR4 systems.  This object is *not* supposed
	to be multi-thread-safe because according to the last that
	I knew, the KSTAT system API was not multi-thread-safe!

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® |gethostid(3c)| */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC */
#include	<ucinfo.h>		/* LIBUC */
#include	<ucnprocs.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strdcpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"loadave.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	LA		loadave
#define	LA_VAL		loadave_val
#define	LA_MID		loadave_mid
#define	LA_MAGIC	LOADAVE_MAGIC
#define	LA_BUFLEN	100
#define	LA_INTUPDATE	(1 * 3600)
#define	LA_INTMAXOPEN	(4 * 3600)

#define	TO_MID		(5 * 60)	/* machine-identification timeout */
#define	TO_NCPU		30		/* time-out n-cpu */
#define	TO_LA		10		/* time-out values */
#define	TO_NPROCS	10		/* time-out n-procs */

#define	NLA		3		/* number of load-average elements */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int loadave_ctor(loadave *op,Args ... args) noex {
    	LOADAVE		*hop = op ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if (op->midp = new(nt) loadave_mid ; op->midp) {
		memclear(op->midp) ;
	        if (op->valp = new(nt) loadave_val ; op->valp) {
		    rs = memclear(op->valp) ;
		} /* end if (new-loadave_val) */
		if (rs < 0) {
		    delete op->midp ;
		    op->midp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-loadave_val) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (loadave_ctor) */

local int loadave_dtor(loadave *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->valp) {
		delete op->valp ;
		op->valp = nullptr ;
	    }
	    if (op->midp) {
		delete op->midp ;
		op->midp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (loadave_dtor) */

template<typename ... Args>
local inline int loadave_magic(loadave *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == LA_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (loadave_magic) */

local int	loadave_midup	(LA *,time_t) noex ;
local int	loadave_valup	(LA *,time_t) noex ;
local int	loadave_la	(LA *,time_t) noex ;
local int	loadave_nprocs	(LA *,time_t) noex ;
local int	loadave_ncpu	(LA *,time_t) noex ;

local int	rethostid() noex ;


/* local variables */

cint		idlen = LOADAVE_IDLEN ;


/* exported variables */


/* exported subroutines */

int loadave_start(loadave *op) noex {
	int		rs ;
	DEBUGPRINTF("ent\n") ;
	if ((rs = loadave_ctor(op)) >= 0) {
	    custime dt = getustime ;
	    {
		op->magval = LA_MAGIC ;
		op->tim_open = dt ;
	    } /* end block */
	    if (rs < 0) {
		loadave_dtor(op) ;
	    } /* end if (error) */
        } /* end if (loadave_ctor) */
	return rs ;
} /* end subroutine (loadave_start) */

int loadave_finish(loadave *op) noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = loadave_magic(op)) >= 0) {
	    {
		rs1 = loadave_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
        } /* end if (loadave_magic) */
	return rs ;
} /* end subroutine (loadave_finish) */

int loadave_readmid(loadave *op,loadave_mid *mip) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = loadave_magic(op,mip)) >= 0) {
	    if ((rs = loadave_midup(op,dt)) >= 0) {
		memcopy(mip,op->midp) ;
	    }
        } /* end if (loadave_magic) */
	return rs ;
} /* end subroutine (loadave_readmid) */

int loadave_readval(loadave *op,loadave_val *vap) noex {
	custime		dt = getustime ;
	int		rs ;
	if ((rs = loadave_magic(op,vap)) >= 0) {
	    if ((rs = loadave_valup(op,dt)) >= 0) {
		memcopy(vap,op->valp) ;
	    }
        } /* end if (loadave_magic) */
	return rs ;
} /* end subroutine (loadave_readval) */

int loadave_check(loadave *op,time_t dt) noex {
	int		rs ;
	if ((rs = loadave_magic(op)) >= 0) {
	    if (dt == 0) dt = getustime ;
	    if ((rs = loadave_midup(op,dt)) >= 0) {
		rs = loadave_valup(op,dt) ;
	    }
        } /* end if (loadave_magic) */
	return rs ;
} /* end subroutine (loadave_check) */


/* private subroutines */

local int loadave_midup(LA *op,time_t dt) noex {
    	loadave_mid	*midp = op->midp ;
    	int		rs = SR_OK ;
	if (dt == 0) dt = getustime ;
	if ((dt - midp->tim_read) >= TO_MID) {
	    if (ucinfo_auxs adata ; (rs = ucinfo_aux(&adata))) {
		midp->hostid = rethostid() ;
		strdcpy(midp->provider,	idlen,adata.hwprovider) ;
		strdcpy(midp->serial,	idlen,adata.hwserial) ;
	        midp->tim_read = dt ;
	    } /* end if (ucinfo_aux) */
	} /* end if (time-out) */
	return rs ;
} /* end subroutine (loadave_midup) */

local int loadave_valup(LA *op,time_t dt) noex {
    	loadave_val	*valp = op->valp ;
    	int		rs = SR_OK ;
	int		n = 0 ; /* return-value */
	if (dt == 0) dt = getustime ;
	if ((dt - valp->tim_la) >= TO_LA) {
	    if ((rs = loadave_la(op,dt)) >= 0) {
		if ((rs = loadave_nprocs(op,dt)) >= 0) {
		    n = rs ;
		    rs = loadave_ncpu(op,dt) ;
		} /* end if */
	    } /* end if (loadave_la) */
	} /* end if (time-out) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (loadave_valup) */

local int loadave_la(LA *op,time_t dt) noex {
	loadave_val	*valp = op->valp ;
    	int		rs ;
	if (uint la[NLA] ; (rs = u_getloadavg(la,NLA)) >= 0) {
	    valp->la1min	= la[0] ;
	    valp->la5min	= la[1] ;
	    valp->la15min	= la[2] ;
	    valp->tim_la = dt ;
	} /* end if (u_getloadave) */
	return rs ;
} /* end subroutine (loadave_la) */

local int loadave_nprocs(LA *op,time_t dt) noex {
	loadave_val	*valp = op->valp ;
    	int		rs = SR_OK ;
	int		n = 0 ; /* return-value */
	if ((dt - valp->tim_nprocs) >= TO_NPROCS) {
	    if ((rs = uc_nprocs(0)) >= 0) {
		valp->nprocs = rs ;
		valp->tim_nprocs = dt ;
		n = rs ;
	    }
	} else {
	    n = valp->nprocs ;
	} /* end if (time-out) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (loadave_nprocs) */

local int loadave_ncpu(LA *op,time_t dt) noex {
	loadave_val	*valp = op->valp ;
    	int		rs = SR_OK ;
	if ((dt - valp->tim_ncpu) >= TO_NCPU) {
	    if ((rs = ucnprocessors) >= 0) {
		valp->ncpu = rs ;
		valp->tim_ncpu = dt ;
	    }
	} /* end if (time-out) */
	return rs ;
} /* end subroutine (loadave_ncpu) */

local int rethostid() noex {
    	clong lid = gethostid() ;
	return intconv(lid) ;
} /* end subroutien (rethostid) */


