/* cm_prime SUPPORT (Communications-Connection Manager) */
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
#include	<expcook.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<sncpy.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
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


/* impored namespaces */

using cm_ns::caller ;			/* type */
using cm_ns::opener ;			/* type */
using cm_ns::vars ;			/* type */
using libuc::libmem ;			/* variable */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */

namespace cm_ns {
    extern int cm_trysys(CM *,SI *,SD *,SD_ARGS *,
			EC *,SY_ENT *,cc *,cc *,mv) noex ;
    extern int cm_trysysargs(CM *,SI *,vecstr *,cc *,int) noex ;
} /* end namespace (cm_ns) */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int cm_ctor(cm *op,Args ... args) noex {
    	CM		*hop = op ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if (caller *callp = new(nt) caller) ylikely {
		op->callp = callp ;
		if (cm_args *cap = new(nt) cm_args) ylikely {
		    op->argp = cap ;
		    rs = SR_OK ;
		} /* end if (new-cm_args) */
		if (rs < 0) {
		    delete callp ;
		    op->callp = nullptr ;
		} /* end if (error) */
	    } /* emd if (new-caller) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cm_ctor) */

local int cm_dtor(cm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (caller *callp = resumelife<caller>(op->callp)) {
		delete callp ;
		op->callp = nullptr ;
	    } /* end if (delete-caller) */
	    if (op->argp) {
		delete op->argp ;
		op->argp = nullptr ;
	    } /* end if (delete-cm_arg) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cm_dtor) */

template<typename ... Args>
local inline int cm_magic(cm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	} /* end */
	return rs ;
} /* end subroutine (cm_magic) */

#ifdef	COMMENT
local int cm_loadcooks(CM *,SI *,EC *,cm_args *,cc *,cc *,mv) noex ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int cm_open(cm *op,cm_args *argp,cc *hn,cc *sn,con mainv av) noex {
	using		cm_ns::var ;
	int		rs = SR_FAULT ;
	if ((rs = cm_ctor(op,argp,hn,sn,av)) >= 0)  ylikely {
	    rs = SR_INVALID ;
	    /* do we have the necessary helper objects */
	    if (argp->dialp && argp->sysp) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) {
		    opener oo(op,argp,hn,sn,av) ;
		    rs = oo ;
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		cm_dtor(op) ;
	    } /* end if (error) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cm_open) */

int cm_close(CM *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cm_magic(op)) >= 0) ylikely {
	    if (op->dname) {
		voidp vp = voidp(op->dname) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dname = nullptr ;
	    } /* end if (memory-release) */
	    /* close the connection */
	    if (caller *callp = resumelife<caller>(op->callp)) {
		if (cauto fun = callp->close) {
	            rs1 = fun(op->dobj) ;
	            if (rs >= 0) rs = rs1 ;
	            callp->close = nullptr ;
		}
	    } /* end */
	    if (op->dobj) {
	        rs1 = lm_free(op->dobj) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dobj = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = cm_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end */
	    op->magval = 0 ;
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_close) */

int cm_getinfo(CM *op,char *nbuf,int nlen,CM_INFO *ip) noex {
    	int		rs ;
	if ((rs = cm_magic(op,nbuf,ip)) >= 0) ylikely {
	    ip->itype = op->itype ;
	    ip->dflags = op->dflags ;
	    rs = sncpy(nbuf,nlen,op->dname) ;
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_info) */

int cm_reade(CM *op,char *bufp,int bufl,int to,int ro) noex {
	int		rs ;
	if ((rs = cm_magic(op,bufp)) >= 0) ylikely {
	    if (caller *callp = resumelife<caller>(op->callp)) {
		if (cauto fun = callp->reade) {
	            rs = fun(op->dobj,bufp,bufl,to,ro) ;
		}
	    } /* end */
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_reade) */

int cm_recve(CM *op,char *bufp,int bufl,int fl,int to,int ro) noex {
	int		rs ;
	if ((rs = cm_magic(op,bufp)) >= 0) ylikely {
	    if (caller *callp = resumelife<caller>(op->callp)) {
		if (cauto fun = callp->recve) {
	            rs = fun(op->dobj,bufp,bufl,fl,to,ro) ;
		}
	    } /* end */
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_recve) */

int cm_write(CM *op,cchar *bufp,int bufl) noex {
	int		rs ;
	if ((rs = cm_magic(op)) >= 0) ylikely {
	    if (caller *callp = resumelife<caller>(op->callp)) {
		if (cauto fun = callp->write) {
	            rs = fun(op->dobj,bufp,bufl) ;
		}
	    }
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_write) */

/* do the shutdown if the module supports it */
int cm_shutdown(CM *op,int cmd) noex {
	int		rs ;
	if ((rs = cm_magic(op)) >= 0) ylikely {
	    if (caller *callp = resumelife<caller>(op->callp)) {
		if (cauto fun = callp->shutdown) {
	            rs = fun(op->dobj,cmd) ;
		}
	    } /* end */
	} /* end if (cm_magic) */
	return rs ;
} /* end subroutine (cm_shutdown) */


/* private subroutines */


