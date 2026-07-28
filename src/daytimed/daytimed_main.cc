/* daytimed_main SUPPORT (daytimed) */
/* charset=ISO8859-1 */
/* lang=C++20 */



#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIXO */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sncpyx.h>		/* LIBUC */
#include	<nistinfo.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"config.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
	nistinfo	ni = {} ;
	custime		dt = time(nullptr) ;
	cchar		*orgp ;
	char		ntbuf[NISTINFO_BUFLEN+ 1] ;

	if ((orgp = getenv(VARORGANIZATION)) == nullptr) {
	    orgp = ORGANIZATION ;
	}

	sncpy1(ni.org,NISTINFO_ORGSIZE,orgp) ;

	if (cchar *cp = timestr_nist(dt,ntbuf,&ni) ; cp) {
	    fprintf(stdout,"%s\n",ntbuf) ;
	}

	return 0 ;
} /* end subroutine (main) */


