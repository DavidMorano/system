/* progpeername SUPPORT */
/* lang=C++20 */

/* try to divine a peer-hostname from a socket address */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-06-23, David A­D­ Morano
	This is split off from the 'progwatch()' module.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	progpeername

	Description:
	This subroutine tries to divine a hostname associated with
	a socket address.

	Synopsis:
	int progpeername(proginfo *pip,clientinfo *cip,char *peername) noex

	Arguments:
	pip		program information pointer
	cip		client information pointer
	peername	caller-specified buffer to receive result

	Returns:
	>=0		length of resulting hostname
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<sockaddress.h>
#include	<connection.h>
#include	<clientinfo.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"proginfo.h"


/* local defines */

#ifndef	LOCALHOST
#define	LOCALHOST	"localhost"
#endif

#ifndef	VARNLSADDR
#define	VARNLSADDR	"NLSADDR"
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	procsocket(proginfo *,clientinfo *,char *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progpeername(proginfo *pip,clientinfo *cip,char *dp,int dl) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		rs1 = 0 ;
	int		f_success = false ;
	cchar		*cp ;

	if (cip == NULL) return SR_FAULT ;
	if (dp == NULL) return SR_FAULT ;

	dp[0] = '\0' ;

/* pipe? */

	if ((rs >= 0) && (dp[0] == '\0')) {
	    rs = u_fstat(cip->fd_input,&sb) ;
	    if ((rs >= 0) && S_ISFIFO(sb.st_mode)) {
	        cip->f_local = TRUE ;
	        rs = sncpy1(dp,dl,LOCALHOST) ;
	    }
	}

/* use 'connection(3dam)' */

	if ((rs >= 0) && (dp[0] == '\0')) {
	    rs = procsocket(pip,cip,dp,-1) ;
	    if ((rs >= 0) && (dp[0] == '/'))
		cip->f_local = TRUE ;
	}

/* use 'nlspeername(3dam)' */

	if ((rs >= 0) && (dp[0] == '\0')) {
	    if ((cp = getenv(VARNLSADDR)) != NULL) {
	        rs1 = nlspeername(cp,pip->domainname,dp,dl) ;
		if (rs1 < 0) dp[0] = '\0' ;
	    }
	} /* end if */

/* done */

	if ((rs >= 0) && (dp[0] != '\0')) {
	    f_success = TRUE ;
	    rs = vecstr_store(&cip->stores,dp,-1,&cp) ;
	    if (rs >= 0) cip->peername = cp ;
	}

	return (rs >= 0) ? f_success : rs ;
}
/* end subroutines (progpeername) */


/* local subroutines */

static int procsocket(proginfo *pip,clientinfo *cip,char *dp,int dl) noex {
	connection	conn, *cnp = &conn ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	peername[0] = '\0' ;
	if ((rs = connection_start(cnp,pip->domainname)) >= 0) {
	    if (cip->salen > 0) {
	        sockaddress	*sap = &cip->sa ;
	        cint		sal = cip->salen ;
	        rs = connection_peername(cnp,sap,sal,dp,dl) ;
	    } else {
	        int		ifd = cip->fd_input ;
	        rs = connection_sockremname(cnp,dp,dl,ifd) ;
	    }
	    rs1 = connection_finish(&conn) ;
	    if (rs >= 0) rs = rs1 ;
	    f = (dp[0] != '\0') ;
	} /* end if (connection) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (procsocket) */


