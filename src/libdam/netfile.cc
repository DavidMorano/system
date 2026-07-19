/* netfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* read a NETRC file and make its contents available */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-22, David A­D­ Morano
	This object module was adopted for use from a previous NETRC
	reading program (from the early 1990s).  This version
	should ignore all 'macdef' entries, but is not currently
	coded to handle that properly.

	= 2024-11-22, David A­D­ Morano
	Ya, the 22nd.  Just a coincidence.  But to the point at
	hand.  I have just (tried) to review this code for C++20
	conformance.  I must have been on drugs when I first wrote
	this.  I think that this code is correct.  But I really had
	to use some thinking to figure out the correctness of this
	stuff.  This might be a sign that my mind is really
	deteriorating over time.  But a main rule for programming
	is that the code should be readable by anyone.  Well, it
	is not clear that the code below meets that standard.  Or
	maybe it is just I who is losing it.

*/

/* Copyright © 1998,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	netfile

	Description:
	OK, here is the deal.  We ignore the 'macdef' key totally
	except to process the fact that it has a value (which is
	supposed to be the name of the newly defined macro) and the
	actual definition (or body of the macro) on the next line.
	If there is no 'machine' key associated with some 'login'
	key, then we make a fake nullptr machine grouping, but only
	one of these within the whole 'netrc' file!  If there are
	more than one 'login', 'account', or what have you, key
	associated with a single 'machine' key, then we ignore all
	but the last one.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<vecitem.h>		/* LIBUC */
#include	<fieldterminit.hh>	/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strwcmp.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"netfile.h"

import libutil ;

/* local defines */

#define	NF		netfile
#define	NF_ENT		netfile_ent

#define	NS		netstate

#define	KEYBUFLEN	10		/* lenstr("password") */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::mem ;			/* variable */


/* local typedefs */

typedef netfile_ent	ent ;
typedef netfile_ent *	entp ;


/* external subroutines */


/* external variables */


/* local structures */

enum netitems {
	netitem_machine,
	netitem_login,
	netitem_password,
	netitem_account,
	netitem_overlast
} ; /* end enum */

struct netstate {
	cchar		*item[netitem_overlast] ;
	int		c ;
} ; /* end struct */

constexpr uchar		termvals[] = {
	CH_NUL, CH_SOH, CH_STX, CH_ETX, CH_EOT, CH_ENQ, CH_ACK, CH_SO,
	CH_SI, CH_SYN, CH_ETB, CH_EM, CH_SUB, CH_ESC, CH_FS, CH_GS,
	CH_RS, CH_US, CH_DEL
} ; /* end array */

constexpr uchar		termchrs[] = "\t\n\v\f\r !#':=" ;

namespace {
    struct terminiter {
	char	terms[fieldterminit_size] = {} ;
	consteval void mkvals() noex {
	    for (cauto ch : termvals) {
		baset(terms,ch) ;
	    } /* end for */
	} ; /* end method */
	consteval void mkchrs() noex {
	    uchar uc ;
	    for (con uchar *ucp = termchrs ; ((uc = *ucp)) ; ucp += 1) {
		baset(terms,uc) ;
	    } /* end for */
	} ; /* end ctor */
	consteval terminiter() noex {
	    mkvals() ;
	    mkchrs() ;
	} ; /* end ctor */
    } ; /* end struct (terminiter) */
} /* end namespace */


/* forward references */

local int	netfile_parse(NF *,netstate *,cchar *) noex ;
local int	netfile_parseln(NF *,netstate *,cchar *,int) noex ;
local int	netfile_item(NF *,netstate *,int,cchar *,int) noex ;

local int	netstate_start(netstate *) noex ;
local int	netstate_reset(netstate *) noex ;
local int	netstate_item(netstate *,int,cchar *,int) noex ;
local int	netstate_ready(netstate *) noex ;
local int	netstate_finish(netstate *) noex ;

local int	entry_start(NF_ENT *,netstate *) noex ;
local int	entry_finish(NF_ENT *) noex ;

local int	getnii(int) noex ;


/* local variables */

enum netkeys {
	netkey_machine,
	netkey_login,
	netkey_username,
	netkey_password,
	netkey_account,
	netkey_macdef,
	netkey_default,
	netkey_overlast
} ; /* end enum */

constexpr cpcchar	netnames[] = {
	"machine",
	"login",
	"username",
	"password",
	"account",
	"macdef",
	"default",
	nullptr
} ; /* end array */

constexpr int		readies[] = {
	netitem_machine,
	netitem_login
} ; /* end array */

constexpr terminiter	ft ;


/* exported variables */


/* exported subroutines */


local int netfile_opens(NF *op,cchar *fn) noex {
	cint		vn = 10 ;
	cint		vo = 0 ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if ((rs = vecitem_start(op,vn,vo)) >= 0) {
	    if (ustat sb ; (rs = uc_stat(fn,&sb)) >= 0) {
	        if (! S_ISDIR(sb.st_mode)) {
	            if (netstate ns ; (rs = netstate_start(&ns)) >= 0) {
			{
	                    rs = netfile_parse(op,&ns,fn) ;
	                    c = rs ;
			}
	                rs1 = netstate_finish(&ns) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (netstate) */
	        } else {
	            rs = SR_ISDIR ;
		}
	    } /* end if (stat) */
	    if (rs < 0) {
	        vecitem_finish(op) ;
	    }
	} /* end if (vecitem) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (netfile_opens) */

int netfile_open(NF *op,cchar *netfname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && netfname) {
	    rs = SR_INVALID ;
	    if (netfname[0]) {
		    rs = netfile_opens(op,netfname) ;
		    c = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (netfile_open) */

int netfile_close(NF *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    void	*vp{} ;
	    for (int i = 0 ; vecitem_get(op,i,&vp) >= 0 ; i += 1) {
	        if (NF_ENT *ep = entp(vp) ; ep) {
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end while */
	    {
	        rs1 = vecitem_finish(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (netfile_close) */

int netfile_get(NF *op,int i,NF_ENT **epp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (void *vp{} ; (rs = vecitem_get(op,i,&vp)) >= 0) {
		if (epp) *epp = entp(vp) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (netfile_get) */

int netfile_fetch(NF *op,cchar *mp,int ml,NF_ENT **epp) noex {
	int		rs = SR_FAULT ;
	if (op && mp) {
	    void	*vp{} ;
	    for (int i = 0 ; (rs = vecitem_get(op,i,&vp)) >= 0 ; i += 1) {
		if (NF_ENT *ep = entp(vp) ; ep) {
		    cchar *mach = ep->machine ;
		    if (mach && (strwcmp(mach,mp,ml) == 0)) {
		        if (epp) *epp = entp(vp) ;
			break ;
		    }
		} /* end if (resumelife) */
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (netfile_fetch) */


/* private subroutines */

local int netfile_parse(NF *op,netstate *nsp,cchar *netfname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *lbuf{} ; (rs = mem.ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    if (bfile nf ; (rs = bopen(&nf,netfname,"r",0)) >= 0) {
	        while ((rs = breadln(&nf,lbuf,llen)) > 0) {
		    cchar	*cp{} ;
		    if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
			rs = netfile_parseln(op,nsp,cp,cl) ;
			c += rs ;
		    }
		    if (rs < 0) break ;
	        } /* end while (reading lines) */
	        if (rs >= 0) {
	            if ((rs = netfile_item(op,nsp,-1,nullptr,0)) >= 0) {
	                rs = vecitem_count(op) ;
		    }
	        } /* end if (ok) */
	        rs1 = bclose(&nf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs1 = mem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (netfile_parse) */

local int netfile_parseln(NF *op,netstate *nsp,cchar *lp,int ll) noex {
	cint		klen = KEYBUFLEN ;
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		kbuf[KEYBUFLEN+1] ;
	bool		f_macdef = false ;
        if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
            cchar   *fp ;
            bool    f_default = false ;
            for (int fl ; (fl = fsb.get(ft.terms,&fp)) > 0 ; ) {
                cint        ml = min(klen,fl) ;
                strwcpylc(kbuf,fp,ml) ;
                if (int nki ; (nki = matpstr(netnames,2,kbuf,ml)) >= 0) {
                    int     cl = 0 ;
                    cchar   *cp = nullptr ;
                    switch (nki) {
                    case netkey_machine:
                    case netkey_login:
                    case netkey_username:
                    case netkey_password:
                    case netkey_account:
                        if (fsb.term == '#') break ;
                        if ((cl = fsb.get(ft.terms,&fp)) >= 0) {
                            cp = fp ;
                        }
                        break ;
                    case netkey_macdef:
                        f_macdef = true ;
                        break ;
                    case netkey_default:
                        f_default = true ;
                        break ;
                    } /* end switch */
                    if (cp) {
                        bool        f = true ;
                        f = f && (nki >= 0) ;
                        f = f && (! f_macdef) ;
                        f = f && (! f_default) ;
                        if (f) {
                            rs = netfile_item(op,nsp,nki,cp,cl) ;
			    c += rs ;
                        }
                    } /* end if (had value) */
                } /* end if (keyword) */
                if (f_macdef || f_default) break ;
                if (fsb.term == '#') break ;
                if (rs < 0) break ;
            } /* end while (processing keys) */
            rs1 = fsb.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (field) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (netfile_parseln) */

local int netfile_item(NF *op,netstate *nsp,int nki,cc *sp,int sl) noex {
	cint		nii = getnii(nki) ;
	int		rs = SR_OK ;
	if ((nii == netitem_machine) || (nii < 0)) {
	    if ((rs = netstate_ready(nsp)) > 0) {
		if (NF_ENT e ; (rs = entry_start(&e,nsp)) >= 0) {
		    cint	esz = szof(NF_ENT) ;
	            if ((rs = vecitem_add(op,&e,esz)) >= 0) {
	                cint	ei = rs ;
	                rs = netstate_reset(nsp) ;
	                if (rs < 0) {
	                    vecitem_del(op,ei) ;
			} /* end if (error) */
	            } /* end if (vecitem_add) */
	            if (rs < 0) {
	                entry_finish(&e) ;
		    } /* end if (error) */
	        } /* end if (entry_start) */
	    } /* end if (netstate_ready) */
	} /* end if */
	if ((rs >= 0) && (nii >= 0) && sp) {
	    rs = netstate_item(nsp,nii,sp,sl) ;
	} /* end if */
	return rs ;
} /* end subroutine (netfile_item) */

local int netstate_start(netstate *nsp) noex {
	int		rs = SR_FAULT ;
	if (nsp) {
	   rs = memclear(nsp) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (netstate_start) */

local int netstate_reset(netstate *nsp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < netitem_overlast ; i += 1) {
	    if (nsp->item[i] != nullptr) {
	        voidp vp = voidp(nsp->item[i]) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        nsp->item[i] = nullptr ;
	    } /* end if (memory-release) */
	} /* end for */
	nsp->c = 0 ;
	return rs ;
} /* end subroutine (netstate_reset) */

local int netstate_item(netstate *nsp,int ki,cchar *sp,int sl) noex {
	int		rs = SR_INVALID ;
	if ((ki >= 0) && (ki < netitem_overlast)) {
	    rs = SR_OK ;
	    if (sp) {
	        if (nsp->item[ki] != nullptr) {
	            voidp vp = voidp(nsp->item[ki]) ;
	            rs = mem.free(vp) ;
	            nsp->item[ki] = nullptr ;
	        } /* end if (memory-release) */
		if (rs >= 0) {
	            if (cchar *cp ; (rs = mem.strw(sp,sl,&cp)) >= 0) {
	                nsp->item[ki] = cp ;
	                nsp->c += 1 ;
	            } /* end if (memory-acquire) */
		} /* end if (ok) */
	    } /* end if (non-null) */
	} /* end if (valid) */
	return rs ;
} /* end subroutine (netstate_item) */

local int netstate_ready(netstate *nsp) noex {
    	int		rs = SR_BUGCHECK ;
    	int		f = false ;
	if (nsp) {
	    rs = SR_OK ;
	    for (cint ni : readies) {
	        cchar	*sp = nsp->item[ni] ;
	        f = (sp && (sp[0] != '\0')) ;
	        if (! f) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (netstate_ready) */

local int netstate_finish(netstate *nsp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < netitem_overlast ; i += 1) {
	    if (nsp->item[i] != nullptr) {
	        voidp vp = voidp(nsp->item[i]) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        nsp->item[i] = nullptr ;
	    } /* end if (memory-release) */
	} /* end for */
	nsp->c = 0 ;
	return rs ;
} /* end subroutine (netstate_finish) */

local int entry_start(NF_ENT *ep,netstate *nsp) noex {
    	int		rs = SR_BUGCHECK ;
	if (ep && nsp) {
	    rs = memclear(ep) ;
	    for (int i = 0 ; i < netitem_overlast ; i += 1) {
	        switch (i) {
	        case netitem_machine:
	            ep->machine = nsp->item[i] ;	/* transfer */
	            break ;
	        case netitem_login:
		    ep->login = nsp->item[i] ;		/* transfer */
	            break ;
	        case netitem_password:
	            ep->password = nsp->item[i] ;	/* transfer */
	            break ;
	        case netitem_account:
	            ep->account = nsp->item[i] ;	/* transfer */
	            break ;
	        } /* end switch */
	        nsp->item[i] = nullptr ;
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_finish(NF_ENT *ep) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->machine) {
	        voidp vp = voidp(ep->machine) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->machine = nullptr ;
	    } /* end if (memory-release) */
	    if (ep->login) {
	        voidp vp = voidp(ep->login) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->login = nullptr ;
	    } /* end if (memory-release) */
	    if (ep->password) {
	        voidp vp = voidp(ep->password) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->password = nullptr ;
	    } /* end if (memory-release) */
	    if (ep->account) {
	        voidp vp = voidp(ep->account) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->account = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_finish) */

local int getnii(int nki) noex {
	int		nii = -1 ;
	switch (nki) {
	case netkey_machine:
	    nii = netitem_machine ;
	    break ;
	case netkey_login:
	case netkey_username:
	    nii = netitem_login ;
	    break ;
	case netkey_password:
	    nii = netitem_password ;
	    break ;
	case netkey_account:
	    nii = netitem_account ;
	    break ;
	} /* end switch */
	return nii ;
} /* end subroutine (getnii) */


