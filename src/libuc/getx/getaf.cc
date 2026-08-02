/* getaf SUPPORT (Get-Address-Family) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get an address-family by name */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_GETAFLEN	1		/* |getaflen()| */

/* revision history:

	= 1998-03-21, David A­D­ Morano
	This was written to get a roughly standardized subroutine
	to handle both IPv4 and IPv6.  Note that the order of the
	AF list is not in the order to the definitions of the defines.
	Rather, since searching is linear (probably the fastest
	way), the order is such that the most popular AFs are near
	the top!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getaf

	Description:
	These subroutines allow for lookup of either an address-family
	name or an address-family number into a database of name-number
	pairs.  The database is not dynamic, but rather compiled
	into this module.  A dynamic database is really only possible
	if we know where the stupid real '<sys/socket.h>' file is
	located (since we would need to read and process that file
	to get the name-number pairs).  This problem really illustrates
	the stupidity of the whole BSD "socket" API and only goes
	to show the far superiority of the AT&T TLI (or XTI) API.

	Synopsis:
	int getaf(cchar *np,int nl) neox

	Arguments:
	np		address family string pointer
	nl		address family string length

	Returns:
	>=0		found address-family
	<0		error (system-return)

	Name:
	getaflen

	Description:
        This subroutine divines the length of a socket address from an
        address-family.

	Synopsis:
	int getaflen(int af) noex

	Arguments:
	af		address-family

	Return:
	>=0		length of address in address-family
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* CSTD |sort(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |libu::cfdec(3u)| */
#include	<uinet.h>		/* LIBU */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<strdcpy.h>		/* LIBUC */
#include	<strwcmp.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC |hasalldig(3uc)| */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU debugging */

#include	"getaf.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |cstrcmp(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using std::sort ;			/* subroutine-template */
using std::partition_point ;		/* subroutine-template */
using libu::cfdec ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct addrfam {
	cchar		*name ;
	int		af ;
} ; /* end struct (addrfam) */

namespace {
    struct getter {
	con addrfam	*ns ;
	con uchar	*tab ;
	int 		ne ;
	getter(con addrfam *ªns,con uchar *ªtab,int n) noex : ns(ªns) {
	    tab = ªtab ;
	    ne = n ;
	} ; /* end ctor */
	int operator () (cchar *sp,int sl) const noex {
            int     rs = SR_AFNOSUPPORT ;
            int     af = 0 ; /* return-value */
            cauto predf = [this,sp,sl] (uchar c) noex -> bool {
                cchar *an = ns[c].name ;
		bool f = false ;
		{
		    cint ch_a = int(an[0] & UCHAR_MAX) ;
		    cint ch_s = int(sp[0] & UCHAR_MAX) ;
		    {
			cint d = (ch_a - ch_s) ;
		        f = f || (d < 0) ;
                        f = f || ((d == 0) && (strwcmp(an,sp,sl) < 0)) ;
		    }
		}
                return f ;
            } ; /* end lambda (predf) */
            con uchar *itf = (tab + 0) ;
            con uchar *itl = (tab + ne) ;
            if (cauto it = partition_point(itf,itl,predf) ; it != itl) {
                cauto mat = [this,sp,sl] (int c) noex -> bool {
                    cchar *an = ns[c].name ;
                    return (strwcmp(an,sp,sl) == 0) ;
                } ; /* end lambda (mat) */
                if (cint ii = *it ; mat(ii)) {
                    if ((af = ns[ii].af) >= 0) rs = SR_OK ;
                } /* end if (got a match) */
            } /* end if */
            return (rs >= 0) ? af : rs ;
	} ; /* end method (operator) */
    } ; /* end struct (getter) */
} /* end namespace */


/* forward references */

local int	getdb(cchar *,int) noex ;


/* local variables */

constexpr addrfam		names_af[] = {
	{ "unspec",		AF_UNSPEC },
	{ "unix",		AF_UNIX },
	{ "inet",		AF_INET },
	{ "inet4",		AF_INET4 },
	{ "inet6",		AF_INET6 },
	{ "implink",		AF_IMPLINK },
	{ "pup",		AF_PUP },
	{ "chaos",		AF_CHAOS },
	{ "ns",			AF_NS },
	{ "nbs",		AF_NBS },
	{ "ecma",		AF_ECMA },
	{ "datakit",		AF_DATAKIT },
	{ "ccitt",		AF_CCITT },
	{ "sna",		AF_SNA },
	{ "decnet",		AF_DECnet },
	{ "dli",		AF_DLI },
	{ "lat",		AF_LAT },
	{ "hylink",		AF_HYLINK },
	{ "appletalk",		AF_APPLETALK },
	{ "nit",		AF_NIT },
	{ "802",		AF_802 },
	{ "osi",		AF_OSI },
	{ "x25",		AF_X25 },
	{ "osinet",		AF_OSINET },
	{ "gosip",		AF_GOSIP },
	{ "ipx",		AF_IPX },
	{ "route",		AF_ROUTE },
	{ "link",		AF_LINK },
	{ "key",		AF_KEY },
	{ "nca",		AF_NCA },
	{ "local",		AF_LOCAL },
	{ "isdn",		AF_ISDN },
	{ "system",		AF_SYSTEM },
	{ "netbios",		AF_NETBIOS },
	{ "ndrv",		AF_NDRV },
	{ "cnt",		AF_CNT },
	{ "coip",		AF_COIP },
	{ "e164",		AF_E164 },
	{ "ieee80211",		AF_IEEE80211 },
	{ "ieee802154",		AF_IEEE802154 },
	{ "iso",		AF_ISO },
	{ "max",		AF_MAX },
	{ "natm",		AF_NATM },
	{ "ppp",		AF_PPP },
	{ "reserved_36", 	AF_RESERVED_36 },
	{ "sip",		AF_SIP },
	{ "utun",		AF_UTUN },
	{ "vsock",		AF_VSOCK },
	{ "alg",		AF_ALG },
	{ "arp",		AF_ARP },
	{ "ash",		AF_ASH },
	{ "atm",		AF_ATM },
	{ "atmpvc",		AF_ATMPVC },
	{ "atmsvc",		AF_ATMSVC },
	{ "ax25",		AF_AX25 },
	{ "bluetooth",		AF_BLUETOOTH },
	{ "bridge",		AF_BRIDGE },
	{ "caif",		AF_CAIF },
	{ "can",		AF_CAN },
	{ "dnssd",		AF_DNSSD },
	{ "dontÿero",		AF_DONTZERO },
	{ "econet",		AF_ECONET },
	{ "file",		AF_FILE },
	{ "force_nosleep",	AF_FORCE_NOSLEEP },
	{ "ib",			AF_IB },
	{ "inet6_sdp",		AF_INET6_SDP },
	{ "inet_offload",	AF_INET_OFFLOAD },
	{ "inet_sdp",		AF_INET_SDP },
	{ "irda",		AF_IRDA },
	{ "iucv",		AF_IUCV },
	{ "kcm",		AF_KCM },
	{ "llc",		AF_LLC },
	{ "max_gain",		AF_MAX_GAIN },
	{ "mctp",		AF_MCTP },
	{ "min_gain",		AF_MIN_GAIN },
	{ "mpls",		AF_MPLS },
	{ "netbeui",		AF_NETBEUI },
	{ "netgraph",		AF_NETGRAPH },
	{ "netlink",		AF_NETLINK },
	{ "netrom",		AF_NETROM },
	{ "nfc",		AF_NFC },
	{ "no_abts",		AF_NO_ABTS },
	{ "packet",		AF_PACKET },
	{ "phonet",		AF_PHONET },
	{ "pltenter",		AF_PLTENTER },
	{ "pltexit",		AF_PLTEXIT },
	{ "policy",		AF_POLICY },
	{ "pppox",		AF_PPPOX },
	{ "qipcrtr",		AF_QIPCRTR },
	{ "rds",		AF_RDS },
	{ "registered",		AF_REGISTERED },
	{ "rose",		AF_ROSE },
	{ "rxrpc",		AF_RXRPC },
	{ "scluster",		AF_SCLUSTER },
	{ "security",		AF_SECURITY },
	{ "slow",		AF_SLOW },
	{ "smc",		AF_SMC },
	{ "sun_hwcapverify",	AF_SUN_HWCAPVERIFY },
	{ "sun_noplm",		AF_SUN_NOPLM },
	{ "sun_setugid",	AF_SUN_SETUGID },
	{ "sys_control",	AF_SYS_CONTROL },
	{ "tipc",		AF_TIPC },
	{ "trill",		AF_TRILL },
	{ "wanpipe",		AF_WANPIPE },
	{ "xdp",		AF_XDP }
} ; /* end array (names_af) */

constexpr int		ne_af	= nelem(names_af) ;

namespace {
    struct namemgr {
	uchar		tab_af	[ne_af] ;
	consteval void tabload_x() noex {
	    for (int i = 0 ; i < ne_af ; i += 1) {
		tab_af[i] = uchar(i) ;
	    } /* end for */
	} ; /* end method (tabload_x) */
	consteval void tabinit() noex {
	    cauto predf = [] (con uchar &ia,con uchar &ib) noex -> bool {
		cchar *s1 = names_af[ia].name ;
		cchar *s2 = names_af[ib].name ;
    		return (cstrcmp(s1,s2) < 0) ;
	    } ; /* end lambda */
	    tabload_x() ;
	    sort(tab_af,(tab_af+ne_af),predf) ;
	} ; /* end method (tabinit) */
	consteval namemgr() noex {
	    tabinit() ;
	} ; /* end ctor (namemgr) */
    } ; /* end struct (namemgr) */
} /* end namespace */

constexpr namemgr	nametab ;

static bufsizevar	maxpathlen(bufsize_mp) ;

cbool			f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int getaf(cchar *sp,int ªsl) noex {
	int		rs = SR_FAULT ;
	if (int sl = getlenstr(sp,ªsl) ; sl >= 0) ylikely {
	    rs = SR_INVALID ;
	    if ((sl > 0) && sp[0]) ylikely {
	        if (hasalldig(sp,sl)) {
	            if (int v ; (rs = cfdec(sp,sl,&v)) >= 0) {
		        rs = v ;
	            }
	        } else {
		    rs = getdb(sp,sl) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getaf) */

#if	CF_GETAFLEN
int getaflen(int af) noex {
	int		rs = SR_AFNOSUPPORT ;
	switch (af) {
	case AF_UNIX:
	    rs = maxpathlen ;
	    break ;
	case AF_INET4:
	    rs = INET4ADDRLEN ;
	    break ;
	case AF_INET6:
	    rs = INET6ADDRLEN ;
	    break ;
	} /* end switch */
	return rs ;
} /* end subroutine (getaflen) */
#endif /* CF_GETAFLEN */

/* reads out as: str-af-name */
cchar *strafname(int af) noex {
	const addrfam	*afs = names_af ;
	int		i{} ;
	for (i = 0 ; afs[i].name ; i += 1) {
	    if (afs[i].af == af) break ;
	} /* end for */
	return (afs[i].name) ? afs[i].name : "unknown" ;
} /* end subroutine (strafname) */


/* local subroutines */

local int getdb(cchar *sp,int sl) noex {
	getter go(names_af,nametab.tab_af,ne_af) ;
	return go(sp,sl) ;
} /* end subroutine (getdb) */


