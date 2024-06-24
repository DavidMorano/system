/* main SUPPORT (erlang) */
/* lang=C++20 */

/* some sort of tersting? */
/* version %I% last-modified %G% */


/* revistion history :

	= August 1988, David A­D­ Morano
	Soe test frameword.

*/

/*******************************************************************************

	Oh.  Who knows why I created this (crap)?

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<fcntl.h>
#include	<math.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<bfile.h>
#include	<localmisc.h>


/* local defines */

#define		DATA_EOF	0x1A

#define		BUFLEN		0x1000
#define		LINELEN		100

#define		TERMTIMEOUT	10L


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static long factorial(int n) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv,mainv) {
	bfile		error, *efp = &error ;
	bfile		output, *ofp = &output ;
	double	a = 3.0 ;
	double	pb, pb_n, pb_d ;
	double	pq ;
	double	roe = (4.0 / 5.0) ;
	double	p0, p0_a, p0_b ;
	double	nq, dq ;
	double	lam = 0.1 ;
	long	i, n, m = 5 ;
	int		rs ;


	rs = bopen(efp,BFILE_STDERR,"dwca",0666) ;


	rs = bopen(ofp,BFILE_STDOUT,"dwct",0666) ;

	if (rs < 0) goto no_open ;

/* get the values */


	p0_a = 0.0 ;
	for (n = 0 ; n < m ; n += 1)
		p0_a += (pow((m * roe),(double) n) / ((double) factorial(n))) ;

	p0_b = 
	  pow((m * roe),(double) m) / ((double) factorial(m)) / (1 - roe) ;

	p0 = 1.0 / (p0_a + p0_b) ;

	pq = p0 * pow((m * roe),(double) m) / ((double) factorial(m)) /
		(1.0 - roe) ;
	
	nq = pq * roe / (1.0 - roe) ;

	dq = nq / lam ;


	bprintf(ofp,"dq=%e\n",dq) ;


exit:
	bclose(ofp) ;

	bclose(efp) ;

	return OK ;

no_open:
	bprintf(efp,"can't open output file %d\n",rs) ;

bad_exit:
	bclose(efp) ;

	return BAD ;
}
/* end subroutine (main) */


/* local subroutines */

static long factorial(int n) noex {
	long		res = 1 ;
	if (n > 1) {
	    res = (n * factorial(n - 1)) ;
	}
	return res ;
}


