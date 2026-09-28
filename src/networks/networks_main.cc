/* networkds_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program to access network information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This program prints out all of the service entries in the
	system 'services' database.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int,con mainv,con mainv) {
    	cnullptr	np{} ;
	in_addr		a4 ;
	int		ex = EXIT_SUCCESS ;
	char		abuf[INET_ADDRSTRLEN + 1] ;
	setnetent(1) ;
	for (netent *nep ; (nep = getnetent()) != np ; ) {
	    cchar	*cp ;
	    cchar	*ap ;
	    if (nep->n_addrtype == AF_INET) {
	        a4 = inet_makeaddr(nep->n_net,0) ;
	        cp = inet_ntoa(a4) ;
	    } else {
	        ap = (char *) &nep->n_net ;
	        cp = inet_ntop(nep->n_addrtype,ap, abuf,INET6_ADDRSTRLEN) ;
	    }
	    if (isnull(cp)) {
		cp = "*invalid*" ;
	    }
	    fprintf(stdout,"%-16s %-15s", nep->n_name,cp) ;
	    if (nep->n_aliases != np) {
	        for (int i = 0 ; nep->n_aliases[i] ; i += 1) {
	            fprintf(stdout," %-16s",nep->n_aliases[i]) ;
	        } /* end for */
	    } /* end if (aliases) */
	    fprintf(stdout,"\n") ;
	} /* end while */
	endnetent() ;
	return ex ;
} /* end subroutine (main) */


