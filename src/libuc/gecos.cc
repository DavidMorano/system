/* gecos SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* parse a GECOS field located in a buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine (object) module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	gecos

	Description:
	This module parses out the various GOCOS information as it
	is given in its encoded form in a specified buffer.

	Extra npotes:
	The GECOS field of the 'passwd' database should be formatted
	in one of the following ways:

	    name,office,workphone,homephone
	    organization-name(account,bin)office,workphone,homephone,printer
	    name(account)office,workphone,homephone,printer
	    name

	Note also that an ampersand character ('&') that appears
	anywhere in the GCOS field is to be logically replaced by
	the corresponding username of the entry.  This substitution is
	not done in this module.

	The original AT&T GECOS field contained:

	    department-name(account,bin)

	and was further put into a 'struct comment' with fields:

	    c_dept
	    c_name
	    c_acct
	    c_bin

	If a real-name 'name' contains a hyphen character naturally
	(it is part of the actual real-name) then it should be
	entered into the GECOS field with an underscore substituted
	for where original hyphen charaters appear.  These are
	converted back to hyphen characters when read out to callers
	by various "read-out" subroutine interfaces.  This object
	does not do this "hyphen" conversion itself and so a higher
	level interface must perform that function.

	Some alternatives for the GECOS field are:

	    orgdept-name(account,bin)office,workphone,homephone
	    orgdept-name(account,bin)office,workphone,homephone,printer

	Fields:
		orgdept		organization-department
		name		real-name
		account		account number
		bin		printer bin
		office		office address
		workphone	work phone number
		homephone	home phone number
		printer		default printer name

	Actual examples:

	    XNR64430-d.a.morano(126483,BIN8221)
	    rockwell-d.a.morano(126283,BIN8221)4B-411,5336,6175679484,hp0

	Note that phone number have no minus sign characters or
	parentheses character in them.  They are just straing decimal
	digits.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<sbuf.h>
#include	<ascii.h>
#include	<strn.h>		/* |strnbrk(3uc)| */
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"gecos.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct gecoshelp ;
    typedef void (gecoshelp::*gecoshelp_m)() noex ;
    struct gecoshelp {
	cnullptr	np{} ;
	gecos		*op ;
	cchar		*sp ;
	cchar		*bp{} ;
	int		sl ;
	int		bl{} ;
	int		n = 0 ;		/* num fields retrieved */
	bool		fparen = false ;
	gecoshelp(gecos *aop,cchar *asp,int asl) noex {
	    op = aop ;
	    if (asl < 0) asl = lenstr(asp) ;
	    sp = asp ;
	    sl = asl ;
	    bp = asp ;
	    bl = asl ;
	} ; /* end ctor */
	int start() noex ;
	int finish() noex ;
	void organization() noex ;
	void realname() noex ;
	void account() noex ;
	void bin() noex ;
	void office() noex ;
	void wphone() noex ;
	void hphone() noex ;
	void printer() noex ;
	void proc() noex ;
    } ; /* end struct (gecoshelp) */
} /* end namespace */


/* forward references */

static int	gecos_storeit(gecos *,sbuf *,int) noex ;
static int	gecos_storename(gecos *,sbuf *,cchar *) noex ;


/* local variables */

constexpr cchar		brkleft[] = {
	CH_COMMA, CH_LPAREN, '\0'
} ;

constexpr cchar		brkright[] = {
	CH_COMMA, CH_RPAREN, '\0'
} ;

constexpr gecoshelp_m	gmems[] = {
	&gecoshelp::organization,
	&gecoshelp::realname,
	&gecoshelp::account,
	&gecoshelp::bin,
	&gecoshelp::office,
	&gecoshelp::wphone,
	&gecoshelp::hphone,
	&gecoshelp::printer
} ; /* end array (gmems) */


/* exported variables */


/* exported subroutines */

int gecos_start(gecos *op,cchar *sp,int sl) noex {
    	GECOS		*hop = op ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (op && sp) ylikely {
	    memclear(hop) ;
	    if (gecoshelp ho(op,sp,sl) ; (rs = ho.start()) >= 0) ylikely {
		{
		    ho.proc() ;
		}
	        n = ho.finish() ;
	        if (rs >= 0) rs = n ;
	    } /* end if (ho) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (gecos_start) */

int gecos_finish(gecos *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = memclear(op->vals) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (gecos_finish) */

int gecos_getval(gecos *op,int i,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_INVALID ;
	    if (i < gecosval_overlast) ylikely {
	        rs = op->vals[i].vl ;
	        if (rpp) {
	            *rpp = op->vals[i].vp ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (gecos_getval) */

/* create a c-string from the stored object data */
int gecos_compose(gecos *op,char *rbuf,int rlen) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && rbuf) ylikely {
            if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) ylikely {
                bool        fparen = false ;
                if (op->vals[gecosval_organization].vp != nullptr) {
                    gecos_storeit(op,&b,gecosval_organization) ;
                    b.strw("-",1) ;
                }
                if (op->vals[gecosval_realname].vp != nullptr) {
                    int     vi = gecosval_realname ;
                    cchar   *tp ;
                    tp = strnchr(op->vals[vi].vp,op->vals[vi].vl,'-') ;
                    if (tp != nullptr) {
                        gecos_storename(op,&b,tp) ;
                    } else {
                        gecos_storeit(op,&b,gecosval_realname) ;
                    }
                } /* end if (realname) */
    		/* do we have account OR printer-bin information? */
		{
		    bool f = false ;
                    f = f || (op->vals[gecosval_account].vp != np) ;
                    f = f || (op->vals[gecosval_bin].vp != np) ;
		    if (f) {
                        fparen = true ;
                        b.chr(CH_LPAREN) ;
                    }
		} /* end block */
                if (op->vals[gecosval_account].vp != nullptr) {
                    gecos_storeit(op,&b,gecosval_account) ;
                }
                if (op->vals[gecosval_bin].vp != nullptr) {
                    b.strw(",",1) ;
                    gecos_storeit(op,&b,gecosval_bin) ;
                }
                if (fparen) {
                    b.chr(CH_RPAREN) ;
                }
    		/* do we have the old finger stuff */
                if (op->vals[gecosval_office].vp != nullptr) {
                    if (! fparen) b.strw(",",1) ;
                    gecos_storeit(op,&b,gecosval_office) ;
                }
                if (op->vals[gecosval_wphone].vp != nullptr) {
                    b.strw(",",1) ;
                    gecos_storeit(op,&b,gecosval_wphone) ;
                }
                if (op->vals[gecosval_hphone].vp != nullptr) {
                    b.strw(",",1) ;
                    gecos_storeit(op,&b,gecosval_hphone) ;
                }
                if (op->vals[gecosval_printer].vp != nullptr) {
                    b.strw(",",1) ;
                    gecos_storeit(op,&b,gecosval_printer) ;
                }
                rs1 = b.finish ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (gecos_compose) */


/* private subroutines */

static int gecos_storeit(gecos *op,sbuf *bp,int vi) noex {
	return bp->strw(op->vals[vi].vp,op->vals[vi].vl) ;
}
/* end subroutine (gecos_storeit) */

static int gecos_storename(gecos *op,sbuf *bp,cchar *tp) noex {
	int		rs = SR_OK ;
	int		vi = gecosval_realname ;
	int		sl ;
	cchar		*sp ;
	{
	    sp = op->vals[vi].vp ;
	    sl = op->vals[vi].vl ;
	}
	/* store the initial segment of the name */
	if ((tp - sp) > 0) {
	    cint tl = intconv(tp - sp) ;
	    bp->strw(sp,tl) ;
	    sl -= intconv(tp - sp) ;
	    sp = tp ;
	}
	/* make the substitution */
	{
	    bp->chr('_') ;
	    sp += 1 ;
	    sl -= 1 ;
	}
	/* loop searching for other segments */
	while ((tp = strnchr(sp,sl,'-')) != nullptr) {
	    cint tl = intconv(tp - sp) ;
	    bp->strw(sp,tl) ;
	    sl -= intconv(tp - sp) ;
	    sp = tp ;
	} /* end while */
	if (sl > 0) {
	    rs = bp->strw(sp,sl) ;
	}
	return rs ;
}
/* end subroutine (gecos_storename) */

int gecoshelp::start() noex {
	return sl ;
}

int gecoshelp::finish() noex {
	return n ;
}

void gecoshelp::organization() noex {
	if (cchar *tp ; (tp = strnchr(bp,bl,'-')) != np) {
	    bool	f = false ;
	    if (! f) {
		cint tl = intconv(bl - (tp - bp)) ;
	        f = (strnbrk(tp,tl,brkleft) != np) ;
	    }
	    if (! f) {
		cint tl = intconv(tp - bp) ;
	        f = (strnbrk(bp,tl,brkleft) == np) ;
	    }
	    if (f) {
	        n += 1 ;
	        op->vals[gecosval_organization].vp = bp ;
	        op->vals[gecosval_organization].vl = intconv(tp - bp) ;
	        bp = (tp + 1) ;
	        bl = intconv((sp + sl) - (tp + 1)) ;
	    } /* end if */
	} /* end if (organization) */
}
/* end method (gecoshelp::organization) */

void gecoshelp::realname() noex {
	if (cchar *tp ; (tp = strnbrk(bp,bl,brkleft)) != np) {
	    if (tp[0] == CH_LPAREN) fparen = true ;
	    n += 1 ;
	    op->vals[gecosval_realname].vp = bp ;
	    op->vals[gecosval_realname].vl = intconv(tp - bp) ;
	    bp = (tp + 1) ;
	    bl = intconv((sp + sl) - (tp + 1)) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_realname].vp = bp ;
	    op->vals[gecosval_realname].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::realname) */

void gecoshelp::account() noex {
	if (cchar *tp ; fparen && ((tp = strnbrk(bp,bl,brkright)) != np)) {
	    if (tp[0] == CH_RPAREN) fparen = false ;
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_account].vp = bp ;
	        op->vals[gecosval_account].vl = intconv(tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = intconv((sp + sl) - (tp + 1)) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_bin].vp = bp ;
	    op->vals[gecosval_bin].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::account) */

void gecoshelp::bin() noex {
	if (fparen) {
	    if (cchar *tp ; (tp = strnbrk(bp,bl,brkright)) != np) {
	        if (tp[0] == CH_RPAREN) fparen = false ;
	        if ((tp - bp) && (op->vals[gecosval_bin].vp == nullptr)) {
	            n += 1 ;
	            op->vals[gecosval_bin].vp = bp ;
	            op->vals[gecosval_bin].vl = intconv(tp - bp) ;
	        }
	        bp = (tp + 1) ;
	        bl = intconv((sp + sl) - (tp + 1)) ;
	    } else if (op->vals[gecosval_bin].vp == nullptr) {
	        n += 1 ;
	        op->vals[gecosval_bin].vp = bp ;
	        op->vals[gecosval_bin].vl = bl ;
	        bl = 0 ;
	    } /* end if */
	} /* end if (possible printer-bin item) */
}
/* end method (gecoshelp::bin) */

void gecoshelp::office() noex {
	if (cchar *tp ; (tp = strnbrk(bp,bl,brkright)) != np) {
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_office].vp = bp ;
	        op->vals[gecosval_office].vl = intconv(tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = intconv((sp + sl) - (tp + 1)) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_office].vp = bp ;
	    op->vals[gecosval_office].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::office) */

void gecoshelp::wphone() noex {
	if (cchar *tp ; (tp = strnbrk(bp,bl,brkright)) != np) {
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_wphone].vp = bp ;
	        op->vals[gecosval_wphone].vl = intconv(tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = intconv((sp + sl) - (tp + 1)) ;
	} else if (op->vals[gecosval_office].vp == nullptr) {
	    n += 1 ;
	    op->vals[gecosval_office].vp = bp ;
	    op->vals[gecosval_office].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::wphone) */

void gecoshelp::hphone() noex {
	if (cchar *tp ; (tp = strnbrk(bp,bl,brkright)) != np) {
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_hphone].vp = bp ;
	        op->vals[gecosval_hphone].vl = intconv(tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = intconv((sp + sl) - (tp + 1)) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_hphone].vp = bp ;
	    op->vals[gecosval_hphone].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::hphone) */

void gecoshelp::printer() noex {
	if (cchar *tp ; (tp = strnbrk(bp,bl,brkright)) != np) {
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_printer].vp = bp ;
	        op->vals[gecosval_printer].vl = intconv(tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = intconv((sp + sl) - (tp + 1)) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_printer].vp = bp ;
	    op->vals[gecosval_printer].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::printer) */

void gecoshelp::proc() noex {
	for (cauto &m : gmems) {
	    (this->*m)() ;
	    if (bl == 0) break ;
	} /* end for */
}
/* end method (gecoshelp::proc) */

int gecos::start(cchar *sp,int sl) noex {
	return gecos_start(this,sp,sl) ;
}

int gecos::compose(char *rb,int rl) noex {
	return gecos_compose(this,rb,rl) ;
}

int gecos::getval(int i,cchar **rpp) noex {
	return gecos_getval(this,i,rpp) ;
}

void gecos::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("gecos",rs,"fini-finish") ;
	}
} /* end method (gecos::dtor) */

gecos_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case gecosmem_finish:
	        rs = gecos_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (gecos_co::operator) */


