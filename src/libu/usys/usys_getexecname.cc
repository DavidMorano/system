/* usys_getexecname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the file-name (file-path) that was used to 'exec' this program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written to get some commonality across
	our most used operating systems. 

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getexecname

	Description:
	This subroutine returns the name (file-path) by which the
	current program was 'exec'ed.

	Synopsis:
	cchar *getexecname() noex

	Returns:
	-		the exec-name pointer or NULL (if not found)

	Notes:
	1. This subroutine (the subroutine that needs emulation by
	some operating systems, which will remain nameless like
	Apple-Darwin®) is already native to Solaris® (SunOS), and its
	declaration on that plarform is accessed though the include
	file |cstdlib|.
	2. This subroutine is Thread-Safe but only Fork-Semi-Safe.
	That is: it is as fork-safe as any general C++ piece of
	code.  The C++ language itself only creates code (in the
	strictest sense) that is only Fork-Semi-Safe.  Actually,
	there are many pieces of code throughout everywhere (when
	written in C++) that are only Fork-Semi-Safe (if you did
	not already know).  The reason is due to the fact that
	executing over statically declared variables in block scape
	that are dynamically initialized by subroutine calls is not
	strictly Fork-Safe in the C++ language.  Of course, the C
	language does not even allow for that, so it is not an issue
	in pure C-language environments.  Executing over statically
	declared variables in namespace (file) scope initialized
	dynamically might also be fork-unsafe in some weirdo
	circumstantes, but that is usually very unlikely since the
	initialization happens at process load time when there is
	not likely to be multiple threads executing at the same
	time and one of them executing over the static variable
	initialization while a fork happens in another thread,
	Further the unlikiness is helped because the loadable module
	image got laoded by the dynamic linker, and then a fork
	happening at the very same time elsewhere is very quite
	unlikely.  This is probably why the ISO C++ Committee did
	not really care about strict fork-safety in dynamically
	initialized static variables.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getprogname(3c)| |getexecname(3c) */
#include	<cstring>		/* |strncpy(3c)| */
#include	<new>			/* |nothrow(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ulogerror.h>
#include	<localmisc.h>

#include	"usys.h"
#include	"getexecname.h"

#ifndef	MAXPATH
#define	MAXPATH		(4 * 1024)	/* resonable value for exec-name */
#endif

using std::nothrow ;			/* constant */

#if	(! defined(SYSHAS_GETEXECNAME)) || (SYSHAS_GETEXECNAME == 0)
#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)

using usys::darwin_execname ;	/* subroutine */

namespace {
    struct execstorer {
	cint	tlen = MAXPATH ;
	char	*ebuf{} ;
	int	elen ;
	int	rss ;
	execstorer() noex {
	    cnullptr	np{} ;
	    rss = SR_NOMEM ;
	    if (char *tbuf ; (tbuf = new(nothrow) char[tlen + 1]) != np) {
	        if ((rss = darwin_execname(tbuf,tlen)) >= 0) {
		    elen = rss ;
		    rss = SR_NOMEM ;
		    if ((ebuf = new(nothrow) char[elen + 1]) != np) {
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
	} ;
	destruct execstorer() {
	    if (ebuf) {
		delete [] ebuf ;
		ebuf = nullptr ;
		elen = 0 ;
	    }
	} ; /* end dtor */
    } ; /* end struct (execstorer) */
} /* end namespace */

static execstorer	exec_data ;

cchar *getexecname() noex {
    	static cint	rss = exec_data ;
	cchar		*rp = nullptr ;
	int		rs ;
	if ((rs = rss) >= 0) {
	    if (rs > 0) {
	        rp = exec_data.ebuf ;
	    }
	} else {
	    errno = (- rs) ;
	    ulogerror("getexecname",rs,"namer::operator") ;
	}
	return rp ;
}
/* end subroutine (getexecname) */

#elif	defined(OSNAME_Linux) && (OSNAME_Linux > 0)

#include	<sys/auxv.h>
#include	<elf.h>

cchar *getexecname() noex {
	culong		at = AT_EXECFN ;
	cchar		*rp = nullptr ;
	if (ulong r ; (r = getauxval(at)) != 0) {
	    rp = charp(r) ;
	} /* end if (have entry) */
	return rp ;
}
/* end subroutine (getexecname) */

#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
#endif /* (! defined(SYSHAS_GETEXECNAME)) || (SYSHAS_GETEXECNAME == 0) */


