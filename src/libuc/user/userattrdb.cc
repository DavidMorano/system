/* userattrdb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* user-attribute database management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-07, David A­D­ Morano
	I wrote this so that both the old UDOMAIN facility and the
	newer user-attribute database capability of Sun Solaris®
	2.6 (first that I have seen of it) can be included into a
	single database lookup mechanism.

	= 2017-05-01, David A­D­ Morano
	I just noticed that I removed the UDOMAIN function and
	forgot to make a note here of it.  Well I removed it a few
	years ago now but have forgotten exactly when it was removed.
	I removed it because the old UDOMAIN data-base was no longer
	really needed.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	userattrdb

	Description:
	This object is used to access (lookup keynames) in various
	user-attribute-like databases.  Currently only two databases
	are possibly queried.  These are:

	UDOMAIN
	Solaris® user-attribiute

	The old UDOMAIN database only stored Internet domain names
	for various users.  But the newer Solaris® user-attribute
	has generalized the idea of storing attribute for users
	(usernames).  So the Solaris® user-attribute database can
	store both the Internet Domain name for a user as well as
	any other desired (made-up) user attributes.

	We always query Solaris® first and then fall-over to the
	UDOMAIN database for a query of an InterNet domain (query
	keyname 'dn') that fail with Solaris®.


*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<climits>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<uckvamatch.h>		/* LIBUC */
#include	<ucuserattr.h>		/* LIBUC |uc_userattr{x})(3uc)| */
#include	<matstr.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<userattr.h>		/* <- money shot */
#include	<localmisc.h>		/* LIBU */

#include	"userattrdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* lenstr(3u)| */

/* local defines */

#ifndef	UA_DN
#define	UA_DN		"dn"		/* INET domain-name */
#endif

#ifndef	CF_UDOMAIN
#define	CF_UDOMAIN	0
#endif


/* imported namespaces */


/* local typedefs */

typedef userattrdb	uad ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static inline int userattrdb_ctor(userattrdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_OK ;
	    op->magval = 0 ;
	    op->init = {} ;
	    op->have = {} ;
	    op->username = nullptr ;
	    op->domain = nullptr ;
	    op->uap = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (userattrdb_ctor) */

static inline int userattrdb_dtor(userattrdb *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (userattrdb_dtor) */

template<typename ... Args>
static int userattrdb_magic(userattrdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == USERATTRDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (userattrdb_magic) */

static int	userattrdb_opensysdb(uad *) noex ;
static int	userattrdb_sysdb(uad *,char *,int,const char *) noex ;

static int	userattrdb_openudomain(uad *) noex ;


/* local variables */

static cpcchar	specials[] = {
	UA_DN,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int userattrdb_open(uad *op,cchar *username) noex {
	int		rs ;
	if ((rs = userattrdb_ctor(op,username)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (username[0]) ylikely {
		rs = SR_OK ;
	        op->username = username ;
	        op->magval = USERATTRDB_MAGIC ;
	    } /* end if (valid) */
	    if (rs < 0) {
		userattrdb_dtor(op) ;
	    }
	} /* end if (userattrdb_ctor) */
	return rs ;
} /* end subroutine (userattrdb_open) */

int userattrdb_close(uad *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = userattrdb_magic(op)) >= 0) ylikely {
	    if (op->domain) ylikely {
		void *vp = voidp(op->domain) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->domain = nullptr ;
	    }
	    if (op->uap) ylikely {
	        rs1 = uc_userattrfree(op->uap) ;
	        if (rs >= 0) rs = rs1 ;
	        op->uap = nullptr ;
	    }
	    {
		rs1 = userattrdb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (userattrdb_close) */

int userattrdb_lookup(uad *op,char *rbuf,int rlen,cchar *keyname) noex {
	int		rs ;
	if ((rs = userattrdb_magic(op,keyname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (keyname[0]) ylikely {
	        rbuf[0] = '\0' ;
	        rs = userattrdb_sysdb(op,rbuf,rlen,keyname) ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (userattrdb_lookup) */

int userattrdb_count(uad *op) noex {
    	int		rs ;
	if ((rs = userattrdb_magic(op)) >= 0) ylikely {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (userattrdb_count) */


/* private subroutines */

static int userattrdb_opensysdb(uad *op) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (! op->init.sysdb) {
	    op->init.sysdb = true ;
	    rs = uc_userattrnam(&op->uap,op->username) ;
	    op->have.sysdb = (rs >= 0) ;
	    f = op->have.sysdb ;
	}
	return (rs >= 0) ? f : rs ;
} /* end subroutine (userattrdb_opensysdb) */

[[maybe_unused]] static int userattrdb_openudomain(uad *op) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (! op->init.udomain) {
	    op->init.udomain = true ;
	    op->have.udomain = true ;	/* assumption, might change */
	    f = op->have.udomain ;
	}
	return (rs >= 0) ? f : rs ;
} /* end subroutine (userattrdb_openudomain) */

static int userattrdb_sysdb(uad *op,char *rbuf,int rlen,cc *kn) noex {
	int		rs = SR_OK ;
	if (! op->init.sysdb) {
	    rs = userattrdb_opensysdb(op) ;
	}
	if (rs >= 0) ylikely {
	    if (op->have.sysdb) {
	        cchar	*cp{} ;
		if ((rs = uc_kvamatch(op->uap->attr,kn,&cp)) >= 0) {
		    if (rbuf) {
		        rs = sncpy1(rbuf,rlen,cp) ;
		    } else {
		        rs = lenstr(cp) ;
		    }
	        } /* end if (successful lookup) */
	    } else {
	        rs = SR_NOTFOUND ;
	    } /* end if (searching the system DB) */
	} /* end if (ok) */
	return rs ;
} /* end subroutine (userattrdb_sysdb) */

int userattrdb::open(cchar *un) noex {
	return userattrdb_open(this,un) ;
} /* end meþhod */

int userattrdb::lookup(char *rbuf,int rlen,cchar *k) noex {
	return userattrdb_lookup(this,rbuf,rlen,k) ;
} /* end method */

void userattrdb::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("userattrdb",rs,"fini-finish") ;
	}
} /* end method */

userattrdb_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case userattrdbmem_count:
	        rs = userattrdb_count(op) ;
	        break ;
	    case userattrdbmem_close:
	        rs = userattrdb_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (userattrdb_co::operator) */


