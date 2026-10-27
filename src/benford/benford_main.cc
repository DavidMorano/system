/* benford_main SUPPORT (benford) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* test program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Description:
	This is aome (wirdo) program to read data from a file and
	do something with it.  I think that "benford" might be a
	name, but I am not sure.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |ctdec(3u)| */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* local namespaces */

using libu::ctdec ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cint diglen	= DECBUFLEN ;
cint nents	= 10 ;


/* exported variables */


/* exported subroutines */

int main(int,con mainv,con mainv) {
	double		r ;
	int		ex = EXIT_SUCCESS ;
	int		rs ;
	int		p[nents + 1] = {} ;
	int		sum ;
	int		iw ;
	short		sw ;
	char		digbuf[diglen + 1] ;
	char		*cp ;
	while ((rs = fread(&sw,sizeof(short),1,stdin)) > 0) {
		iw = sw ;
		ctdec(digbuf,diglen,iw) ;
		cp = digbuf ;
		while (*cp == '\0') {
			cp += 1 ;
		} /* end while */
		iw = (*cp - '0') & UCHAR_MAX ;
		p[iw] += 1 ;
	} /* end while */
	sum = 0 ;
	for (int i = 0 ; i < nents ; i += 1) {
		sum += p[i] ;
	} /* end for */
	printf("sum=%d\n",sum) ;
	for (int i = 0 ; i < nents ; i += 1) {
		r = ((double) p[i]) / ((double) sum) ;
		printf("c[%d]=%d p[%d]=%6.3f\n",i,p[i],i,r) ;
	} /* end for */
	if ((ex == EXIT_SUCCESS) && (rs < 0)) {
	    ex = EXIT_FAILURE ;
	} /* end if (error) */
	return ex ;
} /* end subroutine (main) */


