/* cmd_save SUPPORT */
/* lang=C++20 */

/* save a bulletin board article */
/* version %I% last-modified %G% */

#define	CF_PWCACHE	1		/* use 'ugetpw(3uc)' */

/* revision history:

	= 1994-02-01, David A­D­ Morano
	I wrote this from scratch when I took over the code. The
	previous code was a mess (still is in many places!).

	= 1998-11-22, David A­D­ Morano
	Some clean-up.

*/

/* Copyright © 1994,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Save a bulletin board article to either a mailbox or to standard
	output depending on the mode we are called with.

	mode=SMODE_MAILBOX	save article to mailbox
	mode=SMODE_OUT		write article to standard output


*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<pwd.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<ugetpw.h>
#include	<getax.h>
#include	<bfile.h>
#include	<dater.h>
#include	<mailmsgmatenv.h>
#include	<char.h>
#include	<localmisc.h>

#include	"artlist.h"
#include	"headerkeys.h"
#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	MAILFOLDER
#define	MAILFOLDER	"mail"
#endif

#ifndef	MAILNEW
#define	MAILNEW		"new"
#endif

#ifndef	GNAMELEN
#define	GNAMELEN	80		/* GECOS name length */
#endif

#ifndef	REALNAMELEN
#define	REALNAMELEN	100		/* real name length */
#endif

#if	CF_PWCACHE
#define	GETPW_NAME	ucpwcache_name
#define	GETPW_UID	ucpwcache_uid
#else
#define	GETPW_NAME	getpw_name
#define	GETPW_UID	getpw_uid
#endif /* CF_PWCACHE */

#define	PI		PROGINFO
#define	AENT		ARTLIST_ENT


/* external subroutines */

extern int	snsd(char *,int,cchar *,uint) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	mkmailname(char *,int,cchar *,int) ;

extern int	bbcpy(char *,cchar *) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	strbasename(char *) ;
extern char	*timestr_edate(time_t,char *) ;
extern char	*timestr_hdate(time_t,char *) ;


/* external variables */


/* forward references */

static int	matenv(cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int cmd_save(PI *pip,AENT *ap,cc *ngdir,cc *afname,int mode,cc *mailbox) noex {
	USTAT		stat_a ;
	USTAT		sb ;
	bfile		afile, *afp = &afile ;
	bfile		mfile, *mfp = &mfile ;
	off_t	offset ;
	time_t		t ;
	long		flen ;
	cint		llen = LINEBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		line ;
	int		clen, len ;
	int		f_bol, f_eol, f_clen = FALSE ;
	int		f_from = FALSE ;
	int		f_newsgroups = FALSE ;
	int		f_date = FALSE ;
	int		f_articleid = FALSE ;
	int		f_nofile = FALSE ;
	int		f_envesc = FALSE ;
	cchar	*cp ;
	char		ngname[MAXPATHLEN + 1] ;
	char		maildname[MAXPATHLEN + 1] ;
	char		mfname[MAXPATHLEN + 1] ;
	char		lbuf[LINEBUFLEN + 1] ;
	char		hv_date[LINEBUFLEN + 1] ;
	char		env_from[LINEBUFLEN + 1] ;
	char		env_date[LINEBUFLEN + 1] ;
	char		from_username[USERNAMELEN + 1] ;
	char		from_realname[REALNAMELEN + 1] ;

	if (ngdir == NULL) return SR_FAULT ;
	if (afname == NULL) return SR_FAULT ;

	if (ngdir[0] == '\0') return SR_INVALID ;
	if (afname[0] == '\0') return SR_INVALID ;

	mfname[0] = '\0' ;
	if (mode == SMODE_MAILBOX) {

	    if (mailbox == NULL)
	        mailbox = "new" ;

	    mkpath2(maildname, pip->homedname,MAILFOLDER) ;

	    mkpath2(mfname,maildname,mailbox) ;

	} /* end if (save to mailbox) */

/* get the current time */

	pip->now.time = time(NULL) ;

/* get status of current file */

	flen = -1 ;
	rs1 = bopen(afp,afname,"r",0666) ;
	if (rs1 < 0)
	    goto ret0 ;

	rs1 = bcontrol(afp,BC_STAT,&stat_a) ;
	if (rs1 < 0)
	    goto ret1 ;

	flen = stat_a.st_size ;

/* OK, we start */

	if (mode == SMODE_MAILBOX) {

/* if the directory for mailboxes does not exist, create it */

	    f_nofile = FALSE ;
	    if ((rs = u_stat(maildname,&sb)) < 0) {

/* check if the directory is there or not */

	        if (rs == SR_NOENT) {

	            if (u_mkdir(maildname,0775) < 0) {

	                f_nofile = TRUE ;
	                bprintf(pip->efp,
	                    "%s: couldn't create mail directory (%d)\n",
	                    pip->progname,rs) ;

	            }

	        } else {

/* can't access it */

	            f_nofile = TRUE ;
	            bprintf(pip->efp,
	                "%s: can't access mail directory,",
	                pip->progname) ;

	            bprintf(pip->efp," please check permissions\n") ;

	        }

	    } /* end if (creating missing user mail directory) */

	    if (! f_nofile) {

/* open mailbox and append new message */

	        rs = bopen(mfp,mfname,"wca",0664) ;
		if (rs < 0) mfp = NULL ;

	    } else
	        rs = BAD ;

	} else {

	    mfp = pip->ofp ;

	} /* end if (mailbox or regular mode) */

	if (rs < 0)
	    goto ret1 ;

/* get information about this article */

	from_username[0] = '\0' ;
	from_realname[0] = '\0' ;
	{
	    struct passwd	pw ;
	    const uid_t		uid = stat_a.st_uid ;
	    cint		pwlen = getbufsize(getbufsize_pw) ;
	    char		*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        if ((rs = GETPW_UID(&pw,pwbuf,pwlen,uid)) >= 0) {
	    	    strwcpy(from_username,pw.pw_name,USERNAMELEN) ;
	    	    mkmailname(from_realname,REALNAMELEN,pw.pw_gecos,-1) ;
		} else {
	    	    snsd(from_username,USERNAMELEN,"G",uid) ;
	    	    strwcpy(from_realname,from_username,REALNAMELEN) ;
		} /* end if */
		uc_free(pwbuf) ;
	    } /* end if (memory-allocation) */
	}

	offset = 0 ;

/* process the header lines */

	f_bol = TRUE ;
	line = 0 ;
	while ((rs = breadln(afp,lbuf,llen)) > 0) {
	    len = rs ;

	    f_eol = (lbuf[len - 1] == '\n') ? TRUE : FALSE ;
	    lbuf[len] = '\0' ;
	    offset += len ;
	    if (f_bol) {

	        if (lbuf[0] == '\n')
	            break ;

/* check for the envelope header line */

	        if ((line == 0) && pip->fl.addenv) {
	            cchar	*envfromstr ;
	            t = pip->now.time ;
	            if (pip->fl.envdate) {
	                switch (pip->whichenvdate) {
	                case SORTMODE_MTIME:
	                    t = stat_a.st_mtime ;
	                    break ;
	                case SORTMODE_ATIME:
	                    if ((t = ap->atime) == 0) {
	                        t = ap->mtime ;
			    }
	                    break ;
	                case SORTMODE_PTIME:
	                    if ((t = ap->ptime) == 0)
	                        if ((t = ap->atime) == 0) {
	                            t = ap->mtime ;
				}
			    }
	                    break ;
	                case SORTMODE_CTIME:
	                    if ((t = ap->ctime) == 0)
	                        if ((t = ap->ptime) == 0)
	                            if ((t = ap->atime) == 0) {
	                                t = ap->mtime ;
				    }
				}
			    }
	                    break ;
	                case SORTMODE_NOW:
	                default:
	                    t = pip->now.time ;
	                    break ;
	                case SORTMODE_SUPPLIED:
	                    dater_gettime(&pip->envdate,&t) ;
	                    break ;
	                } /* end switch */
	            } /* end if (whichenvdate) */
	            if (! pip->fl.envfrom) {
	                sncpy3(env_from,LINEBUFLEN,
	                    pip->mailhost,"!",from_username) ;
	            } else {
	                envfromstr = pip->envfrom ;
		    }
	            bprintf(mfp,
	                "From %s %s\n",
	                envfromstr,
	                timestr_edate(t,env_date)) ;
	            f_envesc = TRUE ;
	        } /* end if (adding an envelope) */

/* write an envelope if it does not already have one */

	        if ((line == 0) && (! f_envesc) &&
	            (! matenv(lbuf,-1))) {

	            bprintf(mfp,
	                "From %s!%s %s\n",
	                pip->mailhost,from_username,
	                timestr_edate(stat_a.st_mtime,env_date)) ;

	            f_envesc = FALSE ;

	        } /* end if (writing an envelope header) */

/* possibly escape all other envelopes (since we added one) */

	        if (f_envesc && matenv(lbuf,-1) && (lbuf[0] != '>')) {
	            bputc(mfp,'>') ;
	        } /* end if (escaping extra envelopes) */

/* all other headers */

	        if (strncasecmp(lbuf,"content-length",14) == 0) {
	            f_clen = TRUE ;
	        } else if ((strncasecmp(lbuf,HK_NEWSGROUPS,10)) == 0) {
	            f_newsgroups = TRUE ;
	        } else if ((strncasecmp(lbuf,HK_ARTICLEID,10)) == 0) {
	            f_articleid = TRUE ;
	        } else if ((strncasecmp(lbuf,HK_DATE,4)) == 0) {
	            f_date = TRUE ;
	        } else if ((strncasecmp(lbuf,HK_BOARD, 5)) == 0) {
	            cp = lbuf + 5 ;
	            while (CHAR_ISWHITE(*cp)) {
	                cp += 1 ;
		    }
	            if (*cp == ':') {
	                cp += 1 ;
	                while (CHAR_ISWHITE(*cp)) {
	                    cp += 1 ;
			}
	                bprintf(mfp,"%s: %s",HK_NEWSGROUPS,cp) ;
	            } /* end if */
	            len = 0 ;
	            f_newsgroups = TRUE ;
	        } else if (strncasecmp(lbuf,"from",4) == 0) {
	            cp = lbuf + 4 ;
	            while (CHAR_ISWHITE(*cp)) {
	                cp += 1 ;
		    }
	            if (*cp == ':') {
			cp += 1 ;
		    }
	            while (CHAR_ISWHITE(*cp)) {
	                cp += 1 ;
		    }
	            if (*cp == '\n') {
	                len = 0 ;
		    } else {
	                f_from = TRUE ;
		    }
	        } else if ((strncasecmp(lbuf,HK_TITLE,5)) == 0) {
	            cp = lbuf + 5 ;
	            while (CHAR_ISWHITE(*cp)) {
	                cp += 1 ;
		    }
	            if (*cp == ':') {
	                cp += 1 ;
		    }
	            while (CHAR_ISWHITE(*cp)) {
	                cp += 1 ;
		    }
	            bprintf(mfp,"%s:    %s\n",HK_SUBJECT,cp) ;
	            len = 0 ;
	        } /* end if */
	    } /* end if (BOL processing) */

	    rs = bwrite(mfp,lbuf,len) ;
	    if (rs < 0)
		break ;

	    if (f_eol)
	        line += 1 ;

	    f_bol = f_eol ;

	} /* end while (header lines) */

	if (rs < 0)
	    goto ret2 ;

/* end of reading the original header lines for this article */


/* add header lines that may not have been there! */

/* article ID length */

	if (! f_articleid) {
	    int		cl ;
	    cchar	*cp ;
	    if ((cl = sfbasename(afname,-1,&cp)) > 0) {
	        bprintf(mfp,"%s: %r\n",HK_ARTICLEID,cp,cl) ;
	    }
	}

/* content length */

	if (flen >= 0) {
	    clen = flen - offset ;
	    if (! f_clen) {
	        bprintf(mfp,"%s:  %d\n",HK_CLEN,clen) ;
	    }
	}

/* "from" person */

	if (! f_from) {
	    if (from_realname != NULL) {
	        if (from_username != NULL) {
	            bprintf(mfp, "%s:       %s <%s!%s>\n",
	                HK_FROM,from_realname,
	                pip->mailhost,from_username) ;
	        } else {
	            bprintf(mfp, "%s:       %s <%s!pcs>\n",
	                HK_FROM,from_realname,
	                pip->mailhost) ;
		}
	    } else {
	        bprintf(mfp, "%s:       %s\n",
	            HK_FROM,from_username) ;
	    }
	} /* end if */

/* which newsgroup */

	if (! f_newsgroups) {
	    bbcpy(ngname,ngdir) ;
	    bprintf(mfp,"%s: %s\n",HK_NEWSGROUPS,ngname) ;
	}

/* date of posting */

	if ((! f_date) && (bcontrol(afp,BC_STAT,&sb) >= 0)) {
	    timestr_hdate(sb.st_mtime,hv_date) ;
	    bprintf(mfp,"%s:       %s\n",HK_DATE,hv_date) ;
	} /* end if (posting date) */

/* to whom! (not needed) */

#ifdef	COMMENT
	bprintf(mfp,"%s:       %s\n",HK_TO,pip->username) ;
#endif

/* write out the end-of-header marker (a blank line) */

	bprintf(mfp,"\n") ;

/* copy the bulletin over to the mailbox */

	rs = bcopyblock(afp,mfp,clen) ;

/* close up stuff */
ret2:
	if ((mode == SMODE_MAILBOX) && (mfp != NULL))
	    bclose(mfp) ;

ret1:
	bclose(afp) ;

ret0:
	return rs ;
}
/* end subroutine (cmd_save) */


/* local subroutines */

static int matenv(cchar *sp,int sl) noex {
	mailmsgenv	e ;
	return mailmsgmatenv(&e,sp,sl) ;
}
/* end subroutine (matenv) */



