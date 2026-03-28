/* loguser SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* this is a PCSPOLLS module for performing LOGUSER pseudo-polls */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 2008-10-07, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	loguser

	Description:
	This object is a PCSPOLLS module for performing LOGUSER polls.

	Synopsis:
	int loguser_start(op,pr,sn,envv,pcp)
	PCSPOLLS	*op ;
	cchar	*pr ;
	cchar	*sn ;
	cchar	**envv ;
	PCSCONF		*pcp ;

	Arguments:
	op		object pointer
	pr		program-root
	sn		search-name (of program calling us)
	envv		calling environment
	pcp		PCSCONF pointer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<pcsconf.h>
#include	<upt.h>
#include	<userinfo.h>
#include	<logfile.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"pcspolls.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	LOGUSER		struct loguser_head
#define	LOGUSER_FL	struct loguser_flags
#define	LOGUSER_MAGIC	0x88773424
#define	LOGUSER_LCNAME	"log"
#define	LOGUSER_LBNAME	"pcspolls"


/* typedefs */

extern "C" {
    typedef int	(*thrsub_t)(void *) noex ;
}


/* external subroutines */


/* external variables */

extern cchar	**environ ;


/* local structures */

struct loguser_flags {
	uint		working:1 ;
} ;

struct loguser_head {
	uint		magic ;
	LOGUSER_FL	f ;
	pid_t		pid ;
	pthread_t	tid ;
	cchar	*a ;		/* memory allocation */
	cchar	*pr ;
	cchar	*sn ;
	cchar	**envv ;
	PCSCONF		*pcp ;
	volatile int	f_exiting ;
} ;


/* forward references */

local int loguser_argsbegin(LOGUSER *,cchar *,cchar *) noex ;
local int loguser_argsend(LOGUSER *) noex ;
local int loguser_worker(LOGUSER *) noex ;

local int mklogentry(cchar *,cchar *,mainv,PCSCONF *) noex ;


/* local variables */


/* exported variables */

PCSPOLLS_NAME	loguser = {
	"loguser",
	szof(LOGUSER),
	0
} ;


/* exported subroutines */

int loguser_start(LOGUSER *op,cc *pr,cc *sn,mainv envv,PCSCONF *pcp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("loguser_start: ent\n") ;
	debugprintf("loguser_start: pr=%s\n",pr) ;
	debugprintf("loguser_start: sn=%s\n",sn) ;
#endif

	if (envv == nullptr) envv = environ ;

	memclear(op) ;
	op->envv = envv ;
	op->pcp = pcp ;
	op->pid = getpid() ;

	if ((rs = loguser_argsbegin(op,pr,sn)) >= 0) {
	    if ((pr != nullptr) && (sn != nullptr)) {
	        pthread_t	tid ;
	        thrsub_t	thr = (thrsub_t) loguser_worker ;
	        if ((rs = uptcreate(&tid,nullptr,thr,op)) >= 0) {
	            op->fl.working = true ;
		    op->tid = tid ;
	        }
	    } /* end if (non-null) */
	    if (rs >= 0) {
	        op->magic = LOGUSER_MAGIC ;
	    }
	    if (rs < 0)
		loguser_argsend(op) ;
	} /* end if (loguser_argsbegin) */

#if	CF_DEBUGS
	debugprintf("loguser_start: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (loguser_start) */

int loguser_check(LOGUSER *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != LOGUSER_MAGIC) return SR_NOTOPEN ;

	if (op->fl.working) {
	    const pid_t	pid = getpid() ;
	    if (pid == op->pid) {
	        if (op->f_exiting) {
	            int		trs = 0 ;
	            op->fl.working = false ;
	            rs1 = uptjoin(op->tid,&trs) ;
	            if (rs >= 0) rs = rs1 ;
	            if (rs >= 0) rs = trs ;
	            f = true ;
		}
	    } else {
		op->fl.working = false ;
	    }
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (loguser_check) */

int loguser_finish(LOGUSER *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != LOGUSER_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("loguser_finish: f_working=%d\n",op->fl.working) ;
#endif

	if (op->fl.working) {
	    const pid_t	pid = getpid() ;
	    if (pid == op->pid) {
	        int	trs = 0 ;
	        op->fl.working = false ;
	        rs1 = uptjoin(op->tid,&trs) ;
	        if (rs >= 0) rs = rs1 ;
	        if (rs >= 0) rs = trs ;
	    } else {
		op->fl.working = false ;
		op->tid = 0 ;
	    }
	}
	{
	    rs1 = loguser_argsend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
#if	CF_DEBUGS
	debugprintf("loguser_finish: ret rs=%d\n",rs) ;
#endif

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (loguser_finish) */


/* provate subroutines */

local int loguser_argsbegin(LOGUSER *op,cchar *pr,cchar *sn) noex {
	int		rs ;
	int		sz = 0 ;
	char		*bp ;
	sz += (((pr !=nullptr)?strlen(pr):0)+1) ;
	sz += (((sn !=nullptr)?strlen(sn):0)+1) ;
	if ((rs = uc_malloc(sz,&bp)) >= 0) {
	    op->a = bp ;
	    if (pr != nullptr) {
	        op->pr = bp ;
	        bp = (strwcpy(bp,pr,-1)+1) ;
	    }
	    if (sn != nullptr) {
	        op->sn = bp ;
	        bp = (strwcpy(bp,sn,-1)+1) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (loguser_argsbegin) */

local int loguser_argsend(LOGUSER *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}
	return rs ;
}
/* end subroutine (loguser_argsend) */

local int loguser_worker(LOGUSER *op) noex {
	PCSCONF		*pcp = op->pcp ;
	int		rs ;
	cchar		*pr = op->pr ;
	cchar		*sn = op->sn ;
	cchar		**envv = op->envv ;

#if	CF_DEBUGS
	debugprintf("loguser_worker: ent\n") ;
#endif

	rs = mklogentry(pr,sn,envv,pcp) ;

#if	CF_DEBUGS
	debugprintf("loguser/work_start: ret rs=%d\n",rs) ;
#endif

	op->f_exiting = true ;
	return rs ;
}
/* end subroutine (loguser_worker) */

/* ARGSUSED */
local int mklogentry(cchar *pr,cchar *sn,mainv envv,PCSCONF *pcp) noex {
	int		rs ;
	int		rs1 ;
	cchar	*lcname = LOGUSER_LCNAME ;
	cchar	*lbname = LOGUSER_LBNAME ;
	char		lfname[MAXPATHLEN+1] ;

	if ((rs = mkpath3(lfname,pr,lcname,lbname)) >= 0) {
	    if (ustat sb ; (rs = u_stat(lfname,&sb)) >= 0) {
		userinfo	u ;
		if ((rs = userinfo_start(&u,nullptr)) >= 0) {
		    LOGFILE	lh, *lhp = &lh ;
		    cchar	*logid = u.logid ;
		    if ((rs1 = logfile_open(lhp,lfname,0,0666,logid)) >= 0) {
		        time_t	daytime = time(nullptr) ;
			cchar	*pv = "¥" ;
	                logfile_userinfo(lhp,&u,daytime,sn,pv) ;
		        logfile_close(lhp) ;
		    } else if (! isNotPresent(rs1)) {
		        rs = rs1 ;
		    }
		    rs1 = userinfo_finish(&u) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (userinfo) */
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    } /* end if (stat) */
	} /* end if (mkpath) */

	return rs ;
}
/* end subroutine (mklogentry) */


