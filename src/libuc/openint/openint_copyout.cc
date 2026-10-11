/* openint_copyout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* LOCAL facility open-service (copyout) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

/* revision history:

	= 2003-11-04, David A­D­ Morano
	This code was started by taking the corresponding code from
	the TCP-family module.  In retrospect, that was a mistake.
	Rather I should have started this code by using the
	corresponding UUX dialer module.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	openint_copyout

	Description:
	This is a facility-open-intercept module.

	Synopsis:
	int openint_copyout(pr,dn,bn,prn,of,om,argv,envv,to)
	cchar	*pr ;
	cchar	*dn ;
	cchar	*bn ;
	cchar	*prn ;
	int		of ;
	mode_t		om ;
	cchar	**argv ;
	cchar	**envv ;
	int		to ;

	Arguments:
	pr		program-root
	dn		dir-name
	bn		base-name
	prn		facility name
	of		open-flags
	om		open-mode
	argv		argument array
	envv		environment array
	to		time-out

	Returns:
	>=0		file-descriptor
	<0		error code (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU |DEBPRINTF(3u)| */

#include	"openint_copyout.h"

#pragma		GCC dependency		"mod/deb.ccm"

import deb ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int openint_copyout(cc *pr,cc *dn,cc *bn,cc *prn,
		int of,mode_t om,con mainv argv,con mainv envv,int to) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		fd = -1 ;
	[[maybe_unused]]
	cchar		*inter = "copyout" ;
	DEBPRINTF("pr=%s\n",pr) ;
	DEBPRINTF("dn=%s\n",dn) ;
	DEBPRINTF("prn=%s\n",prn) ;
	(void) argv ;
	(void) envv ;
	(void) to ;
	if (pr && dn && dn && prn) ylikely {
	    if (char *fbuf ; (rs = libmem.mp(&fbuf)) >= 0) {
	        if ((rs = mkpath2(fbuf,dn,bn)) >= 0) {
	            rs = u_open(fbuf,of,om) ;
	            fd = rs ;
	        } /* end */
	        rs1 = libmem.free(fbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (openint_copyout) */


/* local subroutines */



