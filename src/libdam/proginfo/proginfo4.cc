/* proginfo3 MODULE (Module-Implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program information */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

	= 2026-10-20, David A­D­ Morano
	Modularized.

*/

/* Copyright © 1998,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	proginfo

	Description:
	This module provides a very basic sets of simple subroutines
	for basic program management.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<nulstr.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<iostream>		/* C++STD */
#include	<ucmem.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<strw.h>		/* LIBUC */
#include	<sncpy.h>		/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<prognamevar.hh>	/* LIBDAM */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module proginfo ;

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libu::snvprintf ;			/* subroutine */
using libuc::mem ;			/* variable */
using std::cerr ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

statcexpr bool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

int proginfo::eprintf(cchar *fmt,...) noex {
    	va_list		ap ;
    	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (fmt) ylikely {
	    rs = SR_OK ;
	    if (fmt[0]) ylikely {
	        va_begin(ap,fmt) ;
	        if (char *pbuf ; (rs = mem.ml(&pbuf)) >= 0) ylikely {
		    cint plen = rs ;
	    	    if ((rs = snvprintf(pbuf,plen,fmt,ap)) > 0) ylikely {
		        cerr << pbuf ;
		        len = rs ;
		    } /* end if (snvprintf) */
	            rs1 = mem.free(pbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	        va_end(ap) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end method (proginfo::eprintf) */

int proginfo_eprint::operator () (cchar *sp,int sl) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (sp) ylikely {
	    cchar *bp ;
	    if (nulstr ss ; (rs = ss.start(sp,sl,&bp)) >= 0) ylikely {
		cint bl = rs ;
		{
		    cerr << bp << eol ; 
		    len = bl ;
		}
		rs1 = ss.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (nulstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end method (proginfo_eprint::operator) */

int proginfo_eprint::operator () () noex {
    	return operator int () ;
} /* end method (proginfo_eprint::operator) */

proginfo_eprint::operator int () noex {
    	int		rs = SR_OK ;
	int		len = 0 ;
	{
    	    cerr << eol ;
	    len = 1 ;
	}
	return (rs >= 0) ? len : rs ;
} /* end method (proginfo_eprint::operator) */


