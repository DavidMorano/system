/* pimkmailbox SUPPORT */
/* lang=C++20 */

/* make a mailbox file */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-01-23, David A­D­ Morano
	This subroutine was copied and modified from the VMAIL
	original.  A variety of enhancements were made to prevent
	it from crashing due to short buffers. No checks were being
	made about whether a copy into a buffer was overflowing!
	Yes, this was one of the causes of the spread of the 1988
	Internet worm. Of course, nobody likes a program that crashes
	either (including myself). It was the crashing of this (and
	other) programs that lead me to fix this crap up in the
	first place!

	= 1996-06-18, David A­D­ Morano
	I did:
		- remove old mail spool locks

	= 1996-07-24, David A­D­ Morano
	I rewrote the "getnewmail" subroutine in part to :
		- lock user's "new" mailbox when fetching new mail
		  NOTE: This has to be removed when real proper
			mailbox handling is implemented.
		- guard against corrupted "new" mailbox on new mail
		  from file system full
		- added full binary compatibility for new mail

	= 2007-11-13, David A­D­ Morano
	Oh man! How long have I been toiling with this thing? I
	added the ability to grab mail from multiple users. I also
	rewrote from the original (before I started cleaning up
	this crap in 1994) much of the way that this process takes
	place.

*/

/* Copyright © 1994,1996,2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pimkmailbox

	Description:
	This subroutine creates (makes) a mailbox file.

	Synopsis:
	int pimkmailbox(PROGINFO *pip,cchar *mbname) noex

	Arguments:
	pip		program information pointer
	mbname		mailbox-name to setup

	Returns:
	>=0	created
	<0	failed (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	O_FLAGS		(O_WRONLY | O_CREAT)


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local (forward) subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int pimkmailbox(PROGINFO *pip,cchar *mbname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (pip && mbname) {
	    cchar	*mfname = pip->foldername ;
	    if (mfname) {
		rs = SR_INVALID ;
	        if (mbname[0] && mfname[0]) {
	            char	*mbfname ;
		    if ((rs = malloc_mn(&mbfname)) >= 0) {
	                if ((rs = mkpath2(mbfname,mfname,mbname)) >= 0) {
	                    USTAT	sb ;
			    rl = rs ;
	                    rs = u_stat(mbfname,&sb) ;
	                    if (rs < 0) {
				cint	of = O_FLAGS ;
				cmode	om = 0664 ;
	                        if ((rs = u_open(mbfname,of,om)) >= 0) {
	                            cint	fd = rs ;
	                            if (pip->gid_mail > 0) {
	                                u_fchown(fd,-1,pip->gid_mail) ;
		                    }
	                            u_close(fd) ;
	                        } /* end if (opened) */
	                    } /* end if (file did not exist) */
	                } /* end if (mkpath) */
			rs1 = uc_free(mbfname) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pimkmailbox) */


