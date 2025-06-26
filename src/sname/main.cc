/* main (sname) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program to EXEC a program with the named INPUT file and ARG0 */
/* version %I% last-modified %G% */


/* revision history:

	= 1990-11-00, David A-D- Morano
	This program was originally written.

	= 1997-01-00, David A-D- Morano
	I fixed a potentially fatal bug when incorrect arguments
	are given !

*/

/* Copyright © 2017 David A-D- Morano.  All rights reserved. */
/* Use is subject to license terms. */

/**************************************************************************

  	Name:
	main

	Synopsis:
	$ sname program arg0 [arg2 [arg3 [...]]]

**************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<pwd.h>
#include	<grp.h>

#include	<exitcodes.h>
#include	<bfile.h>

#include	"config.h"
#include	"localmisc.h"



/* local defines */

#define	BAD_NOENOUGH	0
#define	BAD_NOEXEC	1

#undef	USER



/* external subroutines */

extern char	*strbasename(char *) ;


/* external variables */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	bfile	errfile ;

	struct passwd	*pp ;

#ifdef	COMMENT
	ustat	ss ;
#endif

	uid_t	uid ;

	int	rs, i = -1 ;
	int	ex ;

	char	*progname ;
	char	*program ;


	progname = strbasename(argv[0]) ;

	if (argc < 3) {

		ex = EX_USAGE ;
		i = BAD_NOENOUGH ;
		goto bad ;
	}


/* check arguments */

#ifdef	COMMENT
	stat(argv[2],&ss) ;

	oldmode = (int) ss.st_mode ;
#endif

#ifdef	USER
	if ((uid = geteuid()) == 0) {

	    if ((pp = getpwnam(USER)) != NULL)
		setuid(pp->pw_uid) ;

	}
#endif /* defined(USER) */

	program = argv[1] ;
	argv[1] = argv[2] ;
	rs = u_execvp(program,argv + 1) ;

	ex = EX_NOEXEC ;
	i = BAD_NOEXEC ;

/* bad things come here */
bad:
	bopen(&errfile,BFILE_STDERR,"dwct",0666) ;

	switch (i) {

	case BAD_NOEXEC:
	bprintf(&errfile,
		"%s: program \"%s\" would not 'exec' correctly (rs %d)\n",
		progname,program,rs) ;

		break ;

	case BAD_NOENOUGH:
	bprintf(&errfile,
		"%s: not enough arguments given\n",progname) ;

	bprintf(&errfile,
		"%s: USAGE> %s program [arg0 [args ...]]\n",
		progname,progname) ;

		break ;

	} /* end switch */

	bclose(&errfile) ;

	return EX_NOEXEC ;
}
/* end subroutine (main) */



