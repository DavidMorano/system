/* getpf SUPPORT (Get-Protocol-Family) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get a protocol family from an address family */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getpf

	Description:
	This subroutine will find a protocol family (if one exists)
	that corresponds with a specified address family.

	Synopsis:
	int getpf(int af) noex

	Arguments:
	af		address family

	Returns:
	>=0		found protocol family
	<0		no corresponding protocol family exists (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netinet/in.h>		/* POSIX® */
#include	<arpa/inet.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"getpf.h"


/* local defines */


/* local namespaces */

using std::sort ;			/* subroutine-template */
using std::partition_point ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct pairent {
	uchar	af, pf ;
} ; /* end struct (pairent) */


/* forward references */

local int	linsearch(int) noex ;


/* local variables */

constexpr pairent		prototab[] = {
	{ AF_UNSPEC,		PF_UNSPEC },
	{ AF_INET,		PF_INET },
	{ AF_INET4,		PF_INET4 },
	{ AF_INET6,		PF_INET6 },
	{ AF_LOCAL,		PF_LOCAL },
	{ AF_UNIX,		PF_UNIX },
	{ AF_APPLETALK,		PF_APPLETALK },
	{ AF_IMPLINK,		PF_IMPLINK },
	{ AF_PUP,		PF_PUP },
	{ AF_CHAOS,		PF_CHAOS },
	{ AF_NS,		PF_NS },
	{ AF_NBS,		PF_NBS },
	{ AF_ECMA,		PF_ECMA },
	{ AF_DATAKIT,		PF_DATAKIT },
	{ AF_CCITT,		PF_CCITT },
	{ AF_SNA,		PF_SNA },
	{ AF_DECnet,		PF_DECnet },
	{ AF_DLI,		PF_DLI },
	{ AF_LAT,		PF_LAT },
	{ AF_HYLINK,		PF_HYLINK },
	{ AF_APPLETALK,		PF_APPLETALK },
	{ AF_NIT,		PF_NIT },
	{ AF_802,		PF_802 },
	{ AF_OSI,		PF_OSI },
	{ AF_X25,		PF_X25 },
	{ AF_OSINET,		PF_OSINET },
	{ AF_GOSIP,		PF_GOSIP },
	{ AF_IPX,		PF_IPX },
	{ AF_ROUTE,		PF_ROUTE },
	{ AF_LINK,		PF_LINK },
	{ AF_KEY,		PF_KEY },
	{ AF_NCA,		PF_NCA },
	{ AF_LOCAL,		PF_LOCAL },
	{ AF_ISDN,		PF_ISDN },
	{ AF_SYSTEM,		PF_SYSTEM },
	{ AF_NETBIOS,		PF_NETBIOS },
	{ AF_NDRV,		PF_NDRV },
	{ AF_CNT,		PF_CNT },
	{ AF_COIP,		PF_COIP },
	{ AF_E164,		PF_E164 },
	{ AF_IEEE80211,		PF_IEEE80211 },
	{ AF_IEEE802154,	PF_IEEE802154 },
	{ AF_ISO,		PF_ISO },
	{ AF_MAX,		PF_MAX },
	{ AF_NATM,		PF_NATM },
	{ AF_PPP,		PF_PPP },
	{ AF_RESERVED_36,	PF_RESERVED_36 },
	{ AF_SIP,		PF_SIP },
	{ AF_UTUN,		PF_UTUN },
	{ AF_VSOCK,		PF_VSOCK },
	{ AF_ALG,		PF_ALG },
	{ AF_ARP,		PF_ARP },
	{ AF_ASH,		PF_ASH },
	{ AF_ATM,		PF_ATM },
	{ AF_ATMPVC,		PF_ATMPVC },
	{ AF_ATMSVC,		PF_ATMSVC },
	{ AF_AX25,		PF_AX25 },
	{ AF_BLUETOOTH,		PF_BLUETOOTH },
	{ AF_BRIDGE,		PF_BRIDGE },
	{ AF_CAIF,		PF_CAIF },
	{ AF_CAN,		PF_CAN },
	{ AF_DNSSD,		PF_DNSSD },
	{ AF_DONTZERO,		PF_DONTZERO },
	{ AF_ECONET,		PF_ECONET },
	{ AF_FILE,		PF_FILE },
	{ AF_FORCE_NOSLEEP,	PF_FORCE_NOSLEEP },
	{ AF_IB,		PF_IB },
	{ AF_INET6_SDP,		PF_INET6_SDP },
	{ AF_INET_OFFLOAD,	PF_INET_OFFLOAD },
	{ AF_INET_SDP,		PF_INET_SDP },
	{ AF_IRDA,		PF_IRDA },
	{ AF_IUCV,		PF_IUCV },
	{ AF_KCM,		PF_KCM },
	{ AF_LLC,		PF_LLC },
	{ AF_MAX_GAIN,		PF_MAX_GAIN },
	{ AF_MCTP,		PF_MCTP },
	{ AF_MIN_GAIN,		PF_MIN_GAIN },
	{ AF_MPLS,		PF_MPLS },
	{ AF_NETBEUI,		PF_NETBEUI },
	{ AF_NETGRAPH,		PF_NETGRAPH },
	{ AF_NETLINK,		PF_NETLINK },
	{ AF_NETROM,		PF_NETROM },
	{ AF_NFC,		PF_NFC },
	{ AF_NO_ABTS,		PF_NO_ABTS },
	{ AF_PACKET,		PF_PACKET },
	{ AF_PHONET,		PF_PHONET },
	{ AF_PLTENTER,		PF_PLTENTER },
	{ AF_PLTEXIT,		PF_PLTEXIT },
	{ AF_POLICY,		PF_POLICY },
	{ AF_PPPOX,		PF_PPPOX },
	{ AF_QIPCRTR,		PF_QIPCRTR },
	{ AF_RDS,		PF_RDS },
	{ AF_REGISTERED,	PF_REGISTERED },
	{ AF_ROSE,		PF_ROSE },
	{ AF_RXRPC,		PF_RXRPC },
	{ AF_SCLUSTER,		PF_SCLUSTER },
	{ AF_SECURITY,		PF_SECURITY },
	{ AF_SLOW,		PF_SLOW },
	{ AF_SMC,		PF_SMC },
	{ AF_SUN_HWCAPVERIFY,	PF_SUN_HWCAPVERIFY },
	{ AF_SUN_NOPLM,		PF_SUN_NOPLM },
	{ AF_SUN_SETUGID,	PF_SUN_SETUGID },
	{ AF_SYS_CONTROL,	PF_SYS_CONTROL },
	{ AF_TIPC,		PF_TIPC },
	{ AF_TRILL,		PF_TRILL },
	{ AF_WANPIPE,		PF_WANPIPE },
	{ AF_XDP,		PF_XDP }
} ; /* end array (prototab) */

constexpr int		ne = nelem(prototab) ;

namespace {
    struct codemgr {
	uchar		sorted[ne] ;
	consteval void tabload_x() noex {
	    for (int i = 0 ; i < ne ; i += 1) {
		sorted[i] = uchar(i) ;
	    } /* end for */
	} ; /* end method (tabload_x) */
	consteval void tabinit() noex {
	    cauto cmpf = [] (cuchar &ia,cuchar &ib) noex -> bool {
    		return (prototab[ia].af < prototab[ib].af) ;
	    } ; /* end lambda */
	    tabload_x() ;
	    sort(sorted,(sorted+ne),cmpf) ;
	} ; /* end method (tabinit) */
	consteval codemgr() noex {
	    tabinit() ;
	} /* end ctor (codemgr) */
	int operator () (int n) const noex {
	    int		rs = SR_AFNOSUPPORT ;
	    int		pf = 0 ; /* return-value */
	    cauto predf = [n] (uchar c) noex -> bool {
		cint i = int(c) ;
		return (prototab[i].af < n) ;
	    } ; /* end lambda (predf) */
	    cuchar *itf = (sorted + 0) ;
	    cuchar *itl = (sorted + ne) ;
	    if (cauto it = partition_point(itf,itl,predf) ; it != itl) {
		if (cint ii = *it ; prototab[ii].af == n) {
		    rs = SR_OK ;
		    pf = prototab[ii].pf ;
		} /* end if (got a match) */
	    } /* end if (partition) */
	    return (rs >= 0) ? pf : rs ;
	} ; /* end method (operator) */
    } ; /* end struct (codemgr) */
} /* end namespace */

constexpr codemgr	codetab ;


/* exported variables */


/* exported subroutines */

int getpf(int af) noex {
    	int	rs = SR_INVALID ;
	if (af > 6) {
    	    rs = codetab(af) ;
	} else if (af >= 0) {
	    rs = linsearch(af) ;
	}
	return rs ;
} /* end subroutine (getpf) */


/* local subroutines */

local int linsearch(int af) noex {
	int		i = 0 ;
	bool		f = false ;
	while ((!f) && (i < ne)) {
	    f = (prototab[i].af == af) ;
	    if (f) break ;
	    i += 1 ;
	} /* end while */
	return (f) ? prototab[i].pf : SR_AFNOSUPPORT ;
} /* end subroutine (linsearch) */


