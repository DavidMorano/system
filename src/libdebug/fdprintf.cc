/* fdprintf SUPPORT */
/* lang=C++20 */

/* 'FileDescriptor' printf subroutine */
/* version %I% last-modified %G% */

#define	CF_EPRINT	1		/* link in '??' support */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Description:
	This subroutine performs a 'printf' like function but to
	the named file which is passed as the first argument.

	Synopsis:
	int fdprintf(int fd,cchar *fmt,va_list ap) noex

	Arguments:
	fd		file descriptor to write to
	fmt		standard format string
	...		enverything else

	Returns:
	>=0		length of data (in bytes) written to file
	<0		error code (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstring>
#include	<cstdarg>
#include	<usystem.h>
#include	<fmtstr.h>
#include	<localmisc.h>


/* local defines */

#undef	BUFLEN
#define	BUFLEN		512


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int fdprintf(int fd,cchar *fmt,...) noex {
	va_list	ap ;

	int	rs = SR_OK ;
	int	len = 0 ;

	char	buf[BUFLEN + 1] ;


#if	CF_DEBUGS
	debugprintf("fdprintf: fd=%d\n",fd) ;
#endif

	if (fd < 0) {

#if	CF_EPRINT
		fd = debuggetfd() ;
#else
		return SR_NOTOPEN ;
#endif

	} /* end if */

#if	CF_DEBUGS
	debugprintf("fdprintf: fd=%d\n",fd) ;
#endif

#if	CF_DEBUGS
	debugprintf("fdprintf: fmt=%s\n",fmt) ;
#endif

	va_begin(ap,fmt) ;

	len = fmtstr(buf,BUFLEN,1,fmt,ap) ;

	va_end(ap) ;

#if	CF_DEBUGS
	    debugprintf("fdprintf: len=%d\n",len) ;
#endif

	if (len > 0)
	    rs = uc_writen(fd,buf,len) ;

#if	CF_DEBUGS
	debugprintf("fdprintf: rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (fdprintf) */



