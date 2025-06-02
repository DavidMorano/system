/* errfile */

/* module to operate on the error file (errfile) */


#define	CF_DEBUGS	0


/* revision history:

	= 1994-06-01, David A­D­ Morano
        This module was created so that a file to store debugging or error
        information could be accessed independent from the regular I-O of the
        program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

        This module implements a sort of debugging or error interface so that
        the regular I-O of the program is not interfered with.


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<cstdarg>
#include	<fmtstr.h>

#if	CF_DEBUGS
#include	<cstdio>
#include	"config.h"
#include	"smail.h"
#endif

#include	<localmisc.h>


/* local defines */

#define	O_FLAGS		(O_WRONLY | O_APPEND | O_CREAT)

#ifndef	BUFLEN
#define	BUFLEN		10000
#endif


/* external subroutines */


/* global data */


/* local data */

static int	efd = -1, pl ;
static char	buf[BUFLEN + 1] ;


/* exported subroutines */


int erropen(errfname,logid)
const char	errfname[] ;
char		logid[] ;
{


	if ((efd = open(errfname,O_FLAGS,0666)) < 0)
	    return BAD ;

	chmod(errfname,0666) ;

	if (strlen(logid) > 13)
	    logid[13] = '\0' ;

	pl = bufprintf(buf,BUFLEN,"%-13s ",logid) ;

	return efd ;
}
/* end subroutine */


int errprintf(cchar *fmt,...) noex {
	va_list		ap ;
	int	sl ;

	if (efd < 0)
	    return BAD ;

	va_begin(ap,fmt) ;

	sl = fmtstr((buf + pl),(BUFLEN - pl),1,fmt,ap) ;

	va_end(ap) ;

	if (sl >= (BUFLEN - pl - 1)) {
	    buf[pl + sl++] = '\n' ;
	}

	sl = write(efd,buf,pl + sl) ;

	return sl ;
}
/* end subroutine (errprintf) */



