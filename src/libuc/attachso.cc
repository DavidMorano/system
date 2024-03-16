/* attachso SUPPORT */
/* lang=C++20 */

/* attach a shared-object to the current process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	attachso

	Description:
	This subroutine implements an interface (a trivial one)
	that attempts to load a named module.

	Synopsis:
	int attachso(cc **dnames,cc *oname,cc **exts,cc **syms,
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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storebuf.h>
#include	<mkx.h>
#include	<ids.h>
#include	<xperm.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"attachso.h"


/* local defines */

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#define	DNAME_SELF	"*SELF*"

#define	SI		SUBINFO


/* imported namespaces */


/* local typedefs */

typedef mainv		mv ;


/* external subroutines */

extern "C" {
    extern int	isSpecialObject(void *) noex ;
}


/* local structures */

struct subinfo_flags {
	uint		id:1 ;
} ;

struct subinfo {
	void		**ropp ;
	mainv		dnames ;
	cchar		*oname ;
	mainv		exts ;
	mainv		syms ;
	void		*sop ;
	SUBINFO_FL	f ;
	ids		id ;
	int		dlmode ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,mv,cchar *,mv,mv,int,void **) noex ;
static int	subinfo_soload(SUBINFO *) noex ;
static int	subinfo_finish(SUBINFO *,int) noex ;

static int	subinfo_sofind(SUBINFO *) noex ;
static int	subinfo_socheck(SUBINFO *,ids *,cchar *) noex ;
static int	subinfo_checksyms(SUBINFO *) noex ;
static int	subinfo_modclose(SUBINFO *) noex ;


/* local variables */

static constexpr cpcchar	defexts[] = {
	"so",
	"o",
	"",
	nullptr
} ;

static constexpr cint	termrs[] = {
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
} ;


/* exported variables */


/* exported subroutines */

int attachso(cc **dnames,cc *oname,cc **exts,cc **syms,int m,void **ropp) noex {
	SUBINFO		si ;
	int		rs ;
	int		rs1 ;
	int		f_abort ;

	if (dnames == nullptr) return SR_FAULT ;
	if (oname == nullptr) return SR_FAULT ;

	if (oname[0] == '\0') return SR_INVALID ;

	if ((rs = subinfo_start(&si,dnames,oname,exts,syms,m,ropp)) >= 0) {

	    rs = subinfo_soload(&si) ;
	    f_abort = (rs < 0) ;

	    f_abort = (rs < 0) ;
	    rs1 = subinfo_finish(&si,f_abort) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */

	return rs ;
}
/* end subroutine (attachso) */


/* private subroutines */

static int subinfo_start(SI *sip,mv dnames,cc *oname,mv exts,mv syms,
			int m,void **ropp) noex {
	if (exts == nullptr) exts = defexts ;

	memclear(sip) ;
	sip->dnames = dnames ;
	sip->oname = oname ;
	sip->exts = exts ;
	sip->syms = syms ;
	sip->dlmode = m ;
	sip->ropp = ropp ;

	return SR_OK ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip,int f_abort) noex {
	if (f_abort && (sip->ropp != nullptr)) {
	    void	*sop = (void *) *(sip->ropp) ;
	    if ((sop != nullptr) && (! isSpecialObject(sop))) dlclose(sop) ;
	    *(sip->ropp) = nullptr ;
	}

	return SR_OK ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_soload(SI *sip) noex {
	int		rs ;

	if ((rs = subinfo_sofind(sip)) >= 0) {
	    if (sip->ropp != nullptr) {
		*(sip->ropp) = sip->sop ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (subinfo_soload) */

static int subinfo_sofind(SI *sip) noex {
	ids		id ;
	int		rs ;
	int		rs1 ;

	if ((rs = ids_load(&id)) >= 0) {
	    cint	soperm = (X_OK | R_OK) ;
	    int		f_open = false ;
	    mainv	dnames = sip->dnames ;
	    cchar	*dname ;
	    for (int i = 0 ; dnames[i] ; i += 1) {
	        dname = dnames[i] ;
	        if (dname[0] != '\0') {
		    if (strcmp(dname,DNAME_SELF) == 0) {
			sip->sop = RTLD_SELF ;
			rs = subinfo_checksyms(sip) ;
		    } else {
		        USTAT		sb ;
	                if ((rs = u_stat(dname,&sb)) >= 0) {
	                    if (! S_ISDIR(sb.st_mode)) {
			        rs = SR_NOTDIR ;
			    }
			}
	                if (rs >= 0) rs = sperm(&id,&sb,soperm) ;
	                if (rs >= 0) rs = subinfo_socheck(sip,&id,dname) ;
		    } /* end if */
		    if (rs >= 0) f_open = true ;
	            if ((rs >= 0) || (! isNotLib(rs))) break ;
	        } /* end if */
	    } /* end for */
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && f_open) {
		subinfo_modclose(sip) ;
	    }
	} /* end if (IDs) */

	return rs ;
}
/* end subroutine (subinfo_sofind) */


static int subinfo_socheck(SI *sip,ids *idp,cc *dname) noex {
	USTAT		sb ;
	cint		soperm = (R_OK | X_OK) ;
	int		rs = SR_OK ;
	int		rs1 = SR_NOTFOUND ;
	int		f = false ;
	mainv		exts = sip->exts ;
	char		sofname[MAXPATHLEN + 1] ;

	sip->sop = nullptr ;
	for (int j = 0 ; exts[j] ; j += 1) {
	    cchar	*ext = exts[j] ;

	    if ((rs1 = mksofname(sofname,dname,sip->oname,ext)) >= 0) {

	        rs1 = u_stat(sofname,&sb) ;
	        if ((rs1 >= 0) && (! S_ISREG(sb.st_mode))) {
	            rs1 = SR_ISDIR ;
		}

	        if (rs1 >= 0)
	            rs1 = sperm(idp,&sb,soperm) ;


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
	                    if (isOneOf(termrs,rs1)) rs = rs1 ;
			}

	            } /* end if (ok) */
	        } /* end if (file and perms) */

	    } /* end if (filename formed) */

	    if (sip->sop != nullptr) break ;
	    if (rs < 0) break ;
	} /* end for (exts) */

	if (rs >= 0) {
	    if (sip->sop == nullptr) rs = rs1 ;
	} else {
	    sip->sop = nullptr ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_socheck) */

static int subinfo_checksyms(SI *sip) noex {
	int		rs = SR_OK ;
	mainv		syms = sip->syms ;

	if (sip->sop == nullptr) return SR_FAULT ;

	if (syms != nullptr) {
	    const void	*symp ;
	    int		i ;
	    for (i = 0 ; (rs >= 0) && (syms[i] != nullptr) ; i += 1) {
	        symp = dlsym(sip->sop,syms[i]) ;
	        if (symp == nullptr) rs = SR_NOTFOUND ;
	    } /* end for */
	} /* end if (syms) */

	return rs ;
}
/* end subroutine (subinfo_checksyms) */

static int subinfo_modclose(SI *sip) noex {
	if (sip->sop != nullptr) {
	    if (! isSpecialObject(sip->sop)) {
		dlclose(sip->sop) ;
	    }
	    sip->sop = nullptr ;
	}
	return SR_OK ;
}
/* end subroutine (subinfo_modclose) */


