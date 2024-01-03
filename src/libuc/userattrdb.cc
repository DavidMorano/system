/* userattrdb */
/* lang=C++20 */

/* user-attribute database management */
/* version %I% last-modified %G% */

#define	CF_UDOMAIN	0		/* call |udomain(3dam)| */

/* revision history:

	= 2001-06-05, David A­D­ Morano
	I wrote this so that both the old UDOMAIN facility and the
	newer user-attribute database capability of Sun Solaris®
	2.6 (first that I have seen of it) can be included into a
	single database lookup mechanism.

	= 2017-05-01, David A­D­ Morano
	I just noticed that I too out the UDOMAIN function and
	forgot to make a note here of it. Well I removed it a few
	years ago now but have forgotten exactly where it was
	removed. I removed it because the old UDOMAIN data-base was
	no longer really needed.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<udomain.h>

#include	"userattr.h"		/* <- money shot */
#include	"userattrdb.h"


/* local defines */

#ifndef	UA_DN
#define	UA_DN		"dn"		/* INET domain-name */
#endif

#ifndef	CF_UDOMAIN
#define	CF_UDOMAIN	0
#endif


/* local namespaces */


/* local typedefs */

typedef userattrdb	uad ;


/* external subroutines */

extern "C" {
    extern int	sncpy1(char *,int,cchar *) noex ;
    extern int	matstr(cchar **,cchar *,int) noex ;
}


/* local structures */


/* forward references */

template<typename ... Args>
static inline int userattrdb_ctor(userattrdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->magic = 0 ;
	    op->init = {} ;
	    op->have = {} ;
	    op->username = nullptr ;
	    op->domain = nullptr ;
	    op->uap = nullptr ;
	}
	return rs ;
}

template<typename ... Args>
static inline int userattrdb_magic(userattrdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == USERATTRDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	userattrdb_opensysdb(uad *) noex ;
static int	userattrdb_sysdb(uad *,char *,int,const char *) noex ;

static int	userattrdb_openudomain(uad *) noex ;
static int	userattrdb_ud(uad *,char *,int) noex ;


/* local variables */

static cchar	*specials[] = {
	UA_DN,
	nullptr
} ;

constexpr bool	f_udomain = CF_UDOMAIN ;


/* exported subroutines */

int userattrdb_open(uad *op,cchar *username) noex {
	int		rs ;
	if ((rs = userattrdb_ctor(op,username)) >= 0) {
	    rs = SR_INVALID ;
	    if (username[0]) {
		rs = SR_OK ;
	        op->username = username ;
	        op->magic = USERATTRDB_MAGIC ;
	    } /* end if (valid) */
	} /* end if (userattrdb_ctor) */
	return rs ;
}
/* end subroutine (userattrdb_open) */

int userattrdb_close(uad *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = userattrdb_magic(op)) >= 0) {
	        if (op->domain) {
	            rs1 = uc_free(op->domain) ;
	            if (rs >= 0) rs = rs1 ;
	            op->domain = nullptr ;
	        }
	        if (op->uap) {
	            rs1 = uc_userattrfree(op->uap) ;
	            if (rs >= 0) rs = rs1 ;
	            op->uap = nullptr ;
	        }
	        op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (userattrdb_close) */

int userattrdb_lookup(uad *op,char *rbuf,int rlen,cchar *keyname) noex {
	int		rs ;
	if ((rs = userattrdb_magic(op,keyname)) >= 0) {
		rs = SR_INVALID ;
	        if (keyname[0]) {
		    [[maybe_unused]] cint	rsn = SR_NOTOPEN ;
	            rbuf[0] = '\0' ;
	            rs = userattrdb_sysdb(op,rbuf,rlen,keyname) ;
		    if constexpr (f_udomain) {
	                if ((rs == rsn) && (matstr(specials,keyname,-1) >= 0)) {
	                    rs = userattrdb_ud(op,rbuf,rlen) ;
	                }
		    } /* end if-constexpr (f_udomain) */
		} /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (userattrdb_lookup) */


/* private subroutines */

static int userattrdb_opensysdb(uad *op) noex {
	int		rs = SR_OK ;
	if (! op->init.sysdb) {
	    op->init.sysdb = true ;
	    rs = uc_userattrnam(&op->uap,op->username) ;
	    op->have.sysdb = (rs >= 0) ;
	}
	return (rs >= 0) ? op->have.sysdb : rs ;
}
/* end subroutine (userattrdb_opensysdb) */

[[maybe_unused]] static int userattrdb_openudomain(uad *op) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (! op->init.udomain) {
	    op->init.udomain = true ;
	    op->have.udomain = true ;	/* assumption, might change */
	    f = op->have.udomain ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (userattrdb_openudomain) */

static int userattrdb_sysdb(uad *op,char *rbuf,int rlen,cc *kn) noex {
	int		rs = SR_OK ;
	if (! op->init.sysdb) {
	    rs = userattrdb_opensysdb(op) ;
	}
	if (rs >= 0) {
	    if (op->have.sysdb) {
	        cchar	*cp{} ;
	        if ((rs = uc_kvamatch(op->uap->attr,kn,&cp)) >= 0) {
		    if (rbuf) {
		        rs = sncpy1(rbuf,rlen,cp) ;
		    } else {
		        rs = strlen(cp) ;
		    }
	        } /* end if (successful lookup) */
	    } else {
	        rs = SR_NOTFOUND ;
	    } /* end if (searching the system DB) */
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (userattrdb_sysdb) */

[[maybe_unused]] static int userattrdb_ud(uad *op,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (! op->init.udomain) {
	    rs = userattrdb_openudomain(op) ;
	}
	if (rs >= 0) {
	    if (op->have.sysdb) {
	        if (op->domain) {
		    cchar	*pr = nullptr ;
		    cchar	*un = op->username ;
	            char	*dbuf ;
		    if ((rs = malloc_hn(&dbuf)) >= 0) {
		        cint	dlen = rs ;
	                if ((rs = udomain(pr,dbuf,dlen,un)) >= 0) {
			    cchar	*cp ;
		            if ((rs = uc_mallocstrw(dbuf,rs,&cp)) >= 0) {
			        op->domain = cp ;
			    }
		        } /* end if (udomain) */
			rs1 = uc_free(dbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (domain) */
	        if ((rs >= 0) && op->domain) {
		    if (rbuf) {
		        rs = sncpy1(rbuf,rlen,op->domain) ;
		    } else {
		        rs = strlen(op->domain) ;
		    }
	        } /* end if (successful lookup) */
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (userattrdb_ud) */


