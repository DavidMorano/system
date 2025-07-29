/* attachso SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* attach a shared-object to the current process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This code was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	attachso

	Description:
	This subroutine implements an interface (a trivial one)
	that attempts to load a named module.

	Synopsis:
	int attachso(mainv dnames,cc *oname,cc **exts,mainv syms,
				int dlmode,void **ropp) noex

	Arguments:
	dnames		string array of directories to search
	oname		root-name of SO object file
	exts		array of externsions to consider
	syms		array of symbols
	dlmode		DLOPEN mode
	ropp		pointer to result object pointer

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<storebuf.h>
#include	<mkx.h>
#include	<ids.h>
#include	<permx.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"attachso.h"

import libutil ;

/* local defines */

#define	DNAME_SELF	"*SELF*"

#define	SI		subinfo


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */

extern "C" {
    extern int	isSpecialObject(void *) noex ;
}


/* external variables */


/* local structures */

struct subinfo_fl {
	uint		id:1 ;
} ;

struct subinfo {
	void		**ropp ;
	void		*sop ;
	cchar		*oname ;
	mainv		dnames ;
	mainv		exts ;
	mainv		syms ;
	ids		id ;
	subinfo_fl	f ;
	int		dlmode ;
} ;


/* forward references */

static int	subinfo_start(SI *,mv,cchar *,mv,mv,int,void **) noex ;
static int	subinfo_soload(SI *) noex ;
static int	subinfo_finish(SI *,int) noex ;

static int	subinfo_sofind(SI *) noex ;
static int	subinfo_socheck(SI *,ids *,cchar *) noex ;
static int	subinfo_checksyms(SI *) noex ;
static int	subinfo_modclose(SI *) noex ;


/* local variables */

constexpr cpcchar	defexts[] = {
	"so",
	"o",
	"",
	nullptr
} ;

constexpr int		rsterm[] = {
	SR_FAULT,
	SR_INVALID,
	SR_NOMEM,
	SR_NOANODE,
	SR_BADFMT,
	SR_NOSPC,
	SR_NOSR,
	SR_NOBUFS,
	SR_BADF,
	SR_OVERFLOW,
	SR_RANGE,
	0
} ; /* end array (rsterm) */


/* exported variables */


/* exported subroutines */

int attachso(mv dnames,cc *oname,mv exts,mv syms,int m,void **ropp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dnames && oname) ylikely {
	    rs = SR_INVALID ;
	    if (oname[0]) {
	        subinfo		si ;
		auto		ss = subinfo_start ;
	        if ((rs = ss(&si,dnames,oname,exts,syms,m,ropp)) >= 0) ylikely {
	            int		f_abort ;
	            {
	                rs = subinfo_soload(&si) ;
	                f_abort = (rs < 0) ;
	            }
	            rs1 = subinfo_finish(&si,f_abort) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (attachso) */


/* private subroutines */

static int subinfo_start(SI *sip,mv dnames,cc *oname,mv exts,mv syms,
			int m,void **ropp) noex {
    	int		rs = SR_FAULT ;
	if (exts == nullptr) exts = defexts ;
	if (sip) ylikely {
	    rs = memclear(sip) ;
	    sip->dnames = dnames ;
	    sip->oname = oname ;
	    sip->exts = exts ;
	    sip->syms = syms ;
	    sip->dlmode = m ;
	    sip->ropp = ropp ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip,int f_abort) noex {
    	int		rs = SR_FAULT ;
	if (sip) ylikely {
    	    rs = SR_OK ;
	    if (f_abort && sip->ropp) {
	        void	*sop = voidp(*(sip->ropp)) ;
	        if (sop && (! isSpecialObject(sop))) {
		    dlclose(sop) ;
	        }
	        *(sip->ropp) = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_soload(SI *sip) noex {
	int		rs ;
	if ((rs = subinfo_sofind(sip)) >= 0) ylikely {
	    if (sip->ropp) {
		*(sip->ropp) = sip->sop ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (subinfo_soload) */

static int subinfo_sofind(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	if (ids id ; (rs = id.load) >= 0) ylikely {
	    cint	soperm = (X_OK | R_OK) ;
	    mainv	dnames = sip->dnames ;
	    bool	f_open = false ;
	    for (int i = 0 ; dnames[i] ; i += 1) {
	        cchar	*dname = dnames[i] ;
	        if (dname[0] != '\0') {
		    if (strcmp(dname,DNAME_SELF) == 0) {
			sip->sop = RTLD_SELF ;
			rs = subinfo_checksyms(sip) ;
		    } else {
		        if (USTAT sb ; (rs = u_stat(dname,&sb)) >= 0) {
	                    if (S_ISDIR(sb.st_mode)) {
	                	rs = permid(&id,&sb,soperm) ;
			    } else {
			        rs = SR_NOTDIR ;
			    }
			}
		    } /* end if */
		    if (rs >= 0) rs = subinfo_socheck(sip,&id,dname) ;
		    if (rs >= 0) f_open = true ;
	            if ((rs >= 0) || (! isNotLib(rs))) break ;
	        } /* end if */
	    } /* end for */
	    rs1 = id.release ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_open) {
		subinfo_modclose(sip) ;
	    }
	} /* end if (IDs) */
	return rs ;
}
/* end subroutine (subinfo_sofind) */

static int subinfo_socheck(SI *sip,ids *idp,cc *dname) noex {
	int		rs ;
	int		rs1 = SR_NOTFOUND ;
	int		f = false ;
	if (char *sofname ; (rs = malloc_mp(&sofname)) >= 0) ylikely {
	    ustat	sb ;
	    cint	soperm = (R_OK | X_OK) ;
	    mainv	exts = sip->exts ;
	    sip->sop = nullptr ;
	    for (int j = 0 ; exts[j] ; j += 1) {
	        cchar	*ext = exts[j] ;
	        if ((rs1 = mksofname(sofname,dname,sip->oname,ext)) >= 0) {
	            rs1 = u_stat(sofname,&sb) ;
	            if ((rs1 >= 0) && (! S_ISREG(sb.st_mode))) {
	                rs1 = SR_ISDIR ;
		    }
	            if (rs1 >= 0) {
	                rs1 = permid(idp,&sb,soperm) ;
		    }
	            if (rs1 >= 0) {
	                sip->sop = dlopen(sofname,sip->dlmode) ;
	                if (sip->sop == nullptr) rs1 = SR_NOENT ;
	                if (rs1 >= 0) {
	                    if ((rs1 = subinfo_checksyms(sip)) >= 0) {
			        f = true ;
	                    } else {
	                        if (sip->sop != nullptr) {
	    		            if (! isSpecialObject(sip->sop)) {
	                                dlclose(sip->sop) ;
				    }
	                            sip->sop = nullptr ;
	                        }
	                        if (isOneOf(rsterm,rs1)) rs = rs1 ;
			    }
	                } /* end if (ok) */
	            } /* end if (file and perms) */
	        } /* end if (filename formed) */
	        if (sip->sop != nullptr) break ;
	        if (rs < 0) break ;
	    } /* end for (exts) */
	    if (rs >= 0) {
	        if (sip->sop == nullptr) {
		    rs = rs1 ;
		}
	    } else {
	        sip->sop = nullptr ;
	    }
	    rs1 = uc_free(sofname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_socheck) */

static int subinfo_checksyms(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip->sop) ylikely {
	    mainv	syms = sip->syms ;
	    rs = SR_OK ;
	    if (syms) ylikely {
	        cvoid	*symp ;
	        for (int i = 0 ; (rs >= 0) && syms[i] ; i += 1) {
	            symp = dlsym(sip->sop,syms[i]) ;
	            if (symp == nullptr) rs = SR_NOTFOUND ;
	        } /* end for */
	    } /* end if (syms) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_checksyms) */

static int subinfo_modclose(SI *sip) noex {
    	int		rs = SR_FAULT ;
	if (sip) ylikely {
	    rs = SR_OK ;
	    if (sip->sop) {
	        if (! isSpecialObject(sip->sop)) {
		    dlclose(sip->sop) ;
	        }
	        sip->sop = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_modclose) */


