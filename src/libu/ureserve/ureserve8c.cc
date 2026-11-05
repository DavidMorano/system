/* ureserve8c MODULE (vecstr - implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* vecstr - process an environment file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstr::addstrfile

	Description:
	This subroutine will read (process) an environment file and
	put any newly encountered environment variables into the
	string list (supplied).  Variables found that already exist
	are ignored.

	Synopsis:
	int vecstr::addstrfile(cchar *fname) noex

	Arguments:
	fname		file to process

	Returns:
	>=0		count of environment variables
	<0		bad

	- Implementation note:
	Although this function is (overall) rather simple, I put
	some slightly cleaver thought into parsing the lines.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<cstring>		/* CSTD |strncasecmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<utimeout.h>		/* LIBU */
#include	<usysconf.h>		/* LIBU */
#include	<sysconfcmds.h>		/* LIBU |_SC_LINE_MAX| */
#include	<usupport.h>		/* LIBU |rmeol(3u)| */
#include	<ccfile.hh>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/usysconf.ccm"

module ureserve ;

import libutil ;			/* |lenstr(3u)| */
import usysconf ;			/* |usysconfval(3u)| */

/* local defines */

#define	LINEBUFMULT	5		/* line-buffer size multiplier */


/* imported namespaces */

using libu::rmeol ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		linebuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local int	vecstr_strfiler	(vecstr *,cchar *) noex ;


/* local variables */

static vars	var ;


/* exported variables */


/* exported subroutines */

int vecstr::addstrfile(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	DPRINTF("ent fn=%s\n",fname) ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		    rs = vecstr_strfiler(this,fname) ;
		    c = rs ;
		} /* end if (mkinit) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (vecstr::envfile) */


/* private subroutines */

int vecstr_strfiler(vecstr *op,cchar *fname) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
	cint		llen = var.linebuflen ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	DPRINTF("ent fn=%s\n",fname) ;
	if (char *lbuf = new(nt) char [llen + 1]) ylikely {
            cmode   om = 0666 ;
            if (ccfile cf ; (rs = cf.open(fname,np,om)) >= 0) ylikely {
                while ((rs = cf.readln(lbuf,llen)) > 0) {
		    if (int len ; (len = rmeol(lbuf,rs)) > 0) {
		        cchar	*cp{} ;
		        if (int cl ; (cl = sfshrink(lbuf,len,&cp)) > 0) {
			    if (cp[0] != '#') {
	            	        rs = op->add(cp,cl) ;
	            	        c += 1 ;
			    } /* end */
                        } /* end if (sfshrink) */
		    } /* end if (rmeol) */
                    if (rs < 0) break ;
                } /* end while (reading lines) */
                rs1 = cf.close ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (ccfile) */
	    delete [] lbuf ;
	} /* end if (m-a-f) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstr_strfiler) */

vars::operator int () noex {
    	cint		cmd = _SC_LINE_MAX ;
	int		rs ;
	if ((rs = usysconfval(cmd)) >= 0) ylikely {
	    linebuflen = (rs * LINEBUFMULT) ;
	} /* end */
	return rs ;
} /* end subroutine (vars::operator) */


