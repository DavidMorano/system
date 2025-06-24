/* localgetorgcode SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the LOCAL organization-code (ORGCODE) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.  This is a minimal
	implementation.

	= 2018-09-14, David A-D- Morano
	I modified this to use |snabbr(3dam)| instead of a local
	custom thing which did the same thing, only not as well.
	So we go with the library solution instead of the local
	custom one.  Note for future: might want to refactor a bit
	to get a loop of attempts to find the ORGCODE rather than
	the current (older) list-like code pattern.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localgetorgcode

	Description:
	This subroutine is used to (try to) get the LOCAL software
	distribution organization-code (ORGCODE).  An (so-called)
	ORGCODE is normally an abbreviation of an organiztion name
	(itself normally multiple words) consisting of the first
	letter of each word, but capitalized.

	Synopsis:
	int localgetorgcode(cchar *pr,char *rbuf,int rlen,cchar *un) noex

	Arguments:
	pr		program root
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer
	un		username

	Returns:
	>=0		length of returned ID
	<0		error in process of creating ID (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<varnames.hh>
#include	<syswords.hh>
#include	<mallocxx.h>
#include	<strlibval.hh>
#include	<sncpyx.h>
#include	<snx.h>
#include	<mkpathx.h>
#include	<getuserhome.h>
#include	<getuserorg.h>
#include	<filereadln.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"localget.h"


/* local defines */

#define	ORGCODEFNAME	"orgcode"


/* external subroutines */


/* external variables */


/* local structures */

enum orgcodercos {
	orgcoderco_finish,
	orgcoderco_overlast
} ;

namespace {
    struct orgcoder ;
    struct orgcoder_co {
	orgcoder	*op{} ;
	int		w = -1 ;
	void operator () (orgcoder *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
        operator int () noex ;
    } ; /* end struct (orgcoder_co) */
    struct orgcoder {
	cchar		*pr ;
	cchar		*un ;
	char		*rbuf ;
	char		*tfname{} ;
	int		rlen ;
	orgcoder_co	finish ;
	orgcoder(cc *p,cc *u,char *b,int l) noex : pr(p), un(u) {
	    rbuf = b ;
	    rlen = l ;
	    finish(this,orgcoderco_finish) ;
	} ;
	int start(char *tb) noex {
	    tfname = tb ;
	    return SR_OK ;
	} ;
	operator int () noex ;
	int userenv() noex ;
	int userconf() noex ;
	int localconf() noex ;
	int sysconf() noex ;
	int orgabbr() noex ;
	int ifinish() noex ;
	~orgcoder() {
	    (void) ifinish() ;
	} ;
    } ; /* end struct (orgcoder) */
}

typedef int (orgcoder::*orgcoder_m)() noex ;


/* forward references */


/* local variables */

static strlibval	orgcode(strlibval_orgcode) ;

static cchar		*etcdname = sysword.w_etcdir ;

constexpr char		ocfname[] = ORGCODEFNAME ;

constexpr orgcoder_m	mems[] = {
	&orgcoder::userenv,
	&orgcoder::userconf,
	&orgcoder::localconf,
	&orgcoder::sysconf,
	&orgcoder::orgabbr,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int localgetorgcode(cchar *pr,char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if ((un == nullptr) || (un[0] == '\0')) un = "-" ;
	    if (pr[0]) {
	        if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
		    orgcoder	oo(pr,un,rbuf,rlen) ;
		    if ((rs = oo.start(tbuf)) >= 0) {
			{
			    rs = oo ;
			    len = rs ;
			}
			rs1 = oo.finish ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (orgcoder) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorgcode) */


/* local subroutines */

int orgcoder::userenv() noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (orgcode && (orgcode[0] != '\0')) {
	    rs = sncpy1(rbuf,rlen,orgcode) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
}

int orgcoder::userconf() noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *hbuf{} ; (rs = malloc_mp(&hbuf)) >= 0) {
	    cint	hlen = rs ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
		if ((rs = mkpath(tfname,hbuf,etcdname,ocfname)) >= 0) {
		     if ((rs = filereadln(tfname,rbuf,rlen)) >= 0) {
		         len = rs ;
		     } else if (isNotPresent(rs)) {
			 rs = SR_OK ;
		     }
	         }
	    } /* end if (getuserhome) */
	    rs1 = uc_free(hbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}

int orgcoder::localconf() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = mkpath(tfname,pr,etcdname,ocfname)) >= 0) {
	    if ((rs = filereadln(tfname,rbuf,rlen)) >= 0) {
		len = rs ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
	
int orgcoder::sysconf() noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = mkpath(tfname,etcdname,ocfname)) >= 0) {
	    if ((rs = filereadln(tfname,rbuf,rlen)) >= 0) {
		len = rs ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}

int orgcoder::orgabbr() noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *obuf{} ; (rs = malloc_mn(&obuf)) >= 0) {
	    cint	olen = rs ;
	    rs = getuserorg(obuf,olen,un) ;
	    if ((rs == SR_NOENT) || (rs == 0)) {
	        rs = localgetorg(pr,obuf,olen,un) ;
	    }
	    if (rs > 0) {
		rs = snabbr(rbuf,rlen,obuf,rs) ;
		len = rs ;
	    }
	    rs1 = uc_free(obuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}

int orgcoder::ifinish() noex {
	return SR_OK ;
}

orgcoder::operator int () noex {
	int		rs = SR_OK ;
	for (int i = 0 ; (rs == SR_OK) && mems[i] ; i += 1) {
	    orgcoder_m	m = mems[i] ;
	    rs = (this->*m)() ;
	} /* end for */
	return rs ;
}

orgcoder_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case orgcoderco_finish:
		rs = op->ifinish() ;
		break ;
	    } /* end switch */
	}
	return rs ;
}
/* end method (orgcoder_co::operator) */


