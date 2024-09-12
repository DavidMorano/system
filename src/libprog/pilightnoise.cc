/* pilightnoise SUPPORT */
/* lang=C20 */

/* gather light noise */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-03, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	pilightnoise

	Desxciption:
	Gather light noise.

	Synopsis:
	int lightnoise(PROGINFO *pip,...) noex

	Arguments:
	pip		PROGINFO object pointer
	-

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sbuf.h>
#include	<userinfo.h>
#include	<hash.h>
#include	<cfdec.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"proginfo.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int pilightnoise(PROGINFO *pip,USERINFO *uip,cchar *ofname) noex {
	TIMEVAL		tod ;
	sbuf		hb ;
	uint		v = 0 ;
	uint		hv = 0 ;
	cint		dlen = BUFLEN ;
	int		rs ;
	int		i ;
	int		bl = 0 ;
	char		dbuf[BUFLEN + 1] ;

	if ((rs = sbuf_start(&hb,dbuf,dlen)) >= 0) {
	    pid_t	pid_parent = getppid() ;

	    if (uip->username)
	        sbuf_strw(&hb,uip->username,-1) ;

	    if (uip->homedname)
	        sbuf_strw(&hb,uip->homedname,-1) ;

	    if (uip->nodename)
	        sbuf_strw(&hb,uip->nodename,-1) ;

	    if (uip->domainname)
	        sbuf_strw(&hb,uip->domainname,-1) ;

	    sbuf_deci(&hb,int(uip->pid)) ;

	    sbuf_decl(&hb,long(pip->daytime)) ;

	    if (ofname)
	        sbuf_strw(&hb,ofname,-1) ;

	    sbuf_deci(&hb,int(pid_parent)) ;

	    uc_gettimeofday(&tod,nullptr) ;

	    sbuf_buf(&hb,(char *) &tod,sizeof(TIMEVAL)) ;

	    bl = sbuf_finish(&hb) ;
	    if (rs >= 0) rs = bl ;
	} /* end if (sbuf) */

	    hv = hash_elf(dbuf,bl) ;

/* pop in our environment also! */

	for (i = 0 ; pip->envv[i] ; i += 1) {
	    v ^= hash_elf(pip->envv[i],-1) ;
	}
	hv ^= v ;

#ifdef	COMMENT
	if ((cp = getenv("RANDOM")) != NULL) {
	    if (cfdecui(cp,-1,&v) >= 0)
	        hv ^= v ;
	}
#endif /* COMMENT */

	pip->hv ;
	return rs ;
}
/* end subroutine (pilightnoise) */


