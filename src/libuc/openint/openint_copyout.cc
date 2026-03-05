/* openint_copyout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* LOCAL facility open-service (copyout) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"openint_copyout.h"
#include	"defs.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

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
{
	int		rs = SR_OK ;
	int		fd = -1 ;
	cchar	*inter = "copyout" ;
	char		fname[MAXPATHLEN+1] ;

#if	CF_DEBUGS
	debugprintf("openint_copyout: pr=%s\n",pr) ;
	debugprintf("openint_copyout: dn=%s\n",dn) ;
	debugprintf("openint_copyout: prn=%s\n",prn) ;
#endif

	if ((rs = mkpath2(fname,dn,bn)) >= 0) {
	    rs = u_open(fname,of,om) ;
	    fd = rs ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openint_copyout) */


/* local subroutines */



