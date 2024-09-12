/* testconstdiv SUPPORT */
/* lang=C++20 */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-10-06, David A­D­ Morano
	Updated and enhanced.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdio>
#include	<usystem.h>
#include	<valuelims.hh>
#include	<digbufsizes.hh>
#include	<ucvariables.hh>
#include	<localmisc.h>

#include	"constdiv.hh"
#include	"int_fastdiv.h"


/* forward references */


/* local variables */


/* exported subroutines */

int main(int,cchar **,cchar **) {
	FILE	*ofp = stdout ;
	uint	nums[] = { 2, 11, 10, 16, 21675, 1836482, 1398795416 } ;
	int	ex = 0 ;
	int	rs = SR_OK ;
	int	rs1 ;

	    fprintf(ofp,"ent\n") ;
	    fflush(ofp) ;
	{
	    int_fastdiv	fd(10) ;
	    bool	f_bad = false ;
	    int		i = 0 ;
	    fprintf(ofp,"m=%08x s=%08x\n",fd.M,fd.s) ;
	    fprintf(ofp,"n_add_sign=%u\n",fd.n_add_sign) ;
	    fflush(ofp) ;
	    fprintf(ofp,"loop\n") ;
	    fflush(ofp) ;
	    while ((!f_bad) && (i < nelem(nums))) {
		const uint	n = nums[i] ;
		uint q1 = n/fd ;
		uint q2 = n/10 ;
		f_bad = (q1 != q2) ;
		if (f_bad) break ;
		i += 1 ;
	    } /* end while */
	    fprintf(ofp,"fbad=%u i=%d\n",f_bad,i) ;
	    fprintf(ofp,"div10\n") ;
	    constdiv	div10(10) ;
	    fprintf(ofp,"m=%08x s=%08x\n",div10.m,div10.s) ;
	    fflush(ofp) ;
	    fprintf(ofp,"loop\n") ;
	    fflush(ofp) ;
	    while ((!f_bad) && (i < nelem(nums))) {
		const uint	n = nums[i] ;
		uint q1 = div10(n) ;
		uint q2 = n/10 ;
		f_bad = (q1 != q2) ;
		if (f_bad) break ;
		i += 1 ;
	    } /* end while */
	    fprintf(ofp,"fbad=%u i=%d\n",f_bad,i) ;
 	    if (f_bad) ex = 1 ;
	} /* end block */

	{
	    constexpr uint	d = 100 ;
	    constdiv	div100(100) ;
	    bdduool	f_bad = false ;
	    fprintf(ofp,"m=%08x s=%08x\n",div100.m,div100.s) ;
	    for (int i = 0 ; ((!f_bad) && (i < nelem(nums)) ; i += 1) {
		const uint	n = nums[i] ;
		uint q1 = div100(n) ;
		uint q2 = n/d ;
		f_bad = (q1 != q2) ;
		if (f_bad) break ;
		i += 1 ;
	    } /* end while */
	    fprintf(ofp,"fbad=%u i=%d\n",f_bad,i) ;
 	    if (f_bad) ex = 1 ;
	}

	return ex ;
}
/* end subroutine (main) */


