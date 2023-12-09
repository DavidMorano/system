/* defproc */
/* lang=C20 */

/* process a "def" (define) file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	NameL
	defproc

	Description:
	This subroutine will read (process) an "def" (define) file
	and put all of the environment variables into an VECSTR
	object (supplied). New environment variables just get added
	to the list. Old environment variables already on the list
	are deleted with a new definition is encountered.

	Synopsis:
	int defproc(vecstr *dlp,cchar **envv,EXPCOOK *clp,cchar *fname) noex

	Arguments:
	dlp		defines list pointer
	envv		process environment
	clp		cookies list pointer
	fname		file to process

	Returns:
	>=0		count of environment variables
	<0		bad

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<bfile.h>
#include	<field.h>
#include	<ascii.h>
#include	<char.h>
#include	<buffer.h>
#include	<expcook.h>
#include	<localmisc.h>

#include	"defproc.h"


/* local defines */

#ifndef	FBUFLEN
#define	FBUFLEN		(4 * MAXPATHLEN)
#endif

#define	ENVNAMELEN	100		/* should be sufficient? */

#define	BUFLEN		(4 * MAXPATHLEN)

#define	SUBINFO		struct subinfo


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	nextfield(cchar *,int,cchar **) ;
extern int	vstrkeycmp(cchar **,cchar **) ;
extern int	vecstr_envadd(VECSTR *,cchar *,cchar *,int) ;
extern int	isprintlatin(int) ;

extern int	getev(cchar **,cchar *,int,cchar **) ;
extern int	sfthing(cchar *,int,cchar *,cchar **) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;


/* external variables */


/* local structures */

struct subinfo {
	cchar	**envv ;
	EXPCOOK		*clp ;
	vecstr		*dlp ;
} ;


/* forward references */

static int	procline(SUBINFO *,cchar *,int) ;
static int	checkdeps(SUBINFO *,cchar *,int) ;
static int	procvalues(SUBINFO *,BUFFER *,cchar *,cchar *,int) ;
static int	procvalue(SUBINFO *,BUFFER *,cchar *,cchar *,int) ;
static int	procsubenv(SUBINFO *,BUFFER *,cchar *,int) ;


/* local variables */

static const uchar	fterms[32] = {
	0x00, 0x1A, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

static const uchar	dterms[32] = {
	0x00, 0x02, 0x00, 0x00,
	0x01, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

static cchar	ssb[] = {
	CH_LBRACE, CH_RBRACE, 0
} ;

#ifdef	COMMENT
static cchar	ssp[] = {
	CH_LPAREN, CH_RPAREN, 0
} ;
#endif /* COMMENT */


/* exported subroutines */

int defproc(vecstr *dlp,cchar **envv,EXPCOOK *clp,cchar *fname) noex {
	SUBINFO		li, *lip = &li ;
	bfile		loadfile, *lfp = &loadfile ;
	int		rs ;
	int		c = 0 ;

	lip->envv = envv ;
	lip->clp = clp ;
	lip->dlp = dlp ;

	if ((rs = bopen(lfp,fname,"r",0666)) >= 0) {
	    const int	llen = LINEBUFLEN ;
	    int		len ;
	    int		cl ;
	    cchar	*cp ;
	    char	lbuf[LINEBUFLEN + 1] ;

	    while ((rs = breadlns(lfp,lbuf,llen,-1,NULL)) > 0) {
	        len = rs ;

	        if (lbuf[len - 1] == '\n') len -= 1 ;
	        lbuf[len] = '\0' ;

	        cp = lbuf ;
	        cl = len ;
	        while (cl && CHAR_ISWHITE(*cp)) {
	            cp += 1 ;
	            cl -= 1 ;
	        }

	        if ((cp[0] == '\0') || (cp[0] == '#'))
	            continue ;

	        rs = procline(lip,cp,cl) ;
		c += rs ;

	        if (rs < 0) break ;
	    } /* end while (reading lines) */

	    bclose(lfp) ;
	} /* end if (file) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (defproc) */


/* local subroutines */


static int procline(SUBINFO *lip,cchar *lbuf,int llen)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl = llen ;
	int		cl ;
	int		len = 0 ;
	int		f_done = FALSE ;
	cchar	*tp, *cp ;
	cchar	*sp = lbuf ;
	cchar	*enp ;

	while (sl && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}

/* extract any dependencies (if we have any) */

	tp = strnpbrk(sp,sl," \t?+=#") ;

	if ((tp != NULL) && (*tp == '?')) {
	    if ((rs1 = checkdeps(lip,sp,(tp - sp))) > 0) {
	        sl -= ((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        while (sl && CHAR_ISWHITE(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        }
	        tp = strnpbrk(sp,sl," \t+=#") ;
	    } else {
		f_done = TRUE ;
	    }
	} /* end if (getting dependencies) */

	if (! f_done) {

	cl = 0 ;
	if ((tp != NULL) && ((*tp == '=') || CHAR_ISWHITE(*tp))) {

	    cl = sfshrink(sp,(tp - sp),&cp) ;

	    sl -= ((tp + 1) - sp) ;
	    sp = (tp + 1) ;

	} /* end if (delimiter) */

	if (cl > 0) {
	    const int	nrs = SR_NOTFOUND ;
	    if ((rs = vecstr_findn(lip->dlp,cp,cl)) == nrs) {
	        BUFFER	b ;
	        char	envname[ENVNAMELEN + 1] ;

	        enp = envname ;
	        snwcpy(envname,ENVNAMELEN,cp,cl) ;

/* loop processing the values */

	        if ((sl >= 0) && ((rs = buffer_start(&b,sl)) >= 0)) {

	            if ((rs = procvalues(lip,&b,ssb,sp,sl)) >= 0) {
	                if ((cl = buffer_get(&b,&cp)) > 0) {
	                    rs = vecstr_envadd(lip->dlp,enp,cp,cl) ;
	                }
	            }

	            len = buffer_finish(&b) ;
	            if (rs >= 0) rs = len ;
	        } /* end if (buffer) */
	    } /* end if (not-already-present) */
	} /* end if (non-zero) */

	} /* end if (not-done) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procline) */


static int checkdeps(SUBINFO *lip,cchar *sp,int sl)
{
	FIELD		fsb ;
	int		rs ;
	int		rs1 = 0 ;

	if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	    int		fl ;
	    cchar	*fp ;

	    while ((fl = field_get(&fsb,dterms,&fp)) >= 0) {
	        if (fl > 0) {
	            rs1 = getev(lip->envv,fp,fl,NULL) ;
		}
	        if (rs1 < 0) break ;
	    } /* end while */

	    field_finish(&fsb) ;
	} /* end if (field) */

	return (rs >= 0) ? (rs1 >= 0) : rs ;
}
/* end subroutine (checkdeps) */


/* process definition values */
static int procvalues(SUBINFO *lip,BUFFER *bp,cchar *ss,cchar *sp,int sl)
{
	FIELD		fsb ;
	int		rs ;
	int		len = 0 ;

	if ((rs = field_start(&fsb,sp,sl)) >= 0) {
	    const int	flen = FBUFLEN ;
	    int		fl ;
	    int		c = 0 ;
	    cchar	*fp ;
	    char	fbuf[FBUFLEN + 1] ;

	    fp = fbuf ;
	    while ((fl = field_sharg(&fsb,fterms,fbuf,flen)) >= 0) {
	        if (fl > 0) {

	            if (c++ > 0) {
	                rs = buffer_char(bp,' ') ;
	                len += rs ;
	            }

	            if (rs >= 0) {
	                rs = procvalue(lip,bp,ss,fp,fl) ;
	                len += rs ;
	            }

	        } /* end if (had a value) */
	        if (fsb.term == '#') break ;
	        if (rs < 0) break ;
	    } /* end while (looping over values) */

	    field_finish(&fsb) ;
	} /* end if (fields) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procvalues) */


static int procvalue(SUBINFO *lip,BUFFER *bp,cchar *ss,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		kl, cl ;
	int		len = 0 ;
	cchar	*kp, *cp ;

	while ((kl = sfthing(sp,sl,ss,&kp)) >= 0) {

	    cp = sp ;
	    cl = ((kp - 2) - sp) ;

	    if (cl > 0) {
	        rs = buffer_strw(bp,cp,cl) ;
	        len += rs ;
	    }

	    if ((rs >= 0) && (kl > 0)) {
	        rs = procsubenv(lip,bp,kp,kl) ;
	        len += rs ;
	    }

	    sl -= ((kp + kl + 1) - sp) ;
	    sp = (kp + kl + 1) ;

	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && (sl > 0)) {
	    rs = buffer_strw(bp,sp,sl) ;
	    len += rs ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procvalue) */


static int procsubenv(SUBINFO *lip,BUFFER *bp,cchar *kp,int kl)
{
	int		rs = SR_OK ;
	int		len = 0 ;

	if (kl >= 0) {
	    int		al = 0 ;
	    int		cl ;
	    cchar	*ap = NULL ;
	    cchar	*tp, *cp ;

	    if ((tp = strnchr(kp,kl,'=')) != NULL) {
	        ap = (tp + 1) ;
	        al = (kp + kl) - (tp + 1) ;
	        kl = (tp - kp) ;
	    }

/* lookup the environment key-name that we have */

	    if ((cl = getev(lip->envv,kp,kl,&cp)) > 0) {
	        rs = buffer_strw(bp,cp,cl) ;
	        len += rs ;
	    } else if (al > 0) {
	        rs = buffer_strw(bp,ap,al) ;
	        len += rs ;
	    }

	} /* end if (non-zero) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (procsubenv) */


