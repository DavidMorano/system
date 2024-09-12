/* ffreadln SUPPORT */
/* lang=C++20 */

/* read a coded line from the STDIO stream */
/* version %I% last-modified %G% */

#define	CF_FGETS	1	/* faster or not? */

/* revision history:

	= 1986-01-17, David A­D­ Morano
	This code was originally written.

	= 2001-09-10, David A­D­ Morano
	I discovered that on the SGI Irix systems, |getc()| does
	not seem to work properly so I hacked it out for that
	system.


*/

/* Copyright © 1986-2001 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This routine will only read at most 'len' number of bytes
	from the file.

	Note that the sematics of this call are not the same as
	'fgets(3c)'.  This call will write a NULLCHAR into the user
	buffer after the supplied length of the buffer is used up.
	With 'fgets(3c)', it will never write more than the user's
	supplied length of bytes.

******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<cerrno>
#include	<cstdio>
#include	<cstring>
#include	<usystem.h>

#include	"ffile.h"


/* local defines */

#if	defined(OSNAME_IRIX)		/* IRIX screws up! (what else is new) */
#define	mygetc(p)	fgetc(p)
#else
#define	mygetc(p)	getc(p)
#endif


/* forward references */

#define	TO_AGAIN	(5*60)

#if	CF_FGETS
static int ffreadln_gets(FFILE *,char *,int) ;
#else
static int ffreadln_getc(FFILE *,char *,int) ;
#endif /* CF_FGETS */


/* local variables */


/* exported variables */


/* exported subroutines */

int ffreadln(FILE *fp,char *rbuf,int rlen) noex {
	uint		to = utimeout[uto_again] ;
	int		rs = SR_FAULT ;
	if (fd && rbuf) {
	    rs = SR_INVALID ;
	    if (len >= 0) {
again:
	if (len > 0) {
#if	CF_FGETS
	    rs = ffreadln_gets(fp,buf,len) ;
#else
	    rs = ffreadln_getc(fp,buf,len) ;
#endif /* CF_FGETS */
	}

	if (rs < 0) {
	    switch (rs) {
	    case SR_INTR:
		goto again ;
	    case SR_AGAIN:
	        if (to-- > 0) {
		    sleep(1) ;
		    goto again ;
		}
		break ;
	    } /* end switch */
	} /* end if */

	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ffreadln) */


/* local subroutines */

#if	CF_FGETS

static int ffreadln_gets(fp,buf,len)
FFILE		*fp ;
char		*buf ;
int		len ;
{
	FILE	*sfp = fp->sfp ;

	int	rs = 0 ;
	char	*bp ;

	bp = fgets(buf,(len + 1),sfp) ;
	if (bp == NULL) {
		if (ferror(sfp)) {
		    rs = (- errno) ;
		} else if (feof(sfp)) {
		    rs = 0 ;
		} else {
		    rs = SR_NOANODE ;
		}
	    clearerr(sfp) ;
	} else
	    rs = strlen(bp) ;

	return rs ;
}
/* end subroutine (ffreadln_gets) */

#else /* CF_FGETS */

static int ffreadln_getc(fp,buf,len)
FFILE		*fp ;
char		*buf ;
int		len ;
{
	int	rs ;
	int	len = 0 ;
	char	*bp = buf ;

	while ((rs = ffgetc(fp)) >= 0) {
	    *bp++ = rs ;
	    len += 1 ;
	    if (rs == '\n') break ;
	} /* end while */

	if (rs == SR_EOF) rs = SR_OK ;

	*bp = '\0' ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ffreadln_getc) */

#endif /* CF_FGETS */


