/* mkhosts_file SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process an input file to the output */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revistion history :

	= 1987-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1987 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/******************************************************************************

  	Name:
	mkhosts_file

	Description:
	This subroutine processes a file by writing its contents out
	to an output file with the correct host name placement.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<netdb.h>
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */

#include	"mkhosts_config.h"

import proginfo ;

/* local defines */

#ifndef	LINELEN
#define	LINELEN		200
#endif

#ifndef	BUFLEN
#define	BUFLEN		(LINELEN + LINELEN)
#endif


/* external subroutines */


/* forward references */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutine */

int procfile(PI pip,cchar *infname,int fn) noex {
	bfile		infile, *ifp = &infile ;
	int	rs, i, j ;
	int	lines = 0, len ;
	int	f_stdinput = false ;

	char	lbuf[LINELEN + 1], *lbp ;
	char	hostname[MAXHOSTNAMELEN + 1] ;
	char	*cp ;


/* check the arguments */

	if ((infname == nullptr) || (infname[0] == '-')) {
	    f_stdinput = true ;
	    rs = bopen(ifp,BFILE_STDIN,"dr",0666) ;
	} else {
	    rs = bopen(ifp,infname,"r",0666) ;
	}
	if (rs < 0) goto badinfile ;

	while ((rs = breadln(ifp,lbuf,LINELEN)) > 0) {

		len = rs ;
	    if (len <= 1) continue ;

	    lbuf[len] = '\0' ;

#if	CF_DEBUG
	    if (gp->debuglevel > 1)
	        DEBUGPRINTF("procfile: LINE>%W",lbuf,len) ;
#endif

	    if (lbuf[0] == ';') continue ;

	    if (isalpha(lbuf[0])) {

	        lbp = lbuf ;
	        while (*lbp && (! CHAR_ISWHITE(*lbp))) 
			lbp += 1 ;

	        strwcpy(hostname,lbuf,
	            MIN((lbp - lbuf),MAXHOSTNAMELEN)) ;

#if	CF_DEBUG
	        if (gp->debuglevel > 1)
	            DEBUGPRINTF("procfile: hostsname=%s\n",hostname) ;
#endif

	        while (CHAR_ISWHITE(*lbp)) 
			lbp += 1 ;

	        i = (lbp - lbuf) ;

	    } else
	        i = 0 ;

#if	CF_DEBUG
	    if (gp->debuglevel > 1)
	        DEBUGPRINTF("procfile: is it an IN record\n") ;
#endif

	    if ((j = sfsub(lbuf + i,len,"IN",&cp)) < 0) 
		continue ;

#if	CF_DEBUG
	    if (gp->debuglevel > 1)
	        DEBUGPRINTF("procfile: %d IN record> %s\n",i + j,lbuf + i) ;
#endif

	    lbp = lbuf + i + j + 2 ;
	    while (CHAR_ISWHITE(*lbp)) 
		lbp += 1 ;

#if	CF_DEBUG
	    if (gp->debuglevel > 1)
	        DEBUGPRINTF("procfile: is it an A record> %s\n",lbp) ;
#endif

	    if (*lbp != 'A') 
		continue ;

/* read the IP address */

#if	CF_DEBUG
	    if (gp->debuglevel > 1)
	        DEBUGPRINTF("procfile: reading IP address\n") ;
#endif

	    lbp += 1 ;
	    while (CHAR_ISWHITE(*lbp)) lbp += 1 ;

	    if (((i = sfshrink(lbp,-1,&cp)) > 0) && (hostname[0] != '\0')) {
	        bprintf(gp->ofp,"%r	%s\n",cp,i,hostname) ;
	    }
	    lines += 1 ;
	} /* end while (line reading loop) */

done:
	bclose(ifp) ;

badinfile:
	return (rs >= 0) ? lines : rs ;
} /* end subroutine (procfile) */


