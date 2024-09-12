/* getostype SUPPORT */
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
	>=0		0=BSD, 1=SYSV
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<usysflag.h>
#include	<uvariables.hh>
#include	<mallocxx.h>
#include	<getsysname.h>
#include	<sfx.h>
#include	<matxstr.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"getostype.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

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
}

struct osguess {
	cchar		*name ;
	ostypes		type ;
} ;


/* forward references */

static int	mktype() noex ;
static int	findtype() noex ;


/* local variables */

constexpr osguess	guesses[] = {
	{ "sunos", ostype_sysv },
	{ "darwin", ostype_darwin },
	{ "linux", ostype_linux },
	{ "sysv", ostype_sysv }
} ;

constexpr bool		f_darwin	= F_DARWIN ;
constexpr bool		f_sunos		= F_SUNOS ;
constexpr bool		f_linux		= F_LINUX ;

constexpr typer_m	mems[] = {
	&typer::envostype,
	&typer::envosname,
	&typer::envsysname,
	&typer::trysysname,
	&typer::other
} ;


/* exported variables */

ostyper		ostype ;

cpcchar		ostypenames[] = {
	"bsd",
	"sysv",
	"linux",
	"darwin",
	nullptr
} ;


/* exported subroutines */

int getostype() noex {
	static cint	rst = mktype() ;
	return rst ;
}
/* end subroutine (getostype) */


/* local subroutines */

static int mktype() noex {
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
}
/* end subroutine (mkostype) */

static int findtype() noex {
	typer		to ;
	return to ;
}
/* end subroutine (findtype) */

typer::operator int () noex {
	int		rs = SR_OK ;
	for (auto &m : mems) {
	    rs = (this->*m)() ;
	    if ((rs < 0) || fgot) break ;
	} /* end for */
	return rs ;
}
/* end method (typer::operator) */

int typer::envostype() noex {
	cchar		*eot = getenv(varname.ostype) ;
	int		rs = SR_OK ;
	if (eot) {
	    cchar	*cp ;
	    if (int cl ; (cl = sfshrink(eot,-1,&cp)) > 0) {
		if (int idx ; (idx = matcasestr(ostypenames,cp,cl)) >= 0) {
		    rs = idx ;
		    fgot = true ;
		}
	    }
	} /* end if */
	return rs ;
}
/* end method (typer::envostype) */

int typer::envosname() noex {
	return env(varname.osname) ;
}
/* end method (typer::envosname) */

int typer::envsysname() noex {
	return env(varname.sysname) ;
}
/* end method (typer::envsysname) */

int typer::env(cchar *vn) noex {
	int		rs = SR_OK ;
	if (cchar *eot ; (eot = getenv(vn)) != nullptr) {
	    cchar	*cp ;
	    if (int cl ; (cl = sfshrink(eot,-1,&cp)) > 0) {
		rs = matguess(cp,cl) ;
	    }
	} /* end if */
	return rs ;
}
/* end method (typer::env) */

int typer::trysysname() noex {
	int		rs ;
	int		rs1 ;
	int		rtype = 0 ;
	char		*sbuf{} ;
	if ((rs = malloc_mn(&sbuf)) >= 0) {
	    cint	slen = rs ;
	    if ((rs = getsysname(sbuf,slen)) >= 0) {
		rs = matguess(sbuf,rs) ;
		rtype = rs ;
	    } /* end if (getsysname) */
	    rs1 = uc_free(sbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rtype : rs ;
}
/* end method (typer::trysysname) */

int typer::matguess(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	for (const osguess &g : guesses) {
	    if (strwcasecmp(g.name,sp,sl) == 0) {
		rs = g.type ;
		fgot = true ;
	    }
	    if (fgot) break ;
	} /* end for */
	return rs ;
}
/* end method (typer::matguess) */

int typer::other() noex {
	int	rs = ostype_bsd ;
	fgot = true ;
	return rs ;
}
/* end method (typer::other) */

ostyper::operator int () noex {
	return getostype() ;
}

int ostyper::operator () () noex {
	return getostype() ;
}


