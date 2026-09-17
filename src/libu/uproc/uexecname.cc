/* uexecname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uexecname

	Coding note:
	Retrieve the "exec" name of the current process.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<errtimer.hh>		/* LIBU */
#include	<usys.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"uexecname.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_execname(char *rbuf,int rlen) noex {
    	int		rs = SR_FAULT ;
	if (rbuf) {
	    rs = SR_INVALID ;
	    if (rlen > 0) {
		rs = SR_OK ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (uexecname) */


/* local subroutines */



#else /* defined(SYSHAS_GETEXECNAME) && (SYSHAS_GETEXECNAME > 0) */
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
/******************************************************************************/

using usys::darwin_execname ;	/* subroutine */

namespace {
    struct execstorer {
	cint	tlen = MAXPATH ;
	char	*ebuf{} ;
	int	elen ;
	int	rss ;
	execstorer() noex {
	    cnullptr	np{} ;
	    cnothrow	nt{} ;
	    rss = SR_NOMEM ;
	    if (char *tbuf = new(nt) char[tlen + 1]) {
	        if ((rss = darwin_execname(tbuf,tlen)) >= 0) {
		    elen = rss ;
		    rss = SR_NOMEM ;
		    if ((ebuf = new(nt) char[elen + 1]) != np) {
			csize	esize = size_t(elen) ;
			strncpy(ebuf,tbuf,esize) ;
			ebuf[elen] = '\0' ;
			rss = elen ;
		    } /* end if (new-ebuf) */
	        } /* end if (darwin_execname) */
	        delete [] tbuf ;
	    } /* end if (new-delete) */
	} ; /* end ctor */
	operator int () noex {
	    return ((rss >= 0) && ebuf) ? elen : rss ;
	} ; /* end */
	destruct execstorer() {
	    if (ebuf) {
		delete [] ebuf ;
		ebuf = nullptr ;
		elen = 0 ;
	    } /* end if (delete) */
	} ; /* end dtor */
    } ; /* end struct (execstorer) */
} /* end namespace */

static execstorer	exec_data ;

cchar *getexecname() noex {
	cchar		*rp = nullptr ;
	int		rs ;
    	if  (static cint rss = exec_data ; (rs = rss) >= 0) {
	    if (rs > 0) {
	        rp = exec_data.ebuf ;
	    }
	} else {
	    errno = (neg rs) ;
	    ulogerror("getexecname",rs,"namer::operator") ;
	} /* end */
	return rp ;
} /* end subroutine (getexecname) */

/******************************************************************************/
#elif	defined(OSNAME_Linux) && (OSNAME_Linux > 0)
/******************************************************************************/

#include	<sys/auxv.h>		/* Solaris® */
#include	<elf.h>			/* Solaris® */

cchar *getexecname() noex {
	culong		at = AT_EXECFN ;
	cchar		*rp = nullptr ; /* return-value */
	if (ulong r ; (r = getauxval(at)) != 0) {
	    rp = charp(r) ;
	} /* end if (have entry) */
	return rp ;
} /* end subroutine (getexecname) */

/******************************************************************************/
#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
#endif /* defined(SYSHAS_GETEXECNAME) && (SYSHAS_GETEXECNAME > 0) */


