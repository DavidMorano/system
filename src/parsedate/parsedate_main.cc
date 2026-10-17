/* parsedate_main SUPPOE (parsedate) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* the "parsedate" daemon */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time */
#define	CF_GETDATE2	0

/* revision history:

	= 1997-05-09, David A­D­ Morano
	This is is the original writing of this subroutine and program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	We parse date-strings here.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cerrno>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<char.h>		/* LIBUC */
#include	<strtime.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"config.h"
#include	"defs.h"
#include	"incfile_datemsk.h"


/* local defines */

#define	MAXARGINDEX	100
#define	MAXARGGROUPS	(MAXARGINDEX/8 + 1)


/* local structures */

#define	ARGOPT_ROOT	0
#define	ARGOPT_VERSION	1
#define	ARGOPT_TMPDIR	2

constexpr cpcchar	argopts[] = {
	"ROOT",
	"VERSION",
	"TMPDIR",
	nullptr
} ; /* end array */


/* extern subroutines */

extern time_t	getabsdate(), getindate() ;

extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mktmpfile(char *,mode_t,cchar *) ;

extern char	*malloc_str(), *malloc_sbuf() ;
extern char	*strbasename() ;


/* external variables */


/* forward references */

static struct tm	*getdate2() ;

local void		openerrfile() ;
local void		printout() ;


/* external variables */

struct global	g ;


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv) {
	bfile		infile, *ifp = nullptr ;
	bfile		outfile, *ofp = &outfile ;
	bfile		errfile ;
	TM		ts, *timep ;
	ustat	sb ;
	int	argr, argl, aol, akl, avl, npa, maxai, kwi ;
	int	ngi, i, j ;
	int	rs ;
	int	len ;
	int	f_optminus, f_optplus, f_optequal ;
	int	f_extra = false ;
	int	f_usage = false ;
	int	f_done = false ;
	int	f_version = false ;
	int	efd ;

	char	*argp, *aop, *akp, *avp ;
	char	argpresent[MAXARGGROUPS] ;
	char	tmpfname[MAXPATHLEN + 1] ;
	char	mskfname_buf[MAXPATHLEN + 1] ;
	char	*mskfname = nullptr ;
	char	*datestring = nullptr ;
	char	buf[BUFSIZE + 1] ;
	char	linebuf[LINELEN + 1] ;
	char	*cp, *cp1, *cp2 ;


#if	CF_DEBUG || CF_DEBUG
	if (fstat(3,&sb) >= 0) {
	    debugsetfd(3) ;
	} else {
	    while ((efd = dup(2)) < 3) ;
	    debugsetfd(efd) ;
	}
#endif

#if	CF_DEBUG
	debugprintf("main: entered\n") ;
#endif

	g.progname = strbasename(argv[0]) ;

	efd = dup(2) ;

	g.efp = nullptr ;
	if (bopen(&errfile,(char *) efd,"dwca",0644) >= 0)
	    g.efp = &errfile ;


/* get some program information */

	if ((g.programroot = getenv("PROGRAMROOT")) == nullptr)
	    g.programroot = PROGRAMROOT ;


/* initialize some stuff before command line argument processing */

	if ((g.tmpdir = getenv("TMPDIR")) == nullptr)
	    g.tmpdir = TMPDIR ;

	g.debuglevel = 0 ;

	g.f.verbose = false ;
	g.f.quiet = false ;
	g.f.daemon = false ;
	g.f.tmpfile = false ;
	g.f.alternate = false ;


/* process program arguments */

	rs = SR_OK ;

	npa = 0 ;			/* number of positional so far */
	maxai = 0 ;
	i = 0 ;
	argr = argc - 1 ;
	while ((rs >= 0) && (! f_done) && (argr > 0)) {

	    argp = argv[++i] ;
	    argr -= 1 ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 0) && (f_optminus || f_optplus)) {

	        if (argl > 1) {

	            aop = argp + 1 ;
	            akp = aop ;
	            aol = argl - 1 ;
	            f_optequal = false ;
	            if ((avp = strchr(aop,'=')) != nullptr) {

	                akl = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                aol = akl ;
	                f_optequal = true ;

	            } else {

	                avl = 0 ;
	                akl = aol ;

	            }

/* do we have a keyword match or should we assume only key letters ? */

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

	                case ARGOPT_ROOT:
	                    if (f_optequal) {

	                        f_optequal = false ;
	                        if (avl) g.programroot = avp ;

	                    } else {

	                        if (argr <= 0) goto badargnum ;

	                        argp = argv[++i] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;

	                        if (argl) g.programroot = argp ;

	                    }

	                    break ;

/* a temporary directory */
	                case ARGOPT_TMPDIR:
	                    if (f_optequal) {

	                        f_optequal = false ;
	                        if (avl) g.tmpdir = avp ;

	                    } else {

	                        if (argr <= 0) goto badargnum ;

	                        argp = argv[++i] ;
	                        argr -= 1 ;
	                        argl = strlen(argp) ;

	                        if (argl) g.tmpdir = argp ;

	                    }

	                    break ;

/* version */
	                case ARGOPT_VERSION:
	                    f_version = true ;
	                    if (f_optequal) goto badargextra ;

	                    break ;

/* default action and user specified help */
	                default:
	                    rs = SR_INVALID ;
	                        bprintf(g.efp,
	                            "%s: unknown argument keyword \"%s\"\n",
	                            g.progname,aop) ;

	                    f_usage = true ;
	                    f_done = true ;
	                    break ;

	                } /* end switch (key words) */

	            } else {

	                while (akl--) {

	                    switch ((int) *akp) {

	                    case 'V':
	                        f_version = true ;
	                        break ;

	                    case 'D':
	                        g.debuglevel = 1 ;
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (cfdec(avp,avl, &g.debuglevel) < 0)
	                                goto badargvalue ;

	                        }

	                        break ;

	                    case 'v':
	                        g.f.verbose = true ;
	                        break ;

	                    case 'a':
	                        g.f.alternate = true ;
	                        break ;

	                    case 'd':
	                        g.f.daemon = true ;
	                        break ;

/* quiet mode */
	                    case 'q':
	                        g.f.quiet = true ;
	                        break ;

/* get a DATEMSK file */
	                    case 'm':
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) mskfname = avp ;

	                        } else {

	                            if (argr <= 0) goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) mskfname = argp ;

	                        }

	                        break ;

/* date string */
	                    case 's':
	                        if (f_optequal) {

	                            f_optequal = false ;
	                            if (avl) datestring = avp ;

	                        } else {

	                            if (argr <= 0) goto badargnum ;

	                            argp = argv[++i] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;

	                            if (argl) datestring = argp ;

	                        }

	                        break ;

	                    case '?':
	                        f_usage = true ;
				break ;

	                    default:
				rs = SR_INVALID ;
	                            bprintf(g.efp.
	                            "%s: invalid option=%c\n",
	                            pip->progname,*akp) ;

	                    } /* end switch */

	                    akp += 1 ;
			    if (rs < 0)
				break ;

	                } /* end while */

	            } /* end if (individual option key letters) */

	        } else {

	            if (i < MAXARGINDEX) {

	                BASET(argpresent,i) ;
	                maxai = i ;
	                npa += 1 ;	/* increment position count */

	            }

	        } /* end if */

	    } else {

	        if (i < MAXARGGROUPS) {

	            BASET(argpresent,i) ;
	            maxai = i ;
	            npa += 1 ;

	        } else {

	            if (! f_extra) {

	                f_extra = true ;
	                if (g.efp != nullptr)
	                    bprintf(g.efp,"%s: extra arguments ignored\n",
	                        g.progname) ;

	            }
	        }

	    } /* end if (key letter/word or positional) */

#if	CF_DEBUG
	    debugprintf("main: bottom of loop\n") ;
#endif

	} /* end while (all command line argument processing) */

#if	CF_DEBUG
	debugprintf("main: finished parsing arguments\n") ;
#endif

	if (f_version) {

	    if (g.efp != nullptr)
	        bprintf(g.efp,"%s: version %s\n",
	            g.progname,VERSION) ;

	}

	if (g.debuglevel > 0) {

	    if (g.efp != nullptr)
	        bprintf(g.efp,"%s: debuglevel %d\n",
	            g.progname,g.debuglevel) ;

	}

	if (f_usage) goto usage ;

	if (f_version) goto earlyret ;


/* process the arguments */

	if ((stat(g.tmpdir,&sb) < 0) && (! S_ISDIR(sb.st_mode)))
	    g.tmpdir = TMPDIR ;


	mskfname_buf[0] = '\0' ;
	if (mskfname == nullptr) {

#if	CF_DEBUG
	    if (g.debuglevel > 0)
	        debugprintf("main: using internal DATEMSK specifications\n") ;
#endif

	    mkapth2(tmpfname,g.tmpdir,"pdXXXXXXXXXXXX") ;

	    rs = mktmpfile(tmpfname, 0664, mskfname_buf) ;
	    if (rs < 0)
	        goto badtmpmk ;

	    g.f.tmpfile  = true ;
	    mskfname = mskfname_buf ;
	    if ((rs = u_open(mskfname,O_WRONLY,0664)) < 0)
	        goto badmskopen ;

	    write(rs,incfile_datemsk,INCFILELEN_datemsk) ;

	    close(rs) ;

	} /* end if (no DATEMSK file was given) */

	if (access(mskfname,R_OK) != 0) {

	    rs = SR_NOENT ;
	    goto badmskfile ;
	}

	bufprintf(buf,BUFSIZE,"DATEMSK=%s",mskfname) ;

	putenv(buf) ;

	g.mskfname = mskfname ;


/* open the files that we need for normal operation */

	if ((rs = bopen(ofp,BFILE_STDOUT,"dwct",0666)) < 0)
	    goto badoutopen ;


	if ((datestring != nullptr) && (datestring[0] != '\0')) {

	    printout(datestring,ofp,g.efp) ;

	} else {

	    ifp = &infile ;
	    if ((rs = bopen(ifp,BFILE_STDIN,"dr",0666)) < 0)
	        goto badinopen ;

/* let's boogie */

	    while ((len = breadln(ifp,linebuf,LINELEN)) > 0) {

	        if (linebuf[len - 1] == '\n') len -= 1 ;

	        linebuf[len] = '\0' ;
	        cp = linebuf ;
	        while (CHAR_ISWHITE(*cp)) cp += 1 ;

	        if (cp[0] == '\n') continue ;

/* do it, baby ! */

	        printout(cp,ofp,g.efp) ;

	    } /* end while */

	    rs = len ;

	    bclose(ifp) ;

	} /* end if (opened input file) */

	bclose(ofp) ;

	if (g.f.tmpfile && (mskfname_buf[0] != '\0'))
	    unlink(mskfname_buf) ;

#if	CF_DEBUG
	if (g.debuglevel > 0)
	debugprintf("main: exiting w/ rs=%d\n",rs) ;
#endif

earlyret:
	if (g.efp != nullptr)
	    bclose(g.efp) ;

	return rs ;

/* handle the bad things */
badret:
	if (ifp != nullptr)
	    bclose(ifp) ;

	bclose(ofp) ;

	if (g.f.tmpfile && (mskfname_buf[0] != '\0'))
	    unlink(mskfname_buf) ;

	if (g.efp != nullptr)
	    bclose(g.efp) ;

	return BAD ;

/* program usage */
usage:
	if (g.efp == nullptr) goto badret ;

	bprintf(g.efp,
	    "%s: USAGE> %s [-s date_string] [-m datemskfile] [-d]\n", 
	    g.progname,g.progname) ;

	bprintf(g.efp,
	    " [-VD?]\n") ;

	goto badret ;

badargextra:
	if (g.efp == nullptr) goto badret ;

	bprintf(g.efp,"%s: no value associated with this option key\n",
	    g.progname) ;

	goto badret ;

badargvalue:
	if (g.efp == nullptr) goto badret ;

	bprintf(g.efp,"%s: bad argument value was specified\n",
	    g.progname) ;

	goto badret ;

badargnum:
	if (g.efp == nullptr) goto badret ;

	bprintf(g.efp,"%s: not enough arguments specified\n",
	    g.progname) ;

	goto badret ;

badinopen:
	if (g.f.quiet || g.f.daemon) goto badret ;

	if (g.efp == nullptr) goto badret ;

	bprintf(g.efp,"%s: could not open input file, rs=%d\n",
	    g.progname,rs) ;

	goto badret ;

badoutopen:
	if (g.f.quiet || g.f.daemon) goto badret ;

	if (g.efp == nullptr) goto badret ;

	bprintf(g.efp,"%s: could not open output file, rs=%d\n",
	    g.progname,rs) ;

	goto badret ;

badtmpmk:
	if (g.f.quiet || g.f.daemon) goto badret ;

	if (g.efp == nullptr) goto badret ;

	bprintf(g.efp,"%s: could not make a temporary file, rs=%d\n",
	    g.progname,rs) ;

	goto badret ;

badmskopen:
badmskfile:
	if (g.f.quiet || g.f.daemon) goto badret ;

	if (g.efp == nullptr) goto badret ;

	bprintf(g.efp,"%s: could not open the mask file, rs=%d\n",
	    g.progname,rs) ;

	goto badret ;

}
/* end subroutine (main) */



local void printout(datestring,ofp,efp)
char	datestring[] ;
bfile	*ofp, *efp ;
{
	struct tm	*timep ;

	time_t		time ;

	int		len, f_good = false  ;

	char		*tp ;
	char		timebuf[TIMEBUFLEN + 1], timebuf2[TIMEBUFLEN + 1] ;


#ifdef	COMMENT

#if	defined(SYSV) && CF_GETDATE2
	if (g.f.alternate)
	timep = getdate2(datestring) ;

	else
#endif
	timep = getdate(datestring) ;

/* try our selves ! */

	if (timep == nullptr) {

	    len = strlen(datestring) ;

	    tp = datestring + len ;
	    while ((tp > datestring) && CHAR_ISWHITE(tp[-1])) tp -= 1 ;

	    while ((tp > datestring) && isdigit(tp[-1])) tp -= 1 ;

	    if ((tp > datestring) && (tp[-1] == '-')) {

	        tp -= 1 ;
	        while ((tp > datestring) && CHAR_ISWHITE(tp[-1])) tp -= 1 ;

	        *tp = '\0' ;

#if	defined(SYSV) && CF_GETDATE2
		if (g.f.alternate)
	        timep = getdate2(datestring) ;

		else
#endif
	        timep = getdate(datestring) ;

	    time = mktime(timep) ;

	    }

	} else
	    time = mktime(timep) ;

	if (timep != nullptr) f_good = true ;

#else

	time = getindate(datestring,nullptr) ;

#if	CF_DEBUG
	if (g.debuglevel > 1)
	    debugprintf("printout: 1st time=%s\n",
		strtime_log(time,timebuf)) ;
#endif

	if (time == ((time_t) -1)) {

		time = getabsdate(datestring,nullptr) ;

#if	CF_DEBUG
	if (g.debuglevel > 1) {
	    debugprintf("printout: 2nd time=%s\n",
		strtime_gmlog(time,timebuf)) ;
#endif

		if (time != ((time_t) -1)) f_good = true ;

	} else {
		f_good = true ;
	}

#endif

	if (! f_good) {

	    if (g.f.daemon) {
	        bprintf(ofp,"BAD\n") ;
	    } else {
	        bprintf(ofp,"*\n") ;
	    }
	    return ;

	} else {
	    if (g.f.daemon) {
	        bprintf(ofp,"OK %ld\n",time) ;
	    } else {
	        bprintf(ofp,"%s LOC, %s GMT\n",
			strtime_log(time,timebuf),
			strtime_gmlog(time,timebuf2)) ;
	    }
	}
} /* end subroutine (printout) */


#if	defined(SYSV) && CF_GETDATE2

static struct tm	tm_getdate ;

static struct tm *getdate2(s)
char	s[] ;
{
	bfile	mskfile, *mfp = &mskfile ;

	TM	ts, *timep ;

	int	len, i ;

	char	linebuf[LINELEN + 1] ;
	char	*cp ;


#if	CF_DEBUG
	if (g.debuglevel > 1)
	    debugprintf("getdate2: entered\n") ;
#endif

	if ((s == nullptr) || (s[0] == '\0')) return nullptr ;

	if (bopen(mfp,g.mskfname,"r",0666) < 0) return nullptr ;

	i = 0 ;
	timep = nullptr ;
	while ((len = breadln(mfp,linebuf,LINELEN)) > 0) {

	    if (linebuf[len - 1] == '\n') len -= 1 ;

	    linebuf[len] = '\0' ;

	    cp = strptime(s,linebuf,&ts) ;

	    if (cp != nullptr) break ;

	    i += 1 ;
	}

	bclose(mfp) ;

	if (cp == nullptr) return nullptr ;

#if	CF_DEBUG
	if (g.debuglevel > 0)
	    debugprintf("getdate: found match at line %d\n",i) ;
#endif

	memcpy(&tm_getdate,&ts,sizeof(struct tm)) ;

	return &tm_getdate ;
}

#endif /* defined(SYSV) && CF_GETDATE2 */



