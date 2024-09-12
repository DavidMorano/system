/* dialprog SUPPORT */
/* lang=C++20 */

/* connect to a local program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialprog

	Description:
	This is a dialer to connect to a local program.

	Synopsis:
	int dialprog(cc *fname,int of,mainv argv,mainv envv,int *fd2p) noex

	Arguments:
	fname		program to execute
	of		open-flags
	argv		arguments to program
	envv		environment to program
	fd2p		pointer to integer to receive STDERR descriptor

	Returns:
	>=0		file descriptor to program STDIN and STDOUT
	<0		error (system-return)

	Notes:
	fd2p		if it was supplied, the pointed-to integer
			received a file descriptor to the STDERR

	Important note on debugging:
	One some (maybe many) OS systems, turning on any debugging
	in this subroutine can cause hangs after the |fork(2)|.
	This is due to the famous (infamous) fork-safety problem
	on many UNIX®i.  One UNIX® OS that has fork-safe lib-C
	subroutines (for the greater most part) is Solaris®.  They
	(the Solaris® people) seem to be among the only ones who
	took fork-safety seriously in their OS.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"dial.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	uc_openprogerr(cchar *,int,mainv,mainv,int *) noex ;
}


/* external variables */


/* local structures */


/* forward reference */


/* local variables */


/* exported variables */


/* exported subroutines */

int dialprog(cchar *fname,int of,mainv argv,mainv envv,int *fd2p) noex {
	return uc_openprogerr(fname,of,argv,envv,fd2p) ;
}
/* end subroutine (dialprog) */


