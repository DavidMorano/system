/* mailmsg_envaddrfold SUPPORT */
/* lang=C++20 */

/* MAILMSG create a folded envelope address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailmsg_envaddrfold

	Description:
	This subroutine creates a folded envelope address from the
	various envelope components.

	Synopsis:
	int mailmsg_envaddrfold(MAILMSG *mmp,char *rbuf,int rlen)

	Arguments:
	mmp		pointer to MAILMSG object
	rbuf		buffer to receive result
	rlen		length of result buffer

	Returns:
	>=0		length of result (in bytes)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mailmsg.h>
#include	<emainfo.h>
#include	<sbuf.h>
#include	<localmisc.h>

#include	"mailmsg_enver.h"


/* local defines */

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
#endif

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(2048,LINE_MAX)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	HDRNAMELEN
#define	HDRNAMELEN	80
#endif

#ifndef	MSGLINELEN
#define	MSGLINELEN	(2 * 1024)
#endif

#ifndef	MAXMSGLINELEN
#define	MAXMSGLINELEN	76
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsg_envaddrfold(MAILMSG *mmp,char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if (mmp == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;

	cint		alen = MAILADDRLEN ;
	char		*abuf ;
	if ((rs = uc_malloc((alen+1),&abuf)) >= 0) {
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	        MAILMSG_ENVER	me, *mep = &me ;
	        int		cl ;
	        cchar		*cp ;
	        for (int i = 0 ; mailmsg_enver(mmp,i,mep) >= 0 ; i += 1) {
	            EMAINFO	ai ;
	            int		atype ;
	            if ((mep->r.ep != NULL) && (mep->r.el > 0)) {
	                cp = mep->r.ep ;
	                cl = mep->r.el ;
	                if (c > 0) rs = sbuf_char(&b,'!') ;
	                if (rs >= 0) {
	                    c += 1 ;
	                    rs = sbuf_strw(&b,cp,cl) ;
	                }
	            } /* end if (remote) */
	            if (rs >= 0) {
	                cint	at = EMAINFO_TUUCP ;
	                int	al ;
	                cp = mep->a.ep ;
	                cl = mep->a.el ;
	                atype = emainfo(&ai,cp,cl) ;
	                if ((al = emainfo_mktype(&ai,at,abuf,alen)) > 0) {
	                    if (c > 0) rs = sbuf_char(&b,'!') ;
	                    if (rs >= 0) {
	                        c += 1 ;
	                        rs = sbuf_strw(&b,abuf,al) ;
	                    }
	                }
	            } /* end if (address) */
	            if (rs < 0) break ;
	        } /* end for (looping through envelopes) */
	        rs1 = sbuf_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	    rs1 = uc_free(abuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailmsg_envaddrfold) */


