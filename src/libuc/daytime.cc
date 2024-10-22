/* daytime SUPPORT (DAYTIME) */
/* lang=C++20 */

/* this is a MFSERVE loadable service-module */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-07, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	daytime

	Description:
	This object is a MFSERVE loadable service-module.

	Synopsis:
	int daytime_start(DAYTIME *op,cc *pr,SREQ *jep,mv argv,mv envv) noex

	Arguments:
	op		object pointer
	pr		program-root
	sn		search-name (of program calling us)
	argv		array-arguments
	envv		array-environment array

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<modload.h>
#include	<upt.h>
#include	<nistinfo.h>
#include	<localmisc.h>		/* |MAXNAMELEN| + |USERNAMELEN| */

#include	"mfserve.h"
#include	"daytime.h"


/* local defines */

#ifndef	ORGCODELEN
#define	ORGCODELEN	MAXNAMELEN
#endif

#ifndef	ORGCODELEN
#define	ORGCODELEN	USERNAMELEN
#endif

#define	DAYTIME_CSIZE	100 	/* default arg-chuck size */

#define	NDF		"daytime.nd"


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int	(*thrsub_f)(void *) noex ;
}

typedef mainv	mv ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int daytime_argsbegin(DAYTIME *,cchar **) noex ;
static int daytime_argsend(DAYTIME *) noex ;
static int daytime_worker(DAYTIME *) noex ;


/* local variables */


/* exported variables */

MFSERVE_OBJ	daytime_mod = {
	"daytime",
	sizeof(DAYTIME),
	0
} ;


/* exported variables */


/* exported subroutines */

int daytime_start(DAYTIME *op,cc *pr,SREQ *jep,mv argv,mv envv) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (pr == nullptr) return SR_FAULT ;
	if (argv == nullptr) return SR_FAULT ;
	if (envv == nullptr) return SR_FAULT ;

	memclear(op) ;
	op->pr = pr ;
	op->jep = jep ;
	op->envv = envv ;

	if ((rs = sreq_getstdout(jep)) >= 0) {
	    op->ofd = rs ;
	    if ((rs = daytime_argsbegin(op,argv)) >= 0) {
	        pthread_t	tid ;
	        thrsub_f	thr = (thrsub_f) daytime_worker ;
	        if ((rs = uptcreate(&tid,nullptr,thr,op)) >= 0) {
	            op->f.working = true ;
	            op->tid = tid ;
	            op->magic = DAYTIME_MAGIC ;
	        }
	        if (rs < 0)
	            daytime_argsend(op) ;
	    } /* end if (daytime_argsbegin) */
	} /* end if (sreq_getstdout) */

	return rs ;
}
/* end subroutine (daytime_start) */

int daytime_finish(DAYTIME *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DAYTIME_MAGIC) return SR_NOTOPEN ;

	if (op->f.working) {
	    const pid_t	pid = getpid() ;
	    if (pid == op->pid) {
	        int	trs = 0 ;
	        op->f.working = false ;
	        rs1 = uptjoin(op->tid,&trs) ;
	        if (rs >= 0) rs = rs1 ;
	        if (rs >= 0) rs = trs ;
	    } else {
	        op->f.working = false ;
	        op->tid = 0 ;
	    }
	}

	rs1 = daytime_argsend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (daytime_finish) */

int daytime_check(DAYTIME *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DAYTIME_MAGIC) return SR_NOTOPEN ;

	if (op->f.working) {
	    const pid_t		pid = getpid() ;
	    if (pid == op->pid) {
	        if (op->f_exiting) {
	            int		trs = 0 ;
	            op->f.working = false ;
	            rs1 = uptjoin(op->tid,&trs) ;
	            if (rs >= 0) rs = rs1 ;
	            if (rs >= 0) rs = trs ;
	            f = true ;
	        }
	    } else {
	        op->f.working = false ;
	    }
	} else {
	    f = true ;
	} /* end if (working) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (daytime_check) */

int daytime_abort(DAYTIME *op) noex {
	cint	f = op->f_exiting ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magic != DAYTIME_MAGIC) return SR_NOTOPEN ;
	op->f_abort = true ;
	return f ;
}
/* end subroutine (daytime_abort) */


/* provate subroutines */

static int daytime_argsbegin(DAYTIME *op,cchar **argv) noex {
	vecpstr		*alp = &op->args ;
	cint		ss = DAYTIME_CSIZE ;
	int		rs ;
	if ((rs = vecpstr_start(alp,5,0,ss)) >= 0) {
	    int		i ;
	    op->f.args = true ;
	    for (i = 0 ; (rs >= 0) && (argv[i] != nullptr) ; i += 1) {
	        rs = vecpstr_add(alp,argv[i],-1) ;
	    }
	    if (rs < 0) {
	        op->f.args = false ;
	        vecpstr_finish(alp) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (daytime_argsbegin) */

static int daytime_argsend(DAYTIME *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->f.args) {
	    vecpstr	*alp = &op->args ;
	    rs1 = vecpstr_finish(alp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (daytime_argsend) */

static int daytime_worker(DAYTIME *op) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (! op->f_abort) {
	    USTAT	sb ;
	    cchar	*pr = op->pr ;
	    if ((rs = uc_stat(pr,&sb)) >= 0) {
	        const uid_t	uid = sb.st_uid ;
	        cint	ulen = USERNAMELEN ;
	        char		ubuf[USERNAMELEN+1] ;
	        if ((rs = getusername(ubuf,ulen,uid)) >= 0) {
	            cint	olen = ORGCODELEN ;
	            char	obuf[ORGCODELEN+1] ;
	            if ((rs = localgetorgcode(pr,obuf,olen,ubuf)) >= 0) {
	                NISTINFO	ni{} ;
	                const time_t	dt = time(nullptr) ;
	                char		ntbuf[NISTINFO_BUFLEN+1+1] ;
	                strdcpy1(ni.org,NISTINFO_ORGLEN,obuf) ;
	                timestr_nist(dt,&ni,ntbuf) ;
	                {
	                    int	tl = strlen(ntbuf) ;
			    ntbuf[tl++] = '\n' ;
			    ntbuf[tl] = '\0' ;
	                    if ((rs = uc_writen(op->ofd,ntbuf,tl)) >= 0) {
	                        wlen += rs ;
				rs = sreq_closefds(op->jep) ;
			    }
	                }
	            } /* end if (localgetorg) */
	        } /* end if (getusername) */
	    } /* end if (uc_stat) */
	} /* end if (not aborting) */

	op->f_exiting = true ;
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (daytime_worker) */


