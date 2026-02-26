/* hostequiv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* rough equivalent host-name check */
/* version %I% last-modified %G% */


/* revision history:

	- 1996-11-21, David A-D- Morano
	This subroutine was started by copying it from the RSLOW
	program.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/**************************************************************************

  	Name:
	hostequiv

	Description:
	Determine if two INET host-names are equivalent.

**************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<strings.h>		/* |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"hostequiv.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |sysword(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct checker {
	cchar		*h1 ;
	cchar		*h2 ;
	cchar		*ld ;
	char		*cp1 ;
	char		*cp2 ;
	int		len1 ;
	int		len2 ;
	bool		fdone ;
	bool		f_h1 = false ;
	bool		f_h2 = false ;
	checker(cchar *ªh1,cchar *ªh2,cchar *ªld) noex : h1(ªh1), h2(ªh2) {
	    ld = ªld ;
	    if ((ld == nullptr) || (ld[0] == '\0')) {
		ld = "local" ;
	    }
	    fdone = false ;
	} ; /* end ctor */
	operator int () noex ;
	int dom() noex ;
	int host1a() noex ;
	int host1b() noex ;
	int host1c() noex ;
	int host2a() noex ;
	int host2b() noex ;
	int host2c() noex ;
    } ; /* end struct (checker) */
    typedef int (checker::*checker_m)() noex ;
} /* end namespace */


/* forward references */


/* local variables */

constexpr checker_m	tries[] = {
    	&checker::dom,
    	&checker::host1a,
    	&checker::host1b,
    	&checker::host1c,
    	&checker::host2a,
    	&checker::host2b,
    	&checker::host2c
} ; /* end array (tries) */


/* exported variables */


/* exported subroutines */

int hostequiv(cchar *h1,cchar *h2,cchar *localdomain) noex {
    	int		rs = SR_FAULT ;
	if (h1 && h2) {
	    rs = SR_INVALID ;
	    if (h1[0] && h2[0]) {
    	        checker co(h1,h2,localdomain) ;
	        rs = co ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (hostequiv) */


/* local subroutines */

checker::operator int () noex {
    	int		f = false ;
	for (cauto &m : tries) {
	    f = (this->*m)() ;
	    if (fdone) break ;
	} /* end for */
	return f ;
} /* end method (checker::operator) */

int checker::dom() noex {
    	int		f = false ;
	if ((cp1 = strchr(h1,'.')) != nullptr) f_h1 = true ;
	if ((cp2 = strchr(h2,'.')) != nullptr) f_h2 = true ;
	if (f_h1 == f_h2) {
	    fdone = true ;
	    f = (strcasecmp(h1,h2) == 0) ;
	}
	return f ;
} /* end method (checker::dom) */

int checker::host1a() noex {
    	int		f = false ;
	if (f_h1) {
	    len1 = intconv(cp1 - h1) ;
	    len2 = lenstr(h2) ;
	    if (len1 != len2) {
		fdone = true ;
	    }
	}
	return f ;
} /* end method (checker::host1a) */

int checker::host1b() noex {
    	int		f = false ;
	if (f_h1) {
	    cp1 += 1 ;
	    if (strcasecmp(cp1,ld) != 0) {
		fdone = true ;
	    }
	}
	return f ;
} /* end method (checker::host1b) */

int checker::host1c() noex {
    	int		f = false ;
	if (f_h1) {
	    f = (strncasecmp(h1,h2,len1) == 0) ;
	    fdone = true ;
	}
	return f ;
} /* end method (checker::host1c) */

int checker::host2a() noex {
    	int		f = false ;
	if (f_h2) {
	    len1 = lenstr(h1) ;
	    len2 = intconv(cp2 - h2) ;
	    if (len1 != len2) {
	        fdone = true ;
	    }
	}
	return f ;
} /* end method (checker::host2a) */

int checker::host2b() noex {
    	int		f = false ;
	if (f_h2) {
	    cp2 += 1 ;
	    if (strcasecmp(cp2,ld) != 0) {
		fdone = true ;
	    }
	}
	return f ;
} /* end method (checker::host2b) */

int checker::host2c() noex {
    	int		f = false ;
	if (f_h2) {
	    f = (strncasecmp(h1,h2,len2) == 0) ;
	    fdone = true ;
	}
	return f ;
} /* end method (checker::host2c) */

#ifdef	COMMENT
int checker::full() noex {
	bool	f_h1 = false ;
	bool	f_h2 = false ;
	int	len1, len2 ;
	char	*cp1, *cp2 ;
	if ((cp1 = strchr(h1,'.')) != nullptr) f_h1 = true ;
	if ((cp2 = strchr(h2,'.')) != nullptr) f_h2 = true ;
	if (LEQUIV(f_h1,f_h2)) {
	    return (! strcasecmp(h1,h2)) ;
	}
	if (f_h1) {
	    len1 = intconv(cp1 - h1) ;
	    len2 = lenstr(h2) ;
	    if (len1 != len2) return false ;
	    cp1 += 1 ;
	    if (strcasecmp(cp1,ld) != 0) return false ;
	    return (strncasecmp(h1,h2,len1) == 0) ;
	}
	if (f_h2) {
	    len1 = lenstr(h1) ;
	    len2 = intconv(cp2 - h2) ;
	    if (len1 != len2) return false ;
	    cp2 += 1 ;
	    if (strcasecmp(cp2,ld) != 0) return false ;
	    return (strncasecmp(h1,h2,len2) == 0) ;
	}
	return 0 ;
} /* end method (checker::full) */
#endif /* COMMENT */


