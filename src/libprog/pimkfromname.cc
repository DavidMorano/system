/* pimkfromname SUPPORT */
/* lang=C++20 */

/* create a FROM-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pimkfromname

	Description:
	This subroutine creates FROM-name for use in making mail
	addresses (always FROM-type addresses).

	Synopsis:
	int pimkfromname(PROGINFO *pip) noex

	Arguments:
	pip		pointer to program information

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>
#include	<usystem.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	REALNAMELEN
#define	REALNAMELEN	100
#endif


/* external subroutines */

extern "C" {
    extern int	pcsfullname(cchar *,char *,int,cchar *) noex ;
    extern int	pcsname(cchar *,char *,int,cchar *) noex ;
}


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int pimkfromname(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	int		nbl = 0 ;
	if (! pip->fl.init_fromname) {
	    cint	nlen = REALNAMELEN ;
	    cchar	*nbp ;
	    char	nbuf[REALNAMELEN + 1] ;
	    pip->fl.init_fromname = TRUE ;
/* for a real-name: first try the PCS facility (we are a PCS program!) */
	    nbp = nbuf ;
	    rs = pcsfullname(pip->pr,nbuf,nlen,pip->username) ;
	    nbl = rs ;
	    if (isNotPresent(rs)) {
	        nbp = nbuf ;
	        rs = pcsname(pip->pr,nbuf,nlen,pip->username) ;
	        nbl = rs ;
	    }
	    if (isNotPresent(rs)) {
	        rs = SR_OK ;
	        nbuf[0] = '\0' ;
	    }
	    if (rs >= 0) {
/* try USERINFO-derived possibilities */
	        if ((nbp == NULL) || (nbp[0] == '\0')) {
	            nbp = pip->fullname ;
	            nbl = -1 ;
	        }
	        if ((nbp == NULL) || (nbp[0] == '\0')) {
	            nbp = pip->name ;
	            nbl = -1 ;
	        }
	        if ((nbp != NULL) && (nbp[0] != '\0')) {
		    cchar	**vpp = &pip->fromname ;
	            rs = proginfo_setentry(pip,vpp,nbp,nbl) ;
	        }
	    } /* end if (ok) */
	} else {
	    if (pip->fromname != NULL) {
	        nbl = strlen(pip->fromname) ;
	    }
	}
	return (rs >= 0) ? nbl : rs ;
}
/* end subroutine (mkfromname) */


