/* mailmsg_loadfd SUPPORT */
/* lang=C++20 */

/* load a mail-message from a file-descriptor (FD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine loads a mail-message (into the MAILMSG
	object) from a source that consists of a file-descriptor
	(FD).

	Note: At first we skip empty lines until we find a non-empty
	line; afterwards we do not ignore empty lines.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<filebuf.h>
#include	<rmx.h>
#include	<localmisc.h>

#include	"mailmsg.h"


/* local defines */

#ifndef	MAILMSGLINEBUFLEN
#define	MAILMSGLINEBUFLEN	(LINEBUFLEN * 5)
#endif

#define	ISEND(c)	(((c) == '\n') || ((c) == '\r'))


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* global variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsg_loadfd(mailmsg *op,int mfd,off_t fbo) noex {
	cint		bsize = 2048 ;
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	if ((rs = mailmsg_magic(op)) >= 0) {
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
		cint	llen = (rs * MAILMSG_MF) ;
		char	*lbuf{} ;
		if ((rs = uc_malloc((llen+1),&lbuf)) >= 0) {
		    filebuf	b ;
	    	    cchar	*lp = lbuf ;
	            if ((rs = filebuf_start(&b,mfd,fbo,bsize,0)) >= 0) {
			int	line = 0 ;
	                while ((rs = filebuf_readln(&b,lbuf,llen,-1)) > 0) {
	                    int		ll = rs ;
	                    tlen += ll ;
	                    while ((ll > 0) && ISEND(lbuf[0])) {
	                        ll -= 1 ;
		            }
	                    if ((ll > 0) || (line > 0)) {
	                        line += 1 ;
	                        rs = mailmsg_loadline(op,lp,ll) ;
	                    }
	                    if (rs <= 0) break ;
	                } /* end while (reading lines) */
	                rs1 = filebuf_finish(&b) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (filebuf) */
	    	    rs1 = uc_free(lbuf) ;
	    	    if (rs >= 0) rs = rs1 ;
		} /* end if (memory-allocation) */
	    } /* end if (getbufsize) */
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (mailmsg_loadfd) */


