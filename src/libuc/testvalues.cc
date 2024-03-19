/* testvalues SUPPORT */
/* lang=C++20 */

/* test the |valuelimits| and |digbufsizes| objects */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-10-06, David A­D­ Morano
	Updated and enhanced.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstdlib>
#include	<cstdio>
#include	<usystem.h>
#include	<valuelims.hh>
#include	<digbufsizes.hh>
#include	<ucvariables.hh>
#include	<localmisc.h>


/* forward references */


/* local variables */


/* exported subroutines */

/* ARGSUSED */
int main(int,cchar **,cchar **) {
	FILE		*ofp = stdout ;
	int		ex = 0 ;
	[[maybe_unused]] int rs = SR_OK ;
	[[maybe_unused]] int rs1 ;

	if (valuelimit.llmax) ex = 1 ;

	fprintf(ofp,"smin=%04hX\n",SHORT_MIN) ;
	fprintf(ofp,"imax=%d\n",valuelimit.imax) ;
	{
	    void	*v = ((void *) -1) ;
	    fprintf(ofp,"v=%p\n",v) ;
	}

	{
	    cint	bases[] = { 2, 10, 16, 26, 32 } ;
	    {
		int	n = nelem(bases) ;
	        for (int i = 0 ; i < n ; i += 1) {
	            cint	b = bases[i] ;
		    fprintf(ofp,"b=%2d",b) ;
	            for (int t = 0 ; t < digbufsizes_ntypes ; t += 1) {
		        fprintf(ofp," %4d",digbufsize.bufsize[t][b]) ;
	            }
		    fprintf(ofp,"\n") ;
	        } /* end for */
	    }
	} /* end block */

	return ex ;
}
/* end subroutine (main) */


