/* loadave SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* loadave gathering and manipulation */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
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
#include	<sys/types.h>
#include	<sys/systeminfo.h>
#include	<unistd.h>
#include	<kstat.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<baops.h>
#include	<strwcpy.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"loadave.h"


/* local defines */

#define	LA		loadave
#define	LA_VALS		loadave_vals
#define	LA_MID		loadave_mid
#define	LA_MAGIC	1686593468
#define	LA_BUFLEN	100
#define	LA_INTUPDATE	(1 * 3600)
#define	LA_INTMAXOPEN	(4 * 3600)

#define	TO_KMAXIDLE	(5 * 60)	/* maximum idle time allowed */
#define	TO_KUPDATE	(15 * 60)	/* time before a chain update */
#define	TO_KMAXOPEN	(60 * 60)	/* maximum open time */
#define	TO_KSYSMISC	5		/* time between system_misc gets */
#define	TO_LOADAVE	1		/* time between LOADAVE gets */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	loadave_kopen(LA *,time_t) noex ;
static int	loadave_kclose(LA *) noex ;
static int	loadave_getid(LA *,time_t) noex ;
static int	loadave_getdata(LA *,LA_VALS *) noex ;

static void	popuint() noex ;
static void	poptime() noex ;


/* local variables */

enum miscnames {
	miscname_ncpus,
	miscname_nprocs,
	miscname_la1min,
	miscname_la5min,
	miscname_la15min,
	miscname_overlast
} ; /* end enum (niscnames) */

constexpr cpcchar	miscnames[] = {
	"ncpus",
	"nprocs",
	"avenrun_1min",
	"avenrun_5min",
	"avenrun_15min",
	nullptr
} ; /* end array (niscnames) */


/* exported variables */


/* exported subroutines */


int loadave_start(loadave *eop) noex {
	kstat_named_t *ksn ;
	time_t		daytime = time(nullptr) ;
	int		rs ;

#if	CF_DEBUGS
	debugprintf("loadave_start: ent\n") ;
#endif

	if (eop == nullptr) return SR_FAULT ;

	memset(eop,0,sizeof(LA)) ;

	rs = loadave_kopen(eop,daytime) ;
	if (rs < 0) goto bad0 ;

/* see if we can actually get any data ! */

	eop->v.tim_read = daytime ;
	rs = loadave_getdata(eop,&eop->v) ;
	if (rs < 0) goto bad1 ;

#if	CF_DEBUGS
	debugprintf("loadave_start: loadave_getdata() rs=%d\n",rs) ;
#endif

/* get the machine boot-up time */

	ksn = (kstat_named_t *)
	    kstat_data_lookup(eop->ksp,"boot_time") ;

#if	CF_DEBUGS
	debugprintf("loadave_start: lookup boot_time %s\n",
	    ((ksn == nullptr) ? "no" : "yes")) ;
#endif

	poptime(ksn,&eop->v.tim_boot) ;

#if	CF_DEBUGS
	debugprintf("loadave_start: popped time \n") ;
#endif


/* load the time this data was taken */

	eop->tim_access = daytime ;

	eop->magic = LA_MAGIC ;

ret0:
	return rs ;

/* bad things */
bad1:
	if (eop->kcp != nullptr) {
	    kstat_close(eop->kcp) ;
	    eop->kcp = nullptr ;
	}

bad0:
	goto ret0 ;
}
/* end subroutine (loadave_start) */

int loadave_finish(loadave *eop) noex {

#if	CF_DEBUGS
	debugprintf("loadave_finish: ent\n") ;
#endif

	if (eop == nullptr) return SR_FAULT ;

	if (eop->magic != LA_MAGIC) return SR_NOTOPEN ;

	if (eop->kcp != nullptr) {
	    kstat_close(eop->kcp) ;
	    eop->kcp = nullptr ;
	}

	return SR_OK ;
}
/* end subroutine (loadave_finish) */

int loadave_readvalues(loadave *eop,loadave_vals *vp) noex {
	time_t		daytime = time(nullptr) ;
	int		rs = SR_OK ;

	if (eop == nullptr) return SR_FAULT ;
	if (vp == nullptr) return SR_FAULT ;

	if (eop->magic != LA_MAGIC) return SR_NOTOPEN ;

/* have we reached the timeout for the chain update? */

	if (eop->fl.open &&
	    ((daytime - eop->tim_update) >= TO_KUPDATE)) {

	    eop->tim_update = daytime ;
	    eop->ksp = nullptr ;
	    (void) kstat_chain_update(eop->kcp) ;

	} /* end if (chain update was needed) */

	if ((daytime - eop->v.tim_read) >= TO_KSYSMISC) {

	    if (! eop->fl.open)
	        rs = loadave_kopen(eop,daytime) ;

	    if (rs >= 0) {
	        eop->v.tim_read = daytime ;
	        rs = loadave_getdata(eop,&eop->v) ;
	    }

	} /* end if */

#if	CF_GETLOADAVG
	if ((eop->v.tim_read != daytime) &&
		((daytime - eop->tim_access) >= TO_LOADAVE)) {
		uint	la[3] ;

		rs = u_getloadavg(la,3) ;

		eop->v.la1min = la[0] ;
		eop->v.la5min = la[1] ;
		eop->v.la15min = la[2] ;

	}
#endif /* CF_GETLOADAVG */

/* satisfy this current request */

	eop->tim_access = daytime ;
	memcpy(vp,&eop->v,sizeof(LA_VALS)) ;

	return rs ;
}
/* end subroutine (loadave_readvalues) */

int loadave_readmid(loadave *eop,loadave_mid *vp) noex {
	time_t		daytime = time(nullptr) ;
	int		rs = SR_OK ;

	if (eop == nullptr) return SR_FAULT ;
	if (vp == nullptr) return SR_FAULT ;

	if (eop->magic != LA_MAGIC) return SR_NOTOPEN ;

	if ((daytime - eop->mid.tim_read) >= TO_KUPDATE)
	    rs = loadave_getid(eop,daytime) ;

	memcpy(vp,&eop->mid,sizeof(LA_MID)) ;

	return rs ;
}
/* end subroutine (loadave_readmid) */

int loadave_check(loadave *eop,time_t daytime) noex {
	int		rs = SR_OK ;

	if (eop == nullptr) return SR_FAULT ;

	if (eop->magic != LA_MAGIC) return SR_NOTOPEN ;

	if (daytime == 0)
	    daytime = time(nullptr) ;

/* check on the weirdo 'K' stuff */

	if (eop->fl.open &&
	    ((daytime - eop->tim_access) >= TO_KMAXIDLE))
	    loadave_kclose(eop) ;

	if (eop->fl.open &&
	    ((daytime - eop->tim_open) >= TO_KMAXOPEN))
	    loadave_kclose(eop) ;

	return rs ;
}
/* end subroutine (loadave_check) */


/* private subroutines */

static int loadave_kopen(loadave *eop,time_t daytime) noex {
	eop->kcp = kstat_open() ;

	if (eop->kcp == nullptr)
	    return SR_NOTSUP ;

	eop->tim_open = daytime ;
	eop->tim_update = daytime ;
	eop->fl.open = TRUE ;

	return SR_OK ;
}
/* end subroutine (loadave_kopen) */

static int loadave_kclose(loadave *eop) noex {
	if (eop->kcp != nullptr) {
	    kstat_close(eop->kcp) ;
	    eop->kcp = nullptr ;
	}
	eop->fl.open = FALSE ;
	return SR_OK ;
}
/* end subroutine (loadave_kclose) */

static int loadave_getid(loadave *eop,time_t daytime) noex {
	loadave_mid	*mp ;
	int		rs ;
	char		buf[LA_BUFLEN + 1] ;

	mp = &eop->mid ;

/* get the easy stuff that does not change */

	rs = u_sysinfo(SI_HW_PROVIDER,buf,LA_BUFLEN) ;
	if (rs < 0)
	    goto bad0 ;

	strwcpy(mp->provider,buf,LA_IDLEN) ;

	rs = u_sysinfo(SI_HW_SERIAL,buf,LA_BUFLEN) ;
	if (rs < 0)
	    goto bad1 ;

	strwcpy(mp->serial,buf,LA_IDLEN) ;

	mp->tim_read = daytime ;
	mp->hostid = (uint) gethostid() ;

ret0:
	return 0 ;

/* bad stuff */
bad1:
bad0:
	goto ret0 ;
}
/* end subroutine (loadave_getid) */

static int loadave_getdata(loadave *eop,loadave_vals *vp) noex {
	kstat_t		*ksp ;
	kstat_named_t	*ksn ;
	kid_t		kid ;
	int		rs = SR_OK ;
	int		i, j ;
	uchar		haveval[miscname_overlast] ;

	if ((ksp = eop->ksp) == nullptr) {

#if	CF_DEBUGS
	    debugprintf("loadave_getdata: lookup 'system_misc' !\n") ;
#endif

	    ksp = kstat_lookup(eop->kcp, "unix", 0, "system_misc") ;

	    if (ksp == nullptr) {
	        rs = SR_NOTSUP ;
	        goto bad0 ;
	    }

	    eop->ksp = ksp ;

	} /* end if (chached KSTAT pointer) */

#if	CF_DEBUGS
	debugprintf("loadave_getdata: got a 'system_misc'\n") ;
#endif

	kid = kstat_read(eop->kcp, ksp, nullptr) ;

	if (kid < 0) {
	    rs = SR_IO ;
	    goto bad0 ;
	}

	memset(haveval,0,miscname_overlast) ;

#if	CF_DEBUGS
	debugprintf("loadave_getdata: read some data, kid=%d\n",
	    kid) ;
#endif

	ksn = (kstat_named_t *) ksp->ks_data ;
	for (i = 0 ; i < ksp->ks_ndata ; i += 1) {

#if	CF_DEBUGS
	    debugprintf("loadave_getdata: looping i=%d\n",i) ;
#endif

/* see if the name is one that we want */

	    for (j = 0 ; miscnames[j] != nullptr ; j += 1) {

	        if ((! haveval[j]) &&
	            (strncmp(ksn->name,miscnames[j],KSTAT_STRLEN) == 0))
	            break ;

	    } /* end for */

	    if (miscnames[j] != nullptr) {
	        haveval[j] = 1 ;
	        switch (j) {
	        case miscname_ncpus:
	            popuint(ksn,&vp->ncpu) ;
	            break ;
	        case miscname_nprocs:
	            popuint(ksn,&vp->nprocs) ;
	            break ;
	        case miscname_la1min:
	            popuint(ksn,&vp->la1min) ;
	            break ;
	        case miscname_la5min:
	            popuint(ksn,&vp->la5min) ;
	            break ;
	        case miscname_la15min:
	            popuint(ksn,&vp->la15min) ;
	            break ;
	        } /* end switch */
	    } /* end if */
	    ksn += 1 ;
	} /* end for */
	return rs ;
}
/* end subroutine (loadave_getdata) */

static void popuint(kstat_named_t *ksn,uint *vp) noex {
	switch (ksn->data_type) {
	case KSTAT_DATA_CHAR:
	    *vp = (uint) ksn->value.c[0] ;
	    break ;
	case KSTAT_DATA_INT32:
	    *vp = (uint) ksn->value.i32 ;
	    break ;
	case KSTAT_DATA_UINT32:
	    *vp = (uint) ksn->value.ui32 ;
	    break ;
	case KSTAT_DATA_INT64:
	    *vp = (uint) ksn->value.i64 ;
	    break ;
	case KSTAT_DATA_UINT64:
	    *vp = (uint) ksn->value.ui64 ;
	    break ;
	} /* end switch */
}
/* end subroutine (popuint) */

static void poptime(kstat_named_t *ksn,time_t *vp) noex {
	switch (ksn->data_type) {
	case KSTAT_DATA_CHAR:
	    *vp = (time_t) ksn->value.c[0] ;
	    break ;
	case KSTAT_DATA_INT32:
	    *vp = (time_t) ksn->value.i32 ;
	    break ;
	case KSTAT_DATA_UINT32:
	    *vp = (time_t) ksn->value.ui32 ;
	    break ;
	case KSTAT_DATA_INT64:
	    *vp = (time_t) ksn->value.i64 ;
	    break ;
	case KSTAT_DATA_UINT64:
	    *vp = (time_t) ksn->value.ui64 ;
	    break ;
	} /* end switch */
}
/* end subroutine (poptime) */


