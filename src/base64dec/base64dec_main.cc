/* base64dec_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* test the |base64(3uc)| subroutines */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* debugging */
#define	CF_TEXT		1		/* text */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:
	This program tests the BASE64 object.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<cstdio>
#include	<cstring>		/* |strcmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<ulogerror.h>
#include	<usupport.h>
#include	<umem.hh>
#include	<getfdfile.h>		/* |FD_STDIN| */
#include	<libf.h>		/* |fwrite(3f)| */
#include	<ascii.h>
#include	<base64.h>
#include	<ccfile.hh>
#include	<bufos.hh>
#include	<rmx.h>			/* |rmeol(3uc)| */
#include	<strnul.hh>
#include	<mkchar.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */
#include	<dprintf.h>		/* debugging */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	MI		maininfo

#define	OUTLEN		BASE64_OUTLEN

#define	BASE64_LINELEN	76
#define	BASE64_BUFLEN	((BASE64_LINELEN / 4) * 3)

#undef	BUFLEN
#define	BUFLEN		(100 * BASE64_BUFLEN)

#ifndef	CF_DEBUG
#define	CF_DEBUG	1		/* debugging */
#endif
#ifndef	CF_TEXT
#define	CF_TEXT		1		/* text */
#endif


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libu::umem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct maininfo_fl {
    	uint		dummy:1 ;
    	uint		text:1 ;
    	uint		needcr:1 ;
    	uint		test:1 ;
} ; /* end struct (maininfo_fl) */

struct maininfo {
    	FILE		*ofp = stdout ;
    	maininfo_fl	fl{} ;
	char		*inbuf{} ;
	char		*fbuf{} ;
	char		*stagebuf{} ;
	int		inlen ;
	int		flen ;
	int		stagelen = 0 ;
	int		stagel = 0 ;
	int start() noex ;
	int finish() noex ;
	int enc(cchar *) noex ;
	int enc_choose(cchar *) noex ;
	int enc_flush() noex ;
	int enc_text(cchar *) noex ;
	int enc_textload(bufos *,cchar *) noex ;
	int enc_textflush(bufos *) noex ;
	int enc_binary(cchar *) noex ;
	int procln(bufos *,cchar *,int) noex ;
	int procdata(bufos *,cchar *,int) noex ;
	int procstage(bufos *,cchar *,int) noex ;
	int procgroup(bufos *,cchar *,int) noex ;
	int procpartial(bufos *) noex ;
	int procout(bufos *) noex ;
	int procouter(bufos *) noex ;
	int putout(bufos *bp,int ch) noex ;
	int putflush(bufos *bp) noex ;
	void dtor() noex ;
	destruct maininfo() noex {
	    if (stagebuf || fbuf || inbuf) dtor() ;
	} /* end dtor */
} ; /* end struct (maininfo) */


/* forward references */


/* local variables */

cbool		f_debug = CF_DEBUG ;
cbool		f_text	= CF_TEXT ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv) {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		ex = EXIT_SUCCESS ;
	if (argc > 0) {
	    if (maininfo mi ; (rs = mi.start()) >= 0) {
	        for (int ai = 1 ; ai < argc ; ai += 1) {
		    if (cchar *arg = argv[ai] ; arg && arg[0]) {
			DPRINTF("arg=%s\n",arg) ;
		        rs = mi.enc(arg) ;
		    } /* end if (argument) */
		    if (rs < 0) break ;
	        } /* end for */
	        rs1 = mi.finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (maininfo) */
	} /* end if (arguments) */
	if ((ex == EXIT_SUCCESS) && (rs < 0)) {
	    ex = EXIT_FAILURE ;
	}
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

int maininfo::start() noex {
    	cnullptr	np{} ;
    	int		rs = SR_NOMEM ;
	fl.text = f_text ;
	stagelen = BASE64_STAGELEN ;
	if ((stagebuf = new(nothrow) char [stagelen + 1]) != np) {
	    flen = BUFLEN ;
	    if ((fbuf = new(nothrow) char [flen + 1]) != np) {
	        rs = SR_OK ;
	    } /* end if (new-char) */
	    if (rs < 0) {
		delete [] stagebuf ;
		stagebuf = nullptr ;
		stagelen = 0 ;
	    }
	} /* end if (new-char) */
	return rs ;
} /* end method (maininfo::start) */

int maininfo::finish() noex {
    	int		rs = SR_OK ;
	if (fbuf) {
	    delete [] fbuf ;
	    fbuf = nullptr ;
	    flen = 0 ;
	}
	if (stagebuf) {
	    delete stagebuf ;
	    stagebuf = nullptr ;
	    stagelen = 0 ;
	}
	return rs ;
} /* end method (maininfo::finish) */

void maininfo::dtor() noex {
	if (cint rs = finish() ; rs < 0) {
	    ulogerror("maininfo",rs,"dtor") ;
	}
} /* end method (maininfo::dtor) */

int maininfo::enc(cchar *fn) noex {
	int		rs ;
	int		rs1 ;
	int		olen = 0 ; /* return-value */
        inlen = (BUFLEN + 4) ;
	if ((rs = umem.vall((inlen + 1),&inbuf)) >= 0) {
	    if ((rs = enc_choose(fn)) >= 0) {
		olen += rs ;
	    } /* end if */
            rs1 = umem.free(inbuf) ;
            if (rs >= 0) rs = rs1 ;
	    inbuf = nullptr ;
	    inlen = 0 ;
        } /* end if (m-a-f) */
	return (rs >= 0) ? olen : rs ;
} /* end method (maininfo::enc) */

int maininfo::enc_choose(cchar *fn) noex {
    	int		rs = SR_OK ;
	int		olen = 0 ;
            if (fl.text) {
		rs = enc_text(fn) ;
		olen = rs ;
            } else {
		rs = enc_binary(fn) ;
		olen = rs ;
	    } /* end if (text or binary) */
	return (rs >= 0) ? olen : rs ;
} /* end method (maininfo::enc_choose) */

int maininfo::enc_text(cchar *fn) noex {
    	int		rs ;
	int		rs1 ;
	int		olen = 0 ; /* return-value */
	if (bufos ob ; (rs = ob.start) >= 0) {
	    if ((rs = enc_textload(&ob,fn)) >= 0) {
		olen += rs ;
		rs = enc_textflush(&ob) ;
		olen += rs ;
	    }
	    rs1 = ob.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bufos) */
	return (rs >= 0) ? olen : rs ;
} /* end method (maininfo::enc_text) */

int maininfo::enc_textload(bufos *obp,cchar *fn) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		olen = 0 ;
	DPRINTF("ent\n") ;
        if (ccfile in ; (rs = in.open(fn,"r")) >= 0) {
	    while ((rs = in.readln(inbuf,inlen)) > 0) {
		if_constexpr (f_debug) {
		    cint rl = rmeol(inbuf,rs) ;
		    {
		    strnul ps(inbuf,rl) ;
	            DPRINTF("inbuf=>%s<\n",ccp(ps)) ;
		    }
		}
		if ((rs = procln(obp,inbuf,rs)) >= 0) {
		    rs = procout(obp) ;
                    olen += rs ;
		}
		if (rs < 0) break ;
	    } /* end while */
            rs1 = in.close ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (opnened input file) */
	DPRINTF("ret rs=%d olen=%d\n",rs,olen) ;
	return (rs >= 0) ? olen : rs ;
} /* end method (maininfo::enc_textload) */

int maininfo::enc_textflush(bufos *obp) noex {
    	int		rs = SR_NOMEM ;
	int		olen = 0 ;
	DPRINTF("ent\n") ;
	if (obp) {
	    if ((rs = procpartial(obp)) >= 0) {
	        if ((rs = procout(obp)) >= 0) {
	            olen += rs ;
	            rs = procouter(obp) ;
	            olen += rs ;
		}
	    }
	}
	DPRINTF("ret rs=%d olen=%d\n",rs,olen) ;
	return (rs >= 0) ? olen : rs ;
} /* end method (maininfo::enc_textflush) */

int maininfo::enc_binary(cchar *fn) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		olen = 0 ; /* return-value */
	int         ifd = FD_STDIN ;
	DPRINTF("ent ifd=%d fn=%s\n",ifd,fn) ;
	if (fn[0] != '-') {
	    rs = u_open(fn,O_RDONLY,0666) ;
	    ifd = rs ;
	} /* end if (std-input) */
	if (rs >= 0) {
	    while ((rs = u_read(ifd,inbuf,inlen)) > 0) {
		if_constexpr (f_debug) {
		    cint rl = rmeol(inbuf,rs) ;
		    {
		    strnul ps(inbuf,rl) ;
	            DPRINTF("inbuf=>%s<\n",ccp(ps)) ;
		    }
		}
		rs = outbase64(this,inbuf,rs) ;
		olen += rs ;
		if (rs < 0) break ;
	    } /* end while */
            rs1 = u_close(ifd) ;
            if (rs >= 0) rs = rs1 ;
	} /* end if (ok) */
	DPRINTF("ret rs=%d olen=%d\n",rs,olen) ;
	return (rs >= 0) ? olen : rs ;
} /* end method (maininfo::enc_binary) */

int maininfo::procln(bufos *obp,cchar *lp,int ll) noex {
    	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	DPRINTF("ent ll=%d\n",ll) ;
	if (ll > 0) {
	    if (hasneedcr(lp,ll) && fl.needcr) {
		if ((rs = procdata(obp,lp,(ll - 1))) >= 0) {
		    char buf[2] = { char(CH_CR) } ;
		    c += rs ;
		    rs = procdata(obp,buf,1) ;
		    c += rs ;
		}
	    } else {
		rs = procdata(obp,lp,ll) ;
		c += rs ;
	    }
	} /* end if (non-zero positive) */
	DPRINTF("ret rs=%d c=%d\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end method (maininfo::procln) */

int maininfo::procdata(bufos *obp,cchar *lp,int ll) noex {
    	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	if ((stagel > 0) && (stagel < stagelen)) {
	    if ((rs = procstage(obp,lp,ll)) > 0) {
		lp += rs ;
		ll -= rs ;
		c += rs ;
	    }
	} /* end if (partial stage fill) */
	if ((rs >= 0) && (ll > 0)) {
	    while (ll >= stagelen) {
		rs = procgroup(obp,lp,ll) ;
		lp += stagelen ;
		ll -= stagelen ;
		c += stagelen ;
		if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (ll > 0)) {
		if ((rs = procstage(obp,lp,ll)) > 0) {
		    lp += rs ;
		    ll -= rs ;
		    c += rs ;
		}
	    } /* end if (remainder) */
	} /* end if (popping line data) */
	return (rs >= 0) ? c : rs ;
} /* end method (maininfo::procdata) */

int maininfo::procstage(bufos *obp,cchar *lp,int ll) noex {
    	int		rs = SR_OK ;
	int		ml = 0 ;
	if (stagel < stagelen) {
	    if ((ml = min((stagelen - stagel),ll)) > 0) {
	        memcopy((stagebuf + stagel),lp,ml) ;
		stagel += ml ;
		if (stagel == stagelen) {
		    rs = procgroup(obp,stagebuf,stagel) ;
		    stagel = 0 ;
		}
	    }
	} /* end if (sgage open for storage) */
	return (rs >= 0) ? ml : rs ;
} /* end method (maininfo::procstage) */

int maininfo::procgroup(bufos *obp,cchar *sp,int sl) noex {
    	int		rs = SR_BUGCHECK ;
	if (sl >= stagelen) {
	    char	outbuf[OUTLEN+1] ;
	    rs = SR_OK ;
	    if (cint len = base64_e(sp,stagelen,outbuf) ; len > 0) {
	        rs = obp->add(outbuf,len) ;
	    }
	} /* end if (ok) */
	return rs ;
} /* end method (maininfo::procgroup) */

int maininfo::procpartial(bufos *obp) noex {
    	int		rs = SR_OK ;
	if (obp) {
	    cint	sl = stagel ;
	    cchar	*sp = stagebuf ;
	    char	outbuf[OUTLEN+1] ;
	    if (cint len = base64_e(sp,sl,outbuf) ; len > 0) {
	        rs = obp->add(outbuf,len) ;
	    }
	} /* end if (ok) */
	return rs ;
} /* end method (maininfo::procpartial) */

int maininfo::procout(bufos *obp) noex {
    	cint		linelen = BASE64_LINELEN ;
    	int		rs ;
	int		olen = 0 ;
	DPRINTF("ent\n") ;
	while ((rs = obp->len) >= linelen) {
	    DPRINTF("len=%d\n",rs) ;
	    rs = procouter(obp) ;
	    olen += rs ;
	    if (rs < 0) break ;
	} /* end while (sufficient data to write out) */
	DPRINTF("ret rs=%d olen=%d\n",rs,olen) ;
	return (rs >= 0) ? olen : rs ;

} /* end method (maininfo::procout) */

int maininfo::procouter(bufos *obp) noex {
    	cint		linelen = BASE64_LINELEN ;
    	int		rs ;
	int		olen = 0 ;
	DPRINTF("ent\n") ;
	if ((rs = obp->len) > 0) {
	    cint readlen = min(flen,linelen) ;
	    DPRINTF("len=%d readlen=%d\n",rs,readlen) ;
	    if ((rs = obp->read(fbuf,readlen)) > 0) {
	        DPRINTF("read rs=%d\n",rs) ;
	        cint len = rs ;
	        if ((rs = obp->adv(rs)) >= 0) {
		    if ((rs = fwriter(ofp,fbuf,len)) >= 0) {
		        olen += rs ;
		        rs = fputch(ofp,CH_NL) ;
		        olen += rs ;
		    }
	        } /* end if (adv) */
	    } /* end if */
	} /* end if (sufficient data to write out) */
	DPRINTF("ret rs=%d olen=%d\n",rs,olen) ;
	return (rs >= 0) ? olen : rs ;

} /* end method (maininfo::procouter) */

int maininfo::putflush(bufos *bp) noex {
    	int		rs = SR_NOMEM ;
	int		olen = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (fbuf) {
	    int	 c = 0 ;
	    cint mlen = min(BASE64_LINELEN,flen) ;
	    while ((rs = bp->read(fbuf,mlen)) > 0) {
		if ((rs = bp->adv(rs)) >= 0) {
		    c += rs ;
	            rs = outbase64(this,fbuf,rs) ;
		    olen += rs ;
		}
		if (rs < 0) break ;
	    } /* end while */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d olen=%d\n",rs,olen) ;
	return (rs >= 0) ? olen : rs ;
} /* end method (maininfo::putflush) */

int maininfo::putout(bufos *bp,int ch) noex {
	int		rs ;
	int		olen = 0 ;
	if ((rs = bp->add(ch)) >= 0) {
	    if ((rs = bp->count) >= BUFLEN) {
		rs = putflush(bp) ;
		olen += rs ;
	    }
	}
	return (rs >= 0) ? olen : rs ;
}
/* end method (maininfo::putout) */


