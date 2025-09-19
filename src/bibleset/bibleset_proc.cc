/* bibleset_proc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a bible-database file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debugging */
#define	CF_BOOKMARK	0		/* set off book titles w/ marks */
#define	CF_CHAPTERBEGIN	1		/* start chapters big */
#define	CF_CHAPTERLARGE	1		/* user larger pointsize */
#define	CF_NUMLEADHALF	0		/* leading space for verse numbers */
#define	CF_NUMTRAILHALF	1		/* leading space for verse numbers */

/* revision history:

	= 2008-03-01, David A­D­ Morano
	This code was originally written (its a fairly straight-forward
	text processing loop).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bibleset_progfile

	Description:
	Read the given file and process it. (ya, right!)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<field.h>
#include	<wordfill.h>
#include	<storebuf.h>
#include	<ascii.h>
#include	<linecenter.h>
#include	<strn.h>
#include	<six.h>			/* |sileader(3uc)| */
#include	<char.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"config.h"
#include	"defs.h"
#include	"config.h"
#include	"progoff.h"
#include	"biblebook.h"
#include	"biblecur.h"
#include	"siletter.h"


/* local defines */

#define	NVERSEDIG	3		/* digits making a verse number */
#define	NZEROCOLS	40		/* number of columns for verse==0 */

#define	CORNERBUFLEN	120		/* page-corner buffer length */
#define	CWBUFLEN	30		/* buffer for verse-number strings */
#define	FIXBUFLEN	100		/* buffer length for fixing words */

#define	PI		proginfo

#define	BB		biblebook

#define	BC		biblecur


/* external subroutines */

extern int	progoutmtheader(PI *,bfile *) noex ;
extern int	progoutmtfooter(PI *,bfile *) noex ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(cchar *) ;
extern int	debugprintf(cchar *,...) ;
extern int	debugprinthex(cchar *,int,cchar *,int) ;
extern int	debugclose() ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */


/* local structures */


/* forward references */

static int procline(PROGINFO *,void *,BC *,cchar *,int) ;

static int bookstart(PI *,bfile *,BC *,BB *) ;
static int bookend(PI *,bfile *,BC *) ;

static int chapterstart(PI *,bfile *,BC *) ;
static int chapterend(PI *,bfile *,BC *) ;

static int versestartone(PI *,bfile *,BC *) ;
static int versestart(PI *,bfile *,BC *) ;
static int verseend(PI *,bfile *,BC *) ;

static int	handleversezero(PI *,bfile *,cchar *,int) ;

static int	printletter(PI *,bfile *,int,cchar *,int) ;
static int	printversenum(PI *,bfile *,int) ;
static int	printwords(PI *,bfile *,cchar *,int) ;
static int	printversezero(PI *,bfile *) ;
static int	printwordsend(PI *,bfile *) ;

static int	mkfixbuf(PI *,char *,int,cchar *,int) ;
static int	mkfixbufend(PI *,char *,int) ;

static int	setchapter(PI *,bfile *,int) ;
static int	setverse(PI *,bfile *,int) ;

static int	keepstart(PI *,bfile *) ;
static int	keepend(PI *,bfile *) ;

static int	outcols_short(PI *) ;
static int	outcols_get(PI *,int) ;
static int	outcols_normal(PI *) ;



/* local variables */


/* exported variables */


/* exported subroutines */

int bibleset_proc(PI *pip,bfile *ofp,cc *fname) noex {
	bfile		infile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if (fname == nullptr)
	    return SR_FAULT ;

	if (fname[0] == '\0')
	    return SR_INVALID ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progfile: fname=%s\n",fname) ;
#endif

	if (strcmp(fname,"-") == 0) fname = BFILE_STDIN ;

	if ((rs = bopen(&infile,fname,"r",0666)) >= 0) {
	    BIBLECUR	bc ;

	    if ((rs = biblecur_start(&bc)) >= 0) {
	        cint	llen = LINEBUFLEN ;
	        int		len ;
	        int		sl ;
	        cchar	*sp ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = breadln(&infile,lbuf,llen)) > 0) {
	            len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("progfile: line>%r<\n",lbuf,len) ;
#endif

		    if ((sl = sfshrink(lbuf,len,&sp)) > 0) {
			if (sp[0] != '#') {
	                    lbuf[(sp-lbuf)+sl] = '\0' ;
			    rs = procline(pip,ofp,&bc,sp,sl) ;
			    wlen += rs ;
			}
		    }

	            if (rs < 0) break ;
	        } /* end while (reading lines) */

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("progfile: while-out rs=%d\n",rs) ;
#endif

	        if ((rs >= 0) && pip->fl.inverse) {
	            rs = verseend(pip,ofp,&bc) ;
	            wlen += rs ;
	        }

	        if ((rs >= 0) && pip->fl.inchapter) {
	            rs = chapterend(pip,ofp,&bc) ;
	            wlen += rs ;
	        }

	        if ((rs >= 0) && pip->fl.inbook) {
	            rs = bookend(pip,ofp,&bc) ;
	            wlen += rs ;
	        }

	        biblecur_finish(&bc) ;
	    } /* end if (biblecur) */

	    rs1 = bclose(&infile) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progfile: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bibleset_proc) */


/* local subroutines */

static int procline(PROGINFO *pip,void *ofp,BC *bcp,cc *sp,int sl) noex {
	int		rs ;
	int		wlen = 0 ;

	if ((rs = biblecur_check(bcp,sp,sl)) >= 0) {
	    int	si = rs ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
		debugprintf("progfile/procline: biblecur_check() rs=%d\n",
	                    rs) ;
#endif

	    if ((bcp->book != 0) || (! pip->fl.ibz)) {
		BIBLEBOOK	*bbp = &pip->bb ;

#ifdef	COMMENT
	            if ((si = sileader(sp,sl)) > 0) {
	                sp += si ;
	                sl -= si ;
	            }
#else
	            if (si > 0) {
	                sp += si ;
	                sl -= si ;
	            }
#endif /* COMMENT */

/* any changes from the last verse? */

	            pip->fl.setchapter = false ;
	            pip->fl.setverse = false ;
	            if ((rs = biblecur_newbook(bcp,bbp)) > 0) {

	                if ((rs >= 0) && pip->fl.inverse) {
	                    rs = verseend(pip,ofp,bcp) ;
	                    wlen += rs ;
	                }

	                if ((rs >= 0) && pip->fl.inchapter) {
	                    rs = chapterend(pip,ofp,bcp) ;
	                    wlen += rs ;
	                }

	                if ((rs >= 0) && pip->fl.inbook) {
	                    rs = bookend(pip,ofp,bcp) ;
	                    wlen += rs ;
	                }

	                if (rs >= 0) {
	                    rs = bookstart(pip,ofp,bcp,bbp) ;
	                    wlen += rs ;
	                }

	            } /* end if (new book) */

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("progfile/procline: newbook? rs=%d\n",rs) ;
#endif

	            if ((rs >= 0) && biblecur_newchapter(bcp)) {

	                if ((rs >= 0) && pip->fl.inverse) {
	                    rs = verseend(pip,ofp,bcp) ;
	                    wlen += rs ;
	                }

	                if ((rs >= 0) && pip->fl.inchapter) {
	                    rs = chapterend(pip,ofp,bcp) ;
	                    wlen += rs ;
	                }

	                if (rs >= 0) {
	                    rs = chapterstart(pip,ofp,bcp) ;
	                    wlen += rs ;
	                }

	            } /* end if (new chapter) */

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("progfile/procline: newchapter? rs=%d\n",
			rs) ;
#endif

	            if ((rs >= 0) && biblecur_newverse(bcp,sl)) {
	    		SILETTER	ls ;

	                if ((rs >= 0) && pip->fl.inverse) {
	                    rs = verseend(pip,ofp,bcp) ;
	                    wlen += rs ;
	                }

	                if ((rs >= 0) && 
	                    (bcp->book == 19) && (bcp->chapter == 119) && 
	                    ((si = siletter(&ls,sp,sl)) > 0)) {

	                    if (bcp->verse == 1) {
	                        rs = versestartone(pip,ofp,bcp) ;
	                        wlen += rs ;
	                    }

	                    if (rs >= 0) {
				cint	v = bcp->verse ;
	                        rs = printletter(pip,ofp,v,ls.lp,ls.ll) ;
	                        wlen += rs ;
	                    }

	                    sp += si ;
	                    sl -= si ;

	                } /* end if (had a letter) */

	                if (rs >= 0) {
	                    rs = versestart(pip,ofp,bcp) ;
	                    wlen += rs ;
	                }

	            } /* end if (new verse) */

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("progfile/procline: newverse? rs=%d\n",rs) ;
#endif

/* process this verse */

	            if ((rs >= 0) && (sl > 0)) {

	                if (pip->fl.inversezero) {
	                    rs = handleversezero(pip,ofp,sp,sl) ;
	                } else {
	                    rs = printwords(pip,ofp,sp,sl) ;
	                    wlen += rs ;
	                } /* end if (which type of verse) */

#if	CF_DEBUG
	                if (DEBUGLEVEL(3))
	                    debugprintf("progfile/procline: procverse rs=%d\n",
				rs) ;
#endif

	            } /* end if (had something to print) */

	    } /* end if (book enabled for processing) */

	} /* end if (biblecur_check) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("progfile/procline: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procline) */

static int bookstart(PI *pip,bfile *ofp,BC *bcp,BB *bbp) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar	*fmt ;

	if (bcp == nullptr) return SR_FAULT ;
	if (bbp == nullptr) return SR_FAULT ;

	pip->fl.inbook = true ;

	if (rs >= 0) {
	    cint	book = bcp->book ;
	    cchar	*lc = pip->troff.linecomment ;
	    fmt = "%s book start (%u - %s)\n" ;
	    rs = bprintf(ofp,fmt,lc,book,bcp->bookname) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = progoffdss(pip,ofp,"bB",bcp->bookname) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = setchapter(pip,ofp,bcp->chapter) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = setverse(pip,ofp,bcp->verse) ;
	    wlen += rs ;
	}

	if ((rs >= 0) && (bcp->book >= 0)) {
	    if ((rs >= 0) && (! pip->fl.maintextheader)) {
	        rs = progoutmtheader(pip,ofp) ;
	        wlen += rs ;
	    }
	    if (rs >= 0) {
	        rs = bwrite(ofp,".OP\n",-1) ;
	        wlen += rs ;
	    }
	    if ((rs >= 0) && (! pip->fl.maintextfooter)) {
	        rs = progoutmtfooter(pip,ofp) ;
	        wlen += rs ;
	    }
	}

	if (rs >= 0) {
	    rs = bwrite(ofp,".1C\n",-1) ;
	    wlen += rs ;
	}

#if	CF_BOOKMARK
	fmt = ".SP 2\n.ce\n\\*(EM \\s+3%s%s%s\\s-3 \\*(EM\n.SP 2\n" ;
#else
	fmt = ".SP 2\n.ce\n\\s+3%s%s%s\\s-3\n.SP 2\n" ;
#endif

	if ((rs >= 0) && (bcp->bookname[0] != '\0')) {
	    if (rs >= 0) {
	        rs = bprintf(ofp,fmt,
	            pip->troff.infont_b,
	            bcp->bookname,
	            pip->troff.infont_p) ;
	        wlen += rs ;
	    }
	    if ((rs >= 0) && pip->fl.tc) {
	        rs = progofftcadd(pip,ofp,
	            (pip->c.book >= 33),bcp->bookname) ;
	        wlen += rs ;
	    }
	} /* end if */

#ifdef	COMMENT
	if (rs >= 0) {
	    rs = bwrite(ofp,".br\n.2C\n",-1) ;
	    wlen += rs ;
	}
#endif /* COMMENT */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bookstart) */

static int bookend(PI *pip,bfile *ofp,BC *bcp) noex {
	biblebook	*bbp = &pip->bb ;
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (bcp == nullptr) return SR_FAULT ;
	if (bbp == nullptr) return SR_FAULT ;

	if (pip->fl.inbook) {

	    if (rs >= 0) {
	        rs = bprintf(ofp,".br\n%s\n",STR_BOOKEND) ;
	        wlen += rs ;
	    }

	    if (rs >= 0) {
	        cchar	*lc = pip->troff.linecomment ;
		cchar	*fmt = "%s book end\n" ;
	        rs = bprintf(ofp,fmt,lc) ;
	        wlen += rs ;
	    }

	    if (rs >= 0) {
	        rs = bwrite(ofp,".EP\n",-1) ;
	        wlen += rs ;
	    }

	    pip->c.book += 1 ;
	    pip->fl.inbook = false ;

	} /* end if (enabled) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bookend) */

static int chapterstart(PI *pip,bfile *ofp,BC *bcp) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		chapter = bcp->chapter ;

	pip->fl.inchapter = true ;

	if (rs >= 0) {
	    cchar	*com = pip->troff.linecomment ;
	    rs = bprintf(ofp,"%s start chapter=%u\n",com,chapter) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = setchapter(pip,ofp,bcp->chapter) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = setverse(pip,ofp,bcp->verse) ;
	    wlen += rs ;
	}

	if (chapter == 1) {
	    if (rs >= 0) {
	        rs = bwrite(ofp,".SP 1\n",-1) ;
	        wlen += rs ;
	        if (rs >= 0) {
	            rs = bwrite(ofp,".2C\n",-1) ;
	            wlen += rs ;
	        }
	    }
	}

	if (chapter > 1) {
	    if (rs >= 0) {
	        rs = bwrite(ofp,".SP 2\n",-1) ;
	        wlen += rs ;
	    }
	}

	if ((rs >= 0) && (chapter > 0)) {
	    cchar	*n ;

	    n = pip->word[word_chapter] ;
	    if (bcp->book == 19)
	        n = pip->word[word_psalm] ;

	    if ((rs = keepstart(pip,ofp)) >= 0) {
	    wlen += rs ;

#if	CF_CHAPTERLARGE
	    if (rs >= 0) {
	        rs = bprintf(ofp,"\\&\\s+1%s %u\\s-1\n",n,chapter) ;
	        wlen += rs ;
	    }
#else
	    if (rs >= 0) {
	        rs = bprintf(ofp,"%s %u\n",n,chapter) ;
	        wlen += rs ;
	    }
#endif /* CF_CHAPTERLARGE */

#ifdef	COMMENT
	    if (rs >= 0) {
	        rs = bwrite(ofp,".SP 0.5\n",-1) ;
	        wlen += rs ;
	    }
#endif /* COMMENT */

#if	CF_CHAPTERBEGIN
	    pip->fl.chapterbegin = true ;
#else
	    rs = keepend(pip,ofp) ;
	    wlen += rs ;
#endif /* CF_CHAPTERBEGIN */
	    } /* end if (keep) */

	} /* end if (chapter > 0) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (chapterstart) */

static int chapterend(PI *pip,bfile *ofp,BC *bcp) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (bcp == nullptr) return SR_FAULT ;

	if (pip->fl.inchapter) {

	    pip->fl.versezerohalf = false ;
	    if (pip->fl.chapterbegin) {
	        pip->fl.chapterbegin = false ;
	        rs = keepend(pip,ofp) ;
	        wlen += rs ;
	    }

	    if (rs >= 0) {
	        rs = bprintf(ofp,".\\%c_ end chapter\n",CH_QUOTE) ;
	        wlen += rs ;
	    }

	    pip->fl.inchapter = false ;

	} /* end if (enabled) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (chapterend) */

static int versestart(PI *pip,bfile *ofp,BC *bcp) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		verse = bcp->verse ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("procfile/versestart: verse=%u\n",verse) ;
#endif

/* finish up some unfinished bussiness */

	if (verse == 1) {
	    rs = versestartone(pip,ofp,bcp) ;
	    wlen += rs ;
	}

	pip->fl.preverseone = false ;

/* start new business */

	pip->fl.inverse = true ;
	if (rs >= 0) {
	    cchar	*com = pip->troff.linecomment ;
	    rs = bprintf(ofp,"%s start verse=%u\n",com,verse) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = setverse(pip,ofp,bcp->verse) ;
	    wlen += rs ;
	}

	if ((verse == 0) && (bcp->book > 0)) {
	    pip->fl.versezerohalf = true ;
	    pip->fl.inversezero = true ;
	    pip->fl.reduced = true ;

	    if (rs >= 0) {
	        rs = linecenter_start(&pip->cv,"R") ;
	    }

	    if (rs >= 0) {
	        rs = bwrite(ofp,".S -1 C\n",-1) ;
	        wlen += rs ;
	    }
	} /* end if */

	if (verse == 0) {
	    pip->fl.quoteblock = true ;
	    if (rs >= 0) {
	        rs = bwrite(ofp,".QS 4\n",-1) ;
	        wlen += rs ;
	    }
	}

	if (verse > 1) {
	    if (rs >= 0) {
	        rs = bwrite(ofp,".br\n",-1) ;
	        wlen += rs ;
	    }
	}

	if (verse <= 1) {
	    if (rs >= 0)
#ifdef	COMMENT
	        rs = bwrite(ofp,".SP 1\n",-1) ;
#else
	    rs = bwrite(ofp,".br\n",-1) ;
#endif /* COMMENT */
	    wlen += rs ;
	}

	if (verse > 0) {
	    if (rs >= 0) {
	        rs = printversenum(pip,ofp,verse) ;
	        wlen += rs ;
	    }
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (versestart) */

static int verseend(PI *pip,bfile *ofp,BC *bcp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("procfile/verseend: ent\n") ;
#endif

	if (pip->fl.inverse) {
	    cint	verse = (bcp->verse - 1) ;

	    if (pip->fl.inversezero) {
	        if (rs >= 0) {
	            rs = printversezero(pip,ofp) ;
	            wlen += rs ;
	        }
	    } /* end if (ending verse=0) */

/* print out the end of any words (this guards against corrupted DBs) */

	    if ((rs = printwordsend(pip,ofp)) >= 0) {

/* continue with end-processing for this verse */

	        if (pip->fl.quoteblock) {
	            pip->fl.quoteblock = false ;
	            if (rs >= 0) {
	                rs = bwrite(ofp,".QE\n",-1) ;
	                wlen += rs ;
	            }
	        }

	        if (pip->fl.reduced) {
	            pip->fl.reduced = false ;
	            if (rs >= 0) {
	                rs = bwrite(ofp,".S +1 P\n",-1) ;
	                wlen += rs ;
	            }
	        }

	        if ((rs >= 0) && pip->fl.inversezero) {
	            pip->fl.inversezero = false ;
	            rs1 = linecenter_finish(&pip->cv) ;
		    if (rs >= 0) rs = rs1 ;
	        }

	        pip->fl.inverse = false ;

	        if ((rs >= 0) && (verse > 0)) {
	            rs = keepend(pip,ofp) ;
	            wlen += rs ;
	        }

	        if ((rs >= 0) && (verse == 1) && pip->fl.chapterbegin) {
	            pip->fl.chapterbegin = false ;
	            rs = keepend(pip,ofp) ;
	            wlen += rs ;
	        }

	    } /* end if (printwordsend) */

	} /* end if (in-verse) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (verseend) */

static int versestartone(PI *pip,bfile *ofp,BC *bcp) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		verse = bcp->verse ;
	if ((verse == 1) && (! pip->fl.preverseone)) {
	    pip->fl.preverseone = true ;
	    if (rs >= 0) {
	        rs = bprintf(ofp,".SP %s\n",
	            ((pip->fl.versezerohalf) ? "0.5" : "1")) ;
	        wlen += rs ;
	    }
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (versestartone) */

static int printletter(PI *pip,bfile *ofp,int verse,cc *lp,int ll) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (pip == nullptr) return SR_FAULT ;

	if (verse > 1) {
	    rs = bwrite(ofp,".SP 1\n",-1) ;
	    wlen += rs ;
	}

#ifdef	COMMENT
	if (rs >= 0) {
	    rs = keepstart(pip,ofp) ;
	    wlen += rs ;
	}
#endif /* COMMENT */

	if (rs >= 0) {
	    rs = bprintf(ofp,".ce\n.SM %r\n",lp,ll) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printletter) */

static int printversenum(PI *pip,bfile *ofp,int verse) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	char		digbuf[DIGBUFLEN + 1] ;

	if ((rs = ctdecui(digbuf,DIGBUFLEN,verse)) >= 0) {
	    int	n = rs ;
	    if (n <= NVERSEDIG) {
	        cint	cwbl = CWBUFLEN ;
	        int		i = 0 ;
	        int		j ;
	        cchar	*fmt ;
	        char		cwbuf[CWBUFLEN + 1] ;

	        if (rs >= 0) {
	            rs = storebuf_strw(cwbuf,cwbl,i,"\\&",-1) ;
	            i += rs ;
	        }

#if	CF_NUMLEADHALF
	        if (rs >= 0) {
	            rs = storebuf_strw(cwbuf,cwbl,i,"\\ ",-1) ;
	            i += rs ;
	        }
#endif /* CF_NUMLEADHALF */

	        if (rs >= 0) {
	            rs = storebuf_strw(cwbuf,cwbl,i,pip->troff.infont_cw,-1) ;
	            i += rs ;
	        }

	        for (j = 0 ; (rs >= 0) && (j < (NVERSEDIG - n)) ; j += 1) {
	            rs = storebuf_strw(cwbuf,cwbl,i,"\\0",2) ;
	            i += rs ;
	        }

	        if (rs >= 0) {
	            rs = storebuf_strw(cwbuf,cwbl,i,digbuf,n) ;
	            i += rs ;
	        }

	        if (rs >= 0) {
	            rs = storebuf_strw(cwbuf,cwbl,i,pip->troff.infont_p,-1) ;
	            i += rs ;
	        }

#if	CF_NUMTRAILDHALF
	        fmt = "\\ " ;
#else
	        fmt = "\\0" ;
#endif /* CF_NUMTRAILHALF */

	        if (rs >= 0) {
	            rs = storebuf_strw(cwbuf,cwbl,i,fmt,-1) ;
	            i += rs ;
	        }

	        if (rs >= 0)
	            rs = bwrite(ofp,cwbuf,i) ;

	        outcols_short(pip) ;

	    } else
	        rs = SR_TOOBIG ;
	} /* end if (ctdecui) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printversenum) */

static int handleversezero(PI *pip,bfile *ofp,char *buf,int buflen) noex {
	int		rs = SR_OK ;
	int		sl, cl, wl ;
	int		si ;
	cchar		*tp, *sp, *wp ;
	cchar		*cp ;
	char		fixbuf[FIXBUFLEN + 1] ;

	if (ofp == nullptr) return SR_FAULT ;

	sp = buf ;
	sl = buflen ;
	if ((tp = strnbrk(sp,sl,"[]")) != nullptr) {

	    if (*tp == '[') {
	        cp = (char *) sp ;
	        cl = (tp - sp) ;
	        if ((si = siskipwhite(cp,cl)) > 0) {
	            cp += si ;
	            cl -= si ;
	        }
	        if (cl > 0) {
	            rs = linecenter_addline(&pip->cv,cp,cl) ;
	        }

	        sl -= (tp - sp) ;
	        sp = tp ;

	    } /* end if (easy front-end) */

	    if ((rs >= 0) && (sl > 0)) {

	        while ((cl = nextfield(sp,sl,&cp)) > 0) {

	            wp = fixbuf ;
	            wl = mkfixbuf(pip,fixbuf,FIXBUFLEN,cp,cl) ;

	            if (wl > 0)
	                rs = linecenter_addword(&pip->cv,wp,wl) ;

	            sl -= ((cp + cl) - sp) ;
	            sp = (cp + cl) ;

	            if (rs < 0) break ;
	        } /* end while (finding the words) */

	    } /* end if (processing individual words) */

	} else {
	    rs = linecenter_addline(&pip->cv,buf,buflen) ;
	}

	return (rs >= 0) ? 0 : rs ;
}
/* end subroutine (handleversezero) */

static int printwords(PI *pip,bfile *ofp,cchar *sp,int sl) noex {
	wordfill	w ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		cl, wl ;
	int		si ;
	int		cbl ;
	int		len ;
	int		linewidth = pip->linewidth ;
	int		wlen = 0 ;
	cchar	*tp, *wp ;
	cchar	*cp ;
	char		fixbuf[FIXBUFLEN + 1] ;
	char		colbuf[LINEBUFLEN + 1] ;

	if ((tp = strnbrk(sp,sl,"[]")) != nullptr) {

	    if (*tp == '[') {
	        cp = (char *) sp ;
	        cl = (tp - sp) ;
	        if ((si = siskipwhite(cp,cl)) > 0) {
	            cp += si ;
	            cl -= si ;
	        }
	        if (cl > 0) {
	            cbl = outcols_get(pip,linewidth) ;
	            outcols_normal(pip) ;
	            rs = bprintlns(ofp,cbl,cp,cl) ;
	            wlen += rs ;
	        }

	        sl -= (tp - sp) ;
	        sp = tp ;

	    } /* end if (easy front-end) */

	    if ((rs >= 0) && (sl > 0) &&
	        ((rs = wordfill_start(&w,nullptr,0)) >= 0)) {

	        while ((cl = nextfield(sp,sl,&cp)) > 0) {

	            wp = fixbuf ;
	            wl = mkfixbuf(pip,fixbuf,FIXBUFLEN,cp,cl) ;

	            if (wl > 0)
	                rs = wordfill_addword(&w,wp,wl) ;

	            while (rs >= 0) {

	                cbl = outcols_get(pip,linewidth) ;
	                len = wordfill_mklinefull(&w,colbuf,cbl) ;

	                if (len <= 0)
	                    break ;

	                outcols_normal(pip) ;
	                rs = bprintln(ofp,colbuf,len) ;
	                wlen += rs ;

	            } /* end while (full lines) */

	            sl -= ((cp + cl) - sp) ;
	            sp = (cp + cl) ;

	            if (rs < 0) break ;
	        } /* end while (finding the words) */

	        while (rs >= 0) {

	            cbl = outcols_get(pip,linewidth) ;
	            len = wordfill_mklinepart(&w,colbuf,cbl) ;

	            if (len <= 0)
	                break ;

	            outcols_normal(pip) ;
	            rs = bprintln(ofp,colbuf,len) ;
	            wlen += rs ;

	        } /* end while (partial line) */

	        rs1 = wordfill_finish(&w) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (wordfill) */

	} else {
	    cbl = outcols_get(pip,linewidth) ;
	    outcols_normal(pip) ;
	    rs = bprintlns(ofp,cbl,sp,sl) ;
	    wlen += rs ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printwords) */

static int printwordsend(PI *pip,bfile *ofp) noex {
	cint		flen = FIXBUFLEN ;
	int		rs ;
	int		wlen = 0 ;
	char		fbuf[FIXBUFLEN + 1] ;
	if ((rs = mkfixbufend(pip,fbuf,flen)) >= 0) {
	    if (cint fbl = rs ; fbl > 0) {
	        rs = bprintln(ofp,fbuf,fbl) ;
	        wlen += rs ;
	    }
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printwordsend) */

static int printversezero(PI *pip,bfile *ofp) noex {
	int		rs = SR_OK ;
	int		c ;
	int		ll ;
	int		wlen = 0 ;
	cchar	*lp ;

	if (rs >= 0) {
	    rs = bprintf(ofp,".SP 0.5\n") ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    rs = linecenter_mklines(&pip->cv,pip->vzlw,pip->vzlb) ;
	    c = rs ;
	}

	if (rs >= 0) {
	    rs = bprintf(ofp,".ce %u\n",c) ;
	    wlen += rs ;
	}

	for (int i = 0 ; (rs >= 0) && (i < c) ; i += 1) {

	    rs = linecenter_getline(&pip->cv,i,&lp) ;
	    ll = rs ;
	    if ((rs >= 0) && (ll > 0)) {
	        rs = bprintln(ofp,lp,ll) ;
	        wlen += rs ;
	    }

	} /* end for */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (printversezero) */

static int mkfixbuf(PI *pip,char *buf,int buflen,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		si ;
	int		cl ;
	int		i = 0 ;
	cchar	*tp, *cp ;

#if	CF_DEBUG && 0
	if (DEBUGLEVEL(5))
	    debugprintf("procfile/mkfixbuf: 1>%r<\n",sp,sl) ;
#endif

	buf[0] = '\0' ;
	if ((si = siskipwhite(sp,sl)) > 0) {
	    sp += si ;
	    sl -= si ;
	}

#if	CF_DEBUG && 0
	if (DEBUGLEVEL(5))
	    debugprintf("procfile/mkfixbuf: 2>%r<\n",sp,sl) ;
#endif

	while ((tp = strnbrk(sp,sl,"[]")) != nullptr) {

	    cp = sp ;
	    cl = (tp - sp) ;
	    if (cl > 0) {
	        rs = storebuf_strw(buf,buflen,i,cp,cl) ;
	        i += rs ;
	    }

	    cp = nullptr ;
	    cl = -1 ;
	    if (*tp == '[') {
	        if (pip->c.vmissing++ == 0) {
	            cp = pip->troff.infont_i ;
		}
	    } else {
	        if (pip->c.vmissing > 0) {
	            if (pip->c.vmissing-- == 1) {
	                cp = pip->troff.infont_p ;
		    }
	        }
	    } /* end if */

	    if ((rs >= 0) && (cp != nullptr)) {
	        rs = storebuf_strw(buf,buflen,i,cp,cl) ;
	        i += rs ;
	    }

	    sl -= ((tp + 1) - sp) ;
	    sp = (tp + 1) ;

	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && (sl > 0)) {
	    rs = storebuf_strw(buf,buflen,i,sp,sl) ;
	    i += rs ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkfixbuf) */

static int mkfixbufend(PI *pip,char *buf,int buflen) noex {
	int		rs = SR_OK ;
	int		i = 0 ; /* return-value */
	if (pip->c.vmissing > 0) {
	    cchar	*cp = pip->troff.infont_p ;
	    pip->c.vmissing = 0 ;
	    if (rs >= 0) {
	        rs = storebuf_strw(buf,buflen,i,"\\&",-1) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = storebuf_strw(buf,buflen,i,cp,-1) ;
	        i += rs ;
	    }
	} /* end if (positive) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkfixbufend) */

static int setverse(PI *pip,bfile *ofp,int v) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (! pip->fl.setverse) {
	    pip->fl.setverse = true ;
	    if (v < 1) v = 1 ;
	    rs = progoffdsn(pip,ofp,"bV",v) ;
	    wlen += rs ;
	} /* end if (not set-verse) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (setverse) */

static int setchapter(PI *pip,bfile *ofp,int v) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (! pip->fl.setchapter) {
	    pip->fl.setchapter = true ;
	    if (v < 1) v = 1 ;
	    rs = progoffdsn(pip,ofp,"bC",v) ;
	    wlen += rs ;
	} /* end if (set-chapter) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (setchapter) */

static int keepstart(PI *pip,bfile *ofp) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (! pip->fl.inkeep) {
	    pip->fl.inkeep = true ;

	    if (rs >= 0) {
	        rs = progoffsrs(pip,ofp,"save","bD","\\n(Ds") ;
	        wlen += rs ;
	    }
	    if (rs >= 0) {
	        rs = progoffsrn(pip,ofp,"display_space","Ds",0) ;
	        wlen += rs ;
	    }
	    if (rs >= 0) {
	        rs = bwrite(ofp,".DS L F\n",-1) ;
	        wlen += rs ;
	    }

	    if ((rs >= 0) && (pip->ffi > 0) && (pip->cckeeps == 0)) {
	        rs = progoffsetbasefont(pip,ofp) ;
	        wlen += rs ;
	    }

	    pip->cckeeps += 1 ;

	} /* end if (not in-keep) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (keepstart) */

static int keepend(PI *pip,bfile *ofp) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (pip->fl.inkeep) {
	    pip->fl.inkeep = false ;

	    if (rs >= 0) {
	        rs = bwrite(ofp,".DE\n",-1) ;
	        wlen += rs ;
	    }
	    if (rs >= 0) {
	        rs = progoffsrs(pip,ofp,"display_space","Ds","\\n(bD") ;
	        wlen += rs ;
	    }

	} /* end if (in-keep) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (keepend) */

static int outcols_short(PI *pip) noex {
	pip->fl.tmpshortcol = true ;
	return SR_OK ;
}
/* end subroutine (outcols_short) */

static int outcols_get(PI *pip,int linewidth) noex {
	int		cbl = MIN(linewidth,COLUMNS) ;
	if (pip->fl.tmpshortcol) {
	    cbl = MIN(40,COLUMNS) ;
	}
	return cbl ;
}
/* end subroutine (outcols_get) */

static int outcols_normal(PI *pip) noex {
	pip->fl.tmpshortcol = false ;
	return SR_OK ;
}
/* end subroutine (outcols_normal) */


