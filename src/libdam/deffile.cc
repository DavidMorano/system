/* deffile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read in a Solaris "default" file of variables */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-05, David A­D­ Morano
	Some code for this subroutine was taken from something that
	did something similar to what we are doing here. The rest
	was originally written for LevoSim.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	deffile

	Description:
	This object reads in the parameter file and makes the
	parameter pairs available thought a key search.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<snwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"deffile.h"


/* local defines */

#define	DEFFILE_ICHECKTIME	2	/* file check interval (seconds) */
#define	DEFFILE_ICHANGETIME	2	/* wait change interval (seconds) */

#define	DF	deffile
#define	DF_CUR	deffile_cur


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int deffile_ctor(deffile *op,Args ... args) noex {
    	DEFFILE		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->vlp = new(nothrow) vecstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (deffile_ctor) */

static int deffile_dtor(deffile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->vlp) {
		delete op->vlp ;
		op->vlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (deffile_dtor) */

template<typename ... Args>
static inline int deffile_magic(deffile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DEFFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (deffile_magic) */

/* local variables */


/* exported variables */


/* exported subroutines */

int deffile_open(DF *op,cchar *fname) noex {
	int		rs = SR_OK ;
	if ((rs = deffile_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (cchar *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	            op->fname = cp ;
	            if (USTAT sb ; (rs = uc_stat(fname,&sb)) >= 0) {
		        cint	vn = 10 ;
		        cint	vo = VECSTR_OSTATIONARY ;
	    	        op->ti_filemod = sb.st_mtime ;
	                if ((rs = vecstr_start(op->vlp,vn,vo)) >= 0) {
	                    if ((rs = vecstr_envfile(op->vlp,fname)) >= 0) {
	                        op->magic = DEFFILE_MAGIC ;
	                    }
	                    if (rs < 0) {
	                        vecstr_finish(op->vlp) ;
		            }
	                } /* end if (vecstr_start) */
	            } /* end if (stat) */
	            if (rs < 0) {
	                uc_free(op->fname) ;
	                op->fname = nullptr ;
	            }
	        } /* end if (memory-allocation) */
		if (rs < 0) {
		    deffile_dtor(op) ;
		}
	    } /* end if (valid) */
	} /* end if (deffile_ctor) */
	return rs ;
}
/* end subroutine (deffile_open) */

int deffile_close(DF *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = deffile_magic(op)) >= 0) {
	    {
	        rs1 = vecstr_finish(op->vlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    {
		rs1 = deffile_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (deffile_close) */

int deffile_curbegin(DF *op,DF_CUR *curp) noex {
    	int		rs ;
	if ((rs = deffile_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	}
	return rs ;
}
/* end subroutine (deffile_curbegin) */

int deffile_curend(DF *op,DF_CUR *curp) noex {
    	int		rs ;
	if ((rs = deffile_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (deffile_curend) */

int deffile_fetch(DF *op,cchar *key,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = deffile_magic(op,key,rpp)) >= 0) {
	    rs = SR_INVALID ;
	    if (key[0]) {
	        rs = vecstr_envget(op->vlp,key,rpp) ;
	        vl = rs ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (deffile_fetch) */

int deffile_curenum(DF *op,DF_CUR *curp,char *kbuf,int klen,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = deffile_magic(op,curp,kbuf)) >= 0) {
	    vecstr	*vlp = op->vlp ;
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    kbuf[0] = '\0' ;
	    if (rpp) *rpp = nullptr ;
	    cchar *ep = nullptr ;
	    for ( ; (rs = vlp->get(i,&ep)) >= 0 ; i += 1) {
	        if (ep == nullptr) continue ;
	        break ;
	    } /* end for */
	    if ((rs >= 0) && ep) {
		int	kl ; 
	        cchar	*vp ;
	        if (cchar *tp ; (tp = strchr(ep,'=')) != nullptr) {
	            kl = (tp-ep) ;
	            vp = (tp+1) ;
	        } else {
	            kl = strlen(ep) ;
	            vp = (ep+kl) ;
	        }
	        vl = strlen(vp) ;
	        if ((rs = snwcpy(kbuf,klen,ep,kl)) >= 0) {
	            if (rpp) *rpp = vp ;
	            curp->i = i ;
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (deffile_curenum) */

int deffile_checkint(DF *op,int intcheck) noex {
    	int		rs ;
	if ((rs = deffile_magic(op)) >= 0) {
	    if (intcheck < 1) intcheck = 1 ;
	    op->intcheck = intcheck ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (deffile_checkint) */

int deffile_check(DF *op,time_t dt) noex {
	int		rs ;
	int		f_changed = false ;
	if ((rs = deffile_magic(op)) >= 0) {
	    if (dt == 0) dt = getustime ;
	    /* should we even check? */
	    if ((dt - op->ti_check) > op->intcheck) {
		vecstr	*vlp = op->vlp ;
	        op->ti_check = dt ;
	        if (USTAT sb ; (rs = uc_stat(op->fname,&sb)) >= 0) {
	            if (sb.st_mtime > op->ti_filemod) {
	                f_changed = true ;
	                op->ti_filemod = sb.st_mtime ;
	                vecstr_delall(op->vlp) ;
	                rs = vlp->envfile(op->fname) ;
		    }
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	            f_changed = true ;
	            rs = vlp->envfile(op->fname) ;
		}
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (deffile_check) */


