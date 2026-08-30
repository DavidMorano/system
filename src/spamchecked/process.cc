/* process SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The program was written from scratch to do what the previous
	program by the same name did.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/******************************************************************************

  	Description:
	This subroutine processes a file for SpamAssassin bugs.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<csignal>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<usystem.h>
#include	<char.h>
#include	<bfile.h>
#include	<headkeymat.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(2048,LINE_MAX)
#else
#define	LINEBUFLEN	2048
#endif
#endif /* LINEBUFLEN */

#ifndef	DEBUGLEVEL
#define	DEBUGLEVEL(n)	(pip->debuglevel >= (n))
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	headkey(cchar *,int) ;


/* local variables */

constexpr cpcchar	spamkeys[] = {
	"x-spam-level",
	"x-spam-status",
	"x-spam-flag",
	"x-spam-report",
	"x-spam-checker-version",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int process(pip,ofp,fname)
struct proginfo	*pip ;
bfile		*ofp ;
cchar	fname[] ;
{
	bfile	infile ;

	int	rs, i, len ;
	int	vi ;
	int	f_inheader = TRUE ;
	int	f_bol, f_eol ;
	int	f ;

	char	linebuf[LINEBUFLEN + 1] ;


	if (fname == nullptr)
	    return SR_FAULT ;

	if (strcmp(fname,"-") == 0)
	    rs = bopen(&infile,BFILE_STDIN,"dr",0666) ;

	else
	    rs = bopen(&infile,fname,"r",0666) ;

	vi = -1 ;
	if (rs >= 0) {

		f_bol = TRUE ;
	    while ((rs = breadln(&infile,linebuf,LINEBUFLEN)) > 0) {

	        len = rs ;
		f_eol = (linebuf[len - 1] == '\n') ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
		debugprintf("process: line> %r\n",
		linebuf,(f_eol) ? (len - 1) : len) ;
#endif

	            if (linebuf[0] == '\n')
	                break ;

/* check for a spam header key */

		if (f_bol) {

		    for (i = 0 ; spamkeys[i] != nullptr ; i += 1) {

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
		debugprintf("process: trying >%s<\n",spamkeys[i]) ;
#endif

	                vi = headkeymat(spamkeys[i],linebuf,len) ;

			if (vi > 0)
				break ;

		    } /* end for */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
		debugprintf("process: vi=%d\n",vi) ;
#endif

		    if (vi > 0)
			break ;

		}

		f_bol = f_eol ;

	    } /* end while (reading lines) */

	} /* end if (opened file) */

	f = (vi > 0) ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (process) */


/* LOCAL SUBROUTINES */

#ifdef	COMMENT

static int headkey(sp,sl)
cchar	sp[] ;
int		sl ;
{
	int	hi = 0 ;


	while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	    hi += 1 ;
	}

	while ((sl > 0) && (! CHAR_ISWHITE(*sp)) && (*sp != ':')) {
	    sp += 1 ;
	    sl -= 1 ;
	}

	return (*sp == ':') ? hi : -1 ;
}
/* end subroutine (headkey) */

#endif /* COMMENT */



