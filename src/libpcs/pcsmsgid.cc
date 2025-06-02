/* pcsmsgid */

/* create a mail message ID (for PCS) */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine is used to create a mail message ID for PCS programs.

	Synopsis:

	int pcsmsgid(pcsroot,rbuf,rlen)
	const char	pcsroot[] ;
	char		rbuf[] ;
	char		rlen ;

	Arguments:

	pcsroot		PCS program root path
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer

	Returns:

	>=0		length of returned ID
	<0		error in process of creating ID


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<time.h>
#include	<netdb.h>

#include	<usystem.h>
#include	<getnodedomain.h>
#include	<sbuf.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	getnodedomain(char *,char *) ;
extern int	pcsgetserial(const char *) ;


/* external variables */


/* local structures */


/* forward references */

static int mkstr(char *,int,cchar *,cchar *,int) ;


/* local variables */


/* exported subroutines */


int pcsmsgid(cchar *pr,char *rbuf,int rlen)
{
	int		rs ;

	if (pr == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	if ((rs = pcsgetserial(pr)) >= 0) {
	    const int	sn = rs ;
	    char	nn[NODENAMELEN+1] ;
	    char	dn[MAXHOSTNAMELEN+1] ;
	    if ((rs = getnodedomain(nn,dn)) >= 0) {
	        rs = mkstr(rbuf,rlen,dn,nn,sn) ;
	    }
	}

	return rs ;
}
/* end subroutine (pcsmsgid) */


/* local subroutines */


static int mkstr(char *rp,int rl,cchar *dn,cchar *nn,int sn)
{
	SBUF		ubuf ;
	int		rs ;
	if ((rs = sbuf_start(&ubuf,rp,rl)) >= 0) {
	    const uint	tv = (uint) time(NULL) ;
	    const int	pid = getpid() ;
	    const int	nl = strlen(nn) ;
	    int		len ;

	    if (nl > USERNAMELEN) {
	        const int	hid = gethostid() ;
	        sbuf_hexi(&ubuf,hid) ;
	        sbuf_chr(&ubuf,'-') ;
	    } else {
	        sbuf_strw(&ubuf,nn,nl) ;
	    }

	    sbuf_deci(&ubuf,pid) ;

	    sbuf_chr(&ubuf,'.') ;

	    sbuf_hexui(&ubuf,tv) ;

	    sbuf_chr(&ubuf,'.') ;

	    sbuf_deci(&ubuf,sn) ;

	    sbuf_chr(&ubuf,'@') ;

	    sbuf_strw(&ubuf,dn,-1) ;

	    len = sbuf_finish(&ubuf) ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return rs ;
}
/* end subroutine (pcsmsgid_join) */


