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
#include	<iostream>
#include	<usystem.h>
#include	<valuelims.hh>
#include	<ucvariables.hh>
#include	<localmisc.h>
#include	<cfdec.h>
#include	<cfchars.h>
#include	<ctdec.h>
#include	<ctchars.h>


/* local defines */


/* name spaces */

using namespace std ;


/* forward references */


/* local variables */

static const char	*nums[] = {
	"2",
	"11",
	"10",
	"16",
	"21675",
	"1836482",
	"1398795416",
	nullptr
} ;


/* exported subroutines */

int main(int,cchar **,cchar **) {
	FILE		*ofp = stdout ;
	cint		linelen = LINEBUFLEN ;
	int		ex = 0 ;
	int		rs = SR_OK ;
	int		rs1 [[maybe_unused]] ;
	const char	*w ;
	char		linebuf[LINEBUFLEN+1] ;

	for (int i = 0 ; (rs >= 0) && ((w = nums[i])) ; i += 1) {
	    int		v ;
	    fprintf(ofp,"w=%s\n",w) ;
	    if ((rs = cfdeci(w,-1,&v)) >= 0) {
		if ((rs = ctdeci(linebuf,linelen,v)) >= 0) {
		    cout << linebuf ;
		}
	    }
	} /* end for */
	if (rs < 0) ex = 1 ;

	return ex ;
}
/* end subroutine (main) */


