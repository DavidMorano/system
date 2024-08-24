/* proguserlist SUPPORT */
/* lang=C++20 */

/* user-list handling (in the background) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
        This subroutine was largely borrowed from someplace and
        pseudo-generalized here.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Here we do some user-list handling, in the background.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<upt.h>
#include	<useraccdb.h>
#include	<localmisc.h>		/* |REALNAMELEN| */

#include	"proguserlist.h"


/* local defines */

#ifndef	LOGCNAME
#define	LOGCNAME	"log"
#endif

#ifndef	USERFSUF
#define	USERFSUF	"users"
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	proglog_printf(proginfo *,cchar *,...) noex ;
}


/* external variables */


/* local structures */

extern "C" {
    typedef int (*workthr)(void *) noex ;
}

struct userlist {
	pthread_t	tid ;		/* thread ID */
} ;


/* forward references */

static int	proguserlist_worker(proginfo *) noex ;


/* local variables */


/* exported variables */


/* exported variables */


/* exported subroutines */

int proguserlist_begin(proginfo *pip) noex {
	userlist	*ulp ;
	cint		sz = sizeof(userlist) ;
	int		rs ;
	if ((rs = uc_malloc(sz,&ulp)) >= 0) {
	    workthr	w = (workthr) proguserlist_worker ;
	    pthread_t	tid ;
	    memset(ulp,0,sz) ;
	    pip->userlist = ulp ;
	    if ((rs = uptcreate(&tid,nullptr,w,pip)) >= 0) {
	        ulp->tid = tid ;
	    }
	    if (rs < 0) {
	        uc_free(ulp) ;
	        pip->userlist = nullptr ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (proguserlist_begin) */

int proguserlist_end(proginfo *pip) noex {
	userlist	*ulp = (userlist *) pip->userlist ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->userlist != nullptr) {
	    pthread_t	tid = ulp->tid ;
	    int		trs ;
	    if ((rs1 = uptjoin(tid,&trs)) >= 0) {
	        if (pip->open.logprog) {
	            cchar	*fmt ;
	            if (trs > 0) {
	                fmt = "user-list file created" ;
	                proglog_printf(pip,fmt) ;
	            } else if (trs < 0) {
	                fmt = "inaccessible user-list file (%d)" ;
	                proglog_printf(pip,fmt,trs) ;
	            }
	        } /* end if */
	    } /* end if (thread-join) */
	    if (rs >= 0) rs = rs1 ;
	    {
	        rs1 = uc_free(ulp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    pip->userlist = nullptr ;
	} /* end if (enabled) */
	return rs ;
}
/* end subroutine (proguserlist_end) */


/* local subroutines */

static int proguserlist_worker(proginfo *pip) noex {
	cint		blen = REALNAMELEN ;
	int		rs ;
	int		rs1 ;
	int		f_created = FALSE ;
	cchar		*pr = pip->pr ;
	cchar		*sn = pip->searchname ;
	cchar		*nn = pip->nodename ;
	cchar		*un = pip->username ;
	cchar		*name = pip->name ;
	char		bbuf[REALNAMELEN+1] ;
	if ((rs = sncpy3(bbuf,blen,nn,"!",un)) >= 0) {
	    USERACCDB	udb ;
	    if ((rs = useraccdb_open(&udb,pr,sn)) >= 0) {
	        f_created = (rs > 0) ;
	        {
	            rs = useraccdb_update(&udb,bbuf,name) ; /* all for this */
	        }
	        rs1 = useraccdb_close(&udb) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (useraccdb) */
	} /* end if (sncpy3) */
	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (proguserlist_worker) */


