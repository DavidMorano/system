/* main SUPPORT */
/* lang=C++20 */

/* return information about clock ticks on the current system */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-09-07, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/times.h>
#include	<sys/time.h>
#include	<climits>
#include	<cstdio>
#include	<usystem>


/* local defines */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	struct tms	u ;
	clock_t	ticks ;
	uint	elapsed, t, days, hours, mins ;
	int	rs ;


	ticks = u_times(&u) ;

	fprintf(stdout,"clockticks=%lu\n",ticks) ;

	elapsed = (ticks / CLK_TCK) ;
	days = elapsed / (24 * 3600) ;
	t = elapsed % (24 * 3600) ;
	hours = t / 3600 ;
	t = t % 3600 ;
	mins = t / 60 ;

	fprintf(stdout,"elapsed days=%u hours=%u mins=%u\n",
		days,hours,mins) ;

	fclose(stdout) ;

	return 0 ;
}
/* end subroutine (main) */




