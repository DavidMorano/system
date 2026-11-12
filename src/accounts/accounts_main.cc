/* main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* whole program for ACCOUNTS */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Description:

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pwd.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<maxex.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


/* external subroutines */

extern PASSWD		*getpwent() noex ;


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
	PASSWD		*pp ;
	int		ex = EXIT_SUCCESS ;
	int		rs = SR_OK ;
	setpwent() ;
	while ((pp = getpwent()) != nullptr) {
		fprintf(stdout,"%-8s %5d %5d (%s) %s %s\n",
			pp->pw_name,pp->pw_uid,pp->pw_gid,
			pp->pw_gecos,pp->pw_dir,pp->pw_shell) ;
	} /* end while */
	endpwent() ;
	return ex ;
} /* end subroutine (main) */


