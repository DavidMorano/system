/* proginfo_setent SUPPORT  */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program information */
/* version %I% last-modified %G% */

#define	CF_GETEXECNAME	1		/* use 'getexecname()' */

/* revision history:

	= 1998-03-17, David A­D­ Morano
	I enhanced this somewhat from my previous version.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	proginfo_setent

	Description:
	This group of subroutines help find and set from variables
	for program start-up type functions.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<shellunder.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	<proginfo.hh>
#include	<proginfo_setent.hh>


/* local defines */

#ifndef	PI
#define	PI		proginfo
#endif

#ifndef	NODENAMELEN
#define	NODENAMELEN	256
#endif

#define	NOPROGNAME	"NP"

#ifndef	VAREXECFNAME
#define	VAREXECFNAME	"_EF"
#endif

#ifndef	VARUNDER
#define	VARUNDER	"_"
#endif


/* external subroutines */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int proginfo_setent(PI *pip,cchar **epp,cchar *vap,int val) noex {
	vecstr		*vsp = &pip->stores ;
	int		rs = SR_OK ;
	int		oi = -1 ;
	int		len = 0 ;

	if (pip == NULL) return SR_FAULT ;
	if (epp == NULL) return SR_FAULT ;

	if (*epp != NULL) {
	    oi = vecstr_findaddr(vsp,*epp) ;
	}
	if (vp != NULL) {
	    len = strnlen(vp,vl) ;
	    rs = vecstr_store(vsp,vp,len,epp) ;
	} else {
	    *epp = NULL ;
	}
	if ((rs >= 0) && (oi >= 0)) {
	    vecstr_del(vsp,oi) ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (proginfo_setent) */


