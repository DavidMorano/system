/* strlistmk SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* STRLISTMK management */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module implements an interface (a trivial one) that
	provides access to the STRLISTMK object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<modload.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"strlistmk.h"
#include	"strlistmks.h"


/* local defines */

#define	SLM		strlistmk

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*open_f)(void *,cchar *,cchar *,int,mode_t,int) noex ;
    typedef int (*add_f)(void *,cchar *,int) noex ;
    typedef int (*abort_f)(void *) noex ;
    typedef int (*chgrp_f)(void *,gid_t) noex ;
    typedef int (*close_f)(void *) noex ;
}

typedef mode_t		om_t ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int strlistmk_ctor(strlistmk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->mlp = new(nothrow) modload) != np) {
		rs = SR_OK ;
	    } /* end if (new-modload) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlistmk_ctor) */

static int strlistmk_dtor(strlistmk *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mlp) {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlistmk_dtor) */

template<typename ... Args>
static inline int strlistmk_magic(strlistmk *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == STRLISTMK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (strlistmk_magic) */

static int	strlistmk_objloadbegin(SLM *,cchar *,cchar *) noex ;
static int	strlistmk_objloadend(SLM *) noex ;
static int	strlistmk_loadcalls(SLM *,cchar *) noex ;

static int	vecstr_loadsubs(vecstr *,cc *) noex ;

static bool	isrequired(int) noex ;


/* local variables */

enum subs {
	sub_open,
	sub_add,
	sub_abort,
	sub_chgrp,
	sub_close,
	sub_overlast
} ;

static constexpr cpcchar	subs[] = {
	"open",
	"add",
	"abort",
	"chgrp",
	"close",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int strlistmk_open(SLM *op,cc *pr,cc *dbn,cc *lfn,int of,om_t om,int n) noex {
	int		rs ;
	cchar		*objn = STRLISTMK_OBJNAME ;
	if ((rs = strlistmk_ctor(op,pr,dbn,lfn)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbn[0] && lfn[0]) {
	        if ((rs = strlistmk_objloadbegin(op,pr,objn)) >= 0) {
		    auto	ofun = op->call.open ;
	            if ((rs = ofun(op->obj,dbn,lfn,of,om,n)) >= 0) {
		        op->magic = STRLISTMK_MAGIC ;
	            }
	            if (rs < 0) {
			strlistmk_objloadend(op) ;
		    }
	        } /* end if (objloadbegin) */
	    } /* end if (valid) */
	    if (rs < 0) {
		strlistmk_dtor(op) ;
	    }
	} /* end if (strlistmk_ctor) */
	return rs ;
}
/* end subroutine (strlistmk_open) */

int strlistmk_close(SLM *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = strlistmk_magic(op)) >= 0) {
	    {
	        rs1 = (*op->call.close)(op->obj) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = strlistmk_objloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = strlistmk_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlistmk_close) */

int strlistmk_add(SLM *op,cc *sp,int sl) noex {
	int		rs ;
	if ((rs = strlistmk_magic(op,sp)) >= 0) {
	    rs = (*op->call.add)(op->obj,sp,sl) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlistmk_add) */

int strlistmk_abort(SLM *op) noex {
	int		rs ;
	if ((rs = strlistmk_magic(op)) >= 0) {
	    if (op->call.abort) {
	        rs = (*op->call.abort)(op->obj) ;
	    } else {
		rs = SR_NOSYS ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlistmk_abort) */

int strlistmk_chgrp(SLM *op,gid_t gid) noex {
	int		rs ;
	if ((rs = strlistmk_magic(op)) >= 0) {
	    if (op->call.chgrp) {
	        rs = (*op->call.chgrp)(op->obj,gid) ;
	    } else {
		rs = SR_NOSYS ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlistmk_chgrp) */


/* private subroutines */

static int strlistmk_objloadbegin(SLM *op,cc *pr,cc *objn) noex {
	vecstr		syms ;
	modload		*lp = op->mlp ;
	int		rs ;
	int		rs1 ;
	cint		ne = sub_overlast ;
	int		vo = VECSTR_OCOMPACT ;
	if ((rs = vecstr_start(&syms,ne,vo)) >= 0) {
	    if ((rs = vecstr_loadsubs(&syms,objn)) >= 0) {
                mainv       sv ;
                if ((rs = vecstr_getvec(&syms,&sv)) >= 0) {
                    cchar	*modbn = STRLISTMK_MODBNAME ;
                    int		mo = 0 ;
                    mo |= MODLOAD_OLIBVAR ;
                    mo |= MODLOAD_OPRS ;
                    mo |= MODLOAD_OSDIRS ;
                    if ((rs = modload_open(lp,pr,modbn,objn,mo,sv)) >= 0) {
                        int         mv[2] ;
                        if ((rs = modload_getmva(lp,mv,1)) >= 0) {
                            void    *p ;
                            op->objsize = mv[0] ;
                            if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
                                op->obj = p ;
                                rs = strlistmk_loadcalls(op,objn) ;
                                if (rs < 0) {
                                    uc_free(op->obj) ;
                                    op->obj = nullptr ;
                                }
                            } /* end if (memory-allocation) */
                        } /* end if (modload_getmva) */
                        if (rs < 0) {
                            modload_close(lp) ;
                        }
                    } /* end if (modload-open) */
		} /* end if (vecstr_getvec) */
            } /* end if (vecstr_loadsubs) */
	    rs1 = vecstr_finish(&syms) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	return rs ;
}
/* end subroutine (strlistmk_objloadbegin) */

static int strlistmk_objloadend(SLM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->obj) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}
	{
	    rs1 = modload_close(op->mlp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (strlistmk_objloadend) */

static int strlistmk_loadcalls(SLM *op,cc *soname) noex {
	modload		*lp = op->mlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		c = 0 ;
	for (int i = 0 ; (rs >= SR_OK) && subs[i] ; i += 1) {
	    cint	slen = SYMNAMELEN ;
	    char	sbuf[SYMNAMELEN + 1] ;
	    cchar	*sn = subs[i] ;
	    if ((rs = sncpy(sbuf,slen,soname,"_",sn)) >= 0) {
		cvoid	*snp ;
	        if ((rs = modload_getsym(lp,sbuf,&snp)) >= 0) {
	            c += 1 ;
		    switch (i) {
		    case sub_open:
		        op->call.open = open_f(snp) ;
		        break ;
		    case sub_add:
		        op->call.add = add_f(snp) ;
		        break ;
		    case sub_abort:
		        op->call.abort = abort_f(snp) ;
		        break ;
		    case sub_chgrp:
		        op->call.chgrp = chgrp_f(snp) ;
		        break ;
		    case sub_close:
		        op->call.close = close_f(snp) ;
		        break ;
		    } /* end switch */
	        } else if ((rs == rsn) && (! isrequired(i))) {
	            rs = SR_OK ;
	        }
	    } /* end if (sncpy) */
	} /* end for (subs) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (strlistmk_loadcalls) */

static int vecstr_loadsubs(vecstr *vlp,cc *objn) noex {
	cint		slen = SYMNAMELEN ;
	cint		ne = sub_overlast ;
	int		rs = SR_OK ;
	int		c = 0 ;
	char		sbuf[SYMNAMELEN + 1] ;
	for (int i = 0 ; (rs >= 0) && (i < ne) && subs[i] ; i += 1) {
	    cchar	*sn = subs[i] ;
            if ((rs = sncpy3(sbuf,slen,objn,"_",sn)) >= 0) {
		c += 1 ;
                rs = vecstr_add(vlp,sbuf,rs) ;
            }
            if (rs < 0) break ;
        } /* end for */
	return rs ;
}
/* end subroutine (vecstr_loadsubs) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_add:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


