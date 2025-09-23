/* pimkreplyaddr SUPPORT */
/* lang=C++20 */

/* create a mail REPLYTO-address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pimkreplyaddr

	Description:
	This subroutine creates a mail REPLYTO-address (if we can).

	Synopsis:
	int pimkreplyaddr(PROGINFO *pip) noex

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


/* external subroutines */

extern "C" {
    extern int	pimkfromname(PROGINFO *) noex ;
}


/* local structures */


/* forward references */

static int	piloadreply(PROGINFO *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pimkreplyaddr(PROGINFO *pip) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

	if (! pip->fl.init_replyto) {
	    pip->fl.init_replyto = true ;
	    if (pip->hdr_replyto == nullptr) {
	        if (pip->hdr_replyto == nullptr) {
		    cchar	*cp = nullptr ;
	            if ((cp = getenv(VARMAILREPLY)) != nullptr) {
	                if (cp[0] != '\0') {
	                    pip->hdr_replyto = cp ;
			    rl = strlen(cp) ;
	                }
	            }
	        }
	        if (pip->hdr_replyto == nullptr) {
	            rs = piloadreply(pip) ;
	 	    rl = rs ;
	        }
	    } /* end if (more needed) */
	} else {
	    if (pip->hdr_replyto != nullptr) {
		rl = strlen(pip->hdr_replyto) ;
	    }
	} /* end if (needed) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pimkreplyaddr) */


/* local subroutines */

static int piloadreply(PROGINFO *pip) noex {
	buffer		b ;
	int		rs ;
	int		rs1 ;
	int		bl = 0 ;
	if ((rs = buffer_start(&b,MABUFLEN)) >= 0) {

	    buffer_strw(&b,pip->username,-1) ;
	    buffer_chr(&b,'@') ;
	    buffer_strw(&b,pip->domainname,-1) ;

/* add a name if we can find one */

	    if (! pip->fl.init_fromname) {
	        rs = mkfromname(pip) ;
	    }

	    if ((rs >= 0) && (pip->fromname != nullptr)) {
	        buffer_chr(&b,' ') ;
	        buffer_chr(&b,CH_LPAREN) ;
	        buffer_strw(&b,pip->fromname,-1) ;
	        buffer_chr(&b,CH_RPAREN) ;
	    } /* end if (adding name) */

	    if (rs >= 0) {
	        cchar	*bp ;
	        if ((rs = buffer_get(&b,&bp)) >= 0) {
	            cchar	**vpp = &pip->hdr_replyto ;
	            bl = rs ;
	            rs = proginfo_setentry(pip,vpp,bp,bl) ;
	        }
	    } /* end if (ok) */

	    rs1 = buffer_finish(&b) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer) */
	return (rs >= 0) ? bl : rs ;
}
/* end subroutine (piloadreply) */


