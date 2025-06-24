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
	one of these within the whole 'netrc' file!

	If there are more than one 'login', 'account', or what have
	you, key associated with a single 'machine' key, then we
	ignore all but the last one.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<vecitem.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<sfx.h>
#include	<matxstr.h>
#include	<strwcpy.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"netfile.h"


/* local defines */

#define	NF		netfile
#define	NF_ENT		netfile_ent

#define	NS		netstate

#define	KEYBUFLEN	10		/* strlen("password") */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


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
} ;

struct netstate {
	cchar		*item[netitem_overlast] ;
	int		c ;
} ;


/* forward references */

static int	netfile_parse(NF *,netstate *,cchar *) noex ;
static int	netfile_parseln(NF *,netstate *,cchar *,int) noex ;
static int	netfile_item(NF *,netstate *,int,cchar *,int) noex ;

static int	netstate_start(netstate *) noex ;
static int	netstate_reset(netstate *) noex ;
static int	netstate_item(netstate *,int,cchar *,int) noex ;
static int	netstate_ready(netstate *) noex ;
static int	netstate_finish(netstate *) noex ;

static int	entry_start(NF_ENT *,netstate *) noex ;
static int	entry_finish(NF_ENT *) noex ;

static int	getnii(int) noex ;


/* local variables */

constexpr char		fterms[32] = {
	0x7F, 0xFE, 0xC0, 0xFE,
	0x8B, 0x00, 0x00, 0x24, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x80,
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
} ;

enum netkeys {
	netkey_machine,
	netkey_login,
	netkey_username,
	netkey_password,
	netkey_account,
	netkey_macdef,
	netkey_default,
	netkey_overlast
} ;

constexpr cpcchar	netkeys[] = {
	"machine",
	"login",
	"username",
	"password",
	"account",
	"macdef",
	"default",
	nullptr
} ;

constexpr int		readies[] = {
	netitem_machine,
	netitem_login
} ;


/* exported variables */


/* exported subroutines */

int netfile_open(NF *vep,cchar *netfname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (vep && netfname) {
	    rs = SR_INVALID ;
	    if (netfname[0]) {
		cint	vn = 10 ;
		cint	vo = 0 ;
	        if ((rs = vecitem_start(vep,vn,vo)) >= 0) {
	            if (USTAT sb ; (rs = uc_stat(netfname,&sb)) >= 0) {
	                if (! S_ISDIR(sb.st_mode)) {
	                    if (netstate ns ; (rs = netstate_start(&ns)) >= 0) {
				{
	                            rs = netfile_parse(vep,&ns,netfname) ;
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
	                vecitem_finish(vep) ;
	            }
	        } /* end if (vecitem) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (netfile_open) */

int netfile_close(NF *vep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (vep) {
	    rs = SR_OK ;
	    void	*vp{} ;
	    for (int i = 0 ; vecitem_get(vep,i,&vp) >= 0 ; i += 1) {
	        NF_ENT	*ep = entp(vp) ;
	        if (vp) {
	            rs1 = entry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end while */
	    {
	        rs1 = vecitem_finish(vep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (netfile_close) */

int netfile_get(NF *vep,int i,NF_ENT **epp) noex {
	int		rs = SR_FAULT ;
	if (vep) {
	    if (void *vp{} ; (rs = vecitem_get(vep,i,&vp)) >= 0) {
		if (epp) *epp = entp(vp) ;
	    }
	}
	return rs ;
}
/* end subroutine (netfile_get) */

int netfile_fetch(NF *vep,cchar *mp,int ml,NF_ENT **epp) noex {
	int		rs = SR_FAULT ;
	if (vep && mp) {
	    void	*vp{} ;
	    for (int i = 0 ; (rs = vecitem_get(vep,i,&vp)) >= 0 ; i += 1) {
		NF_ENT	*ep = entp(vp) ;
		if (vp) {
		    if (ep->machine && (strwcmp(ep->machine,mp,ml) == 0)) {
		        if (epp) *epp = entp(vp) ;
			break ;
		    }
		}
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (netfile_fetch) */


/* private subroutines */

static int netfile_parse(NF *vep,netstate *nsp,cchar *netfname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    if (bfile nf ; (rs = bopen(&nf,netfname,"r",0)) >= 0) {
	        while ((rs = breadln(&nf,lbuf,llen)) > 0) {
		    cchar	*cp{} ;
		    if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
			rs = netfile_parseln(vep,nsp,cp,cl) ;
			c += rs ;
		    }
		    if (rs < 0) break ;
	        } /* end while (reading lines) */
	        if (rs >= 0) {
	            if ((rs = netfile_item(vep,nsp,-1,nullptr,0)) >= 0) {
	                rs = vecitem_count(vep) ;
		    }
	        }
	        rs1 = bclose(&nf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs = rsfree(rs,lbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (netfile_parse) */

static int netfile_parseln(NF *vep,netstate *nsp,cchar *lp,int ll) noex {
	cint		klen = KEYBUFLEN ;
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		kbuf[KEYBUFLEN+1] ;
	bool		f_macdef = false ;
        if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
            int     fl ;
            cchar   *ft = fterms ;
            cchar   *fp ;
            bool    f_default = false ;
            while ((fl = fsb.get(ft,&fp)) > 0) {
                cint        ml = min(klen,fl) ;
                strwcpylc(kbuf,fp,ml) ;
                if (int nki ; (nki = matpstr(netkeys,2,kbuf,ml)) >= 0) {
                    int     cl = 0 ;
                    cchar   *cp = nullptr ;
                    switch (nki) {
                    case netkey_machine:
                    case netkey_login:
                    case netkey_username:
                    case netkey_password:
                    case netkey_account:
                        if (fsb.term == '#') break ;
                        if ((cl = fsb.get(ft,&fp)) >= 0) {
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
                            rs = netfile_item(vep,nsp,nki,cp,cl) ;
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
}
/* end subroutine (netfile_parseln) */

static int netfile_item(NF *vep,netstate *nsp,int nki,cc *sp,int sl) noex {
	cint		nii = getnii(nki) ;
	int		rs = SR_OK ;
	if ((nii == netitem_machine) || (nii < 0)) {
	    if ((rs = netstate_ready(nsp)) > 0) {
		if (NF_ENT e ; (rs = entry_start(&e,nsp)) >= 0) {
		    cint	esize = szof(NF_ENT) ;
	            if ((rs = vecitem_add(vep,&e,esize)) >= 0) {
	                cint	ei = rs ;
	                rs = netstate_reset(nsp) ;
	                if (rs < 0) {
	                    vecitem_del(vep,ei) ;
			}
	            } /* end if (vecitem_add) */
	            if (rs < 0) {
	                entry_finish(&e) ;
		    }
	        } /* end if (entry_start) */
	    } /* end if (netstate_ready) */
	} /* end if */
	if ((rs >= 0) && (nii >= 0) && sp) {
	    rs = netstate_item(nsp,nii,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (netfile_item) */

static int netstate_start(netstate *nsp) noex {
	int		rs = SR_FAULT ;
	if (nsp) {
	   rs = memclear(nsp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (netstate_start) */

static int netstate_reset(netstate *nsp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < netitem_overlast ; i += 1) {
	    if (nsp->item[i] != nullptr) {
	        rs1 = uc_free(nsp->item[i]) ;
	        if (rs >= 0) rs = rs1 ;
	        nsp->item[i] = nullptr ;
	    }
	} /* end for */
	nsp->c = 0 ;
	return rs ;
}
/* end subroutine (netstate_reset) */

static int netstate_item(netstate *nsp,int ki,cchar *sp,int sl) noex {
	int		rs = SR_INVALID ;
	if ((ki >= 0) && (ki < netitem_overlast)) {
	    rs = SR_OK ;
	    if (sp) {
	        if (nsp->item[ki] != nullptr) {
	            rs = uc_free(nsp->item[ki]) ;
	            nsp->item[ki] = nullptr ;
	        }
		if (rs >= 0) {
	            if (cchar *cp{} ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	                nsp->item[ki] = cp ;
	                nsp->c += 1 ;
	            } /* end if (memory-allocation) */
		} /* end if (ok) */
	    } /* end if (non-null) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (netstate_item) */

static int netstate_ready(netstate *nsp) noex {
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
}
/* end subroutine (netstate_ready) */

static int netstate_finish(netstate *nsp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; i < netitem_overlast ; i += 1) {
	    if (nsp->item[i] != nullptr) {
	        rs1 = uc_free(nsp->item[i]) ;
	        if (rs >= 0) rs = rs1 ;
	        nsp->item[i] = nullptr ;
	    }
	} /* end for */
	nsp->c = 0 ;
	return rs ;
}
/* end subroutine (netstate_finish) */

static int entry_start(NF_ENT *ep,netstate *nsp) noex {
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
}
/* end subroutine (entry_start) */

static int entry_finish(NF_ENT *ep) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->machine) {
	        rs1 = uc_free(ep->machine) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->machine = nullptr ;
	    }
	    if (ep->login) {
	        rs1 = uc_free(ep->login) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->login = nullptr ;
	    }
	    if (ep->password) {
	        rs1 = uc_free(ep->password) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->password = nullptr ;
	    }
	    if (ep->account) {
	        rs1 = uc_free(ep->account) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->account = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int getnii(int nki) noex {
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
}
/* end subroutine (getnii) */


