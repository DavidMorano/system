/* configvars_parse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Configuration-Variables - Parse */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	configvars_parse

	Description:
	This is an object that reads configuration files and organizes
	the content into the object for structured access.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<fieldterminit.hh>	/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<matostr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */
#include	<bfliner.h>		/* LIBB */

#include	"configvarsobj.hh"


/* local defines */

#define	KEYBUFLEN	20


/* local namespaces */

using std::min ;			/* subroutine (template) */
using std::max ;			/* subroutine (template) */

using namespace		configvars_obj ;


/* local typedefs */

typedef	CV_FILE *	filep ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct keytyper {
	cchar	*type[keytype_overlast + 1] ;
	constexpr keytyper() noex {
	    type[keytype_define]	= "define" ;
	    type[keytype_export]	= "export" ;
	    type[keytype_set]		= "set" ;
	    type[keytype_unset]		= "unset" ;
	    type[keytype_overlast]	= nullptr ;
	} ; /* end vtor */
    } ; /* end struct (keytyper) */
} /* end namespce */

namespace {
    struct confparser {
        CV		*cvp ;
	vecobj		*eep ;
	int		fi ;
        confparser(CV *cp,int idx,vecobj *ep) noex : cvp(cp), eep(ep) {
	    fi = idx ;
	} ;
	operator int () noex ;
	int parsing		(filep,bfile *) noex ;
	int parseln		(filep,cchar *,int) noex ;
	int parse_define	(filep,field *,cc *,int) noex ;
	int parse_export	(filep,field *,cc *,int) noex ;
	int parse_set		(filep,field *,cc *,int) noex ;
	int parse_unset		(filep,field *,cc *,int) noex ;
	int parse_addvar	(filep,field *,cc *,int,int) noex ;
    } ; /* end struct (confparser) */
} /* end namespce */


/* forward references */


/* local variables */

constexpr fieldterminit	pt("\b\t\v\f ") ;	/* parsing terminators */
static const keytyper	key ;
constexpr int		keylen = KEYBUFLEN ;


/* exported variables */


/* exported subroutines */

namespace configvars_obj {
    int configvars_parse(CV *cvp,int fi,vecobj *eep) noex {
	confparser	po(cvp,fi,eep) ;
	return po ;
    } /* end subroutine */
} /* end namespace */

confparser::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	void		*vp{} ;
	if ((rs = vecobj_get(cvp->fesp,fi,&vp)) >= 0) ylikely {
	    CV_FILE	*fep = filep(vp) ;
	    if (vp) ylikely {
	        bfile	cfile, *fp = &cfile ;
	        cchar	*fn = fep->filename ;
	        if ((rs = bopen(fp,fn,"r",0664)) >= 0) ylikely {
		    ustat sb ;
		    if ((rs = bcontrol(fp,BC_STAT,&sb)) >= 0) {
			if (sb.st_mtime >= fep->mtime) {
			    fep->mtime = sb.st_mtime ;
			    rs = parsing(fep,fp) ;
			    rv = rs ;
			}
		    } /* end if (bcontrol) */
		    rs1 = bclose(fp) ;
		    if (rs >= 0) rs = rs1 ;
	 	} /* end if (bfile) */
	    }
	} /* end if (vecobj_get) */
	return (rs >= 0) ? rv : rs ;
} /* end method (confparser::operator) */

int confparser::parsing(filep fep,bfile *fp) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (bfliner bl ; (rs = bl.start(fp)) >= 0) ylikely {
	    cchar	*lp ;
	    while ((rs = bl.getlns(&lp)) > 0) {
	        cchar	*cp ;
	        if (int cl ; (cl = sfcontent(lp,rs,&cp)) > 0) {
	            rs = parseln(fep,cp,cl) ;
	            rv += rs ;
	        }
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = bl.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfliner) */
	return (rs >= 0) ? rv : rs ;
} /* end mthod (confparser::parsing) */

int confparser::parseln(filep fep,cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) ylikely {
	    if (cchar *fp ; (rs = fsb.get(pt.terms,&fp)) > 0) ylikely {
		char	keybuf[keylen + 1] ;
	        cint	kl = min(rs,keylen) ;
	        strwcpylc(keybuf,fp,kl) ;
	        if (int idx ; (idx = matostr(key.type,2,keybuf,kl)) > 0) {
		    if (fsb.term != '#') {
		        cchar	*kp = keybuf ;
		        rv = 1 ;
		        switch (idx) {
	                case keytype_define:
			    rs = parse_define(fep,&fsb,kp,kl) ;
			    break ;
	                case keytype_export:
			    rs = parse_export(fep,&fsb,kp,kl) ;
			    break ;
	                case keytype_set:
			    rs = parse_set(fep,&fsb,kp,kl) ;
			    break ;
	                case keytype_unset:
			    rs = parse_unset(fep,&fsb,kp,kl) ;
			    break ;
		        } /* end switch */
		    } /* end if (not truncated) */
		} else {
		    cauto	av = configvars_addvar ;
		    cint	w = configvarsw_vars ;
	            rs = av(cvp,fi,w,keybuf,kl,fsb.lp,fsb.ll) ;
		    rv = 1 ;
		} /* end if (matocasestr) */
	    } /* end if (field_get) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? rv : rs ;
} /* end mthod (confparser::parseln) */

int confparser::parse_define(filep fep,field *fsbp,cc *,int) noex {
	cnullptr	np{} ;
	cint		kt = keytype_define ;
	return parse_addvar(fep,fsbp,np,0,kt) ;
} /* end method */

int confparser::parse_export(filep fep,field *fsbp,cc *,int) noex {
	cnullptr	np{} ;
	cint		kt = keytype_export ;
	return parse_addvar(fep,fsbp,np,0,kt) ;
} /* end method */

int confparser::parse_set(filep fep,field *fsbp,cc *,int) noex {
	cnullptr	np{} ;
	cint		kt = keytype_set ;
	return parse_addvar(fep,fsbp,np,0,kt) ;
} /* end method */

int confparser::parse_unset(filep fep,field *fsbp,cc *,int) noex {
	int		rs ;
	cchar		*cp{} ;
	if ((rs = fsbp->get(pt.terms,&cp)) > 0) {
	    cint	type = keytype_unset ;
	    rs = file_addvar(fep,type,fi,cp,rs,nullptr,0) ;
	}
	return rs ;
} /* end method (confparser::parse_unset) */

int confparser::parse_addvar(filep fep,field *fsbp,cc *,int,int kt) noex {
	int		rs ;
	cchar		*kp{} ;
	if ((rs = fsbp->get(pt.terms,&kp)) > 0) ylikely {
	    cint	kl = rs ;
	    int		cl = 0 ;
	    cchar	*cp = nullptr ;
	    if (fsbp->term != '#') {
		if ((rs = fsbp->get(pt.terms,&cp)) > 0) {
		    cl = rs ;
		}
	    } /* end if */
	    if (rs >= 0) ylikely {
	        if (kt == keytype_set) {
		    cint	w = CONFIGVARS_WSETS ;
	            rs = configvars_addvar(cvp,fi,w,kp,kl,cp,cl) ;
	        } else {
		    rs = file_addvar(fep,kt,fi,kp,kl,cp,cl) ;
	        }
	    } /* end if (ok) */
	} /* end if (field_get) */
	return rs ;
} /* end method (confparser::parse_addvar) */


