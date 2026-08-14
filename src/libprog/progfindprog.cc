/* progfindprog */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find the program to execute */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debug print-outs switchable */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progfindprog

	Synopsis:
	int progfindprog()

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<getprogpath.h>		/* LIBUC */
#include	<keyopt.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<strx.h>		/* LIBUC */
#include	<xfile.h>		/* LIBUC */
#include	<vstrcmp.h>		/* LIBUC |vstrkeycmp(3uc)| */
#include	<vardefs.h>		/* LIBU */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */

#include	"proginfo.hh"
#include	"progfindprog.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	BUFLEN
#define	BUFLEN		((2 * MAXPATHLEN) + 20)
#endif

#ifndef	ARCHBUFLEN
#define	ARCHBUFLEN	80
#endif

#define	VARBUFLEN	(20 * MAXPATHLEN)

#ifndef	elementsof
#define	elementsof(a)	(szof(a) / szof((a)[0]))
#endif

#undef	DEBUGFNAME
#define	DEBUGFNAME	"/tmp/lsh.nd"

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debug print-outs switchable */
#endif

#ifndef	PI
#define	PI		proginfo
#endif


/* external subroutines */

extern int	progdefprog(PI *,cchar **) noex ;


/* external variables */


/* local structures */


/* forward references */

local int	procsearch(PI *,vecstr *,char *,cchar *) noex ;
local int	loadpathlist(PI *,vecstr *,vecstr *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progfindprog(PI *pip,char *rbuf,ccha *pn) noex {
	int	rs = SR_FAULT ;
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	DEBUGPRINTF("ent pn=>%s<\n",pn) ;
#endif
	if (pip && rbuf && pn) ylikely {
	int	f ;
	cchar	*pnp = pn ;
	vecstr		*elp = &pip->exports ;
	rbuf[0] = '\0' ;
	rs = SR_OK ;
	if (pn == nullptr) {

	    rs = progdefprog(pip,&pnp) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	DEBUGPRINTF("progdefprog() rs=%d\n",rs) ;
#endif

	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(3) && (rs >= 0))
	DEBUGPRINTF("pnp=%s\n",pnp) ;
#endif

	if ((rs >= 0) && ((pnp == nullptr) || (pnp[0] == '\0'))) {
	    rs = SR_NOENT ;
	}

	    if (rs >= 0) ylikely {
	        if (strchr(pnp,'/') == nullptr) {
	            rs = procsearch(pip,elp,rbuf,pnp) ;
	            if ((rs < 0) && (pn == nullptr)) {
		        f = false ;
		        f = f || (rs == SR_NOENT) ;
		        if (f) {
		            pnp = DEFPROGFNAME ;
	                    rs = mkpath1(rbuf,pnp) ;
		        }
	            }
	        } else {
	            rs = mkpath1(rbuf,pnp) ;
	        } /* end if */
	    } /* end if (ok) */
	} /* end if (non-null) */
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	DEBUGPRINTF("ret rs=%d\n",rs) ;
#endif
	return rs ;
} /* end subroutine (progfindprog) */


/* local subroutines */

local int procsearch(PI *pip,vecstr *elp,char *rbuf,cchar *pn) noex {
	vecstr	pathlist ;
	int	rs ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	DEBUGPRINTF("pn=>%s<\n",pn) ;
#endif

	rs = vecstr_start(&pathlist,10,0) ;
	if (rs < 0)
	    goto ret0 ;

	rs = loadpathlist(pip,&pathlist,elp) ;
	if (rs < 0)
	    goto ret1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
		int	i ;
		char	*cp ;
		for (i = 0 ; vecstr_get(&pathlist,i,&cp) >= 0 ; i += 1)
	DEBUGPRINTF("pc=%s\n",cp) ;
	}
#endif

	rs = getprogpath(&pip->id,&pathlist,rbuf,pn,-1) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	DEBUGPRINTF("getprogpath() rs=%d\n",rs) ;
#endif

	if (rs == SR_NOENT) {
	    if ((rs = xfile(&pip->id,pn)) >= 0) {
	        rs = mkpath1(rbuf,pn) ;
	    }
	}

ret1:
	vecstr_finish(&pathlist) ;

ret0:
	return rs ;
} /* end subroutine (procsearch) */

local int loadpathlist(PI *pip,vecstr *plp,vecstr *elp) noex {
	int		rs ;
	cchar		*varpath = VARPATH ;
	cchar		*pp ;
	if ((rs = vecstr_search(elp,varpath,vstrkeycmp,&pp)) >= 0) {
	    rs = SR_NOENT ;
	    if (cchar *(tp = strchr(pp,'=') ; tp) {
	        rs = vecstr_addpathclean(plp,(tp + 1)) ;
	    } /* end if (path-value) */
	} /* end if (vecstr_sæarch) */
	return rs ;
} /* end subroutine (loadpathlist) */


