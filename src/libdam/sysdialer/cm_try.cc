/* cm_try SUPPORT (Communications-Connection Manager) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Connection Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cm

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<systems.h>		/* LIBUC */
#include	<sysdialer.h>		/* LIBUC */
#include	<fieldterms.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"cm_defs.h"
#include	"cm_vars.hh"
#include	"cm_caller.hh"
#include	"cm_subinfo.hh"
#include	"cm_opener.hh"
#include	"cm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

namespace cm_ns {
    extern int cm_trysys(CM *,SI *,SD *,SD_ARGS *,
			EC *,SY_ENT *,cc *,cc *,mv) noex ;
    extern int cm_trysysargs(CM *,SI *,vecstr *,cc *,int) noex ;
} /* end namespace */

local void cm_trysyserr(cm *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

namespace cm_ns {
    int cm_trysys(cm *op,SI *sip,SD *dp,SD_ARGS *dap,
	    EC *cookp,SY_ENT *sep,cc *sysname,cc *svcname,mainv av) noex {
	int		rs ;
	int		rs1 ;
	bool		f_loaded = false ;
	bool		f_args = false ;

	op->dobj = nullptr ;
	SD_ENT *dep ;
	rs = sysdialer_loadin(dp,sep->dialernamep,&dep) ;
	f_loaded = (rs >= 0) ;

	/* does this module have our attributes */
	if ((rs >= 0) && (dap->options != 0)) {
	    if ((dap->options & op->dflags) != op->dflags) {
	        rs = SR_NOTSUP ;
	    }
	}

	/* OK, allocate and open the module */
	if (rs >= 0) {
	    op->dsz = dep->osz ;
	    rs = lm_mall(dep->osz,&op->dobj) ;
	}

	vecstr args ;
	if (rs >= 0) {
	    rs = vecstr_start(&args,5,0) ;
	    f_args = (rs >= 0) ? true : false ;
	}

	buffer barg ;
	if ((rs >= 0) && ((rs = barg.start(sip->argslen)) >= 0)) {
	    rs = SR_BUGCHECK ;
	    if (caller *callp = resumelife<caller>(op->callp)) {
		cint csz = szof(caller) ;
	    cint	dnl = sep->dialernamel ;
	    ccharp	dnp = sep->dialernamep ;
	    cchar	*abuf = nullptr ;
	    int		alen = 0 ;
	    memclear(op->dobj,op->dsz) ;
	    memcopy(op->callp,dep->callp,csz) ;
	    /* expand out the dialer arguments */
	    if ((rs = args.add(dnp,dnl)) >= 0) {
	        if (sep->dialerargsp) {
		    cint	dargl = sep->dialerargsl ;
		    ccharp	dargp = sep->dialerargsp ;
		    if ((rs = cookp->expbuf(0,&barg,dargp,dargl)) >= 0) {
		        rs = barg.get(&abuf) ;
		        alen = rs ;
		    } /* end if (expcook_expbuf) */
		} /* end if (non-null) */
	    } /* end if (vecstr_add) */
	    /* separate the individual arguments out */
	    if ((rs >= 0) && (alen > 0)) {
		rs = cm_trysysargs(op,sip,&args,abuf,alen) ;
	    } /* end if (ok) */
	    if (rs >= 0) {
	            rs = SR_LIBACC ;
		if (cauto fun = callp->open) {
 	            if (mainv tav ; (rs = args.getvec(&tav)) >= 0) {
	                dap->argv = tav ;
	                rs = fun(op->dobj,dap,sysname,svcname,av) ;
		    } /* end */
		} /* end if (ok) */
	    } /* end if (ok) */
	    } /* end if (non-null) */
	    rs1 = barg.finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open the module) */

	if (f_args) {
	    args.finish() ;
	}

	if (rs < 0) {
	    cm_trysyserr(op) ;
	    if (f_loaded) {
	        sysdialer_loadout(dp,sysname) ;
	    } /* end */
	} /* end if (error) */
	return rs ;
    } /* end subroutine (cm_trysys) */
} /* end namespace (cm_ns) */

local void cm_trysyserr(cm *op) noex {
	    if (op->dobj) {
	        lm_free(op->dobj) ;
		op->dobj = nullptr ;
	    } /* end if (memory-release) */
} /* end subroutine (cmd_trysyserr) */

namespace cm_ns {
    int cm_trysysargs(cm *op,SI *sip,VS *alp,char *abuf,int alen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && sip) ylikely {
	    if (char *fbuf ; (rs = lm_mn(&fbuf)) >= 0) ylikely {
		cint	flen = rs ;
	        if (field fsb ; (rs = fsb.start(abuf,alen)) >= 0) {
	            for (int fl ; (fl = fsb.sharg(terms,fbuf,flen)) > 0 ; ) {
		        rs = alp->add(fbuf,fl) ;
		        if (rs < 0) break ;
	            } /* end for */
	            rs1 = fsb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (field) */
		{
	            rs1 = lm_free(fbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end (memory-release) */
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (cm_trysysargs) */
} /* end namespace (cm_ns) */


