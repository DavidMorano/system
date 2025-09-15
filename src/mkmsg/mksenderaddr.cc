/* pimksenderaddr SUPPORT */
/* lang=C++20 */

/* create a mail SENDER-address */
/* version %I% last-modified %G% */

#define	CF_SENDERNAME	1		/* add a sender name */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pimksenderaddr

	Description:
	This subroutine creates a mail SENDER-address (if we can).

	Synopsis:
	int pimksenderaddr(PROGINFO *pip) noex

	Arguments:
	pip		pointer to program information

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<buffer.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	MABUFLEN
#define	MABUFLEN	((3 * MAXHOSTNAMELEN) + 1024)
#endif

#ifndef	REALNAMELEN
#define	REALNAMELEN	100
#endif

#ifndef	VARMAILFROM
#define	VARMAILFROM	"MAILFROM"
#endif

#ifndef	VARMAILREPLY
#define	VARMAILREPLY	"MAILREPLY"
#endif

#ifndef	VARMAILSENDER
#define	VARMAILSENDER	"MAILSENDER"
#endif


/* external subroutines */

extern "C" {
    extern int	pimkfromname(PROGINFO *) noex ;
}


/* local structures */


/* forward references */

static int	piloadsender(PROGINFO *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pimksenderaddr(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;
	if (! pip->fl.init_sender) {
	    pip->fl.init_sender = TRUE ;
	    if (pip->hdr_sender == NULL) {
	        cchar	*cp = NULL ;
	        if ((cp = getenv(VARMAILSENDER)) != NULL) {
	            if (cp[0] != '\0') {
	                pip->hdr_sender = cp ;
			rl = strlen(pip->hdr_sender) ;
	            }
	        }
	        if (pip->hdr_sender == NULL) {
	            rs = piloadsender(pip) ;
		    rl = rs ;
	        }
	    }
	} else {
	    if (pip->hdr_sender != NULL) {
		rl = strlen(pip->hdr_sender) ;
	    }
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pimksenderaddr) */


/* local subroutines */

static int piloadsender(PROGINFO *pip) noex {
	buffer		b ;
	int		rs ;
	int		rs1 ;
	int		bl = 0 ;
	if ((rs = buffer_start(&b,MABUFLEN)) >= 0) {

	    buffer_strw(&b,pip->username,-1) ;
	    buffer_chr(&b,'@') ;
	    buffer_strw(&b,pip->domainname,-1) ;

/* add a name if we can find one */

#if	CF_SENDERNAME
	    if ((rs = mkfromname(pip)) >= 0) {
	        if (pip->fromname != NULL) {
	            buffer_chr(&b,' ') ;
	            buffer_chr(&b,CH_LPAREN) ;
	            buffer_strw(&b,pip->fromname,-1) ;
	            buffer_chr(&b,CH_RPAREN) ;
	        } /* end if (adding name) */
	    } /* end if 9mkfromname) */
#endif /* CF_SENDERNAME */

	    if (rs >= 0) {
	        cchar	*bp ;
	        if ((rs = buffer_get(&b,&bp)) >= 0) {
	            const char	**vpp = &pip->hdr_sender ;
	            bl = rs ;
	            rs = proginfo_setentry(pip,vpp,bp,bl) ;
	        }
	    }

	    rs1 = buffer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (piloadsender) */


