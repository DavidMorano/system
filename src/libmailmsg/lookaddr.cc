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
#include	<usystem.h>
#include	<mallocxx.h>
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

import libutil ;			/* |gelenstr(eu)| */

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


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int lookaddr_ctor(lookaddr *op,Args ... args) noex {
    	LOOKADDR	*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->svp = new(nothrow) vecstr) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaddr_ctor) */

static int lookaddr_dtor(lookaddr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->svp) {
		delete op->svp ;
		op->svp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaddr_dtor) */

template<typename ... Args>
static inline int lookaddr_magic(lookaddr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LOOKADDR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (lookaddr_magic) */

static int lookaddr_loadvars(LA *,cchar *,cchar *) noex ;
static int lookaddr_swl(LA *,cchar *) noex ;
static int lookaddr_sbl(LA *,cchar *) noex ;
static int lookaddr_uwl(LA *,LA_US *,cchar *) noex ;
static int lookaddr_ubl(LA *,LA_US *,cchar *) noex ;


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
} ;

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
} ;


/* exported variables */


/* exported subroutines */

int lookaddr_start(LA *op,cchar *pr,cchar *sn) noex {
	int		rs ;
	if ((rs = lookaddr_ctor(op,pr,sn)) >= 0) {
	    if (char *pb ; (rs = malloc_mp(&pb)) >= 0) {
	        cint	vn = 2 ;
	        cint	vo = 0 ;
	    	op->pbuf = pb ;
		op->plen = rs ;
	        if ((rs = vecstr_start(op->svp,vn,vo)) >= 0) {
	            if ((rs = lookaddr_loadvars(op,pr,sn)) >= 0) {
	                op->magic = LOOKADDR_MAGIC ;
	            }
	            if (rs < 0) {
	                vecstr_finish(op->svp) ;
	            }
	        } /* end if (vecstr_start) */
	        if (rs < 0) {
		    void *vp = voidp(op->pbuf) ;
		    malloc_free(vp) ;
		    op->pbuf = nullptr ;
		}
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
	if ((rs = lookaddr_magic(op)) >= 0) {
	    if (op->open.swl) {
	        rs1 = whitelist_close(&op->swl) ;
	        if (rs >= 0) rs = rs1 ;
	        op->open.swl = false ;
	    }
	    if (op->open.sbl) {
	        rs1 = whitelist_close(&op->sbl) ;
	        if (rs >= 0) rs = rs1 ;
	        op->open.sbl = false ;
	    }
	    if (op->svp) {
	        rs1 = vecstr_finish(op->svp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->pbuf) {
		rs1 = malloc_free(op->pbuf) ;
	        if (rs >= 0) rs = rs1 ;
		op->pbuf = nullptr ;
		op->plen = 0 ;
	    }
	    {
		rs1 = lookaddr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (lookaddr_magic) */
	return rs ;
}
/* end subroutine (lookaddr_finish) */

int lookaddr_userbegin(LA *op,LA_US *up,cchar *un) noex {
	int		rs ;
	if ((rs = lookaddr_magic(op,up,un)) >= 0) {
	    cint	hlen = op->plen ;
	    char	*hbuf = op->pbuf ;
	    memclear(up) ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
	        cint	hl = rs ;
	        if (ustat sb ; (rs = uc_stat(hbuf,&sb)) >= 0) {
	            if (S_ISDIR(sb.st_mode)) {
	                if ((rs = perm(hbuf,-1,-1,nullptr,X_OK)) >= 0) {
	                    cchar	*cp ;
	                    if ((rs = libmem.strw(hbuf,hl,&cp)) >= 0) {
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
	        up->magic = LOOKADDR_MAGIC ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lookaddr_userbegin) */

int lookaddr_userend(LA *op,LA_US *up) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lookaddr_magic(op,up)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (up->magic == LOOKADDR_MAGIC) {
	        rs = SR_OK ;
	        if (up->open.uwl) {
	            rs1 = whitelist_close(&up->uwl) ;
	            if (rs >= 0) rs = rs1 ;
	            up->open.uwl = false ;
	        }
	        if (up->open.ubl) {
	            rs1 = whitelist_close(&up->ubl) ;
	            if (rs >= 0) rs = rs1 ;
	            up->open.ubl = false ;
	        }
	        if (up->dname) {
		    void *vp = voidp(up->dname) ;
	            rs1 = libmem.free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            up->dname = nullptr ;
	        }
	        up->magic = 0 ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lookaddr_userend) */

/* result: 0=ok, 1=bad */
int lookaddr_usercheck(LA *op,LA_US *up,cchar *ema,int f) noex {
	int		rs ;
	if ((rs = lookaddr_magic(op,up,ema)) >= 0) {
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

static int lookaddr_loadvars(LA *op,cchar *pr,cchar *sn) noex {
	int		rs ;
	if ((rs = vecstr_envset(op->svp,"p",pr,-1)) >= 0) {
	    rs = vecstr_envset(op->svp,"n",sn,-1) ;
	}
	return rs ;
}
/* end subroutine (lookaddr_loadvars) */

static int lookaddr_swl(LA *op,cchar *ema) noex {
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

static int lookaddr_sbl(LA *op,cchar *ema) noex {
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

static int lookaddr_uwl(LA *op,LA_US *up,cchar *ema) noex {
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

static int lookaddr_ubl(LA *op,LA_US *up,cchar *ema) noex {
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


