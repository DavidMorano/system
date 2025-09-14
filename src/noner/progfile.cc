/* progfile */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_DEBUG	0		/* time-time debug print-outs */
#define	CF_AGECHECK	0		/* age-check */

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	This subroutine provides the actual check and fix on the files
	specified.

******************************************************************************/

#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<csignal>
#include	<unistd.h>
#include	<time.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctype.h>

#include	<usystem.h>
#include	<bfile.h>
#include	<paramopt.h>
#include	<vecstr.h>
#include	<ascii.h>
#include	<strn.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	SUFBUFLEN
#define	SUFBUFLEN		MAXNAMELEN
#endif


/* external subroutines */

extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matcasestr(cchar **,cchar *,int) ;
extern int	sfbasename(cchar *,int,cchar **) ;

extern char	*timestr_logz(time_t,char *) ;


/* external variables */


/* local structures */

struct echars {
	uint	latin : 1 ;
	uint	ff : 1 ;
	uint	cr : 1 ;
	uint	bell : 1 ;
	uint	bs : 1 ;
} ;


/* forward references */

static int	proccheck(struct proginfo *,cchar *) ;
static int	isourprint(struct echars,int) ;
static int	isprintreg(int) ;


/* local variables */


/* exported subroutines */


int progfile(pip,pop,fname)
struct proginfo	*pip ;
PARAMOPT	*pop ;
cchar	fname[] ;
{
	bfile	infile ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	bnl ;
	int	rc = 0 ;
	int	f_skipfile = FALSE ;

	char	timebuf[TIMEBUFLEN + 1] ;
	char	*bnp ;
	char	*tp ;


	if (fname == NULL)
	    return SR_FAULT ;

	if (fname[0] == '\0')
	    return SR_INVALID ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("progfile: fname=%s\n",fname) ;
	    debugprintf("progfile: progmode=%u\n",pip->progmode) ;
	    debugprintf("progfile: suffix=%u\n",pip->fl.suffix) ;
	}
#endif /* CF_DEBUG */

	pip->c_total += 1 ;

/* check if this file has the correct file-suffix */

	f_skipfile = FALSE ;
	if (pip->fl.suffix) {

	    bnl = sfbasename(fname,-1,&bnp) ;

	    tp = strnrchr(bnp,bnl,'.') ;

	    if (tp != NULL) {

	        rs1 = vecstr_find(&pip->suffixes,(tp + 1)) ;
	        if (rs1 < 0)
	            f_skipfile = TRUE ;

	    } else
	        f_skipfile = TRUE ;

	} /* end if (suffix check) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile: MIDDLE f_skipfile=%u\n",f_skipfile) ;
#endif

	if (! f_skipfile) {

	    ustat	sb ;


	    rs1 = u_stat(fname,&sb) ;

	    if ((rs1 < 0) || (! S_ISREG(sb.st_mode)))
	        f_skipfile = TRUE ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2)) {
	        debugprintf("progfile: ageint=%u \n",
	            pip->ageint) ;
	        debugprintf("progfile: mtime=%s\n",
	            timestr_logz(sb.st_mtime,timebuf)) ;
	    }
#endif /* CF_DEBUG */

#if	CF_AGECHECK
	    if (! f_skipfile) {
	        if ((pip->daytime - sb.st_mtime) < pip->ageint)
	            f_skipfile = TRUE ;
	    }
#endif /* CF_AGECHECK */

	} /* end if (age check) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile: FINAL f_skipfile=%u\n",f_skipfile) ;
#endif

	if (! f_skipfile) {

/* process this file */

	    pip->c_scanned += 1 ;
	    rs = proccheck(pip,fname) ;
	    rc = (rs > 0) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("progfile: proccheck() rs=%d\n",rs) ;
#endif

	} /* end if (processing file) */

/* should we put out an informational message? */

	if (pip->debuglevel > 0)
	    bprintf(pip->efp,"%s: file=%s (%d)\n",
	        pip->progname, fname,rc) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile: ret rs=%d rc=%u\n",rs,rc) ;
#endif

	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (progfile) */


/* local subroutines */

static int proccheck(pip,fname)
struct proginfo	*pip ;
cchar	fname[] ;
{
	struct echars	ec ;

	bfile	cfile, *cfp = &cfile ;

	uint	ch ;

	int	rs ;
	int	len ;
	int	line = 0 ;
	int	i = 0 ;
	int	f_none = FALSE ;
	int	f ;

	char	linebuf[LINEBUFLEN + 1] ;
	char	*kp ;


	memset(&ec,0,sizeof(struct echars)) ;
	ec.latin = pip->fl.latin ;
	ec.ff = pip->fl.formfeed ;
	ec.cr = pip->fl.carriage ;
	ec.bell = pip->fl.bell ;
	ec.bs = pip->fl.backspace ;

	if ((fname[0] != '\0') && (fname[0] != '-'))
	    rs = bopen(cfp,fname,"r",0666) ;

	else
	    rs = bopen(cfp,BFILE_STDIN,"dr",0666) ;

	if (rs < 0)
	    goto ret0 ;

	while ((rs = breadln(cfp,linebuf,LINEBUFLEN)) > 0) {

	    len = rs ;
	    if (linebuf[len-1] == '\n')
	        len -= 1 ;

	    for (i = 0 ; i < len ; i += 1) {
	        ch = linebuf[i] ;
	        f = isourprint(ec,ch) ;
	        f_none = (! f) ;
	        if (f_none)
	            break ;
	    } /* end for */

	    if (f_none) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(2)) {
		    int		ch, sch = MKCHAR(linebuf[i]) ;
	            ch = (isprintlatin(sch) ? linebuf[i] : ' ' ;
	            debugprintf("progfile/proccheck: NONE i=%u ch=>%c< %02X\n",
	                i,ch,linebuf[i]) ;
	        }
#endif

	        break ;
	    }

	    line += 1 ;

	} /* end while (reading lines) */

	bclose(cfp) ;

	if ((rs >= 0) && f_none && (pip->verboselevel >= 1)) {

	    if (pip->verboselevel >= 2)
	        bprintf(pip->ofp,"%s %u:%u\n",fname,(line+1),i) ;

	    else
	        bprintf(pip->ofp,"%s\n",fname) ;

	} /* end if (report) */

ret0:
	return (rs >= 0) ? f_none : rs ;
}
/* end subroutine (proccheck) */


static int isourprint(struct echars ec,int ch)
{
	int		f ;

	f = (ec.latin) ? isprintlatin(ch) : isprintreg(ch) ;
	f = f || (ec.ff && (ch == CH_FF)) ;
	f = f || (ec.cr && (ch == CH_CR)) ;
	f = f || (ec.bell && (ch == CH_BELL)) ;
	f = f || (ec.bs && (ch == CH_BS)) ;
	return f ;
}
/* end subroutines (isourprint) */


static int isprintreg(int ch)
{
	int		f ;
	f = isprint(ch) ;
	f = f || (ch == '\t') ;
	return f ;
}
/* end subroutine (isprintreg) */


