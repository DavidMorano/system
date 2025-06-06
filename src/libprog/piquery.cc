/* progquery SUPPORT */
/* lang=C++20 */

/* make a query */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time */
#define	CF_DEBUG 	0		/* run-time debug print-outs */
#define	CF_READSTDIN	0		/* read STDIN failing arguments */

/* revision history:

	= 1994-03-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	progquery

	Description:
	This subroutine processes a single file.

	Synopsis:
	int progquery(pip,aip,terms,dbname,ofname)
	PROGINFO	*pip ;
	ARGINFO		*aip ;
	const uchar	terms[] ;
	const uchar	dbname[] ;
	char		ofname[] ;

	Arguments:
	- pip		program information pointer

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<baops.h>
#include	<bfile.h>
#include	<vecstr.h>
#include	<field.h>
#include	<localmisc.h>

#include	"textlook.h"
#include	"config.h"
#include	"defs.h"


/* local defines */

#define	OUTBUFLEN	(MAXPATHLEN + 50)
#define	WPBUFLEN	(20 * 100)


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	sfcasesub(cchar *,int,cchar *,cchar **) ;
extern int	nextfield(cchar *,int,cchar **) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matostr(cchar **,int,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	optbool(cchar *,int) ;
extern int	mkexpandpath(char *,cchar *,int) ;
extern int	bufprintf(char *,int,cchar *,...) ;
extern int	isdigitlatin(int) ;

#if	CF_DEBUG || CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern int	progexit(PROGINFO *) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpylc(char *,cchar *,int) ;


/* external variables */


/* local structures */


/* forward references */

static int	procdb(PROGINFO *,ARGINFO *,bfile *,cchar *) ;

static int	procspecs(PROGINFO *,bfile *,TEXTLOOK *,cchar *,int) ;
static int	procspec(PROGINFO *,bfile *,TEXTLOOK *,vecstr *) ;

static int	procout(PROGINFO *,bfile *,TEXTLOOK_TAG *) ;


/* local variables */

static const uchar	aterms[] = {
	0x00, 0x3E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported subroutines */


int progquery(pip,aip,terms,dbname,ofname)
PROGINFO	*pip ;
ARGINFO		*aip ;
const uchar	terms[] ;
cchar	dbname[] ;
cchar	ofname[] ;
{
	bfile		ofile, *ofp = &ofile ;
	int		rs ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf( "progquery: dbname=%s\n",dbname) ;
	    debugprintf( "progquery: ofname=%s\n",ofname) ;
	}
#endif /* CF_DEBUG */

	if (terms == NULL) return SR_FAULT ;
	if (dbname == NULL) return SR_FAULT ;

	if (dbname[0] == '\0')
	    return SR_INVALID ;

/* open the output file */

	if ((ofname == NULL) || (ofname[0] == '\0')) ofname = BFILE_STDOUT ;

	if ((rs = bopen(ofp,ofname,"wct",0666)) >= 0) {

	    rs = procdb(pip,aip,ofp,dbname) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf( "progquery: procdb() rs=%d\n",rs) ;
#endif

	    bclose(ofp) ;
	} /* end if (file) */

	if ((rs >= 0) && (pip->debuglevel > 0))
	    bprintf(pip->efp,"%s: matches=%u\n",pip->progname,rs) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf( "progquery: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (progquery) */


/* local subroutines */


static int procdb(pip,aip,ofp,dbname)
PROGINFO	*pip ;
ARGINFO		*aip ;
bfile		*ofp ;
cchar	dbname[] ;
{
	TEXTLOOK	tl, *tlp = &tl ;
	vecstr		qstr ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		pan = 0 ;
	int		opts ;
	int		ai ;
	int		c_hits = 0 ;
	int		f ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;
	cchar	*afname = aip->afname ;
	cchar	*cp ;
	char		tmpfname[MAXPATHLEN+1] ;

	rs = mkexpandpath(tmpfname,dbname,-1) ;
	if (rs > 0) dbname = tmpfname ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf( "progquery/procdb: mkexpandpath() rs=%d\n",rs) ;
#endif
	if (rs < 0) goto ret0 ;

	if ((rs = textlook_open(tlp,pip->pr,dbname,pip->basedname)) >= 0) {

	if (pip->f.optaudit) {
	    rs = textlook_audit(tlp) ;
	    if ((rs < 0) || (pip->debuglevel > 0))
	        bprintf(pip->efp, "%s: DB audit (%d)\n",
	            pip->progname,rs) ;
	} /* end if (audit) */

/* process the positional arguments */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf( "progquery/procdb: keys as arguments\n") ;
#endif

	opts = VECSTR_OCOMPACT ;
	if ((rs >= 0) && ((rs = vecstr_start(&qstr,5,opts)) >= 0)) {
	    BITS	*bop = &aip->pargs ;
	    int		c = 0 ;

	    for (ai = 1 ; ai < aip->argc ; ai += 1) {

	        f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != NULL)) ;
	        if (f) {
	            cp = aip->argv[ai] ;
	            pan += 1 ;
	            rs = vecstr_adduniq(&qstr,cp,-1) ;
	            c += ((rs < INT_MAX) ? 1 : 0) ;
		}

	        if (rs >= 0) rs = progexit(pip) ;
	        if (rs < 0) break ;
	    } /* end for (looping through positional arguments) */

	    if ((rs >= 0) && (c > 0)) {
	            if ((rs = vecstr_count(&qstr)) > 0) {
	                rs = procspec(pip,ofp,tlp,&qstr) ;
			c_hits += rs ;
		    }
	    } /* end if */

	    rs1 = vecstr_finish(&qstr) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (positional arguments) */

/* process any files in the argument filename list file */

	if ((rs >= 0) && (afname != NULL) && (afname[0] != '\0')) {
	    bfile	afile, *afp = &afile ;

	    if (strcmp(afname,"-") == 0) afname = BFILE_STDIN ;

	    if ((rs = bopen(afp,afname,"r",0666)) >= 0) {
		cint	llen = LINEBUFLEN ;
		int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = breadln(afp,lbuf,llen)) > 0) {
	            len = rs ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("progquery/procdb: line=>%r<\n",
	                    lbuf,strlinelen(lbuf,len,60)) ;
#endif

	            rs = procspecs(pip,ofp,tlp,lbuf,len) ;
		    c_hits += rs ;

	            if (rs >= 0) rs = progexit(pip) ;
	            if (rs < 0) break ;
	        } /* end while (reading lines) */

	        rs1 = bclose(afp) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
		fmt = "%s: inaccessible argument-list (%d)\n" ;
	        bprintf(pip->efp,fmt,pn,rs) ;
	        bprintf(pip->efp,"%s: afile=%s\n",pn,aip->afname) ;
	    } /* end if */

	} /* end if (argument file) */

#if	CF_READSTDIN
	if ((rs >= 0) && (pan == 0)) {
	    bfile	infile, *ifp = &infile ;

	    if ((rs = bopen(ifp,BFILE_STDIN,"dr",0666)) >= 0) {
		cint	llen = LINEBUFLEN ;
		int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = breadln(ifp,lbuf,llen)) > 0) {
	            int	len = rs ;

	            rs = procspecs(pip,ofp,tlp,lbuf,len) ;
	            c_hits += rs ;

	            if (rs >= 0) rs = progexit(pip) ;
	            if (rs < 0) break ;
	        } /* end while (reading lines) */

	        bclose(ifp) ;
	    } else {
	        fmt = "%s: inaccessible input list (%d)\n" ;
	        bprintf(pip->efp,fmt,pn,rs) ;
	    } /* end if (error) */

	} /* end if (file list arguments or not) */
#endif /* CF_READSTDIN */

	rs1 = textlook_close(tlp) ;
	if (rs >= 0) rs = rs1 ;
	} /* end if (textlook) */

ret0:

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progquery/procdb: ret rs=%d c_hits=%u\n",rs,c_hits) ;
#endif

	return (rs >= 0) ? c_hits : rs ;
}
/* end subroutine (procdb) */


static int procspecs(pip,ofp,tlp,sp,sl)
PROGINFO	*pip ;
bfile		*ofp ;
TEXTLOOK	*tlp ;
cchar	*sp ;
int		sl ;
{
	VECSTR		qstr ;
	int		rs ;
	int		c = 0 ;

	if (sp == NULL) return SR_FAULT ;

	if ((rs = vecstr_start(&qstr,5,0)) >= 0) {
	    FIELD	fsb ;

	    if ((rs = field_start(&fsb,sp,sl)) >= 0) {
		cint	wlen = WPBUFLEN ;
		int		fl ;
		cchar		*fp ;
		char		wbuf[WPBUFLEN + 1] ;

	        fp = wbuf ;
	        while ((fl = field_wordphrase(&fsb,aterms,wbuf,wlen)) >= 0) {
	            if (fl > 0) {
	                rs = vecstr_adduniq(&qstr,fp,fl) ;
	                c += ((rs < INT_MAX) ? 1 : 0) ;
		    }
	            if (fsb.term == '#') break ;
	            if (rs < 0) break ;
	        } /* end while */

	        field_finish(&fsb) ;
	    } /* end if (field) */

	    if ((rs >= 0) && (c > 0)) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(3)) {
	            int		i ;
		    cchar	*fp ;
	            for (i = 0 ; vecstr_get(&qstr,i,&fp) >= 0 ; i += 1)
	                debugprintf("progquery/procspecs: qs=>%s<\n",fp) ;
	        }
#endif /* CF_DEBUG */

	        rs = procspec(pip,ofp,tlp,&qstr) ;

	    } /* end if */

	    vecstr_finish(&qstr) ;
	} /* end if (queries) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procspecs) */


static int procspec(pip,ofp,tlp,qsp)
PROGINFO	*pip ;
bfile		*ofp ;
TEXTLOOK	*tlp ;
vecstr		*qsp ;
{
	TEXTLOOK_CUR	cur ;
	TEXTLOOK_TAG	tag ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		ntags ;
	int		qopts = 0 ;
	int		c = 0 ;
	cchar	**qkeya ;

	if (qsp == NULL) return SR_FAULT ;

	if (pip->f.prefix) qopts |= TEXTLOOK_OPREFIX ;

	if ((rs = vecstr_getvec(qsp,&qkeya)) >= 0) {

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    int	i ;
	    for (i = 0 ; qkeya[i] != NULL ; i += 1)
	        debugprintf("progquery/procspec: sk=>%s<\n",qkeya[i]) ;
	}
#endif /* CF_DEBUG */

	if ((rs = textlook_curbegin(tlp,&cur)) >= 0) {

	    rs = textlook_lookup(tlp,&cur,qopts,qkeya) ;
	    ntags = rs ;

	    while ((rs >= 0) && (ntags > 0)) {
	        rs1 = textlook_read(tlp,&cur,&tag) ;
	        if (rs1 == SR_NOTFOUND) break ;
	        rs = rs1 ;

	        if (rs >= 0) {
	            c += 1 ;
	            rs = procout(pip,ofp,&tag) ;
	        }

	        if (rs >= 0) rs = progexit(pip) ;
	        if (rs < 0) break ;
	    } /* end while */

	    textlook_curend(tlp,&cur) ;
	} /* end if (cursor) */

	} /* end if (vecstr_getvec) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf( "progquery/procspec: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procspec) */

static int procout(PROGINFO *pip,bfile *ofp,TEXTLOOK_TAG *tagp) noex {
	uint		recoff ;
	uint		reclen ;
	cint	olen = OUTBUFLEN ;
	int		rs = SR_OK ;
	int		len ;
	int		wlen = 0 ;
	cchar	*fmt ;
	cchar	*fn ;
	char		obuf[OUTBUFLEN + 1] ;

	if (pip == NULL) return SR_FAULT ;

	fmt = "%s:%u,%u\n" ;
	fn = tagp->fname ;
	recoff = tagp->recoff ;
	reclen = tagp->reclen ;

	if ((rs = bufprintf(obuf,olen,fmt,fn,recoff,reclen)) >= 0) {
	    len = rs ;
	    rs = bprintf(ofp,"%r",obuf,len) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procout) */


