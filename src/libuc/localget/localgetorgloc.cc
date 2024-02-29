/* localgetorglocer SUPPORT */
/* lang=C++20 */

/* get the LOCAL organization location (ORGLOC) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written. This is a minimal
	implementation, but at least there is something here. ORGLOCs
	are not currently a part of the system-user-database system
	('passwd', 'user_attr', et cetera) so currently this
	subroutine does not look anywhere in those places for an
	ORGLOC. I guess that an ORGLOC should be added to the
	'user_attr' database but this practice has not yet begun,
	if it ever will be.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localgetorglocer

	Descrption:
	This subroutine is used to (try to) get the LOCAL software
	distribution organization location (ORGLOC). This particular
	value is not often set in the LOCAL software distribution
	and also this subroutine does not look hard in other places
	to find a possible location. So an ORGLOC is not often
	returned. But nonetheless there are places (pieces of code,
	middleware et cetera) that still call this subroutine rather
	regularly.

	Synopsis:
	int localgetorglocer(cchar *pr,char *rbuf,int rlen,cchar *un) noex

	Arguments:
	pr		program root
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer
	un		username

	Returns:
	>=0		length of returned ID
	<0		error in process of creating ID (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<syswords.hh>
#include	<mallocxx.h>
#include	<getuserhome.h>
#include	<filereadln.h>
#include	<strlibval.hh>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"localget.h"


/* local defines */

#define	ETCCNAME	"etc"
#define	ORGLOCFNAME	"orgloc"


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum orgloccos {
	orglocerco_finish,
	orglocerco_overlast
} ;

namespace {
    struct orglocer ;
    struct orglocer_co {
	orglocer	*op ;
	int		w ;
	void operator () (orglocer *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
        operator int () noex ;
    } ; /* end struct (orglocer_co) */
    struct orglocer {
	cchar		*pr ;
	cchar		*un ;
	char		*rbuf ;
	char		*tfname = nullptr ;
	int		rlen ;
	orglocer_co	finish ;
	orglocer(cc *p,cc *u,char *b,int l) noex : pr(p), un(u) {
	    rbuf = b ;
	    rlen = l ;
	    finish(this,orglocerco_finish) ;
	} ;
	int start(char *tb) noex {
	    tfname = tb ;
	    return SR_OK ;
	} ;
	operator int () noex ;
	int userenv() noex ;
	int userconf() noex ;
	int localconf() noex ;
	int sysconf() noex ;
	int ifinish() noex ;
	~orglocer() {
	    (void) ifinish() ;
	} ;
    } ; /* end struct (orglocer) */
}

typedef int (orglocer::*orglocer_m)() noex ;


/* forward references */


/* local variables */

static strlibval	orgloc(strlibval_orgloc) ;

static cchar		*etcdname = sysword.w_etcdir ;

constexpr cchar		oln[] = ORGLOCFNAME ;

constexpr orglocer_m	mems[] = {
	&orglocer::userenv,
	&orglocer::userconf,
	&orglocer::localconf,
	&orglocer::sysconf,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int localgetorgloc(cchar *pr,char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0]) {
	        char	*tfname{} ;
	        if ((un == nullptr) || (un[0] == '\0')) un = "-" ;
	        if ((rs = malloc_mp(&tfname)) >= 0) {
		    orglocer	oo(pr,un,rbuf,rlen) ;
		    if ((rs = oo.start(tfname)) >= 0) {
			{
			    rs = oo ;
			    len = rs ;
			}
			rs1 = oo.finish ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (orglocer) */
		    rs1 = uc_free(tfname) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorglocer) */


/* local subroutines */

int orglocer::userenv() noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (orgloc && (orgloc[0] != '\0')) {
	    rs = sncpy1(rbuf,rlen,orgloc) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
}

int orglocer::userconf() noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*hbuf{} ;
	if ((rs = malloc_mp(&hbuf)) >= 0) {
	    cint	hlen = rs ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
		if ((rs = mkpath3(tfname,hbuf,etcdname,oln)) >= 0) {
		     if ((rs = filereadln(tfname,rbuf,rlen)) >= 0) {
		         len = rs ;
		     } else if (isNotPresent(rs)) {
			 rs = SR_OK ;
		     }
	         }
	    } /* end if (getuserhome) */
	    rs1 = uc_free(hbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}

int orglocer::localconf() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = mkpath3(tfname,pr,etcdname,oln)) >= 0) {
	    if ((rs = filereadln(tfname,rbuf,rlen)) >= 0) {
		len = rs ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
	
int orglocer::sysconf() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = mkpath(tfname,etcdname,oln)) >= 0) {
	    if ((rs = filereadln(tfname,rbuf,rlen)) >= 0) {
		len = rs ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}

int orglocer::ifinish() noex {
	return SR_OK ;
}

orglocer::operator int () noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs != 0) && mems[i] ; i += 1) {
	    orglocer_m	m = mems[i] ;
	    rs = (this->*m)() ;
	} /* end for */
	return rs ;
}

orglocer_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case orglocerco_finish:
		rs = op->ifinish() ;
		break ;
	    } /* end switch */
	}
	return rs ;
}


