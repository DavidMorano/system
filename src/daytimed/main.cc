/* main (daytimed) */
/* lang=C20 */


#include	<envstandards.h>
#include	<sys/types.h>
#include	<time.h>
#include	<cstdlib>
#include	<string.h>
#include	<cstdio>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"nistinfo.h"
#include	"config.h"


/* local defines */


/* external subroutines */

extern char	*timestr_nist(time_t,struct nistinfo *,char *) ;


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	struct nistinfo	ni ;
	const time_t	dt = time(nullptr) ;
	cchar		*orgp ;
	cchar		*cp ;
	char		ntbuf[NISTINFO_BUFLEN+ 1] ;

	if ((orgp = getenv(VARORGANIZATION)) == nullptr) {
		orgp = ORGANIZATION ;
	}

	memset(&ni,0,sizeof(struct nistinfo)) ;

	sncpy1(ni.org,NISTINFO_ORGSIZE,orgp) ;

	if ((cp = timestr_nist(dt,&ni,ntbuf)) != nullptr) {
	    fprintf(stdout,"%s\n",ntbuf) ;
	}

	return 0 ;
}
/* end subroutine (main) */


