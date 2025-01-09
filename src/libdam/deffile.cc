/* deffile SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<localmisc.h>

#include	"deffile.h"


/* local defines */

#define	DEFFILE_ICHECKTIME	2	/* file check interval (seconds) */
#define	DEFFILE_ICHANGETIME	2	/* wait change interval (seconds) */

#define	DF	deffile
#define	DF_CUR	deffile_cur


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int deffile_open(DF *op,cchar *fname) noex {
	int		rs = SR_OK ;
	int		opts ;
	cchar	*cp ;

	if (op == nullptr) return SR_FAULT ;
	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(DEFFILE)) ;

	if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	    USTAT		sb ;
	    op->fname = cp ;
	    if ((rs = uc_stat(fname,&sb)) >= 0) {
		const int	opts = VECSTR_OSTATIONARY ;
	    	op->ti_filemod = sb.st_mtime ;
	        if ((rs = vecstr_start(&op->vars,10,opts)) >= 0) {
	            if ((rs = vecstr_envfile(&op->vars,fname)) >= 0) {
	                op->magic = DEFFILE_MAGIC ;
	            }
	            if (rs < 0)
	                vecstr_finish(&op->vars) ;
	        } /* end if (vecstr_start) */
	    } /* end if (stat) */
	    if (rs < 0) {
	        uc_free(op->fname) ;
	        op->fname = nullptr ;
	    }
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (deffile_open) */

int deffile_close(DF *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DEFFILE_MAGIC) return SR_NOTOPEN ;
	{
	rs1 = vecstr_finish(&op->vars) ;
	if (rs >= 0) rs = rs1 ;
	}
	if (op->fname) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (deffile_close) */

int deffile_curbegin(DF *op,DF_CUR *cp) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DEFFILE_MAGIC) return SR_NOTOPEN ;

	cp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (deffile_curbegin) */

int deffile_curend(DF *op,DF_CUR *cp) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DEFFILE_MAGIC) return SR_NOTOPEN ;

	cp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (deffile_curend) */

int deffile_fetch(DF *op,cchar *key,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (key == nullptr) return SR_FAULT ;

	if (op->magic != DEFFILE_MAGIC) return SR_NOTOPEN ;

	if (key[0] == '\0') return SR_INVALID ;

	rs = vecstr_envget(&op->vars,key,rpp) ;
	vl = rs ;

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (deffile_fetch) */

int deffile_enum(DF *op,DF_CUR *curp,char *kbuf,int klen,cchar **rpp) noex {
	int		rs ;
	int		i ;
	int		kl ;
	int		vl = 0 ;
	cchar	*ep ;

	if (op == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != DEFFILE_MAGIC) return SR_NOTOPEN ;

	kbuf[0] = '\0' ;
	if (rpp != nullptr) *rpp = nullptr ;

	i = 0 ;
	if (curp != nullptr) {
	    i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	}

	for ( ; (rs = vecstr_get(&op->vars,i,&ep)) >= 0 ; i += 1) {
	    if (ep == nullptr) continue ;
	    break ;
	} /* end for */

	if (rs >= 0) {
	    cchar	*tp = strchr(ep,'=') ;
	    cchar	*vp ;
	    if (tp != nullptr) {
	        kl = (tp-ep) ;
	        vp = (tp+1) ;
	    } else {
	        kl = strlen(ep) ;
	        vp = (ep+kl) ;
	    }
	    vl = strlen(vp) ;
	    if ((rs = snwcpy(kbuf,klen,ep,kl)) >= 0) {
	        if (rpp != nullptr) *rpp = vp ;
	        if (curp != nullptr) curp->i = i ;
	    }
	}

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (deffile_enum) */

int deffile_checkint(DF *op,int intcheck) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DEFFILE_MAGIC) return SR_NOTOPEN ;

	if (intcheck < 1) intcheck = 1 ;
	op->intcheck = intcheck ;
	return SR_OK ;
}
/* end subroutine (deffile_checkint) */

int deffile_check(DF *op,time_t daytime) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_changed = false ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DEFFILE_MAGIC) return SR_NOTOPEN ;

	if (daytime == 0)
	    daytime = time(nullptr) ;

/* should we even check? */

	if ((daytime - op->ti_check) > op->intcheck) {
	    op->ti_check = daytime ;
	    if ((rs1 = uc_stat(op->fname,&sb)) >= 0) {
	        if (sb.st_mtime > op->ti_filemod) {
	            f_changed = true ;
	            op->ti_filemod = sb.st_mtime ;
	            vecstr_delall(&op->vars) ;
	            rs = vecstr_envfile(&op->vars,op->fname) ;
		}
	    }
	}

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (deffile_check) */


