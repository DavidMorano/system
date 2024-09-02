/* freadln SUPPORT */
/* lang=C++20 */

/* read a coded line from the STDIO stream */
/* version %I% last-modified %G% */

#define	CF_FGETS	1		/* faster or not? */

/* revision history:

	= 1998-08-17, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This routine will only read at most 'len' number of bytes
	from the file.

	Note that the sematics of this call are not the same as
	'fgets(3c)'.  This call will write a NULLCHAR into the user
	buffer after the supplied length of the buffer is used up.
	With 'fgets(3c)', it will never write more than the user's
	supplied length of bytes.

	Notes:
	The IRIX operating system is messed up somehow.  An attempt
	to correct for this is below.

******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<string.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdio>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#if	defined(IRIX)		/* IRIX screws up! (what else is new?) */
#define	mygetc(p)	fgetc(p)
#else
#define	mygetc(p)	getc(p)
#endif


/* exported variables */


/* exported subroutines */

int freadln(FILE *fp,char *buf,int len) noex {
	int	i = 0 ;

	if (len == 0)
	    return 0 ;

	if (len < 1)
	    return -1 ;

#if	CF_FGETS
	{
	    char	*bp ;


	    bp = fgets(buf,(len + 1),fp) ;

	    i = (bp != NULL) ? strlen(bp) : 0 ;

	} /* end block */
#else /* CF_FGETS */
	{
	    int	c ;

	    char	*bp = buf ;


	    c = 0 ;
	    for (i = 0 ; (i < len) && (c != '\n') ; i += 1) {

	        if ((c = mygetc(fp)) == EOF)
	            break ;

	        *bp++ = c ;

	    } /* end for */

	    *bp++ = '\0' ;

	} /* end block */
#endif /* CF_FGETS */

	if ((i == 0) && ferror(fp)) {
	   clearerr(fp) ;
	   i = SR_IO ;
	}

	return i ;
}
/* end subroutine (freadln) */

int fgetline(FILE *fp,char *buf,int len) noex {
	return freadln(fp,buf,len) ;
}
/* end subroutine (fgetline) */


