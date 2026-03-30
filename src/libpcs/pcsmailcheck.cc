/* pcsmailcheck SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* determine if the given user has mail (as PCS determines it) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsmailcheck

  	Description:
	This subroutine determines if the given user (supplied) has
	mail waiting (unread).

	Synopsis:
	int pcsmailcheck(cc *pr,char *rbuf,int rlen,cc *un) noex

	Arguments:
	pr		PCS system program root (if available)
	rbuf		buffer to hold result
	rlen		length of supplied result buffer
	un		username to check

	Returns:
	>=0		OK
	<0		error (system-return)


	= The "mail" situation

        What follows is still based on the idea that mail comes from the system
        in a spool area. Here are the rules for finding the spool area:

        + the "MAIL" encironment variable can only hold ONE filename (this
        restriction is from the SHELL)

        + the "MAILPATH" environment variable is strictly a SHELL device and not
        related to system mail operations

        + the "MAILDIR" environment variable was introduced to specify the
        mail-spool-directory, but it was introduced to hold only a single
        directory name

        + so we use the newer "MAILDIRS" environment variable, which holds one
        or more mail-spool-directory names

        + a pool of mail-spool-directories is compiled by gathering up all
        directories from the the "MAIL", "MAILDIR", and "MAILDIRS" environment
        variables

	+ enjoy

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<cstddef>		/* |wchar_t| */
#include	<cstdlib>		/* |getenv(3c)| others */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getbufsize.h>
#include	<getusername.h>
#include	<estrings.h>
#include	<vecstr.h>
#include	<dirlist.h>
#include	<strn.h>
#include	<mkx.h>
#include	<pathadd.h>
#include	<mailbox.h>		/* MAILMSG */
#include	<hdrdecode.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"pcsmailcheck.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| + |sysword(3u)|  */

/* local defines */

#ifndef	VARPRPCS
#define	VARPRPCS	"PCS"
#endif

#define	SI		subinfo
#define	SI_FL		subinfo_fl

#define	ISEND(c)	(((c) == '\n') || ((c) == '\r'))


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	maxpathlen ;
	int	mailaddrlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */

namespace {
    struct checker {
	cchar		*pr ;
	cchar		*un ;
	char		*dbuf ;
	int		dlen ;
	checker(cc *p,char *b,int l,cc *u) noex :  pr(p), dbuf(b), dlen(l) {
	    un = u ;
	} ;
	operator int () noex ;
    } ; /* end struct (checker) */
} /* end namespace */

struct subinfo_fl {
	uint		userself:1 ;
	uint		uid:1 ;
	uint		allocusername:1 ;
} ; /* end struct */

struct subinfo {
	cchar		*pr ;
	cchar		*username ;
	char		*a ;		/* allocation reference point */
	char		*rbuf ;		/* supplied argument */
	char		*fbuf ;		/* allocated */
	char		*tbuf ;
	subinfo_fl	init, fl ;
	dirlist		*mlp ;
	time_t		ti_first ;
	uid_t		uid ;
	int		rlen ;		/* supplied argument */
	int		tlen ;
	int		flen ;		/* allocated amount */
	int		froml ;		/* from-address len (given us) */
} ; /* end struct */

typedef int (*subinfo_f)(SI *) noex ;


/* forward references */

local int	subinfo_start(SI *,cchar *,char *,int,cchar *) noex ;
local int	subinfo_finish(SI *) noex ;
local int	subinfo_username(SI *,cchar *) noex ;
local int	subinfo_getfrom(SI *) noex ;
local int	subinfo_cvtfrom(SI *) noex ;

#ifdef	COMMENT
local int	subinfo_userself(SI *) noex ;
local int	subinfo_getuid(SI *,uid_t *) noex ;
#endif

local int	subinfo_getsysmail(SI *) noex ;
local int	subinfo_mailfile(SI *) noex ;

local int	maildirs_begin(SI *) noex ;
local int	maildirs_varmaildirs(SI *,cchar *) noex ;
local int	maildirs_varmail(SI *,cchar *) noex ;
local int	maildirs_default(SI *,cchar *) noex ;
local int	maildirs_add(SI *,cchar *,int) noex ;
local int	maildirs_end(SI *) noex ;


/* local variables */

constexpr subinfo_f	getmails[] = {
	subinfo_getsysmail,
	nullptr
} ; /* end array (subinfo_f) */

constexpr int		rsdirs[] = {
	SR_ACCESS,
	SR_NOENT,
	SR_NAMETOOLONG,
	SR_NOLINK,
	SR_NOTDIR,
	0
} ; /* end array (rsdirs) */

static vars		var ;


/* exported variables */


/* exported subroutines */

int pcsmailcheck(cchar *pr,char *dbuf,int dlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		n = 0 ; /* return-value */
	if (dbuf && un) {
	    if (pr == nullptr) {
		static cchar *vpr = getenv(VARPRPCS) ;
		pr = vpr ;
	    }
	    if (pr) {
		static cint	rsv = var ;
	        dbuf[0] = '\0' ;
		if ((rs = rsv) >= 0) {
		    if (checker co(pr,dbuf,dlen,un) ; (rs = co) >= 0) {
		        n = rs ;
		    }
		} /* end if (vars) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (pcsmailcheck) */


/* local subroutines */

template<typename ... Args>
local inline int subinfo_ctor(SI *op,Args ... args) noex {
    	subinfo		*hop = op ;
    	cnullptr	np{} ;
	cnothrow	nt{} ;
    	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->mlp = new(nt) dirlist) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_ctor) */

local int subinfo_dtor(SI *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->mlp) {
		delete op->mlp ;
		op->mlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (subinfo_dtor) */

local int subinfo_start(SI *sip,cchar *pr,char *rbuf,int rlen,cchar *un) noex {
	int		rs ;
	if ((rs = subinfo_ctor(sip,pr,rbuf,un)) >= 0) {
	    sip->pr = pr ;
	    sip->rbuf = rbuf ;
	    sip->rlen = rlen ;
	    sip->ti_first = TIME_MAX ;
	    if ((rs = dirlist_start(sip->mlp)) >= 0) {
	        if ((rs = subinfo_username(sip,un)) >= 0) {
		    cint	tlen = var.maxpathlen ;
		    cint	flen = var.mailaddrlen ;
		    int	sz = 0 ;
		    sz += (tlen + 1) ;
		    sz += (flen + 1) ;
		    if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
		        sip->a = bp ;
		        sip->tbuf = bp ;
		        sip->tlen = tlen ;
		        sip->fbuf = (bp + (tlen + 1)) ;
		        sip->flen = flen ;
		    } /* end if (memory-allocation) */
	        } /* end if (subinfo_username) */
	        if (rs < 0) {
	            dirlist_finish(sip->mlp) ;
	        }
	    } /* end if (dirlist_start) */
	    if (rs < 0) {
		subinfo_dtor(sip) ;
	    }
	} /* end if (subinfo_ctor) */
	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    if (sip->fl.allocusername && sip->username) {
	        void *vp = voidp(sip->username) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        sip->username = nullptr ;
	    }
	    if (sip->a) {
	        rs1 = lm_free(sip->a) ;
	        if (rs >= 0) rs = rs1 ;
	        sip->a = nullptr ;
	        sip->tbuf = nullptr ;
	        sip->tlen = 0 ;
	        sip->fbuf = nullptr ;
	        sip->flen = 0 ;
	    }
	    if (sip->mlp) {
	        rs1 = dirlist_finish(sip->mlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = subinfo_dtor(sip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    sip->pr = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

local int subinfo_username(SI *sip,cchar *un) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	sip->username = un ;
	if ((un == nullptr) || (un[0] == '\0') || (un[0] == '-')) {
	    static cchar *vusername = getenv(varname.username) ;
	    if (vusername && vusername[0]) {
	        sip->username = vusername ;
	    } else {
		if (char *ubuf ; (rs = lm_un(&ubuf)) >= 0) {
		    cint ulen = rs ;
	            if ((rs = getusername(ubuf,ulen,-1)) >= 0) {
	                if (cchar *cp ; (rs = lm_strw(ubuf,rs,&cp)) >= 0) {
	                    sip->fl.allocusername = true ;
	                    sip->username = cp ;
	                }
	            } /* end if (getusername) */
		    rs1 = lm_free(ubuf) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if */
	} /* end if (getting username) */
	return rs ;
}
/* end subroutine (subinfo_username) */

#ifdef	COMMENT
local int subinfo_userself(SI *sip) noex {
	int		rs = SR_OK ;
	if (! sip->init.userself) {
	    cchar	*cp ;
	    sip->init.userself = true ;
	    if (((cp = getenv(varname.username)) != nullptr) &&
	        (strcmp(cp,sip->username) == 0)) {

	        sip->fl.userself = true ;

	    } /* end if */

	} /* end if (initializing UID) */
	if ((rs >= 0) && (! sip->fl.userself)) {
	    rs = SR_SRCH ;
	}
	return rs ;
}
/* end subroutine (subinfo_userself) */
#endif /* COMMENT */

#ifdef	COMMENT
local int subinfo_getuid(SI *sip,uid_t *uidp) noex {
	int		rs = SR_OK ;
	if (! sip->init.uid) {
	    cchar	*var = varname.username ;
	    cchar	*un = sip->username ;
	    cchar	*cp ;
	    sip->init.uid = true ;
	    if (((cp = getenv(var)) != nullptr) && (strcmp(cp,un) == 0)) {
	        sip->fl.uid = true ;
	        sip->uid = getuid() ;
	    } else {
		if ((rs = getuid_name(un,-1)) >= 0) {
	            sip->fl.uid = true ;
	            sip->uid = rs ;
	        }
	    } /* end if */
	} /* end if (initializing UID) */
	if (uidp != nullptr) {
	    *uidp = sip->uid ;
	}
	if ((rs >= 0) && (! sip->fl.uid)) {
	    rs = SR_NOTFOUND ;
	}
	return rs ;
}
/* end subroutine (subinfo_getuid) */
#endif /* COMMENT */

local int subinfo_getsysmail(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */

#if	CF_DEBUGS
	debugprintf("pcsmailcheck/subinfo_getsysmail: ent\n") ;
#endif

	if ((rs = maildirs_begin(sip)) >= 0) {
	    dirlist	*dlp = sip->mlp ;
	    dirlist_cur	cur ;
	    if ((rs = dirlist_curbegin(dlp,&cur)) >= 0) {
	        cint	tlen = sip->tlen ;
	        int		dl ;
		cchar		*un = sip->username ;
	        char		*tbuf = sip->tbuf ;
	        while (rs >= 0) {
	            dl = dirlist_curenum(dlp,&cur,tbuf,tlen) ;
	            if (dl == SR_NOTFOUND) break ;
		    rs = dl ;
#if	CF_DEBUGS
		    debugprintf("pcsmailcheck/subinfo_getsysmail: "
			    "dirlist_curenum() rs=%d\n",rs) ;
		    debugprintf("pcsmailcheck/subinfo_getsysmail: "
			    "md=%r\n",tbuf,dl) ;
#endif
		    if (rs >= 0) {
	                if ((rs = pathadd(tbuf,dl,un)) >= 0) {
			    rs = subinfo_mailfile(sip) ;
			    c += rs ;
			} /* end if (pathadd) */
	            } /* end if (ok) */
	        } /* end while */
	        rs1 = dirlist_curend(dlp,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (dirlist-cursor) */
	    rs1 = maildirs_end(sip) ;
	    if (rs >= 0) rs = rs1 ;

	} /* end if (maildirs) */

#if	CF_DEBUGS
	debugprintf("pcsmailcheck/subinfo_getsysmail: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_getsysmail) */

local int subinfo_mailfile(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*mfn = sip->tbuf ;

#if	CF_DEBUGS
	debugprintf("pcsmailcheck/subinfo_mailfile: ent mfn=%s\n",mfn) ;
#endif

	if (ustat sb ; (rs = u_stat(mfn,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode) && (sb.st_size > 0)) {
		mailbox		mb ;
		mailbox_info	mbinfo ;
		cint	mo = (mailboxm.rdonly | mailboxm.noclen) ;
		if ((rs = mailbox_open(&mb,mfn,mo)) >= 0) {
	    	    if ((rs = mailbox_getinfo(&mb,&mbinfo)) >= 0) {
	                if ((c = mbinfo.nmsgs) > 0) {
			    cint	mi = (c-1) ;
			    cint	tl = sip->flen ;
			    char	*tb = sip->fbuf ;
	            	    if ((rs = mailbox_getfrom(&mb,tb,tl,mfn,mi)) >= 0) {
				sip->froml = rs ; /* returned length */

#if	CF_DEBUGS
	        	    debugprintf("pcsmailcheck/subinfo_mailfile: "
				"mailbox_getfrom() rs=%d\n",rs) ;
	        	    debugprintf("pcsmailcheck/subinfo_mailfile: "
				"f=>%r<\n",sip->fbuf,rs) ;
#endif
			    } else if (rs == SR_OVERFLOW) {
				tb[0] = '\0' ;
				rs = SR_OK ;
			    }
			} /* end if (positive) */

	            } /* end if (n-msgs) */
	            rs1 = mailbox_close(&mb) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mailbox) */
	    } /* end if (reg-file) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (exists) */

#if	CF_DEBUGS
	debugprintf("pcsmailcheck/subinfo_mailfile: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_mailfile) */

local int subinfo_getfrom(SI *sip) noex {
	int		rs = SR_OK ;
	int		n = 0 ;
	for (int i = 0 ; (rs >= 0) && getmails[i] ; i += 1) {
	    rs = (*getmails[i])(sip) ;
	    n += rs ;
	} /* end for */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (subinfo_getfrom) */

local int subinfo_cvtfrom(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		froml = sip->froml ;
	int		wlen = sip->froml ;
	int		sz ;
	int		len = 0 ; /* return-value */
#if	CF_DEBUGS
	debugprintf("pcsmailcheck/subinfo_cvtfrom: ent\n") ;
#endif
	sz = ((wlen + 1) * szof(wchar_t)) ;
	if (wchar_t *wbuf ; (rs = lm_mall(sz,&wbuf)) >= 0) {
	    hdrdecode	d ;
#if	CF_DEBUGS
	debugprintf("pcsmailcheck/subinfo_cvtfrom: hdrdecode\n") ;
#endif
	    if ((rs = hdrdecode_start(&d,sip->pr)) >= 0) {
		cchar		*fbuf = sip->fbuf ;
#if	CF_DEBUGS
	debugprintf("pcsmailcheck/subinfo_cvtfrom: mid1\n") ;
#endif
		if ((rs = hdrdecode_proc(&d,wbuf,wlen,fbuf,froml)) >= 0) {
		    cint	dlen = (2 * rs) ;
		    int		wl = rs ;
		    if (char *dbuf ; (rs = lm_mall((dlen+1),&dbuf)) >= 0) {
			if (wl > sip->rlen) wl = sip->rlen ;
		        if ((rs = snwcpywidehdr(dbuf,dlen,wbuf,wl)) >= 0) {
			    rs = mkaddrdisp(sip->rbuf,sip->rlen,dbuf,rs) ;
		            len = rs ;
		        }
			rs1 = lm_free(dbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		}
		rs1 = hdrdecode_finish(&d) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (hdrdecode) */
	    rs1 = lm_free(wbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_cvtfrom) */

local int maildirs_begin(SI *sip) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	if (rs >= 0) {
	    static cchar *vmaildir = getenv(varname.maildir) ;
	    if (vmaildir) {
	        rs = maildirs_varmaildirs(sip,vmaildir) ;
	        c += rs ;
	    }
	}
	if (rs >= 0) {
	    static cchar *vmaildirs = getenv(varname.maildirs) ;
	    if (vmaildirs) {
	        rs = maildirs_varmaildirs(sip,vmaildirs) ;
	        c += rs ;
	    }
	}
	if (rs >= 0) {
	    static cchar *vmail = getenv(varname.mail) ;
	    if (vmail) {
	        rs = maildirs_varmail(sip,vmail) ;
	        c += rs ;
	    }
	}
	if (rs >= 0) {
	    rs = maildirs_default(sip,sysword.w_maildir) ;
	    c += rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (maildirs_begin) */

local int maildirs_end(SI *sip) noex {
    	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (maildirs_end) */

local int maildirs_varmaildirs(SI *sip,cchar *sp) noex {
	dirlist		*vlp = sip->mlp ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (vlp && sp) {
	    int		sl = lenstr(sp) ;
	    int		cl ;
	    cchar	*cp ;
	    for (cchar *tp ; (tp = strnbrk(sp,sl," \t,:;")) != np ; ) {
		cint tl = intconv(tp - sp) ;
	        if ((cl = sfshrink(sp,tl,&cp)) > 0) {
	            rs = maildirs_add(sip,cp,cl) ;
	            c += rs ;
	        } /* end if */
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (sl > 0)) {
	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
	            rs = maildirs_add(sip,cp,cl) ;
	            c += rs ;
	        } /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (maildirs_varmaildirs) */

local int maildirs_varmail(SI *sip,cchar *mvfn) noex {
	dirlist		*vlp = sip->mlp ;
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (vlp) {
	    if ((mvfn != nullptr) && (mvfn[0] != '\0')) {
	        cchar	*cp ;
	        if (int cl ; (cl = sfdirname(mvfn,-1,&cp)) > 0) {
	            rs = maildirs_add(sip,cp,cl) ;
	            c += rs ;
	        } /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (maildirs_varmail) */

local int maildirs_default(SI *sip,cchar *sp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (sp) {
	    rs = SR_OK ;
	    if (sp[0] != '\0') {
	        rs = maildirs_add(sip,sp,-1) ;
	        c += rs ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (maildirs_default) */

local int maildirs_add(SI *sip,cchar *cp,int cl) noex {
	dirlist		*vlp = sip->mlp ;
	int		rs ;
	int		f_added = false ;
	if ((rs = dirlist_add(vlp,cp,cl)) >= 0) {
	    f_added = (rs > 0) ;
	} else if (isOneOf(rsdirs,rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f_added : rs ;
}
/* end subroutine (maildirs_add) */

checker::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if (SI si ; (rs = subinfo_start(&si,pr,dbuf,dlen,un)) >= 0) {
	    if ((rs = subinfo_getfrom(&si)) >= 0) {
		n = rs ;
		rs = subinfo_cvtfrom(&si) ;
	    } /* end if (subinfo_getfrom) */
	    rs1 = subinfo_finish(&si) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */
	return (rs >= 0) ? n : rs ;
} /* end method (checker::operator) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(bufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	    if ((rs = getbufsize(bufsize_mailaddr)) >= 0) {
		mailaddrlen = rs ;
	    }
	}
	return rs ;
} /* end method (vars::operator) */


