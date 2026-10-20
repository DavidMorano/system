/* cm_opener SUPPORT (Communications-Connection Manager) */
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
#include	<fieldterms.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<systems.h>		/* LIBUC */
#include	<sysdialer.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"cm_defs.h"
#include	"cm_subinfo.hh"
#include	"cm_vars.hh"
#include	"cm_caller.hh"
#include	"cm_opener.hh"
#include	"cm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef mainv		mv ;

extern "C" {
    typedef int (*open_f)	(void *) noex ;
    typedef int (*close_f)	(void *) noex ;
    typedef int (*shutdown_f)	(void *,int) noex ;
    typedef int (*reade_f)	(void *,void *,int,int,int) noex ;
    typedef int (*recve_f)	(void *,void *,int,int,int,int) noex ;
    typedef int (*write_f)	(void *,cvoid *,int) noex ;
} /* end extern (C) */


/* external subroutines */

namespace cm_ns {
    extern int cm_trysys(CM *,SI *,SD *,SD_ARGS *,
			EC *,SY_ENT *,cc *,cc *,mv) noex ;
    extern int cm_trysysargs(CM *,SI *,vecstr *,cc *,int) noex ;
} /* end namespace (cm_ns) */


/* external variables */


/* local structures */

namespace {
    struct caller {
    	open_f		open ;
    	close_f		close ;
    	shutdown_f	shutdown ;
    	reade_f		reade ;
    	recve_f		recve ;
    	write_f		write ;
    } ; /* end struct (caller) */
} /* end namespace */


/* forward references */

local void cm_errclose(cm *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

namespace cm_ns {
    opener::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = procargs()) >= 0) ylikely {
	    {
	        if ((rs = cooks.start) >= 0) ylikely {
	    	    if ((rs = loadcooks()) >= 0) ylikely {
			if (SY_ENT *sep ; (rs = tryfind(&sep)) >= 0) ylikely {
			    rs = trysave(sep)  ;
			} /* end */
		    } /* end if (cm_loadcooks) */
	        } /* end if (load-cooks) */
	        rs1 = cooks.finish ;
                if (rs >= 0) rs = rs1 ;
	    } /* end block */
	    if (rs < 0) {
		cm_errclose(op) ;
	    } /* end if (error) */
	} /* end if (procargs) */
    	return rs ;
    } /* end method (opener::operator) */
} /* end namespce (cm_ns) */

namespace cm_ns {
    int opener::procargs() noex {
    	int		rs = SR_OK ;
	{
	    da.pr	= argp->pr ;
	    da.prn	= argp->prn ;
	    da.timeout	= argp->timeout ;
	    da.options	= argp->options ;
	}
	return rs ;
    } /* end method (opener::procrgs) */
} /* end namespce (cm_ns) */

namespace cm_ns {
    int opener::tryfind(SY_ENT **sepp) noex {
	systems		*sysp = argp->sysp ;
	int		rs ;
	int		rs1 ;
	*sepp = nullptr ;
	if (SY_CUR cur ; (rs = systems_curbegin(sysp,&cur)) >= 0) {
	    SY_ENT *sep ;
	    while ((rs = systems_fetch(sysp,hn,&cur,&sep)) >= 0) {
		sysdialer *sdp = argp->dialp ;
	        rs = cm_trysys(op,&si,sdp,&da,&cooks,sep,hn,sn,av) ;
		*sepp = sep ;
	        if (rs != 0) break ;
	    } /* end while */
	    rs1 = systems_curend(sysp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return rs ;
    } /* end method (opener::tryfind) */
} /* end namespce (cm_ns) */

namespace cm_ns {
    int opener::trysave(SY_ENT *sep) noex {
    	int		rs = SR_BUGCHECK ;
        if (sep) {
	    cint	sl = sep->dialernamel ;
	    ccharp	sp = sep->dialernamep ;
	    if (cchar *cp ; (rs = lm_strw(sp,sl,&cp)) >= 0) {
	        op->magval = CM_MAGIC ;
	        op->dname = cp ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
    } /* end method (opener::trysave) */
} /* end namespce (cm_ns) */


/* private subroutines */

#ifdef	COMMENT
int opener::loadcooks() noex {
	int		rs = SR_OK ;
	int		sz ;
	int		cl ;
	char		*argsbuf = nullptr ;

	if ((rs >= 0) && argp->searchname) {
	    rs = cools.add("S",argp->searchname) ;
	}

	if ((rs >= 0) && (argp->pr != nullptr)) {
	    rs = expcook_add(cookp,"R",argp->pr,-1) ;
	}

	if ((rs >= 0) && (argp->prn != nullptr)) {
	    rs = expcook_add(cookp,"RN",argp->prn,-1) ;
	}

	if (rs >= 0) {
	    cchar	*nnp, *dnp ;
	    char	domainname[MAXHOSTNAMELEN + 1] ;
	    char	nodename[NODENAMELEN + 1] ;

	    if ((argp->nodename == nullptr) || (argp->domainname == nullptr)) {
	        nnp = nodename ;
	        dnp = domainname ;
	        if ((rs = getnodedomain(nodename,domainname)) >= 0) {
	            if ((rs = expcook_add(cookp,"N",nodename,-1)) >= 0) {
	        	rs = expcook_add(cookp,"D",domainname,-1) ;
		    }
		} 
	    } else {
	        nnp = argp->nodename ;
	        dnp = argp->domainname ;
	        if ((rs = expcook_add(cookp,"N",argp->nodename,-1)) >= 0) {
	            rs = expcook_add(cookp,"D",argp->domainname,-1) ;
		}
	    } /* end if */

	    if ((rs >= 0) && (argp->hostname == nullptr)) {
	        char	hbuf[MAXHOSTNAMELEN + 1] ;
	        sz = snsds(hbuf,MAXHOSTNAMELEN,nnp,dnp) ;
	        rs = expcook_add(cookp,"H",hbuf,sz) ;
	    } else {
	        rs = expcook_add(cookp,"H",argp->hostname,-1) ;
	    }

	} /* end if */

	if ((rs >= 0) && (argp->username == nullptr)) {
	    char	ubuf[USERNAMELEN + 1] ;
	    if ((rs = getusername(ubuf,USERNAMELEN,-1)) >= 0) {
	        rs = expcook_add(cookp,"U",ubuf,-1) ;
	    }
	} else {
	    rs = expcook_add(cookp,"U",argp->username,-1) ;
	}

	if (rs >= 0) {
	    char	timebuf[TIMEBUFLEN + 1] ;
	    cl = ctdecui(timebuf,TIMEBUFLEN,argp->timeout) ;
	    rs = expcook_add(cookp,"t",timebuf,cl) ;
	} /* end block */

	if (rs >= 0) {
	    rs = expcook_add(cookp,"h",hostname,-1) ;
	}

	if (rs >= 0) {
	    rs = expcook_add(cookp,"s",svcname,-1) ;
	}

	sz = 1 ;
	if ((rs >= 0) && (av != nullptr)) {
	    for (int i = 0 ; av[i] != nullptr ; i += 1) {
		sz += (lenstr(av[i]) + 1) ;
	    } /* end for */
	}

	sip->argslen = (sz - 1) ;
	if ((rs >= 0) && ((rs = lm_mall(sz,&argsbuf)) >= 0)) {
	    int		alen = 0 ;

	    argsbuf[0] = '\0' ;
	    if (av != nullptr) {
	        sbuf ab ;
	        if ((rs = sbuf_start(&ab,argsbuf,sip->argslen)) >= 0) {
	            for (int i = 0 ; av[i] != nullptr ; i += 1) {
	                if (i > 0) sbuf_chr(&ab,' ') ;
	                sbuf_strw(&ab,av[i],-1) ;
	            } /* end for */
	            alen = sbuf_finish(&ab) ;
		    if (rs >= 0) rs = alen ;
		} /* end if (sbuf) */
	    } /* end if (had some arguments) */

	    if (rs >= 0) {
	        rs = expcook_add(cookp,"a",argsbuf,alen) ;
	    } /* end if (ok) */

	    lm_free(argsbuf) ;
	} /* end if (memory_allocation) */

	return rs ;
} /* end method (opener::loadcooks) */
#endif /* COMMENT */

namespace cm_ns {
    int opener::loadcooks() noex {
    	return SR_OK ;
    } /* end method (opener::loadcooks) */
} /* end namespce (cm_ns) */

local void cm_errclose(cm *op) noex {
    	using cm_ns::caller ;
	if (op->dobj && op->callp) {
	    if (caller *callp = resumelife<caller>(op->callp)) {
		if (cauto fun = callp->close) {
	            fun(op->dobj) ;
	        } /* end if */
	    } /* end if (non-null) */
	    {
	        lm_free(op->dobj) ;
	        op->dobj = nullptr ;
	    } /* end (memory-release) */
	} /* end if (non-null) */
	op->magval = 0 ;
} /* end subroutine (cm_errclose) */


