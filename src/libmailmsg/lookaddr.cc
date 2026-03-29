/* lookaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Addres-Look-List */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	lookaddr

	Description:
	This object provider address look-up for both the system
	and user white-lists and black-lists.

	Usage:
	int lookaddr_usercheck(LA *op,LA_US *up,cc *ema,int f_spam) noex

	Arguments:
	op		object pointer
	up		user-cursor pointer
	ema		e-mail-address
	f_spam		spam-flag:
				1=spam
				0=not_spam

	Returns:
	f_spam		spam-flag:
				1=spam
				0=not_spam
				<0=error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getusername.h>
#include	<getuserhome.h>
#include	<permx.h>
#include	<char.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"lookaddr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(eu)| */

/* local defines */

#define	LA		lookaddr
#define	LA_US		lookaddr_us

#define	LISTDNAME	"etc/mail"
#define	WLFNAME		"whitelist"
#define	BLFNAME		"blacklist"


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_stat(cchar *,ustat *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int lookaddr_ctor(lookaddr *op,Args ... args) noex {
    	LOOKADDR	*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->svp = new(nothrow) vecstr) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lookaddr_ctor) */

local int lookaddr_dtor(lookaddr *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->svp) ylikely {
		delete op->svp ;
		op->svp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lookaddr_dtor) */

template<typename ... Args>
local inline int lookaddr_magic(lookaddr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == LOOKADDR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (lookaddr_magic) */

local int lookaddr_loadvars(LA *,cchar *,cchar *) noex ;
local int lookaddr_swl(LA *,cchar *) noex ;
local int lookaddr_sbl(LA *,cchar *) noex ;
local int lookaddr_uwl(LA *,LA_US *,cchar *) noex ;
local int lookaddr_ubl(LA *,LA_US *,cchar *) noex ;


/* local variables */

/* addrlist file search (for system lists) */
constexpr cpcchar	sched2[] = {
	"%p/etc/%n/%n.%f",
	"%p/etc/%n/%f",
	"%p/etc/%n.%f",
	"%p/etc/mail/%n.%f",
	"%p/etc/mail/%f",
	"%p/etc/mail.%f",
	"%p/etc/%f",
	nullptr
} ; /* end array (sched2) */

/* addrlist file search (for local-user lists) */
constexpr cpcchar	sched3[] = {
	"%h/etc/%n/%n.%f",
	"%h/etc/%n/%f",
	"%h/etc/%n.%f",
	"%h/etc/mail/%n.%f",
	"%h/etc/mail/%f",
	"%h/etc/mail.%f",
	"%h/etc/%f",
	nullptr
} ; /* end array (sched3) */


/* exported variables */


/* exported subroutines */

int lookaddr_start(LA *op,cchar *pr,cchar *sn) noex {
	int		rs ;
	if ((rs = lookaddr_ctor(op,pr,sn)) >= 0) ylikely {
	    if (char *pb ; (rs = lm_mp(&pb)) >= 0) ylikely {
	        cint	vn = 2 ;
	        cint	vo = 0 ;
	    	op->pbuf = pb ;
		op->plen = rs ;
	        if ((rs = vecstr_start(op->svp,vn,vo)) >= 0) ylikely {
	            if ((rs = lookaddr_loadvars(op,pr,sn)) >= 0) ylikely {
	                op->magval = LOOKADDR_MAGIC ;
	            }
	            if (rs < 0) {
	                vecstr_finish(op->svp) ;
	            }
	        } /* end if (vecstr_start) */
	        if (rs < 0) {
		    void *vp = voidp(op->pbuf) ;
		    lm_free(vp) ;
		    op->pbuf = nullptr ;
		} /* end if (error) */
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
		lookaddr_dtor(op) ;
	    }
	} /* end if (lookaddr_ctor) */
	return rs ;
}
/* end subroutine (lookaddr_start) */

int lookaddr_finish(LA *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lookaddr_magic(op)) >= 0) ylikely {
	    if (op->open.swl) ylikely {
	        rs1 = whitelist_close(&op->swl) ;
	        if (rs >= 0) rs = rs1 ;
	        op->open.swl = false ;
	    }
	    if (op->open.sbl) ylikely {
	        rs1 = whitelist_close(&op->sbl) ;
	        if (rs >= 0) rs = rs1 ;
	        op->open.sbl = false ;
	    }
	    if (op->svp) ylikely {
	        rs1 = vecstr_finish(op->svp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->pbuf) ylikely {
		rs1 = lm_free(op->pbuf) ;
	        if (rs >= 0) rs = rs1 ;
		op->pbuf = nullptr ;
		op->plen = 0 ;
	    }
	    {
		rs1 = lookaddr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (lookaddr_magic) */
	return rs ;
}
/* end subroutine (lookaddr_finish) */

int lookaddr_userbegin(LA *op,LA_US *up,cchar *un) noex {
	int		rs ;
	if ((rs = lookaddr_magic(op,up,un)) >= 0) ylikely {
	    cint	hlen = op->plen ;
	    char	*hbuf = op->pbuf ;
	    memclear(up) ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) ylikely {
	        cint	hl = rs ;
	        if (ustat sb ; (rs = uc_stat(hbuf,&sb)) >= 0) ylikely {
	            if (S_ISDIR(sb.st_mode)) ylikely {
	                if ((rs = perm(hbuf,-1,-1,nullptr,X_OK)) >= 0) {
	                    if (cc *cp ; (rs = lm_strw(hbuf,hl,&cp)) >= 0) {
	                        up->dname = cp ;
	                    }
	                } else if (isNotPresent(rs)) {
	                    rs = SR_OK ;
		        }
	            } else {
	                rs = SR_NOTDIR ;
		    }
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	    if (rs >= 0) {
	        up->magval = LOOKADDR_MAGIC ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lookaddr_userbegin) */

int lookaddr_userend(LA *op,LA_US *up) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lookaddr_magic(op,up)) >= 0) ylikely {
	    rs = SR_NOTOPEN ;
	    if (up->magval == LOOKADDR_MAGIC) {
		rs = SR_OK ;
	        if (up->open.uwl) ylikely {
	            rs1 = whitelist_close(&up->uwl) ;
	            if (rs >= 0) rs = rs1 ;
	            up->open.uwl = false ;
	        }
	        if (up->open.ubl) ylikely {
	            rs1 = whitelist_close(&up->ubl) ;
	            if (rs >= 0) rs = rs1 ;
	            up->open.ubl = false ;
	        }
	        if (up->dname) ylikely {
		    void *vp = voidp(up->dname) ;
	            rs1 = libmem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            up->dname = nullptr ;
	        }
	    } /* end if (user-magic) */
	} /* end if (object-magic) */
	return rs ;
}
/* end subroutine (lookaddr_userend) */

/* result: 0=ok, 1=bad */
int lookaddr_usercheck(LA *op,LA_US *up,cchar *ema,int f) noex {
	int		rs ;
	if ((rs = lookaddr_magic(op,up,ema)) >= 0) ylikely {
	    if (f) {
	        if ((rs = lookaddr_swl(op,ema)) == 0) {
	            if ((rs = lookaddr_uwl(op,up,ema)) > 0) {
	                f = false ;
	            }
	        } else if (rs > 0) {
	            f = false ;
	        }
	    } else {
	        if ((rs = lookaddr_sbl(op,ema)) > 0) {
	            f = true ;
	            if ((rs = lookaddr_swl(op,ema)) == 0) {
	                if ((rs = lookaddr_uwl(op,up,ema)) > 0) {
	                    f = false ;
	                }
	            } else if (rs > 0) {
	                f = false ;
	            }
	        } else {
	            if ((rs = lookaddr_ubl(op,up,ema)) > 0) {
	                f = true ;
	                if ((rs = lookaddr_uwl(op,up,ema)) > 0) {
	                    f = false ;
	                }
	            }
	        }
	    } /* end if (spam or not) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (lookaddr_usercheck) */


/* private subroutines */

local int lookaddr_loadvars(LA *op,cchar *pr,cchar *sn) noex {
	int		rs ;
	if ((rs = vecstr_envset(op->svp,"p",pr,-1)) >= 0) ylikely {
	    rs = vecstr_envset(op->svp,"n",sn,-1) ;
	}
	return rs ;
}
/* end subroutine (lookaddr_loadvars) */

local int lookaddr_swl(LA *op,cchar *ema) noex {
	int		rs = SR_OK ;
	if (! op->init.swl) {
	    vecstr	*svp = op->svp ;
	    cint	tlen = op->plen ;
	    cchar	*fn = WLFNAME ;
	    char	*tbuf = op->pbuf ;
	    op->init.swl = true ;
	    if ((rs = permsched(sched2,svp,tbuf,tlen,fn,R_OK)) >= 0) {
	        if ((rs = whitelist_open(&op->swl,tbuf)) >= 0) {
	            rs = SR_OK ;
	            op->open.swl = true ;
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (init-needed) */
	if ((rs >= 0) && op->open.swl) {
	    if ((rs = whitelist_prematch(&op->swl,ema)) > 0) {
	        rs = 1 ;
	    }
	}
	return rs ;
}
/* end subroutine (lookaddr_swl) */

local int lookaddr_sbl(LA *op,cchar *ema) noex {
	int		rs = SR_OK ;
	if (! op->init.sbl) {
	    vecstr	*svp = op->svp ;
	    cint	tlen = op->plen ;
	    cchar	*fn = BLFNAME ;
	    char	*tbuf = op->pbuf ;
	    op->init.sbl = true ;
	    if ((rs = permsched(sched2,svp,tbuf,tlen,fn,R_OK)) >= 0) {
	        if ((rs = whitelist_open(&op->sbl,tbuf)) >= 0) {
	            rs = SR_OK ;
	            op->open.sbl = true ;
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (init-needed) */
	if ((rs >= 0) && op->open.sbl) {
	    if ((rs = whitelist_prematch(&op->sbl,ema)) > 0) {
	        rs = 1 ;
	    }
	}
	return rs ;
}
/* end subroutine (lookaddr_sbl) */

local int lookaddr_uwl(LA *op,LA_US *up,cchar *ema) noex {
	int		rs = SR_OK ;
	if (up->dname != nullptr) {
	    if (! up->init.uwl) {
	        vecstr	*svp = op->svp ;
	        cint	tlen = op->plen ;
	        cchar	*fn = WLFNAME ;
	        char	*tbuf = op->pbuf ;
	        up->init.uwl = true ;
	        if ((rs = svp->envset("h",up->dname,-1)) >= 0) {
	            if ((rs = permsched(sched3,svp,tbuf,tlen,fn,R_OK)) >= 0) {
	                if ((rs = whitelist_open(&up->uwl,tbuf)) >= 0) {
	                    rs = SR_OK ;
	                    up->open.uwl = true ;
	                }
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
		    }
	        } /* end if (vecstr_envset) */
	    } /* end if (init-needed) */
	    if ((rs >= 0) && up->open.uwl) {
	        if ((rs = whitelist_prematch(&up->uwl,ema)) > 0) {
	            rs = 1 ;
	        }
	    }
	} /* end if (have-real-user) */
	return rs ;
}
/* end subroutine (lookaddr_uwl) */

local int lookaddr_ubl(LA *op,LA_US *up,cchar *ema) noex {
	int		rs = SR_OK ;
	if (up->dname != nullptr) {
	    if (! up->init.ubl) {
	        vecstr	*svp = op->svp ;
	        cint	tlen = op->plen ;
	        cchar	*fn = BLFNAME ;
	        char	*tbuf = op->pbuf ;
	        up->init.ubl = true ;
	        if ((rs = vecstr_envset(svp,"h",up->dname,-1)) >= 0) {
		    cint	am = R_OK ;
	            if ((rs = permsched(sched3,svp,tbuf,tlen,fn,am)) >= 0) {
	                if ((rs = whitelist_open(&up->ubl,tbuf)) >= 0) {
	                    rs = SR_OK ;
	                    up->open.ubl = true ;
	                }
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
		    }
	        } /* end if (vecstr_envset) */
	    } /* end if (init-needed) */
	    if ((rs >= 0) && up->open.ubl) {
	        if ((rs = whitelist_prematch(&up->ubl,ema)) > 0) {
	            rs = 1 ;
	        }
	    }
	} /* end if (have-real-user) */
	return rs ;
}
/* end subroutine (lookaddr_ubl) */


