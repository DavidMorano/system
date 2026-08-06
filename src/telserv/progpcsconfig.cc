/* progpcsconf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process any PCS configuration information */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable print-outs */
#define	CF_DEBUG	0		/* switchable print-outs */
#define	CF_PCSCONF	0		/* compile in PCSCONF */

/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was borrowed and modified from previous generic
	front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Descrpition:
	This subroutine processes any PCS configuration information
	that may be available for the current program (every program
	may have some different possible PCS information available
	for it).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>
#include	<pcsconf.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	REALNAMELEN
#define	REALNAMELEN	100
#endif

#ifndef	BUFLEN
#define	BUFLEN		((8 * 1024) + REALNAMELEN)
#endif

#ifndef	ARCHBUFLEN
#define	ARCHBUFLEN	80
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	procsets(struct proginfo *,vecstr *) ;
static int	matme(cchar *,cchar *,cchar **,cchar **) ;


/* local variables */

enum pcskeys {
	pcskey_timestamp,
	pcskey_pollint,
	pcskey_overlast
} ;

constexpr cpcchar	pcskeys[] = {
	"timestamp",
	"pollint",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

#if	CF_PCSCONF

int progpcsconf(PROGINFO *pip)
{
	vecstr		sets ;
	int		rs = SR_OK ;
	int		rs1 ;

	if ((rs = vecstr_start(&sets,10,0)) >= 0) {
	    PCSCONF	pc ;
	    const int	plen = PCSCONF_LEN ;
	    char	pbuf[PCSCONF_LEN + 1] ;
	    cchar	*pr = pip->pr ;
	    if ((rs1 = pcsconf(cp,nullptr,&pc,&sets,nullptr,pbuf,plen)) >= 0) {
	       rs = procsets(pip,&sets) ;
	    }
	    vecstr_finish(&sets) ;
	} /* end if (vecstr) */

	return rs ;
} 
/* end subroutine (progpcsconf) */

#else /* CF_PCSCONF */

int progpcsconf(PROGINFO *pip)
{
	if (pip == nullptr) return SR_FAULT ;
	return SR_OK ;
} 
/* end subroutine (progpcsconf) */

#endif /* CF_PCSCONF */


/* local subroutines */


/* get any of my values from the main configuration */
static int procsets(PROGINFO *pip,vecstr *slp)
{
	int		rs = SR_OK ;
	int		i ;
	int		kl, cl ;
	int		v ;
	cchar	*kp, *vp ;
	cchar	*sp, *cp ;

	if (slp == nullptr) return SR_FAULT ;

	for (i = 0 ; vecstr_get(slp,i,&sp) >= 0 ; i += 1) {
	    if (sp == nullptr) continue ;

	    if ((kl = matme(pip->searchname,sp,&kp,&vp)) < 0)
	        continue ;

	    i = matostr(pcskeys,2,kp,kl) ;

	    if (i >= 0) {

	        switch (i) {

	        case pcskey_timestamp:
		    if (pip->finval.stampfname) {
			pip->have.stampfname = TRUE ;
	                cl = sfshrink(vp,-1,&cp) ;
	                if (cl > 0)
	                    rs = proginfo_setentry(pip,&pip->stampfname,cp,cl) ;
		    }
	            break ;

	        case pcskey_pollint:
		    if (pip->finval.intpoll) {
			pip->have.intpoll = TRUE ;
	                cl = sfshrink(vp,-1,&cp) ;
	                if (cl)
	                    if (cfdecti(cp,cl,&v) >= 0)
	                        pip->intpoll = v ;
	            }
	            break ;

	        } /* end switch */

	    } /* end if */

	} /* end for */

	return rs ;
}
/* end subroutine (procsets) */


/* does a key match my search name? */
static int matme(key,ts,kpp,vpp)
cchar	key[] ;
cchar	ts[] ;
cchar	**kpp, **vpp ;
{
	char		*cp2, *cp3 ;

	if ((cp2 = strchr(ts,'=')) == nullptr)
	    return -1 ;

	if (vpp != nullptr)
	    *vpp = cp2 + 1 ;

	if ((cp3 = strchr(ts,':')) == nullptr)
	    return -1 ;

	if (cp3 > cp2)
	    return -1 ;

	if (kpp != nullptr)
	    *kpp = cp3 + 1 ;

	if (strncmp(ts,key,(cp3 - ts)) != 0)
	    return -1 ;

	return (cp2 - (cp3 + 1)) ;
}
/* end subroutine (matme) */


