/* unlinker SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to try and invoke the UNLINK daemon */
/* version %I% last-modified %G% */

#define	CF_SETRUID	0		/* set real UID to EUID */
#define	CF_RMER		0		/* try the RMER server */

/* revision history:

	= 1998-05-14, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/******************************************************************************

	Name:
	unlinkd

	Description:
	This subroutine calls the UNLINKD program to delete (unlink)
	files.

	Synopsis:
	int unlinkd(cchar *filename,int delay) noex

	Arguments:
	filename	filename to unlink
	delay		time to wait before the unlink in seconds

	Returns:
	>=0		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/wait.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usysdata.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucproc.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getprogpath.h>		/* LIBUC */
#include	<spawnproc.h>		/* LIBUC */
#include	<mkpath.h>		/* LIBUC */
#include	<mkpr.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<mapex.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |NOFILE| */

#include	"unlinkd.h"
#include	"rmermsg.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	VARPRLOCAL
#define	VARPRLOCAL	"LOCAL"
#endif

#define	DEFDELAY	30

#define	PROG_RMER	"rmer"
#define	PROG_UNLINKD	"unlinkd"

#define	DEFEXECPATH	"/usr/bin:/usr/extra/bin"

#define	SI		subinfo
#define	SI_FL		subinfo_fl
#define	SI_ARGS		subinfo_args


/* imported namespaces */

using libuc::mem ;		/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum subinfomems {
    subinfomem_finish,
    subinfomem_overlast
} ; /* end enum (subinfomems) */

namespace {
    struct vars {
	uint		sysid ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */

namespace {
    struct subinfo ;
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
    struct subinfo_args {
	cchar		*fname ;
	uint		delay ;
    } ; /* end struct */
    struct subinfo_fl {
	uint		here:1 ;
    } ; /* end struct */
    struct subinfo {
	friend		subinfo_co ;
	subinfo_co	finish ;
	SI_ARGS		arg ;
	mainv		envv ;
	time_t		daytime ;
	SI_FL		fl ;
	subinfo() noex {
	    finish	(this,subinfomem_finish) ;
	} ; /* end ctor */
	int start	(cchar *,int) noex ;
    } ; /* end struct */
} /* end namespace */

typedef int (*subinfo_f)(subinfo *) noex ;


/* forward references */

local int subinfo_start	(SI *,cchar *,int) noex ;
local int subinfo_finish	(SI *) noex ;
local int subinfo_trylocal	(SI *) noex ;
local int subinfo_rmlocal	(SI *) noex ;
local int closefds(int) noex ;


/* local variables */

constexpr subinfo_f	scheds[] = {
	subinfo_trylocal,
} ; /* end array */

static vars	var ;
cbool		f_setruid = CF_SETRUID ;


/* exported variables */


/* exported subroutines */

int unlinkd(cchar *fname,int delay) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		    if (ustat sb ; (rs = u_stat(fname,&sb)) >= 0) ylikely {
	                if (subinfo si ; (rs = si.start(fname,delay)) >= 0) {
		            for (cauto &fun : scheds) {
	    	                rs = (*fun)(&si) ;
	    	                if (rs >= 0) break ;
	                    } /* end for */
	                    rs1 = si.finish ;
		            if (rs >= 0) rs = rs1 ;
			} /* end if (subinfo) */
		    } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
		    } /* end if (uc_stat) */
		} /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (unlinkd) */


/* local subroutines */

local int subinfo_start(SI *sip,cchar *fname,int delay) noex {
	int		rs = SR_FAULT ;
	if (delay <= 0) delay = DEFDELAY ;
	if (sip && fname) ylikely {
	    memclear(sip) ;
	    if (mainv ev ; (rs = u_getenviron(&ev)) >= 0) ylikely {
		sip->envv = ev ;
	        sip->daytime = getustime ;
	        sip->arg.fname = fname ;
	        sip->arg.delay = delay ;
	    } /* end if (u_getenviron) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
    	int		rs = SR_FAULT ;
	if (sip) ylikely {
	    sip->daytime = 0 ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_finish) */

local int subinfo_trylocal(SI *sip) noex {
	int		rs ;
	if ((rs = u_fork()) == 0) ylikely {
	    rs = subinfo_rmlocal(sip) ;
	} /* end if (u_fork) */
	return rs ;
} /* end subroutine (subinfo_trylocal) */

local int subinfo_rmlocal(SI *sip) noex {
	int		rs = SR_OK ;
	if_constexpr (f_setruid) {
	        con uid_t	uid = getuid() ;
	        con uid_t	euid = geteuid() ;
	        if (euid != uid) {
	            u_setreuid(euid,-1) ;
	        }
	} /* end if_constexpr (f_setruid) */
	if (rs >= 0) {
    	    int		ex = EX_OK ;
	    if ((rs = closefds(NOFILE)) >= 0) {
	        time_t	ti_expire ;
	        ustat	sb ;
	        u_setsid() ;
	        ti_expire = (sip->daytime + sip->arg.delay) ;
	        while (ti_expire > sip->daytime) {
	            uc_safesleep(1) ;
	            sip->daytime = time(nullptr) ;
	            rs = u_stat(sip->arg.fname,&sb) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && sip->arg.fname) {
	            rs = u_unlink(sip->arg.fname) ;
	        } /* end if */
	    } /* end if (closefds) */
	    if ((ex == EX_OK) && (rs < 0)) {
		rs = mapex(nullptr,rs) ;
	    } /* end if */
	    uc_exit(ex) ;
	} /* end if (u_ok) */
	return rs ;
} /* end subroutine (subinfo_rmlocal) */

int subinfo::start(cchar *sp,int sl) noex {
    	return subinfo_start(this,sp,sl) ;
} /* end method (subinfo::start) */

subinfo_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case subinfomem_finish:
	        rs = subinfo_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (subinfo_co::operator) */

vars::operator int () noex {
    	int		rs ;
	if (ulong hid ; (rs = u_gethostid(&hid)) >= 0) {
	    sysid = conv<uint>(hid) ;
	} /* end if (u_gethostid) */
	return rs ;
} /* end method (vars::operator) */

local int closefds(int n) noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < n ; i += 1) {
	    if ((rs1 = uc_close(i)) < 0) {
		if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	    }
	} /* end for */
    	return rs ;
} /* end subroutine (closefds) */


