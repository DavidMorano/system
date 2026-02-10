/* uids SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* LIBU process ID operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This code was originally written.  This subroutines
	(or something similar to it) is standard on some UNIXes but
	not on others, so it is now provided.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	uids

	Description:
	This is a cheap combination of the IDS (libuc) object and
	the |permid(3uc)| subroutine.  Only the process user-id and
	group-id are considered in permission evaluations.  So use
	of the permssions element of this object is not conclusive.
	On permission failures, other methods needs to be emplyed
	to determine real accessibility.  But permission success
	is conclusive.

	Synopsis:

	Arguments:

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>		/* |O_ACCMODE| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"uids.hh"


/* local defines */


/* imported namespace */

using libu::uids ;			/* type */


/* external subroutines */


/* external variables */


/* local structues */

enum pfields {
	pfield_oth,
	pfield_grp,
	pfield_usr,
	pfield_overlast
} ; /* end enum (pfields) */

/* permissoin bits per field */
constexpr int		pbits = pfield_overlast ;	

constexpr local int	mkperms(int) noex ;

namespace {
    constexpr int	tablen = (1 << pbits) ;
    struct permtab {
	uchar	tab[tablen] = {} ;
	constexpr permtab() noex {
	    for (int i = 0 ; i < tablen ; i += 1) {
		if (i & R_OK) tab[i] |= S_IROTH ;
		if (i & W_OK) tab[i] |= S_IWOTH ;
		if (i & X_OK) tab[i] |= S_IXOTH ;
	    } /* end for */
	} ; /* end ctor */
	constexpr int operator [] (int a) const noex {
	    return tab[a] ;
	} ;
    } ; /* end struct (permtab) */
} /* end namespace */

namespace {
    struct trier {
	uids	*op ;
        ustat	*sbp ;
        int	am ;
	int	pm ;			/* permissions-mask */
	bool	fdone{} ;
	trier(uids *p,ustat *s,int a) noex : op(p), sbp(s), am(a) { 
	    pm = mkperms(am) ;
	} ;
	operator int () noex ;
	int checkpm(int) const noex ;
	int root() noex ;
	int usr() noex ;
	int grp() noex ;
	int oth() noex ;
    } ; /* end struct (trier) */
    typedef int (trier::*trier_m)() noex ;
} /* end namespace */


/* forward references */


/* local variables */

constexpr trier_m	tries[] = {
	&trier::root,
	&trier::usr,
	&trier::grp,
	&trier::oth
} ; /* end array (tries) */

constexpr permtab	perms ;


/* exported variables */


/* exported subroutines */

namespace libu {
    int uids::perm(ustat *sbp,int am) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (sbp) {
	    rs = SR_OK ;
	    if (am) {
	        if (trier to(this,sbp,am) ; (rs = to) >= 0) {
		    f = rs ;
	        } /* end if (trier) */
	    } /* end if (non-zero positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
    } /* end method (uids::perm) */
} /* end namespace (libu) */


/* local subroutines */

trier::operator int () noex {
    	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if (rs || fdone) break ;
	} /* end for */
	return rs ;
} /* end method (trier::operator) */

int trier::checkpm(int pfield) const noex {
	return (((sbp->st_mode >> (pfield * pbits)) & pm) == pm) ;
} /* end method (trier::checkpm) */

int trier::root() noex {
	return (op->euid == 0) ;
} /* end method (trier::root) */

int trier::usr() noex {
    	int	f = false ;
	if (sbp->st_uid == op->euid) {
	    f = checkpm(pfield_usr) ;
	    fdone = true ;
	}
    	return f ;
} /* end method (trier::usr) */

int trier::grp() noex {
    	int	f = false ;
	if (sbp->st_gid == op->egid) {
	    f = checkpm(pfield_grp) ;
	    fdone = true ;
	}
    	return f ;
} /* end method (trier::grp) */

int trier::oth() noex {
    	int	f = false ;
	{
	    f = checkpm(pfield_oth) ;
	}
    	return f ;
} /* end method (trier::oth) */

constexpr local int mkperms(int am) noex {
    	am &= O_ACCMODE ;
    	return perms[am] ;
} /* end subroutine (mkperms) */


