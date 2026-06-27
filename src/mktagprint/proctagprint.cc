/* progtagprint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process key tags */
/* version %I% last-modified %G% */

#define	CF_DEBUG 	0		/* run-time debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/***********************************************************************

  	Description:
	This subroutine processes a single tag.

	Synopsis:
	int progtagprint(PI *pip,basedname,bbp,ofi,ofp,tag)
	cchar	basedname[] ;
	biblebook	*bbp ;
	int		ofi ;
	bfile		*ofp ;
	cchar	tag[] ;

	Arguments:
	pip		program information pointer
	basedname	base directory path
	ofi		output-format-index
	ofp		output file pointer
	tag		tag-string to process

	Returns:
	>=0		OK
	<0		error

***********************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<bfile.h>		/* LIBUC */
#include	<fifostr.h>		/* LIBUC */
#include	<biblecite.h>		/* LIBDAM */
#include	<biblebook.h>		/* LIBDAM */
#include	<sfill.h>		/* LIBDAM */
#include	<outfmt.h>		/* LIBDAM */
#include	<taginfo.h>		/* LIBDAM */
#include	<localmisc.h>		/* LIBU */

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	WORDBUFLEN
#define	WORDBUFLEN	100
#endif

#ifndef	BUFLEN
#define	BUFLEN		100
#endif


/* external subroutines */

extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	nextfield(cchar *,int,cchar **) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matpstr(cchar **,int,cchar *,int) ;
extern int	cfdecui(cchar *,int,uint *) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern int	mktagfname(char *,cchar *,cchar *,int) ;

extern char	*strwcpy(char *,cchar *,int) ;


/* external variables */


/* local structures */


/* forward references */

local int	procoutcite(PI *,biblebook *,bfile *,biblecite *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progtagprint(PI  *pip,basedname,bbp,ofi,ofp,tag)
cchar	basedname[] ;
biblebook	*bbp ;
int		ofi ;
bfile		*ofp ;
cchar	tag[] ;
{
	sfill		fillout ;
	biblecite	bc ;
	taginfo		ti ;
	bfile		itagfile, *tfp = &itagfile ;
	off_t	boff ;
	int	rs = SR_OK ;
	int	len ;
	int	ki, li ;
	int	ll ;
	int	tlen ;
	int	wlen = 0 ;

	char	tagfname[MAXPATHLEN + 1] ;
	char	linebuf[LINEBUFLEN + 1] ;
	char	*lp ;


#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progtagprint: basedname=%s\n",basedname) ;
	    debugprintf("progtagprint: tag=%s\n",tag) ;
	}
#endif

	if (tag == NULL)
	    return SR_FAULT ;

	if (tag[0] == '\0')
	    return SR_NOENT ;

/* parse the tag into its parts */

	rs = taginfo_parse(&ti,tag,-1) ;
	ki = rs ;
	if (rs < 0)
	    goto ret0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progtagprint: recoff=%u reclen=%u\n",
		ti.recoff,ti.reclen) ;
#endif

/* get the full tag filename */

	rs = mktagfname(tagfname,basedname,tag,ti.fnamelen) ;
	if (rs < 0)
	    goto ret0 ;

/* some verbosity */

	if ((rs >= 0) && (pip->nprocessed > 0) && pip->fl.optoutcookie) {
	    rs = bprintf(ofp,"%%\n") ;
	    wlen += rs ;
	}

	if ((rs >= 0) && (pip->verboselevel >= 2)) {
	    rs = bprintf(ofp,"%r\n", tag,((ki > 0) ? (ki - 1) : -1)) ;
	    wlen += rs ;
	} /* end if */

	if (rs < 0)
	    goto ret0 ;

/* open up the file */

	rs = bopen(tfp,tagfname,"r",0666) ;
	if (rs < 0)
	    goto badtagopen ;

	boff = ti.recoff ;
	rs = bseek(tfp,boff,SEEK_SET) ;
	if (rs < 0)
	    goto badtagseek ;

/* what output format are we working on */

	if (ofi < 0)
	    ofi = outfmt_raw ;

	if ((ofi == outfmt_bible) && (pip->indent == 0))
	    pip->indent = 1 ;

/* prepare the output the data */

	switch (ofi) {
	case outfmt_fill:
	case outfmt_bible:
	    rs = sfill_start(&fillout,pip->indent,ofp) ;
	    break ;
	} /* end switch */

	if (rs < 0)
	    goto badproc ;

/* print out (or further process) the entry */

	tlen = 0 ;
	while ((tlen < ti.reclen) &&
	    ((rs = breadln(tfp,linebuf,LINEBUFLEN)) > 0)) {

	    len = rs ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
		debugprintf("progtagprint: >%r<\n",
			linebuf,strlinelen(linebuf,len,60)) ;
#endif

	    lp = linebuf ;
	    ll = len ;
	    switch (ofi) {
	    case outfmt_raw:
		if (pip->indent > 0) {
	            rs = bwriteblanks(ofp,pip->indent) ;
	    	    wlen += rs ;
		}
		if (rs >= 0) {
	            rs = bwrite(ofp,lp,ll) ;
	    	    wlen += rs ;
		}
	        break ;
	    case outfmt_bible:
	        if ((rs = biblecite_ver(&bc,lp,ll)) > 0) {
		    li = rs ;
		    lp += li ;
		    ll -= li ;
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
		debugprintf("progtagprint: isbiblecite() li=%u\n",li) ;
#endif
		    rs = procoutcite(pip,bbp,ofp,&bc) ;
	    	    wlen += rs ;
	        } /* end if */
		falldown ;
	    case outfmt_fill:
		if (rs >= 0) {
	            rs = sfill_proc(&fillout,pip->linelen,lp,ll) ;
	    	    wlen += rs ;
		}
	        break ;
	    } /* end switch */
	    tlen += len ;
	    if (rs < 0) break ;
	} /* end while (reading lines) */

/* finish up with outputting the data */

	switch (ofi) {

	case outfmt_fill:
	case outfmt_bible:
	    if (rs >= 0) {

	        while (sfill_remaining(&fillout) > 0) {

	            rs = sfill_wline(&fillout,pip->linelen) ;
		    wlen += rs ;
	            if (rs < 0)
	                break ;

	        } /* end while */

	    } /* end if */

	    sfill_finish(&fillout) ;

	    break ;

	} /* end switch */

/* close the tag file */
done:
	pip->nprocessed += 1 ;

badproc:
badtagseek:
	bclose(tfp) ;

badtagopen:
ret0:
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (progtagprint) */


/* local subroutines */

local int procoutcite(PI *pip,biblebook *bbp,bfile *ofp,biblecite *bcp) noex {
	int	rs = SR_OK ;
	int	buflen = BUFLEN ;
	int	rs1 ;
	int	bi ;
	int	bl ;
	int	wlen = 0 ;
	char	buf[BUFLEN + 1] ;
	bi = bcp->b ;
	rs1 = biblebook_get(bbp,bi,buf,buflen) ;
	bl = rs1 ;
	if (rs1 >= 0) {
		rs = bprintf(ofp,"%r %u:%u\n",buf,bl,bcp->c,bcp->v) ;
		wlen += rs ;
	} else {
		rs = bprintf(ofp,"%u:%u:%u\n",bcp->b,bcp->c,bcp->v) ;
		wlen += rs ;
	} /* end if */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procoutcite) */


