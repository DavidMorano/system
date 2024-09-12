/* procfileenv SUPPORT */
/* lang=C++20 */

/* process an environment file */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	procfileenv

	Description:
	This subroutine will read (process) an environment file and
	put all of the environment variables into the string list
	(supplied).  New environment variables just get added to the
	list.  Old environment variables already on the list are
	deleted with a new definition is encountered.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<field.h>
#include	<vecstr.h>
#include	<char.h>
#include	<localmisc.h>


/* local defines */

#define	BUFLEN		(4 * MAXPATHLEN)

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif


/* external subroutines */

extern int	vstrkeycmp(char **,char **) ;

extern char	*strwcpy(char *,const char *,int) ;


/* externals variables */


/* forward references */


/* local structures */


/* local variables */

static const uchar	fterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported subroutines */


int procfileenv(cchar *programroot,cchar *fname,VECSTR *lp)
{
	FIELD		fsb ;
	bfile		file, *fp = &file ;
	const int	llen = LINEBUFLEN ;
	int		rs ;
	int		c = 0 ;
	int		len, cl ;
	char		lbuf[LINEBUFLEN + 1] ;
	char		buf[BUFLEN + 1], *bp ;
	char		*cp ;

	if ((rs = bopenroot(fp,programroot,fname,NULL,"r",0666)) < 0)
	    return rs ;

	while ((rs = breadln(fp,lbuf,llen)) > 0) {
	    len = rs ;

	    if (lbuf[len - 1] == '\n')
	        len -= 1 ;

	    cp = lbuf ;
	    cl = len ;
	    while ((cl > 0) && CHAR_ISWHITE(*cp)) {
		cp += 1 ;
		cl -= 1 ;
	    }

	    if ((cp[0] == '\0') || (cp[0] == '#'))
	        continue ;

	    if ((rs = field_start(&fsb,cp,cl)) >= 0) {
		int	fl ;
		char	*fp ;

	    fl = field_get(&fsb,fterms,&fp) ;

	    if (fl > 0) {

	        if (strncasecmp(fp,"export",fl) == 0)
	            fl = field_get(&fsb,fterms,&fp) ;

	        bp = buf ;
	        bp = strwcpy(bp,fp,MIN(fl,BUFLEN)) ;

	        if (bp < (buf + BUFLEN - 1))
	            *bp++ = '=' ;

	        fp = bp ;
	        fl = field_sharg(&fsb,fterms,bp,(buf + BUFLEN - bp)) ;

	        if (fl > 0) {
	            bp += fl ;
	        } else
	            bp -= 1 ;

	        *bp = '\0' ;

	        if ((rs = vecstr_finder(lp,buf,vstrkeycmp,NULL)) >= 0)
	            vecstr_del(lp,rs) ;

	        rs = vecstr_add(lp,buf,(bp - buf)) ;

	        c += 1 ;

	    } /* end if */

	    field_finish(&fsb) ;
	    } /* end if (field) */

	    if (rs < 0) break ;
	} /* end while (reading lines) */

	bclose(fp) ;

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procfileenv) */


