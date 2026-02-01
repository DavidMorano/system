/* ureadext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-2 */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_readextdesc
	u_readextfile

	General-Description:
	This module contains the UNIX® file-system system calls.

	Name:
	u_readextdesc

	Description:
	This subroutine provides special control over I-O devices.
	See note-4 below for more information on the implementation.

	Synopsis:
	int u_readextdesc(int fd,int dfd) noex

	Arguments:
	fd		file-descriptor
	dfd		file-descriptor destination


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<numeric>		/* |sat_add(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>		/* |u_sysconfval(3u)| */
#include	<usupport.h>		/* |um(3u)| */
#include	<intceil.h>		/* |ceil(3u)| */
#include	<intsat.h>
#include	<localmisc.h>

#include	"ureadext.h"


/* local defines */

#define	PAGEMULT		4	/* page-multiplier */
#define	ALLOCMIN		128	/* allocation-minimum */


/* imported namespaces */

using std::min ;			/* subroutine */
using std::max ;			/* subroutine */
using std::sat_add ;			/* subroutine */
using libu::um ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int u_writedesc(int,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	pagesz ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local int getsize(int) noex ;
local int upagesize() noex ;


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int u_readextdesc(int fd,int dfd) noex {
	int		rs = SR_BADFD ;
	int		rs1 ;
	int		tlen = 0 ; /* return-value */
	if ((fd >= 0) && (dfd >= 0)) ylikely {
    	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) ylikely {
	        if ((rs = getsize(fd)) >= 0) ylikely {
		    cint fsz = rs ;
	            cint csz = (var.pagesz * PAGEMULT) ;
		    {
			size_t	tsize = 0 ;
		        cint	clen = min(fsz,csz) ;
	                if (char *cbuf ; (rs = um.mall(clen,&cbuf)) >= 0) {
			    while ((rs = u_read(fd,cbuf,clen)) > 0) {
			        rs = u_write(dfd,cbuf,rs) ;
				tsize += size_t(rs) ;
			        if (rs < 0) break ;
			    } /* end while */
			    rs1 = um.free(cbuf) ;
			    if (rs >= 0) rs = rs1 ;
		        } /* end if (m-a-f) */
			tlen = intsat(tsize) ;
		    } /* end block */
	        } /* end if (getsize) */
	    } /* end if (vars) */
	} /* end if (valid) */
    	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (u_readextdesc) */

int u_readextfile(int fd,cchar *name) noex {
    	cint		of = (O_WRONLY | O_TRUNC) ;
    	int		rs ;
	int		rs1 ;
	int		tlen = 0 ; /* return-value */
	cmode		om = 0666 ;
	if ((rs = u_open(name,of,om)) >= 0) ylikely {
	    cint dfd = rs ;
	    {
		rs = u_readextdesc(fd,dfd) ;
		tlen = rs ;
	    }
	    rs1 = u_close(dfd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open-close) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (u_readextfile) */


/* local subroutines */

vars::operator int () noex {
    	int		rs ;
	if ((rs = upagesize()) >= 0) ylikely {
	    var.pagesz = rs ;
	}
	return rs ;
} /* end method (vars::operator) */

local int getsize(int fd) noex {
    	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
	    if (S_ISREG(sb.st_mode)) ylikely {
		if (sb.st_size == 0uz) {
		    rs = min(var.pagesz,ALLOCMIN) ;
		} else {
		    clong fsize = long(sb.st_size) ;
		    rs = intsat(fsize) ;
		}
	    } else {
		rs = var.pagesz ;
	    }
	} /* end if (u_fstat) */
    	return rs ;
} /* end subroutine (getsize) */

local int upagesize() noex {
    	cint		cmd = _SC_PAGESIZE ;
	return u_sysconfval(cmd,nullptr) ;
} /* end subroutine (upagesize) */


