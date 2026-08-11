/* daytime SUPPORT (DAYTIME) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* DAYTIME loadable service-module for MFSERVE */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-07, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
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

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<estrings.h>
#include	<modload.h>
#include	<nistinfo.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<localmisc.h>		/* |REALNAMELEN| */

#include	"mfserve.h"
#include	"daytime.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	DT		daytime

#ifndef	ORGCODELEN
#define	ORGCODELEN	REALNAMELEN
#endif

#define	DAYTIME_CSIZE	100 	/* default arg-chuck size */


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

template<typename ... Args>
local int daytime_ctor(lfm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (daytime_ctor) */

local int daytime_dtor(lfm *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (daytime_dtor) */

template<typename ... Args>
local inline int daytime_magic(lfm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == DAYTIME_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (daytime_magic) */

local int daytime_argsbegin(DT *,cchar **) noex ;
local int daytime_argsend(DT *) noex ;
local int daytime_worker(DT *) noex ;


/* local variables */


/* exported variables */

DAYTIME_OBJ	daytime_modinfo = {
	"daytime",
	szof(daytime),
	0
} ;


/* exported subroutines */

int daytime_start(DT *op,cc *pr,SREQ *jep,mv argv,mv envv) noex {
	int		rs = SR_OK ;
	if ((rs = daytime_ctor(op,pr,jep,argv,envv)) >= 0) ylikely {
	    op->pr = pr ;
	    op->jep = jep ;
	    op->envv = envv ;
	    if ((rs = sreq_getstdout(jep)) >= 0) ylikely {
	        op->ofd = rs ;
	        if ((rs = daytime_argsbegin(op,argv)) >= 0) ylikely {
	            pthread_t	tid ;
	            thrsub_f	thr = (thrsub_f) daytime_worker ;
	            if ((rs = uptcreate(&tid,nullptr,thr,op)) >= 0) ylikely {
	                op->fl.working = true ;
	                op->tid = tid ;
	                op->magic = DAYTIME_MAGIC ;
	            }
	            if (rs < 0) {
	                daytime_argsend(op) ;
		    }
	        } /* end if (daytime_argsbegin) */
	    } /* end if (sreq_getstdout) */
	    if (rs < 0) {
		daytime_dtor(op) ;
	    }
	} /* end if (daytime_ctor) */
	return rs ;
}
/* end subroutine (daytime_start) */

int daytime_finish(DT *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = daytime_magic(op)) >= 0) ylikely {
	    if (op->fl.working) {
		if ((rs = ucpid) >= 0) {
	            if (rs == op->pid) {
	                int	rst{} ;
	                op->fl.working = false ;
	                rs1 = uptjoin(op->tid,&rst) ;
	                if (rs >= 0) rs = rs1 ;
	                if (rs >= 0) rs = rst ;
	            } else {
	                op->fl.working = false ;
	                op->tid = 0 ;
	            }
		} /* end if (ucpid) */
	    } /* end if */
	    {
	        rs1 = daytime_argsend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (daytime_magic) */
	return rs ;
}
/* end subroutine (daytime_finish) */

int daytime_check(DT *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if ((rs = daytime_magic(op)) >= 0) ylikely {
	    if (op->fl.working) {
	        const pid_t		pid = getpid() ;
	        if (pid == op->pid) {
	            if (op->f_exiting) {
	                int		rst = 0 ;
	            	op->fl.working = false ;
	                rs1 = uptjoin(op->tid,&rst) ;
	                if (rs >= 0) rs = rs1 ;
	                if (rs >= 0) rs = rst ;
	                f = true ;
	            }
	        } else {
	            op->fl.working = false ;
	        }
	    } else {
	        f = true ;
	    } /* end if (working) */
	} /* end if (daytime_magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (daytime_check) */

int daytime_abort(DT *op) noex {
    	int		rs ;
	int		f = false ;
	if ((rs = daytime_magic(op)) >= 0) ylikely {
	    f = op->f_exiting ;
	    op->f_abort = true ;
	} /* end if (magic) */
	return f ;
}
/* end subroutine (daytime_abort) */


/* provate subroutines */

local int daytime_argsbegin(DT *op,cchar **argv) noex {
	vecpstr		*alp = &op->args ;
	cint		ss = DAYTIME_CSIZE ;
	int		rs ;
	if ((rs = vecpstr_start(alp,5,0,ss)) >= 0) ylikely {
	    op->fl.args = true ;
	    for (int i = 0 ; (rs >= 0) && (argv[i] != nullptr) ; i += 1) {
	        rs = vecpstr_add(alp,argv[i],-1) ;
	    }
	    if (rs < 0) {
	        op->fl.args = false ;
	        vecpstr_finish(alp) ;
	    }
	} /* end if (m-a) */
	return rs ;
} /* end subroutine (daytime_argsbegin) */

local int daytime_argsend(DT *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.args) ylikely {
	    vecpstr	*alp = &op->args ;
	    rs1 = vecpstr_finish(alp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (daytime_argsend) */

local int daytime_worker(DT *op) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (! op->f_abort) {
	    cchar	*pr = op->pr ;
	    if (ustat sb ; (rs = uc_stat(pr,&sb)) >= 0) {
		if (char *ybuf{} ; (rs = malloc_un(&ubuf)) >= 0) {
	        const uid_t	uid = sb.st_uid ;
	        if ((rs = getusername(ubuf,rs,uid)) >= 0) {
	            cint	olen = ORGCODELEN ;
	            char	obuf[ORGCODELEN+1] ;
	            if ((rs = localgetorgcode(pr,obuf,olen,ubuf)) >= 0) {
	                nistinfo	ni{} ;
	                custime		dt = getustime ;
	                char		ntbuf[NISTINFO_BUFLEN+1+1] ;
	                strdcpy1(ni.org,NISTINFO_ORGLEN,obuf) ;
	                timestr_nist(dt,ntbuf,&ni) ;
	                {
	                    int		tl = strlen(ntbuf) ;
			    ntbuf[tl++] = '\n' ;
			    ntbuf[tl] = '\0' ;
	                    if ((rs = uc_writen(op->ofd,ntbuf,tl)) >= 0) {
	                        wlen += rs ;
				rs = sreq_closefds(op->jep) ;
			    }
	                } /* end block */
	            } /* end if (localgetorg) */
	        } /* end if (getusername) */
	    } /* end if (uc_stat) */
	} /* end if (not aborting) */
	op->f_exiting = true ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (daytime_worker) */


