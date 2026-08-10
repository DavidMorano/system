/* mkufname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a "user" file-name (the name is quite misleading) */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-04-01, David A­D­ Morano
	This is part of our cleanup-compatibility effort.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkufname

	Description:
	Make a "user" file-name.  What this actually does is to
	just make a filename that is the union of a user home
	directory and another filename.  It checks that the (so-called)
	file-name being added to the result is not actually an
	absolute file-path-name.  If the file-name being added is
	already absolete, no home-directory-name is prefixed.

	Synopsis:
	int mkufname(char *rbuf,int rlen,cchar *hn,cchar *fn) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	hn		home-directory-name
	fn		file-name

	Results:
	>=0		number of bytes in result
	<0		error (system-return)

	Notes:
	1. This subroutine was horribly named.  But it is retained for
	historical purposes.
	2. The result buffer is assumed to be MAXPATHLEN bytes (plus 1)
	long.  This is not actually uncommon in most path-handling
	code.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkpathx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"mkx.h"
#include	"mkufname.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* local variables */


/* exported variables */


/* exported subroutines */

int mkufname(char *rbuf,cchar *hn,cchar *fn) noex {
	int		rs = SR_FAULT ;
	if (rbuf && hn && fn) ylikely {
	    rs = SR_INVALID ;
	    if (hn[0] && fn[0]) ylikely {
		bool	f = true ;
		rs = SR_OK ;
		f = f && (fn[0] != '/') ;
		f = f && (! ((fn[0] == '.') && (fn[1] == '/'))) ;
		f = f && (! ((fn[0] == '.') && (fn[1] == '.'))) ;
		if (f) {
	            rs = mkpath(rbuf,hn,fn) ;
		} else {
		    rs = mkpath(rbuf,fn) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (mkufname) */


