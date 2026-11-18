/* vecstr_filepaths SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a "paths" file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	vecstr_filepaths

	Description:
	This subroutine will read (process) a file that has directory
	paths in it.  This reads the directory paths in the file
	and creates a new single environment variable named 'PATH'.
	That environment variable is then added to the specified
	list.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<field.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<pathclean.h>		/* LIBUC */
#include	<cmpxx.h>		/* LIBUC */
#include	<vstrcmp.h>		/* LIBUC |vstrkeycmp(3uc)| */
#include	<char.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"vecstr_filepaths.h"


/* local defines */

#undef	BUFLEN
#define	BUFLEN		(4 * MAXPATHLEN)

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#define	PATHBUFLEN	(60 * MAXPATHLEN)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debug print-outs */
#endif


/* external subroutines */


/* externals variables */


/* forward references */

#if	CF_DEBUG
local int pathdump(cchar *,int) noex ;
#endif


/* local structures */


/* local variables */

constexpr char		fterms[] = {
	0x00, 0x00, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array (fterms) */


/* exported variables */


/* exported subroutines */

int vecstr_filepaths(vecstr *lp,ccp programroot,ccp fname) noex {
	bfile	pfile, *pfp = &pfile ;
	int	rs, rs1 ;
	int	c, len ;
	int	fl, cl ;
	int	psi, pbi ;
	char	linebuf[LINEBUFLEN + 1] ;
	char	buf[BUFLEN + 1] ;
	char	pathbuf[PATHBUFLEN + 1], *pp ;
	char	*cp, *fp ;

#if	CF_DEBUG
	char	outname[MAXPATHLEN + 1] ;
#endif


#if	CF_DEBUG
	debugprintf("vecstr_filepaths: ent=%s\n",fname) ;

	rs = bopenroot(pfp,programroot,fname,outname,"r",0666) ;

	if (rs < 0) {
	    debugprintf("vecstr_filepaths: bopenroot() rs=%d\n",rs) ;
	    return rs ;
	}

	debugprintf("vecstr_filepaths: file=%s\n",outname) ;
#else
	if ((rs = bopenroot(pfp,programroot,fname,NULL,"r",0666)) < 0)
	    return rs ;
#endif /* CF_DEBUG */

#if	CF_DEBUG
	debugprintf("vecstr_filepaths: opened\n") ;
#endif

/* get the PATH variable as it exists now, if it exists */

	psi = -1 ;
	pbi = 0 ;
	pathbuf[0] = '\0' ;
	if ((psi = vecstr_finder(lp,"PATH",vstrkeycmp,&pp)) >= 0) {
	    rs = storebuf_buf(pathbuf,PATHBUFLEN,pbi,pp,-1) ;
		pbi += rs ;
#if	CF_DEBUG
	    debugprintf("vecstr_filepaths: existing=>%s<\n",
	        pathbuf) ;
#endif
	}
	if (rs < 0) goto ret1 ;

/* read the file and process any paths that we find */

	c = 0 ;
	while ((rs = breadln(pfp,linebuf,LINEBUFLEN)) > 0) {
	    len = rs ;

	    if (linebuf[len - 1] == '\n') len -= 1 ;
	    linebuf[len] = '\0' ;

#if	CF_DEBUG
	    debugprintf("vecstr_filepaths: line> %r\n",linebuf,len) ;
#endif

	    cp = linebuf ;
	    cl = len ;
	    while ((cl > 0) && CHAR_ISWHITE(*cp)) {
		cp += 1 ;
		cl -= 1 ;
	    }
	    if ((cp[0] == '\0') || (cp[0] == '#')) continue ;
	    field fsb ;
	    if ((rs = field_start(&fsb,cp,cl)) >= 0) {

		fp = buf ;
	        if ((fl = field_sharg(&fsb,fterms,buf,BUFLEN)) > 0) {

#if	CF_DEBUG
	    debugprintf("vecstr_filepaths: 1 field> %r\n",fp,fl) ;
#endif

	    while ((fl > 1) && (fp[fl - 1] == '/')) {
	        fl -= 1 ;
	    }

	    fp[fl] = '\0' ;
	    if (pbi > 0) {
	        if (cmpstrvaln(pathbuf,fp,fl) != 0) {
	            c += 1 ;
	            rs = storebuf_chr(pathbuf,PATHBUFLEN,pbi,':') ;
			pbi += rs ;
			if (rs >= 0)
	            rs = storebuf_buf(pathbuf,PATHBUFLEN,pbi,fp,fl) ;
			pbi += rs ;
	        } /* end if (adding) */
	    } else {
	        rs = storebuf_buf(pathbuf,PATHBUFLEN,pbi,"PATH=",5) ;
		pbi += rs ;
	        c += 1 ;
		if (rs >= 0)
	        rs = storebuf_buf(pathbuf,PATHBUFLEN,pbi,fp,fl) ;
		pbi += rs ;
	    } /* end if */
	    } /* end if (non-zero field) */
	    field_finish(&fsb) ;
	    } /* end if (field) */
		if (rs < 0) break ;
	} /* end while (reading lines) */

/* do we have the standard path component? */

	cp = "/usr/bin" ;
	if ((rs >= 0) && (cmpstrvaln(pathbuf,cp,-1) != 0)) {
	    c += 1 ;
	    rs = storebuf_chr(pathbuf,PATHBUFLEN,pbi,':') ;
		pbi += rs ;
		if (rs >= 0) {
	    rs = storebuf_buf(pathbuf,PATHBUFLEN,pbi,cp,-1) ;
		pbi += rs ;
		}
	}

#if	CF_DEBUG
	debugprintf("vecstr_filepaths: final rs=%d\n",rs) ;
	pathdump(pathbuf,pbi) ;
#endif /* CF_DEBUG */

/* add the PATH variable back to the environment */

	if ((rs >= 0) && (c > 0)) {
	    if (psi >= 0) {
	        vecstr_del(lp,psi) ;
	    }
	    vecstr_add(lp,pathbuf,pbi) ;
	}

ret1:
	bclose(pfp) ;

#if	CF_DEBUG
	debugprintf("vecstr_filepaths: ret rs=%d c=%u\n",rs,c) ;
#endif

ret0:
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstr_filepaths) */


/* local subroutines */

#if	CF_DEBUG
local int pathdump(cchar *pathbuf,int pbi) noex {
	int	rs = SR_OK ;
	int	mlen, rlen = pbi ;
	int	wlen = 0 ;
		cchar	*pp = pathbuf ;
		while (rlen > 0) {
			mlen = MIN(rlen,40) ;
			rs = debugprintf("vecstr_filepaths: path| %r\n",
				pp,mlen) ;
			wlen += rs ;
			if (rs < 0) break ;
			pp += mlen ;
			rlen -= mlen ;
		}
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (pathdump) */
#endif /* CF_DEBUG */


