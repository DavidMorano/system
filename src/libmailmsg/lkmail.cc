/* lkmail SUPPORT (Lock-Mail) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a mail lock */
/* version %I% last-modified %G% */

#define	CF_SGIDGROUPS	0		/* SGID to a supplemetal group? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	lkmail

	Description:
	This object is used to manipulate a mail lock file.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getbufsize.h>
#include	<sbuf.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<mkfname.h>
#include	<localmisc.h>

#include	"lkmail.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	LKMAIL_AGE
#define	LKMAIL_AGE	(5 * 60)
#endif

#ifndef	CF_SGIDGROUPS
#define	CF_SGIDGROUPS	0
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int lkmail_ctor(lkmail *op,Args ... args) noex {
    	LKMAIL		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    op->lfd = -1 ;
	    if (char *cp ; (rs = lm_mp(&cp)) >= 0) ylikely {
	        op->lockfname = cp ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lkmail_ctor) */

local int lkmail_dtor(lkmail *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->lockfname) ylikely {
		rs1 = lm_free(op->lockfname) ;
		if (rs >= 0) rs = rs1 ;
		op->lockfname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lkmail_dtor) */

template<typename ... Args>
local inline int lkmail_magic(lkmail *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == LKMAIL_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (lkmail_magic) */

local int	lkmail_starter(lkmail *) noex ;


/* local variables */

cbool		f_comment = false ;

cbool		f_sgidgroups = CF_SGIDGROUPS ;


/* exported variables */


/* exported subroutines */

int lkmail_start(lkmail *op,lkmail_ids *idp,cchar *mfname) noex {
	int		rs ;
	if ((rs = lkmail_ctor(op,idp,mfname)) >= 0) ylikely {
	    cchar	*suf = ".lock" ;
	    if ((rs = mkfnamesuf1(op->lockfname,mfname,suf)) >= 0) ylikely {
	        op->id = *idp ;
	        if (op->id.gid_maildir <= 0) {
		    if ((rs = lkmail_starter(op)) >= 0) {
	        	op->magic = LKMAIL_MAGIC ;
		    }
		} /* end if (maildir) */
	    } /* end if (mkfnamesuf) */
	    if (rs < 0) {
		lkmail_dtor(op) ;
	    }
	} /* end if (lkmail_ctor) */
	return rs ;
}
/* end subroutine (lkmail_start) */

int lkmail_finish(lkmail *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lkmail_magic(op)) >= 0) ylikely {
	    if (op->lfd >= 0) ylikely {
	        rs1 = u_close(op->lfd) ;
	        if (rs >= 0) rs = rs1 ;
		op->lfd = -1 ;
	    }
	    {
		rs1 = lkmail_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lkmail_finish) */

int lkmail_create(lkmail *op) noex {
	cint		nrs = SR_NOENT ;
	int		rs ;
	int		lfd = -1 ;
	if ((rs = lkmail_magic(op)) >= 0) ylikely {
	    if (op->lfd >= 0) {
	        u_close(op->lfd) ;
	        op->lfd = -1 ;
	    }
	    /* blow out if the lock file is already there! */
	    if (ustat sb ; (rs = u_stat(op->lockfname,&sb)) == nrs) {
	        /* go ahead and try to create the lock file */
		if_constexpr (f_comment) {
	            rs = u_creat(op->lockfname,0444) ;
		} else {
	            rs = SR_ACCES ;
		} /* end if_constexpr (f_comment) */
	        lfd = rs ;
	        if ((rs == SR_ACCES) && (op->id.egid != op->id.gid)) {
	            if ((rs = u_setegid(op->id.gid_maildir)) >= 0) {
	                rs = u_creat(op->lockfname,0444) ;
	                lfd = rs ;
	                u_setegid(op->id.gid) ;
	            }
	        } /* end if (tryint to SGID to maildir) */
		if_constexpr (f_sgidgroups) {
	            if ((rs == SR_ACCES) || (rs == SR_PERM)) {
	                if ((rs = u_setegid(op->id.gid_maildir)) >= 0) {
	                    rs = u_creat(op->lockfname,0444) ;
	                    lfd = rs ;
	                    u_setegid(op->id.gid) ;
	                }
	            } /* end if (tryint to SGID to maildir) */
		} /* end if_constexpr (f_sgidgroups) */
	        if ((rs == SR_ACCES) || (rs == SR_PERM)) {
	            rs = u_creat(op->lockfname,0444) ;
	            lfd = rs ;
	        }
	        if (op->id.euid == 0) {
	            if ((rs == SR_ACCES) || (rs == SR_PERM)) {
	                u_setegid(op->id.gid_maildir) ;
	                rs = u_creat(op->lockfname,0444) ;
	                lfd = rs ;
	                u_setegid(op->id.gid) ;
	            }
	            if ((rs == SR_ACCES) || (rs == SR_PERM)) {
	                u_seteuid(op->id.uid_maildir) ;
	                rs = u_creat(op->lockfname,0444) ;
	                lfd = rs ;
	                u_seteuid(op->id.uid) ;
	            } /* end if */
	        } /* end if (playing as super user) */
	        if (rs >= 0) {
	            op->lfd = lfd ;
	        } else {
	            rs = SR_TXTBSY ;
	        }
	    } /* end if (u_stat) */
	} /* end if (magic) */
	return (rs >= 0) ? lfd : rs ;
}
/* end subroutine (lkmail_create) */

int lkmail_unlink(lkmail *op) noex {
	int		rs ;
	if ((rs = lkmail_magic(op)) >= 0) ylikely {
	    rs = SR_ACCESS ;
	    if (op->id.egid != op->id.gid) {
	        u_setegid(op->id.egid) ;
	        rs = u_unlink(op->lockfname) ;
	        u_setegid(op->id.gid) ;
	    }
	    if (rs == SR_ACCES) {
	        rs = u_unlink(op->lockfname) ;
	    }
	    if (op->id.euid == 0) {
	        if (rs == SR_ACCES) {
	            u_setegid(op->id.gid_maildir) ;
	            rs = u_unlink(op->lockfname) ;
	            u_setegid(op->id.gid) ;
	        }
	        if (rs == SR_ACCES) {
	            u_seteuid(op->id.uid_maildir) ;
	            rs = u_unlink(op->lockfname) ;
	            u_seteuid(op->id.uid) ;
	        }
	    } /* end if (trying as superuser) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lkmail_unlink) */

int lkmail_old(lkmail *op,time_t dt,int age) noex {
	int		rs ;
	int		f = false ;
	if ((rs = lkmail_magic(op)) >= 0) ylikely {
	    if (age < 0) age = LKMAIL_AGE ;
	    if (ustat sb ; (rs = u_stat(op->lockfname,&sb)) >= 0) {
	        f = ((dt - sb.st_mtime) > age) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (lkmail_old) */


/* private subrouties */

local int lkmail_starter(lkmail *op) noex {
    	int		rs ;
	int		rs1 ;
	if (char *mbuf ; (rs = lm_mp(&mbuf)) >= 0) ylikely {
	    cchar	*fn = op->lockfname ;
	    cchar	*cp ;
	    if (int cl ; (cl = sfdirname(fn,-1,&cp)) > 0) {
	        if ((rs = mkpath1w(mbuf,cp,cl)) >= 0) {
		    if (ustat sb ; (rs = u_stat(mbuf,&sb)) >= 0) {
			op->id.uid_maildir = sb.st_uid ;
			op->id.gid_maildir = sb.st_gid ;
		    }
		}
	    } /* end if (sfdirname) */
	    rs1 = lm_free(mbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (lkmail_starter) */


