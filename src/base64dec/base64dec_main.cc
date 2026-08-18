/* base64dec_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* test the |base64(3uc)| subroutines */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:
	This program tests the BASE64 object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<cstdio>		/* |stdout(3stdio)| */
#include	<cstring>		/* |strcmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<ulogerror.h>
#include	<umem.hh>
#include	<libf.h>		/* |fwrite(3f)| */
#include	<ascii.h>
#include	<base64.h>
#include	<ccfile.hh>
#include	<bufos.hh>
#include	<rmx.h>			/* |rmeol(3uc)| */
#include	<strnul.hh>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU debugging */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"base64dec.h"		/* program-private */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	MI		maininfo

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debugging */
#endif
#ifndef	CF_TEXT
#define	CF_TEXT		1		/* text */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct maininfo : proginfo {
	maininfo(int c,mainv a,mainv e) noex : proginfo(c,a,e) { } ;
	int start() noex ;
	int finish() noex ;
	int dec(cchar *) noex ;
	void dtor() noex ;
	destruct maininfo() noex {
	    if (argv) dtor() ;
	} /* end dtor */
} ; /* end struct (maininfo) */


/* forward references */


/* local variables */

cbool		f_debug = CF_DEBUG ;
cbool		f_text	= CF_TEXT ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		ex = EXIT_SUCCESS ;
	if (argc > 0) {
	    if (maininfo mi(argc,argv,envv) ; (rs = mi.start()) >= 0) {
		mi.ofp = stdout ;
	        for (int ai = 1 ; ai < argc ; ai += 1) {
		    if (cchar *arg = argv[ai] ; arg && arg[0]) {
			DPRINTF("arg=%s\n",arg) ;
			if (strcmp(arg,"text") == 0) {
			    mi.fl.text = true ;
			} else {
		            rs = mi.dec(arg) ;
			}
		    } /* end if (argument) */
		    if (rs < 0) break ;
	        } /* end for */
	        rs1 = mi.finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (maininfo) */
	} /* end if (arguments) */
	if ((ex == EXIT_SUCCESS) && (rs < 0)) {
	    ex = EXIT_FAILURE ;
	}
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

int maininfo::start() noex {
    	return SR_OK ;
} /* end method (maininfo::start) */

int maininfo::finish() noex {
    	return SR_OK ;
} /* end method (maininfo::finish) */

void maininfo::dtor() noex {
	if (cint rs = finish() ; rs < 0) {
	    ulogerror("maininfo",rs,"dtor") ;
	}
} /* end method (maininfo::dtor) */

int maininfo::dec(cchar *fn) noex {
    	return base64dec_dec(this,fn) ;
} /* end method (maininfo::enc) */


