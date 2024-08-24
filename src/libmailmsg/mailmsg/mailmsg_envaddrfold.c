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
	int mailmsg_envaddrfold(MAILMSG *op,char *rbuf,int rlen)

	Arguments:
	op		pointer to MAILMSG object
	rbuf		buffer to receive result
	rlen		length of result buffer

	Returns:
	>=0		length of result (in bytes)
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<emainfo.h>
#include	<sbuf.h>
#include	<localmisc.h>

#include	"mailmsg.h"
#include	"mailmsg_envget.h"


/* local defines */

#ifndef	MAILADDRLEN
#define	MAILADDRLEN	(3 * MAXHOSTNAMELEN)
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

typedef mailmsg			mm ;
typedef mailmsg_envdat		dat ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int mailmsg_ema(mm *,dat *,char *,int,sbuf *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsg_envaddrfold(mailmsg *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mailmsg_magic(op,rbuf)) >= 0) {
	    cint	alen = MAILADDRLEN ;
	    char	*abuf ;
	    if ((rs = uc_malloc((alen+1),&abuf)) >= 0) {
	        sbuf	b ;
	        if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	            mailmsg_envdat	me, *mep = &me ;
		    auto		mef = mailmsg_envget ;
	            int			cl ;
	            cchar		*cp ;
	            for (int i = 0 ; mef(op,i,mep) >= 0 ; i += 1) {
	                if ((mep->r.ep != NULL) && (mep->r.el > 0)) {
	                    cp = mep->r.ep ;
	                    cl = mep->r.el ;
	                    if (c > 0) rs = sbuf_chr(&b,'!') ;
	                    if (rs >= 0) {
	                        c += 1 ;
	                        rs = sbuf_strw(&b,cp,cl) ;
	                    }
	                } /* end if (remote) */
	                if (rs >= 0) {
			    rs = mailmsg_ema(op,mep,abuf,alen,&b) ;
			    c += rs ;
	                } /* end if (address) */
	                if (rs < 0) break ;
	            } /* end for (looping through envelopes) */
	            rs1 = sbuf_finish(&b) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	        rs1 = uc_free(abuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailmsg_envaddrfold) */


/* local subroutines */

static int mailmsg_ema(mm *op,dat *mep,char *abuf,int alen,sbuf *sbp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && mep && abuf && sbp) {
	    emainfo	ai ;
	    int		cl = mep->a.el ;
	    cchar	*cp = mep->a.ep ;
	    if ((rs = emainfo_load(&ai,cp,cl)) >= 0) {
	        cint	at = rs ;
	        if ((rs = emainfo_mktype(&ai,at,abuf,alen)) > 0) {
		    cint	al = rs ;
		    if (c > 0) rs = sbuf_chr(sbp,'!') ;
		    if (rs >= 0) {
		        c += 1 ;
		        rs = sbuf_strw(sbp,abuf,al) ;
		    } /* end if (ok) */
	        } /* end if */
	    } /* end if (emainfo_load) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailmsg_ema) */


