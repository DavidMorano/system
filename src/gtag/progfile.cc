/* progfile (Program-File) */
/* lang=C99 */

/* process a file by inserting bibliographical references */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debug print-outs */
#define	CF_MULTICITE	1		/* allow multiple citations */


/* revision history:

	= 1992-03-01, David A�D� Morano
	This code was originally written.

	= 1998-09-01, David A�D� Morano
        This subroutine was modified to process the way MMCITE does citation. It
        used to use the old GNU 'lookbib' program in addition to the (old)
        standard UNIX version. But neither of these are used now. Straight out
        citeation keywrd lookup is done directly in a BIB database (files of
        which are suffixed 'rbd').

*/

/* Copyright � 1992,1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

        This subroutine processes a file by looking up and inserting the
        bibliographical references into the text. All input is copied to the
        output with the addition of the bibliographical references.


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>

#include	<usystem.h>
#include	<paramopt.h>
#include	<bfile.h>
#include	<vecstr.h>
#include	<field.h>
#include	<ascii.h>
#include	<char.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"bdb.h"
#include	"citedb.h"


/* local defines */

#define	SUBINFO		struct subinfo
#define	MBDINFO		struct mbdinfo


/* external subroutines */

extern int	sfshrink(const char *,int,const char **) ;
extern int	sfsub(const char *,int,const char **) ;
extern int	nextfield(const char *,int,const char **) ;
extern int	sicite(const char *,int,const char *,int) ;
extern int	silbrace(const char *,int) ;
extern int	matstr(cchar **,cchar *,int) ;

extern int	bprinter(bfile *,int,cchar *,int) ;
extern int	findbibfile(PROGINFO *,PARAMOPT *,cchar *,char *) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) ;
extern int	debugprinthex(cchar *,int,cchar *,int) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern char	*strnchr(cchar *,int,int) ;


/* external variables */


/* local structures */

struct subinfo {
	PARAMOPT	*app ;
	BDB		*bdbp ;
	CITEDB		*cdbp ;
	int		fi ;
} ;

struct mbdinfo {
	const char	*pp ;
	const char	*rp ;
	const char	*kp ;
	uint		loff ;
	int		pl ;
	int		rl ;
	int		kl ;
} ;


/* forward references */

static int	procmacro(PROGINFO *,SUBINFO *,cchar *,int,int) ;
static int	procescape(PROGINFO *,SUBINFO *,MBDINFO *) ;

static int	mbdmacro(PROGINFO *,cchar *,int) ;
static int	mbdmacrofiles(PROGINFO *,VECSTR *,cchar *,int) ;
static int	mbdescape(PROGINFO *,MBDINFO *,uint,cchar *,int) ;


/* local variables */

static const char	*macronames[] = {
	BIBMACRO1,
	BIBMACRO2,
	NULL
} ;

/****
	  9	(tab)
	 10	(new line)
	 11	(vertical tab)
	 12	(form feed)
	 13	(carriage return)
	 32	(space)
	 44	,
****/

static const uchar	fterms[] = {
	0x00, 0x3E, 0x00, 0x00,
	0x01, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported subroutines */


int progfile(PROGINFO *pip,PARAMOPT *app,BDB *bdbp,CITEDB *cdbp,cchar *fname)
{
	SUBINFO		pc ;
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;
	const char	*cp ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile: fname=%s\n",fname) ;
#endif

	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	memset(&pc,0,sizeof(SUBINFO)) ;
	pc.app = app ;
	pc.bdbp = bdbp ;
	pc.cdbp = cdbp ;

/* enter the filename into storage for possible need later (on error) */

	cp = fname ;
	if (cp[0] == '-')
	    cp = "*STDIN*" ;

	if ((rs = vecstr_add(&pip->filenames,cp,-1)) >= 0) {
	    bfile	ifile, *ifp = &ifile ;
	    bfile	*tfp = &pip->tf.tfile ;
	    pc.fi = rs ;

/* proceed to open the file */

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("progfile: open file\n") ;
#endif

	    if (fname[0] == '-') fname = BFILE_STDIN ;

	    if ((rs = bopen(ifp,fname,"r",0666)) >= 0) {
	        MBDINFO		info ;
	        const int	llen = LINEBUFLEN ;
	        uint		foff = pip->tf.tlen ;
	        int		ll ;
	        int		li ;
	        int		f_bol, f_eol ;
	        cchar		*lp ;
	        char		lbuf[LINEBUFLEN + 1] ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("progfile: while-above\n") ;
#endif

	        f_bol = TRUE ;
	        while ((rs = breadln(ifp,lbuf,llen)) > 0) {
	            uint	loff = (foff+tlen) ;
	            int		len = rs ;

	            f_eol = (lbuf[len - 1] == '\n') ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(2))
	                debugprintf("progfile: begin loff=%u\n",loff) ;
#endif

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("progfile: line=>%r<\n",lbuf,
	                    ((lbuf[len - 1] == '\n') ? (len - 1) : len)) ;
#endif

/* check for macros and escapes */

	            lp = lbuf ;
	            ll = (f_eol) ? (len - 1) : len ;
	            if (f_bol && ((li = mbdmacro(pip,lp,ll)) > 0)) {
	                rs = procmacro(pip,&pc,lp,ll,li) ;
	                tlen += rs ;
	            } else if (mbdescape(pip,&info,loff,lp,ll)) {
	                rs = procescape(pip,&pc,&info) ;
	                tlen += rs ;
	            } else {
	                rs = bprinter(tfp,f_eol,lp,ll) ;
	                tlen += rs ;
	            } /* end if (specialized processing) */

	            f_bol = f_eol ;
	            if (rs < 0) break ;
	        } /* end while (reading input lines) */

	        rs1 = bclose(ifp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (file-open) */
	    pip->tf.tlen += tlen ;

	    if (rs < 0) {
	        vecstr_del(&pip->filenames,pc.fi) ;
	        pc.fi = -1 ;
	    }
	} /* end if (vecstr_add) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile: ret rs=%d tlen=%u\n",rs,tlen) ;
#endif

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (progfile) */


/* local subroutines */


static int procmacro(PROGINFO *pip,SUBINFO *pcp,cchar *lp,int ll,int li)
{
	VECSTR		mbs ;
	int		rs ;
	int		rs1 ;
	int		tlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progfile: have macro\n") ;
#endif

	if ((rs = vecstr_start(&mbs,10,0)) >= 0) {
	    if ((rs = mbdmacrofiles(pip,&mbs,(lp + li),(ll - li))) >= 0) {
		bfile	*tfp = &pip->tf.tfile ;
	        int	i ;
		cchar	*cp ;
	        char	tmpfname[MAXPATHLEN+1] ;

	        for (i = 0 ; vecstr_get(&mbs,i,&cp) >= 0 ; i += 1) {
	            if (cp != NULL) {

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("progfile: BIB fname=%s\n",cp) ;
#endif

	            rs1 = SR_OK ;
	            if (cp[0] != '/') {
	                ustat	sb ;

#if	CF_DEBUG
	                if (DEBUGLEVEL(3))
	                    debugprintf("progfile: BIB non-rooted\n") ;
#endif

	                rs1 = u_stat(cp,&sb) ;

	                if ((rs1 < 0) || (! S_ISREG(sb.st_mode))) {

	                    rs1 = findbibfile(pip,pcp->app,cp,tmpfname) ;

#if	CF_DEBUG
	                    if (DEBUGLEVEL(3))
	                        debugprintf("progfile: "
	                            "findbibfile() rs=%d\n", rs1) ;
#endif

	                    cp = tmpfname ;

	                } /* end if (didn't exist locally) */

	            } /* end if (not-rooted) */

	            if (rs1 >= 0) {

	                rs = bdb_add(pcp->bdbp,cp) ;

#if	CF_DEBUG
	                if (DEBUGLEVEL(3))
	                    debugprintf("progfile: bdb_add() rs=%d\n",
	                        rs) ;
#endif

	            }

		    } /* end if (have) */
	            if (rs < 0) break ;
	        } /* end for */

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("progfile: mid rs=%d\n",rs) ;
#endif

	        if (rs >= 0) {
	            const int	all= (ll-1) ;
	            cchar	*alp = (lp+1) ;
	            rs = bprintf(tfp,".\\\"_ %r\n",alp,all) ;
	            tlen = rs ;
	        }

	    } /* end if (mbdmacrofiles) */
	    rs1 = vecstr_finish(&mbs) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mbs) */

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (procmacro) */


static int procescape(PROGINFO *pip,SUBINFO *pcp,MBDINFO *eip)
{
	bfile		*tfp = &pip->tf.tfile ;
	uint		loff = eip->loff ;
	uint		coff ;
	int		rs = SR_OK ;
	int		sl ;
	int		tlen = 0 ;
	const char	*sp ;

	if (eip->pl > 0) {

	    rs = bwrite(tfp,eip->pp,eip->pl) ;
	    tlen += rs ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("progfile: leader pl=%u bwrite() rs=%d\n",
		eip->pl,rs) ;
#endif

	}

/* process this key */

	if (rs >= 0) {
	    coff = (loff + tlen) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("progfile: citeoff=%u\n",coff) ;
#endif

	    rs = citedb_adds(pcp->cdbp,pcp->fi,coff,eip->kp,eip->kl) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("progfile: citedb_adds() rs=%d\n",rs) ;
#endif

	} /* end if */

	sp = eip->rp ;
	sl = eip->rl ;
	while ((rs >= 0) && mbdescape(pip,eip,loff,sp,sl)) {

	    if (eip->pl > 0) {
	        rs = bwrite(tfp,eip->pp,eip->pl) ;
	        tlen += rs ;
	    }

/* process this key */

	    if (rs >= 0) {
	        coff = (loff+tlen) ;
	        rs = citedb_adds(pcp->cdbp,pcp->fi,coff,eip->kp,eip->kl) ;
	    }

	    sp = eip->rp ;
	    sl = eip->rl ;

	} /* end while */

	if ((rs >= 0) && (sl >= 0)) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("progfile: final=>%r<\n",sp,sl) ;
#endif

	    rs = bprinter(tfp,0,sp,sl) ;
	    tlen += rs ;
	}

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (procescape) */


/* do we have a MBD macro? */
static int mbdmacro(PROGINFO *pip,cchar *lp,int ll)
{
	int		rs = SR_OK ;
	int		li = 0 ;

	if (ll < 0)
	    ll = strlen(lp) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile/mbdmacro: lp=>%r<\n",
	        lp,
	        ((lp[ll - 1] == '\n') ? (ll - 1) : ll)) ;
#endif

	if ((ll >= 2) && (lp[0] == '.')) {
	    int		cl ;
	    cchar	*cp ;
	    if ((cl = nextfield((lp + 1),(ll - 1),&cp)) > 0) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(2)) {
	            debugprintf("progfile/mbdmacro: nextfield() rs=%d\n",rs) ;
	            if (cl > 0)
	                debugprintf("progfile/mbdmacro: cp=>%r<\n",cp,cl) ;
	        }
#endif

	        if (matstr(macronames,cp,cl) >= 0) {
	            li = (cp + cl) - lp ;
	        }

	    } /* end if (nextfield) */
	} /* end if (big enough) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile/mbdmacro: ret rs=%d li=%u\n",rs,li) ;
#endif

	return (rs >= 0) ? li : rs ;
}
/* end subroutine (mbdmacro) */


/* extract the RBD file names from an RBD macro invocation */
static int mbdmacrofiles(PROGINFO *pip,VECSTR *flp,cchar *lp,int ll)
{
	int		rs = SR_OK ;
	int		c = 0 ;

	if (ll < 0)
	    ll = strlen(lp) ;

	if (ll >= 1) {
	    FIELD	fsb ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("progfile/mbdmacrofiles: line=>%r<\n",
	            lp,
	            ((lp[ll - 1] == '\n') ? (ll - 1) : ll)) ;
#endif

	    if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	        int	fl ;
	        cchar	*fp ;

	        while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {
	            if (fl > 0) {

#if	CF_DEBUG
	            if (DEBUGLEVEL(2))
	                debugprintf("progfile/mbdmacrofiles: "
			    "field=>%r<\n", fp,fl) ;
#endif

	                c += 1 ;
	                rs = vecstr_add(flp,fp,fl) ;

		    }
	            if (rs < 0) break ;
	        } /* end while */

	        field_finish(&fsb) ;
	    } /* end if */

	} /* end if (big enough) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile/mbdmacrofiles: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mbdmacrofiles) */


static int mbdescape(PROGINFO *pip,MBDINFO *ip,uint loff,cchar *lp,int ll)
{
	const int	el = strlen(BIBESCAPE) ;
	int		si ;
	int		f = FALSE ;

	memset(ip,0,sizeof(MBDINFO)) ;
	ip->loff = loff ;
	ip->pp = lp ;
	ip->pl = ll ;
	ip->kp = NULL ;
	ip->kl = 0 ;
	ip->rp = lp ;
	ip->rl = ll ;

	if ((si = sicite(lp,ll,BIBESCAPE,el)) >= 0) {
	    int		sl, cl ;
	    cchar	*tp ;
	    cchar	*sp, *cp ;
	    ip->pl = si ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("progfile/mbdescape: pl=%u\n",ip->pl) ;
#endif

	    sp = (lp + (si + el + 1)) ;
	    sl = ll - (si + el + 1) ;
	    if ((si = silbrace(sp,sl)) >= 0) {

	        cp = (sp + si + 1) ;
	        cl = sl - (cp - sp) ;
	        tp = strnchr(cp,cl,CH_RBRACE) ;

	        if (tp != NULL) {

	            f = TRUE ;

#if	CF_MULTICITE
	            ip->kp = cp ;
	            ip->kl = (tp - cp) ;
#else
	            ip->kl = nextfield(cp,(tp - cp),&ip->kp) ;
#endif /* CF_MULTICITE */

	            ip->rp = (tp + 1) ;
	            ip->rl = sl - ((tp + 1) - sp) ;

	        } else {
	            f = FALSE ;
	        }

	    } /* end if (open brace) */

	} /* end if (possible escape) */

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("progfile/mbdescape: ret f=%u\n",f) ;
#endif

	return f ;
}
/* end subroutine (mbdescape) */


