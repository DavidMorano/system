/* emit_header SUPPORT */
/* lang=C++20 */

/* print out a header value from the article */
/* version %I% last-modified %G% */

#define	CF_UCPWCACHE	1		/* use 'ugetpw(3uc)' */

/* revision history:

	= 1994-11-01, David A­D­ Morano
	1) added a mode to intercept for mailbox use

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

	= 2018-01-22, David A­D­ Morano
        I updated this to decode coded (mail-message) header values.

*/

/* Copyright © 1994,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	emit_header

	Descrition:
	This subroutine is one of the "EMIT" subroutines used for
	"emitting" articles in different ways.  This subroutine is
	used to print out the "subject" header (if present) of a
	article.

	Synopsis:
	int emit_header(pip,dsp,ai,ap,ngdir,af)
	PROGINFO	*pip ;
	MKDIRLIST_ENT	*dsp ;
	int		ai ;
	ARTLIST_ENT	*ap ;
	char		ngdir[] ;
	char		af[] ;

	Arguments:
	pip		program information pointer
	dsp		user structure pointer
	ai		article index within newsgroup
	ap		article ARTLIST_ENT pointer
	ngdir		directory (relative to spool directory) of article
	af		article base file name

	Returns:
	-		??

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<estrings.h>
#include	<ugetpw.h>
#include	<getax.h>
#include	<bfile.h>
#include	<dater.h>
#include	<mailmsghdrs.h>
#include	<hdrdecode.h>
#include	<localmisc.h>

#include	"headerkeys.h"
#include	"config.h"
#include	"defs.h"
#include	"proghdr.h"
#include	"mkdirlist.h"
#include	"artlist.h"


/* local defines */

#ifndef	HEADBUFLEN
#define	HEADBUFLEN	2048
#endif

#ifndef	BUFLEN
#define	BUFLEN		MAXPATHLEN
#endif

#ifndef	MAXFIELDLEN
#define	MAXFIELDLEN	(COLUMNS-2)
#endif

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#define	GETPW_UID	ucpwcache_uid
#else
#define	GETPW_NAME	getpw_name
#define	GETPW_UID	getpw_uid
#endif /* CF_UCPWCACHE */


/* external subroutines */

extern "C" {
    extern int	bbcpy(char *,cchar *) noex ;
    extern int	mm_getfield() noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	emit_headersubj(PROGINFO *,ARTLIST_ENT *) ;
static int	emit_headerfrom(PROGINFO *,ARTLIST_ENT *) ;


/* local variables */


/* exported variables */


/* exported subroutines */

int emit_header(pip,dsp,ai,ap,ngdir,af)
PROGINFO	*pip ;
MKDIRLIST_ENT	*dsp ;
int		ai ;
ARTLIST_ENT	*ap ;
cchar	ngdir[] ;
cchar	af[] ;
{
	USTAT		mmsb ;
	bfile		afile, *afp = &afile ;
	cint	hlen = HEADBUFLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl ;
	int		cl ;
	cchar	*nd = pip->newsdname ;
	cchar	*cp ;
	char		hbuf[HEADBUFLEN + 1] ;
	char		fname[MAXPATHLEN + 1] ;

	if ((ngdir == nullptr) || (*ngdir == '\0'))
	    return EMIT_OK ;

	if (af == nullptr)
	    return EMIT_OK ;

/* create the full path to this article file */

	mkpath3(fname,nd,ngdir,af) ;

/* this is a hack to only print out the newsgroup name when we have articles */

	if (pip->fl.header) {
	    bbcpy(hbuf,ngdir) ;
	    pip->fl.header = false ;
	    bprintf(pip->ofp,"newsgroup> %s\n",hbuf) ;
	}

/* open the article file and get its status */

	if ((rs = bopen(afp,fname, "r",0666)) >= 0) {
	    if ((rs = bcontrol(afp,BC_STAT,&mmsb)) >= 0) {
		    cchar	*hk ;
		    /* do whatever! */
	            switch (pip->header) {
	            case HI_ARTICLEID:
	                bprintf(pip->ofp,"  %s\n",af) ;
	                break ;
	            case HI_SUBJECT:
	                if (ap->subject != nullptr) {
			    rs = emit_headersubj(pip,ap) ;
	                }
	                break ;
/* try to read out a "from" header from the posted article */
	            case HI_FROM:
			rs = emit_headerfrom(pip,ap) ;
			break ;
/* try to get the date out of the article */
	            case HI_DATE:
	                {
	                    time_t	t ;
	                    char	timebuf[TIMEBUFLEN + 1] ;
		 	    hk = HK_DATE ;
	                    sl = mm_getfield(afp,0L,mmsb.st_size,hk,hbuf,hlen) ;
	                    if (sl > 0) {
	                        rs = dater_setmsg(&pip->tmpdate,hbuf,sl) ;
	                        if (rs >= 0) {
	                            dater_gettime(&pip->tmpdate,&t) ;
	                            bprintf(pip->ofp,"  %s\n",
	                                timestr_edate(t,timebuf)) ;
	                        } else {
	                            bprintf(pip->ofp, "  %s (arrival)\n",
	                                timestr_edate(mmsb.st_mtime,timebuf)) ;
	                        }
	                    } else {
	                        bprintf(pip->ofp, "  %s (arrival)\n",
	                            timestr_edate(mmsb.st_mtime,timebuf)) ;
	                    }
	                } /* end block */
	                break ;
/* message-ID */
	            case HI_MSGID:
			hk = HK_MESSAGEID ;
	                sl = mm_getfield(afp,0L,mmsb.st_size,hk,hbuf,hlen) ;
	                if (sl > 0) {
	                    cint	ilen = MAXNAMELEN ;
	                    char	ibuf[MAXNAMELEN+1] ;
	                    cp = ibuf ;
	                    if ((rs = hdrextid(ibuf,ilen,hbuf,sl)) == 0) {
	                        cchar	*tp ;
	                        cchar	*sp = hbuf ;
	                        int		sl = hlen ;
	                        if ((tp = strnchr(sp,sl,',')) != nullptr) {
				    sl = (tp-hbuf) ;
				}
	                        if ((cl = sfcenter(sp,sl,"<>",&cp)) <= 0) {
	                            cp = sp ;
	                            cl = sl ;
	                        }
	                    }
#ifdef	COMMENT
	                    if ((cp = strchr(hbuf,'<')) != nullptr) {
	                        char	*cp2 ;
	                        cp += 1 ;
	                        if ((cp2 = strchr(cp,'>')) != nullptr) {
	                            *cp2 = '\0' ;
				}
	                    } else {
	                        cp = hbuf ;
	                    }
	                    cp[MAXFIELDLEN] = '\0' ;
#endif /* COMMENT */
	                    if (cl > MAXFIELDLEN) cl = MAXFIELDLEN ;
	                    bprintf(pip->ofp,"  %r\n",cp,cl) ;
	                } else {
	                    bprintf(pip->ofp,"  ** article ID - %s\n",af) ;
	                }
	                break ;
	            } /* end switch */
	    } /* end if (bcontrol) */
	    rs1 = bclose(afp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */

	return (rs >= 0) ? EMIT_DONE : rs ;
}
/* end subroutine (emit_header) */


/* local subroutines */

static int emit_headersubj(PROGINFO *pip,ARTLIST_ENT *ap) noex {
	int		rs ;
	cchar		*sp = ap->subject ;
	if (sp != nullptr) {
	    cint	c = strnlen(ap->subject,MAXFIELDLEN) ;
	    cint	rlen = MAXFIELDLEN ;
	    char	rbuf[MAXFIELDLEN+1] ;
	    if ((rs = proghdr_trans(pip,rbuf,rlen,sp,-1,c)) >= 0) {
	        rs = bprintf(pip->ofp,"  %r\n",rbuf,rs) ;
	    }
	}
	return rs ;
}
/* end subroutine (emit_headersubj) */

static int emit_headerfrom(PROGINFO *pip,ARTLIST_ENT *ap) noex {
	int		rs = SR_OK ;
	int		hl ;
	cchar		*hp ;
	cchar		*hk = HK_FROM ;
	if (ap->from != nullptr) {
	    cint	flen = strlen(ap->from) ;
	    char	*fbuf ;
	    if ((rs = uc_malloc((flen+1),&fbuf)) >= 0) {
		int	c = flen ;
		if (c > MAXFIELDLEN) c = MAXFIELDLEN ;
		if ((rs = mkaddrfrom(fbuf,flen,ap->from,-1)) >= 0) {
		    rs = bprintf(pip->ofp,"  %r\n",fbuf,rs) ;
		}
		uc_free(fbuf) ;
	    } /* end if (m-a-f) */
	} else {
	    char	fname[MAXNAMELEN+1] ;
	    if ((rs = mkpath2(fname,ap->ngdir,ap->name)) >= 0) {
	        USTAT	sb ;
	        if ((rs = uc_stat(fname,&sb)) >= 0) {
		    cint	ulen = USERNAMELEN ;
		    char	ubuf[USERNAMELEN+1] ;
		    if ((rs = getusername(ubuf,ulen,sb.st_uid)) >= 0) {
		        rs = bprintf(pip->ofp,"  %r\n",ubuf,rs) ;
		    }
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		} /* end if (uc_stat) */
	    } /* end if (mkpath) */
	}
	return rs ;
}
/* end subroutine (emit_headerfrom) */


