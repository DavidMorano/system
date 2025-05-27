/* main (date) */
/* lang=C20 */


/* revision history:

	= 2017-09-07, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<time.h>
#include	<cstdio>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


int main(int,mainv,mainv) {
	time_t		daytime = time(nullptr) ;
	char		timebuf[TIMEBUFLEN + 1] ;
	timestr_log(daytime,timebuf) ;
	fprintf(stdout,"%s\n",timebuf) ;
	return 0 ;
}
/* end subroutine (main) */


