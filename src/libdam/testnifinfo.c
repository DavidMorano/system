/* main SUPPORT */
/* lang=C89 */

/* test the Network-Interaface-Information facility */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	1		/* compile-time debugging */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdio>
#include	<iostream>
#include	<usystem.h>
#include	<nifinfo.h>
#include	<ucmallreg.h>
#include	<strwcpy.h>
#include	<getourenv.h>
#include	<localmisc.h>

#define	VARDEBUGFNAME	"TESTNIFINFO_DEBUGFILE"

extern "C" {
    extern int	sninetaddr(char *,int,int,cchar *) noex ;
}


int main(int argc,mainv argv,mainv envv) {
	FILE		*ofp = stdout ;
	NIFINFO		nif ;

#if	CF_DEBUGS && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		rs ;
	int		rs1 ;
	cchar		*cp ;

#if	CF_DEBUGS
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != NULL) {
	    rs = debugopen(cp) ;
	    debugprintf("main: starting DFD=%d\n",rs) ;
	}
#endif /* CF_DEBUGS */

#if	CF_DEBUGS && CF_DEBUGMALL
	uc_mallset(1) ;
	uc_mallout(&mo_start) ;
#endif

	if ((rs = nifinfo_start(&nif)) >= 0) {
	    NIFINFO_ENT	*ep ;
	    cint	alen = INETX_ADDRSTRLEN ;
	    cchar	*fmt ;
	    char	abuf[INETX_ADDRSTRLEN+1] ;
	    fmt = "IDX AF INTER    ADDR\n" ;
	    fprintf(ofp,fmt) ;
	    for (int i = 0 ; nifinfo_get(&nif,i,&ep) >= 0 ; i += 1) {
	        cint	idx = ep->index ;
	        cint	af = ep->af ;
		cchar	*inter = ep->inter ;
		cvoid	*a = (cvoid *) &ep->addr ;
		fmt = "%3u %2u %-8s %-64s\n" ;
		sninetaddr(abuf,alen,af,a) ;
		fprintf(ofp,fmt,idx,af,inter,abuf) ;
	    } /* end for */
	    rs1 = nifinfo_finish(&nif) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nifinfo) */

#if	CF_DEBUGS && CF_DEBUGMALL
	{
	    uint	mi[12] ;
	    uint	mo ;
	    uint	mdiff ;
	    uc_mallout(&mo) ;
	    mdiff = (mo-mo_start) ;
	    debugprintf("main: final mallout=%u\n",mdiff) ;
	    if (mdiff > 0) {
	        UCMALLREG_CUR	cur ;
	        UCMALLREG_REG	reg ;
	        cint		size = int(10*sizeof(uint)) ;
	        cchar		*ids = "main" ;
	        uc_mallinfo(mi,size) ;
	        debugprintf("main: MIoutnum=%u\n",mi[ucmallreg_outnum]) ;
	        debugprintf("main: MIoutnummax=%u\n",
	            mi[ucmallreg_outnummax]) ;
	        debugprintf("main: MIoutsize=%u\n",mi[ucmallreg_outsize]) ;
	        debugprintf("main: MIoutsizemax=%u\n",
	            mi[ucmallreg_outsizemax]) ;
	        debugprintf("main: MIused=%u\n",mi[ucmallreg_used]) ;
	        debugprintf("main: MIusedmax=%u\n",mi[ucmallreg_usedmax]) ;
	        debugprintf("main: MIunder=%u\n",mi[ucmallreg_under]) ;
	        debugprintf("main: MIover=%u\n",mi[ucmallreg_over]) ;
	        debugprintf("main: MInotalloc=%u\n",mi[ucmallreg_notalloc]) ;
	        debugprintf("main: MInotfree=%u\n",mi[ucmallreg_notfree]) ;
	        if ((rs = ucmallreg_curbegin(&cur)) >= 0) {
	        while ((rs = ucmallreg_curenum(&cur,&reg)) > 0) {
	            debugprintf("main: MIreg.addr=%p\n",reg.addr) ;
	            debugprintf("main: MIreg.size=%u\n",reg.size) ;
	            debugprinthexblock(ids,80,reg.addr,reg.size) ;
	        }
	        rs1 = ucmallreg_curend(&cur) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_mallset(0) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end block */
#endif /* CF_DEBUGMALL */

#if	CF_DEBUGS
	debugclose() ;
#endif

	return 0 ;
}
/* end subroutine (main) */


