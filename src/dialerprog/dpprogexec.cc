/* dpprogexec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* progexec the execution request */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* debug print-outs (non-switchable) */
#define	CF_DEBUG	0		/* debug print-outs switchable */

/* revision history:

	= 1990-11-01, David A­D­ Morano
	This code was originally written.

	= 2001-04-11, David A­D­ Morano
	This old dog program has been enhanced to serve as the
	environment wiper for executing MIPS programs.

*/

/* Copyright © 1990,2001 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/**************************************************************************

  	Description:
	This subroutine performs an |exec(2)| on the given program
	with its environment and arguments.

**************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<keyopt.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vechand.h>
#include	<buffer.h>
#include	<hasx.h>
#include	<exitcodes.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */

#include	"config.h"
#include	"defs.h"
#include	"envs.h"


/* local defines */

#define	PI	proginfo

#ifndef	VARPATH
#define	VARPATH	"PATH"
#endif

#ifndef	VARAST
#define	VARAST	"AST"
#endif



/* external subroutines */


/* external variables */


/* local structures */

struct xfile {
	char	*fname ;
	char	*vname ;
} ;

struct execmap {
	char	*fname ;
	char	*interpreter ;
} ;

struct intprog {
	char	fname[MAXPATHLEN + 1] ;
	char	arg[MAXPATHLEN + 1] ;
} ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int progexec(PI *pip,cchar *progfname,int argv,mainv argr) noex {
	VECHAND	avs ;
	BUFFER	b ;
	vecstr	*elp ;
	int	rs = SR_OK ;
	int	i ;
	int	si ;
	int	opts ;
	int	cl ;
	int	start = 10 ;
	int	f_m = FALSE ;
	int	f_sa = FALSE ;
	cchar	**av ;
	cchar	**ev ;
	cchar	*abuf = nullptr ;
	cchar	*cp ;


#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
		debugprintf("progexec: argr=%d\n",argr) ;
		for (i = 0 ; argv[i] != nullptr ; i += 1)
		debugprintf("progexec: arg[%u]=>%s<\n",i,argv[i]) ;
	}
#endif

	bflush(pip->efp) ;

	elp = &pip->exports ;
	rs = vecstr_envadd(elp,"_",progfname,-1) ;
	if (rs < 0)
	    goto ret0 ;

	opts = VECHAND_OCOMPACT ;
	rs = vechand_start(&avs,10,opts) ;
	if (rs < 0)
	    goto ret0 ;

/* setup the zeroth argument */

	si = 0 ;
	cp = argv[0] ;
	cl = -1 ;
	f_sa = (argv[0] == nullptr) ;
	f_sa = f_sa || ((argv[0] != nullptr) && hasonlypm(argv[0],-1)) ;
	f_sa = f_sa || pip->fl.shell ;
	if (f_sa) {
	    si = pip->fl.shell ? 0 : 1 ;
	    cl = sfbasename(progfname,-1,&cp) ;
	    start = (cl+2) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
		debugprintf("progexec: si=%u argz=>%r<\n",si,cp,cl) ;
#endif

	f_m = FALSE ;
	f_m = f_m || ((argv[0] == nullptr) && pip->fl.progminus) ;
	f_m = f_m || ((argv[0] == nullptr) && pip->fl.progdash) ;
	f_m = f_m || ((argv[0] != nullptr) && hasonlymi(argv[0],-1)) ;

	rs = buffer_start(&b,start) ;
	if (rs < 0)
	    goto ret1 ;

	if (f_sa || f_m) {

	    if ((argr > 0) && pip->fl.shell) {
	        si = 1 ;
	        argr -= 1 ;
	    }

	    if (f_m)
	        rs = buffer_chr(&b,'-') ;

	    if (rs >= 0)
	        rs = buffer_strw(&b,cp,cl) ;

	    if (rs >= 0) {

	        buffer_get(&b,&abuf) ;

	        rs = vechand_add(&avs,abuf) ;

	    } /* end if */

	} /* end if */

/* setup all remaining arguments */

	for (i = si ; (rs >= 0) && (argr > 0) && (argv[i] != nullptr) ; i += 1) {

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
		debugprintf("progexec: arg[%u]=>%s<\n",i,argv[i]) ;
#endif

	    argr -= 1 ;
	    rs = vechand_add(&avs,argv[i]) ;

	} /* end for */

	vechand_getvec(&avs,&av) ;

	if (rs >= 0) {
	    cchar	*pfn = progfname ;
	    char	tmpfname[MAXPATHLEN + 1] ;
	    if (progfname[0] != '/') {
		rs = proginfo_pwd(pip) ;
		if (rs >= 0) {
		    pfn = tmpfname ;
		    rs = mkpath2(tmpfname,pip->pwd,progfname) ;
		}
	    }
	    if (rs >= 0)
	        rs = vecstr_envadd(elp,"_EF",pfn,-1) ;
	}

	if (rs >= 0)
	    rs = vecstr_envadd(elp,"_A0",av[0],-1) ;

	if (rs < 0)
	    goto ret2 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
		int	i ;
		for (i = 0 ; av[i] != nullptr ; i += 1)
		debugprintf("progexec: av[%u]=>%s<\n",i,av[i]) ;
	}
#endif

	vecstr_getvec(elp,&ev) ;

	{
	    cchar **eav = (cchar **) av ;
	    cchar **eev = (cchar **) ev ;
	    rs = u_execve(progfname,eav,eev) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progexec: u_execve() rs=%d\n",rs) ;
#endif

ret2:
	buffer_finish(&b) ;

ret1:
	vechand_finish(&avs) ;

ret0:
	return rs ;
}
/* end subroutine (progexec) */


/* local subroutines */



