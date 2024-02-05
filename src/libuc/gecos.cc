/* gecos SUPPORT */
/* lang=C++20 */

/* parse a GECOS field located in a buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine (object) module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module parses out the various GOCOS information as it
	is given in its encoded form in a specified buffer.

	Extra npotes:

	The gecos field of the 'passwd' database should be formatted
	in one of the following ways:

	    name,office,workphone,homephone
	    organization-name(account,bin)office,workphone,homephone,printer
	    name(account)office,workphone,homephone,printer
	    name

	Note also that an ampersand character ('&') that appears
	anywhere in the GCOS field is to be logically replaced by
	the corresponding username of the entry.  This substitution is
	not done in this module.

	The original AT&T gecos field contained:

	    department-name(account,bin)

	and was further put into a 'struct comment' with fields:

	    c_dept
	    c_name
	    c_acct
	    c_bin

	If a real-name 'name' contains a hyphen character naturally
	(it is part of the actual real-name) then it should be
	entered into the gecos field with an underscore substituted
	for where original hyphen charaters appear.  These are
	converted back to hyphen characters when read out to callers
	by various "read-out" subroutine interfaces.  This object
	does not do this "hyphen" conversion itself and so a higher
	level interface must perform that function.

	Some alternatives for the gecos field are:

	    orgdept-name(account,bin)office,workphone,homephone
	    orgdept-name(account,bin)office,workphone,homephone,printer

	Actual examples:

	    XNR64430-d.a.morano(126483,BIN8221)
	    rockwell-d.a.morano(126283,BIN8221)4B-411,5336,6175679484,hp0

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<usupport.h>		/* <- for |memclear(3c++)| */
#include	<sbuf.h>
#include	<ascii.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"gecos.h"


/* local defines */


/* local namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct gecoshelp ;
    typedef void (gecoshelp::*gecoshelp_m)() noex ;
    struct gecoshelp {
	const nullptr_t	np{} ;
	gecos		*op ;
	cchar		*sp ;
	cchar		*bp{} ;
	int		sl ;
	int		bl{} ;
	int		n = 0 ;		/* num fields retrieved */
	bool		fparen = false ;
	gecoshelp(gecos *aop,cchar *asp,int asl) noex {
	    op = aop ;
	    if (asl < 0) asl = strlen(asp) ;
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
}


/* forward references */

static int	gecos_storeit(gecos *,sbuf *,int) noex ;
static int	gecos_storename(gecos *,sbuf *,cchar *) noex ;


/* local variables */

static constexpr cchar		brkleft[] = {
	CH_COMMA, CH_LPAREN, '\0'
} ;

static constexpr cchar		brkright[] = {
	CH_COMMA, CH_RPAREN, '\0'
} ;

static constexpr gecoshelp_m	gmems[] = {
	&gecoshelp::organization,
	&gecoshelp::realname,
	&gecoshelp::account,
	&gecoshelp::bin,
	&gecoshelp::office,
	&gecoshelp::wphone,
	&gecoshelp::hphone,
	&gecoshelp::printer,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int gecos_start(gecos *op,cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (op && sbuf) {
	    gecoshelp	ho(op,sbuf,slen) ;
	    memclear(op) ;
	    if ((rs = ho.start()) >= 0) {
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
	if (op) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < gecosval_overlast ; i += 1) {
	        op->vals[i].vp = nullptr ;
	        op->vals[i].vl = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (gecos_finish) */

int gecos_getval(gecos *op,int i,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    if (i < gecosval_overlast) {
	        rs = op->vals[i].vl ;
	        if (rpp) {
	            *rpp = op->vals[i].vp ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (gecos_getval) */

int gecos_compose(gecos *op,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && rbuf) {
            sbuf	b ;
            if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
                bool        fparen = false ;
                if (op->vals[gecosval_organization].vp != nullptr) {
                    gecos_storeit(op,&b,gecosval_organization) ;
                    sbuf_strw(&b,"-",1) ;
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
    /* do we have account and printer-bin information? */
                if ((op->vals[gecosval_account].vp != nullptr) || 
                    (op->vals[gecosval_bin].vp != nullptr)) {
                    fparen = true ;
                    sbuf_char(&b,CH_LPAREN) ;
                }
                if (op->vals[gecosval_account].vp != nullptr) {
                    gecos_storeit(op,&b,gecosval_account) ;
                }
                if (op->vals[gecosval_bin].vp != nullptr) {
                    sbuf_strw(&b,",",1) ;
                    gecos_storeit(op,&b,gecosval_bin) ;
                }
                if (fparen) {
                    sbuf_char(&b,CH_RPAREN) ;
                }
    /* do we have the old finger stuff */
                if (op->vals[gecosval_office].vp != nullptr) {
                    if (! fparen) sbuf_strw(&b,",",1) ;
                    gecos_storeit(op,&b,gecosval_office) ;
                }
                if (op->vals[gecosval_wphone].vp != nullptr) {
                    sbuf_strw(&b,",",1) ;
                    gecos_storeit(op,&b,gecosval_wphone) ;
                }
                if (op->vals[gecosval_hphone].vp != nullptr) {
                    sbuf_strw(&b,",",1) ;
                    gecos_storeit(op,&b,gecosval_hphone) ;
                }
                if (op->vals[gecosval_printer].vp != nullptr) {
                    sbuf_strw(&b,",",1) ;
                    gecos_storeit(op,&b,gecosval_printer) ;
                }
                rs1 = sbuf_finish(&b) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (gecos_compose) */


/* private subroutines */

static int gecos_storeit(gecos *op,sbuf *bp,int vi) noex {
	int		rs ;
	rs = sbuf_strw(bp,op->vals[vi].vp,op->vals[vi].vl) ;
	return rs ;
}
/* end subroutine (gecos_storeit) */

static int gecos_storename(gecos *op,sbuf *bp,cchar *tp) noex {
	int		rs = SR_OK ;
	int		vi = gecosval_realname ;
	int		sl ;
	cchar		*sp ;
	sp = op->vals[vi].vp ;
	sl = op->vals[vi].vl ;
/* store the initial segment of the name */
	if ((tp - sp) > 0) {
	    sbuf_strw(bp,sp,(tp - sp)) ;
	    sl -= (tp - sp) ;
	    sp = tp ;
	}
/* make the substitution */
	{
	    sbuf_char(bp,'_') ;
	    sp += 1 ;
	    sl -= 1 ;
	}
/* loop searching for other segments */
	while ((tp = strnchr(sp,sl,'-')) != nullptr) {
	    sbuf_strw(bp,sp,(tp - sp)) ;
	    sl -= (tp - sp) ;
	    sp = tp ;
	} /* end while */
	if (sl > 0) {
	    rs = sbuf_strw(bp,sp,sl) ;
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
	    f = f || (strnpbrk(tp,(bl - (tp - bp)),brkleft) != nullptr) ;
	    f = f || (strnpbrk(bp,(tp - bp),brkleft) == nullptr) ;
	    if (f) {
	        n += 1 ;
	        op->vals[gecosval_organization].vp = bp ;
	        op->vals[gecosval_organization].vl = (tp - bp) ;
	        bp = (tp + 1) ;
	        bl = ((sp + sl) - (tp + 1)) ;
	    } /* end if */
	} /* end if (organization) */
}
/* end method (gecoshelp::organization) */

void gecoshelp::realname() noex {
	if (cchar *tp ; (tp = strnpbrk(bp,bl,brkleft)) != np) {
	    if (tp[0] == CH_LPAREN) fparen = true ;
	    n += 1 ;
	    op->vals[gecosval_realname].vp = bp ;
	    op->vals[gecosval_realname].vl = (tp - bp) ;
	    bp = (tp + 1) ;
	    bl = (sp + sl) - (tp + 1) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_realname].vp = bp ;
	    op->vals[gecosval_realname].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::realname) */

void gecoshelp::account() noex {
	if (cchar *tp ; fparen && ((tp = strnpbrk(bp,bl,brkright)) != np)) {
	    if (tp[0] == CH_RPAREN) fparen = false ;
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_account].vp = bp ;
	        op->vals[gecosval_account].vl = (tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = (sp + sl) - (tp + 1) ;
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
	    if (cchar *tp ; (tp = strnpbrk(bp,bl,brkright)) != np) {
	        if (tp[0] == CH_RPAREN) fparen = false ;
	        if ((tp - bp) && (op->vals[gecosval_bin].vp == nullptr)) {
	            n += 1 ;
	            op->vals[gecosval_bin].vp = bp ;
	            op->vals[gecosval_bin].vl = (tp - bp) ;
	        }
	        bp = (tp + 1) ;
	        bl = (sp + sl) - (tp + 1) ;
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
	if (cchar *tp ; (tp = strnpbrk(bp,bl,brkright)) != np) {
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_office].vp = bp ;
	        op->vals[gecosval_office].vl = (tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = (sp + sl) - (tp + 1) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_office].vp = bp ;
	    op->vals[gecosval_office].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::office) */

void gecoshelp::wphone() noex {
	if (cchar *tp ; (tp = strnpbrk(bp,bl,brkright)) != np) {
	        if (tp - bp) {
	            n += 1 ;
	            op->vals[gecosval_wphone].vp = bp ;
	            op->vals[gecosval_wphone].vl = (tp - bp) ;
	        }
	        bp = (tp + 1) ;
	        bl = (sp + sl) - (tp + 1) ;
	    } else if (op->vals[gecosval_office].vp == nullptr) {
	        n += 1 ;
	        op->vals[gecosval_office].vp = bp ;
	        op->vals[gecosval_office].vl = bl ;
	        bl = 0 ;
	    } /* end if */
}
/* end method (gecoshelp::wphone) */

void gecoshelp::hphone() noex {
	if (cchar *tp ; (tp = strnpbrk(bp,bl,brkright)) != np) {
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_hphone].vp = bp ;
	        op->vals[gecosval_hphone].vl = (tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = (sp + sl) - (tp + 1) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_hphone].vp = bp ;
	    op->vals[gecosval_hphone].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::hphone) */

void gecoshelp::printer() noex {
	if (cchar *tp ; (tp = strnpbrk(bp,bl,brkright)) != np) {
	    if (tp - bp) {
	        n += 1 ;
	        op->vals[gecosval_printer].vp = bp ;
	        op->vals[gecosval_printer].vl = (tp - bp) ;
	    }
	    bp = (tp + 1) ;
	    bl = (sp + sl) - (tp + 1) ;
	} else {
	    n += 1 ;
	    op->vals[gecosval_printer].vp = bp ;
	    op->vals[gecosval_printer].vl = bl ;
	    bl = 0 ;
	} /* end if */
}
/* end method (gecoshelp::printer) */

void gecoshelp::proc() noex {
	for (int i = 0 ; gmems[i] && (bl > 0) ; i += 1) {
	    gecoshelp_m		m = gmems[i] ;
	    (this->*m)() ;
	} /* end for */
}
/* end method (gecoshelp::proc) */


