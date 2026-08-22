/* opendialer_hello SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open-dialer (hello) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	= 2003-11-04, David A­D­ Morano
	This code was started by taking the corresponding code from
	the TCP-family module.  In retrospect, that was a mistake.
	Rather I should have started this code by using the
	corresponding UUX dialer module.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	opendialer_hello

	Description:
	This is an open-dialer.

	Synopsis:

	int opendialer_hello(pr,prn,svc,of,om,argv,envv,to)
	char	*pr ;
	char	*prn ;
	char	*svc ;
	int		of ;
	mode_t		om ;
	char	**argv ;
	char	**envv ;
	int		to ;

	Arguments:

	pr		program-root
	prn		facility name
	svc		service name
	of		open-flags
	om		open-mode
	argv		argument array
	envv		environment array
	to		time-out

	Returns:

	>=0		file-descriptor
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ids.h>			/* LIBUC */
#include	<keyopt.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<paramfile.h>		/* LIBUC */
#include	<nulstr.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"opendialer_hello.h"
#include	"defs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int opendialer_hello(pr,prn,svc,of,om,argv,envv,to)
char	*pr ;
char	*prn ;
char	*svc ;
int		of ;
mode_t		om ;
char	**argv ;
char	**envv ;
int		to ;
{
	int		rs = SR_OK ;
	int		pipes[2] ;
	int		fd = -1 ;
	int		sl = -1 ;
	char	*sp = "hello world!\n" ;

	if ((rs = u_pipe(pipes)) >= 0) {
	    const int	wfd = pipes[1] ;
	    fd = pipes[0] ;

	    if (sl < 0) sl = lenstr(sp) ;

	    rs = u_write(wfd,sp,sl) ;

	    u_close(wfd) ;
	    if (rs < 0) u_close(fd) ;
	} /* end if */

	return (rs >= 0) ? fd : rs ;
} /* end subroutine (opendialer_hello) */


