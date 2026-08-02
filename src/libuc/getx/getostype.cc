/* getostype SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the type of operating system we are on */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getostype

	Description:
	This subroutine returns the type of operating system it is
	currently running on.

	Synopsis:
	int getostype() noex

	Arguments:
	-

	Returns:
	>=0		OS-type:
				ostype_bsd,
				ostype_sysv,
				ostype_linux,
				ostype_darwin,
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usysflag.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getsysname.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<strwcmp.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getostype.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct initer {
	initer() noex ;
    } ; /* end struct (initer) */
} /* end namespace */

namespace {
    struct typer ;
    typedef int (typer::*typer_m)() noex ;
    struct typer {
	bool		fgot = false ;
	operator int () noex ;
	int envostype() noex ;
	int envosname() noex ;
	int envsysname() noex ;
	int env(cchar *) noex ;
	int trysysname() noex ;
	int other() noex ;
	int matguess(cchar *,int) noex ;
    } ; /* end struct (typer) */
} /* end namespace */

struct osguess {
	cchar		*name ;
	ostypes		type ;
} ; /* end struct (osguess) */


/* forward references */

local int	mktype() noex ;
local int	findtype() noex ;


/* local variables */

constexpr osguess	guesses[] = {
	{ "darwin",	ostype_darwin },
	{ "linux",	ostype_linux },
	{ "sysv",	ostype_sysv },
	{ "sunos",	ostype_sysv },
	{ "unix",	ostype_sysv },	/* <- original AT&T "system" name */
	{ "bsd",	ostype_bsd }	/* some BSD systems */
} ; /* end array (guesses) */

constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_linux		= F_LINUX ;

constexpr typer_m	tries[] = {
	&typer::envostype,
	&typer::envosname,
	&typer::envsysname,
	&typer::trysysname,
	&typer::other
} ; /* end array (tries) */

static initer		init_data ;


/* exported variables */

libdam::ostyper		ostype ;

ccharp			ostypenames[ostype_overlast + 1] ;


/* exported subroutines */

int getostype() noex {
	static cint	rst = mktype() ;
	return rst ;
} /* end subroutine (getostype) */


/* local subroutines */

initer::initer() noex {
    	ccharp			*n = ostypenames ;
	n[ostype_bsd]		= "bsd" ;
	n[ostype_sysv]		= "sysv" ;
	n[ostype_linux]		= "linux" ;
	n[ostype_darwin]	= "darwin" ;
	n[ostype_overlast]	= nullptr ;
} /* end ctor (initer) */

local int mktype() noex {
	int		rs ;
	if_constexpr (f_sunos) {
	    rs = ostype_sysv ;
	} else if_constexpr (f_darwin) {
	    rs = ostype_darwin ;
	} else if_constexpr (f_linux) {
	    rs = ostype_linux ;
	} else {
	    rs = findtype() ;
	} /* end if_constexpr */
	return rs ;
} /* end subroutine (mktype) */

local int findtype() noex {
	typer to ;
	return to ;
} /* end subroutine (findtype) */

typer::operator int () noex {
	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if ((rs < 0) || fgot) break ;
	} /* end for */
	return rs ;
} /* end method (typer::operator) */

int typer::envostype() noex {
	cchar		*eot = getenver(varname.ostype) ;
	int		rs = SR_OK ;
	if (eot) {
	    cchar	*cp ;
	    if (int cl ; (cl = sfshrink(eot,-1,&cp)) > 0) {
		if (cint idx = matcasestr(ostypenames,cp,cl) ; idx >= 0) {
		    rs = idx ;
		    fgot = true ;
		}
	    }
	} /* end if */
	return rs ;
} /* end method (typer::envostype) */

int typer::envosname() noex {
	return env(varname.osname) ;
} /* end method (typer::envosname) */

int typer::envsysname() noex {
	return env(varname.sysname) ;
} /* end method (typer::envsysname) */

int typer::env(cchar *vn) noex {
	int		rs = SR_OK ;
	if (cchar *eot ; (eot = getenver(vn)) != nullptr) {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfshrink(eot,-1,&cp)) > 0) {
		rs = matguess(cp,cl) ;
	    }
	} /* end if */
	return rs ;
} /* end method (typer::env) */

int typer::trysysname() noex {
	int		rs ;
	int		rs1 ;
	int		rtype = 0 ;
	if (char *sbuf ; (rs = lm_mn(&sbuf)) >= 0) {
	    if ((rs = getsysname(sbuf,rs)) >= 0) {
		rs = matguess(sbuf,rs) ;
		rtype = rs ;
	    } /* end if (getsysname) */
	    rs1 = lm_free(sbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rtype : rs ;
} /* end method (typer::trysysname) */

int typer::matguess(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	for (cauto &g : guesses) {
	    if (strwcasecmp(g.name,sp,sl) == 0) {
		rs = g.type ;
		fgot = true ;
	    }
	    if (fgot) break ;
	} /* end for */
	return rs ;
} /* end method (typer::matguess) */

int typer::other() noex {
	cint		rs = ostype_bsd ;
	fgot = true ;
	return rs ;
} /* end method (typer::other) */

namespace libdam {
    ostyper::operator int () noex {
	return getostype() ;
    }
    int ostyper::operator () () noex {
	return getostype() ;
    }
} /* end namespace (libdam) */


