/* usysdefs_af HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* these are (a multitude) of various UNIX® system pre-processor defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-09, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This file defines various preprocessor variables for use
	in setting up the UNIX®-system default for various buffer
	sizes.  The real run-time system buffer sizes are (indeed)
	determined at runtime where they may exist.

	Usage:
	Among the most important uses for the data contained herein
	is the BUFSIZE management facility.  It uses the data below
	to set default buffer sizes for those systems that do not
	have dynamicly managed buffer size support in the system
	kernel.

*******************************************************************************/

#ifndef	USYSDEFSAF_INCLUDE
#define	USYSDEFSAF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */


enum missingafs {
	missingaf_START = 150,
	missingaf_implink,
	missingaf_pup,
	missingaf_chaos,
	missingaf_ns,
	missingaf_nbs,
	missingaf_ecma,
	missingaf_datakit,
	missingaf_ccitt,
	missingaf_sna,
	missingaf_decnet,
	missingaf_dli,
	missingaf_lat,
	missingaf_hylink,
	missingaf_appletalk,
	missingaf_nit,
	missingaf_802,
	missingaf_osi,
	missingaf_x25,
	missingaf_osinet,
	missingaf_gosip,
	missingaf_ipx,
	missingaf_route,
	missingaf_link,
	missingaf_key,
	missingaf_nca,
	missingaf_isdn,
	missingaf_system,
	missingaf_netbios,
	missingaf_ndrv,
	missingaf_cnt,
	missingaf_coip,
	missingaf_e164,
	missingaf_ieee80211,
	missingaf_ieee802154,
	missingaf_iso,
	missingaf_max,
	missingaf_natm,
	missingaf_ppp,
	missingaf_reserved_36,
	missingaf_sip,
	missingaf_utun,
	missingaf_vsock,
	missingaf_alg,
	missingaf_arp,
	missingaf_ash,
	missingaf_atm,
	missingaf_atmpvc,
	missingaf_atmsvc,
	missingaf_ax25,
	missingaf_bluetooth,
	missingaf_bridge,
	missingaf_caif,
	missingaf_can,
	missingaf_dnssd,
	missingaf_dontÿero,
	missingaf_econet,
	missingaf_file,
	missingaf_force_nosleep,
	missingaf_ib,
	missingaf_inet6_sdp,
	missingaf_inet_offload,
	missingaf_inet_sdp,
	missingaf_irda,
	missingaf_iucv,
	missingaf_kcm,
	missingaf_llc,
	missingaf_max_gain,
	missingaf_mctp,
	missingaf_min_gain,
	missingaf_mpls,
	missingaf_netbeui,
	missingaf_netgraph,
	missingaf_netlink,
	missingaf_netrom,
	missingaf_nfc,
	missingaf_no_abts,
	missingaf_packet,
	missingaf_phonet,
	missingaf_pltenter,
	missingaf_pltexit,
	missingaf_policy,
	missingaf_pppox,
	missingaf_qipcrtr,
	missingaf_rds,
	missingaf_registered,
	missingaf_rose,
	missingaf_rxrpc,
	missingaf_scluster,
	missingaf_security,
	missingaf_slow,
	missingaf_smc,
	missingaf_sun_hwcapverify,
	missingaf_sun_noplm,
	missingaf_sun_setugid,
	missingaf_sys_control,
	missingaf_tipc,
	missingaf_trill,
	missingaf_wanpipe,
	missingaf_xdp,
	missingaf_overlast
} ; /* end enum (missingafs) */


/* aliases */
#if	! defined(AF_UNSPEC)
#define	AF_UNSPEC	0
#endif /* AF_UNSPEC */
#if	! defined(AF_LOCAL)
#ifdef	AF_UNIX
#define	AF_LOCAL	AF_UNIX
#else
#define	AF_LOCAL	1
#endif
#endif /* AF_LOCAL */
#if	! defined(AF_INET4)
#ifdef	AF_INET
#define	AF_INET4	AF_INET
#else
#define	AF_INET4	2
#endif
#endif /* AF_INET4 */
/* missing */
#if	! defined(AF_IMPLINK)
#define	AF_IMPLINK	(missingaf_implink)
#endif
#if	! defined(AF_PUP)
#define	AF_PUP		(missingaf_pup)
#endif
#if	! defined(AF_CHAOS)
#define	AF_CHAOS	(missingaf_chaos)
#endif
#if	! defined(AF_NS)
#define	AF_NS		(missingaf_ns)
#endif
#if	! defined(AF_NBS)
#define	AF_NBS		(missingaf_nbs)
#endif
#if	! defined(AF_ECMA)
#define	AF_ECMA		(missingaf_ecma)
#endif
#if	! defined(AF_DATAKIT)
#define	AF_DATAKIT	(missingaf_datakit)
#endif
#if	! defined(AF_CCITT)
#define	AF_CCITT	(missingaf_ccitt)
#endif
#if	! defined(AF_SNA)
#define	AF_SNA		(missingaf_sna)
#endif
#if	! defined(AF_DECnet)
#define	AF_DECnet	(missingaf_decnet)
#endif
#if	! defined(AF_DLI)
#define	AF_DLI		(missingaf_dli)
#endif
#if	! defined(AF_LAT)
#define	AF_LAT		(missingaf_lat)
#endif
#if	! defined(AF_HYLINK)
#define	AF_HYLINK	(missingaf_hylink)
#endif
#if	! defined(AF_APPLETALK)
#define	AF_APPLETALK	(missingaf_appletalk)
#endif
#if	! defined(AF_NIT)
#define	AF_NIT		(missingaf_nit)
#endif
#if	! defined(AF_802)
#define	AF_802		(missingaf_802)
#endif
#if	! defined(AF_OSI)
#define	AF_OSI		(missingaf_osi)
#endif
#if	! defined(AF_X25)
#define	AF_X25		(missingaf_x25)
#endif
#if	! defined(AF_OSINET)
#define	AF_OSINET	(missingaf_osinet)
#endif
#if	! defined(AF_GOSIP)
#define	AF_GOSIP	(missingaf_gosip)
#endif
#if	! defined(AF_IPX)
#define	AF_IPX		(missingaf_ipx)
#endif
#if	! defined(AF_ROUTE)
#define	AF_ROUTE	(missingaf_route)
#endif
#if	! defined(AF_LINK)
#define	AF_LINK		(missingaf_link)
#endif
#if	! defined(AF_KEY)
#define	AF_KEY		(missingaf_key)
#endif
#if	! defined(AF_NCA)
#define	AF_NCA		(missingaf_nca)
#endif
#if	! defined(AF_ISDN)
#define	AF_ISDN		(missingaf_isdn)
#endif
#if	! defined(AF_SYSTEM)
#define	AF_SYSTEM	(missingaf_system)
#endif
#if	! defined(AF_NETBIOS)
#define	AF_NETBIOS	(missingaf_netbios)
#endif
#if	! defined(AF_NDRV)
#define	AF_NDRV		(missingaf_ndrv)
#endif
#if	! defined(AF_CNT)
#define AF_CNT		(missingaf_cnt)
#endif
#if	! defined(AF_COIP)
#define AF_COIP		(missingaf_coip)
#endif
#if	! defined(AF_E164)
#define AF_E164		(missingaf_e164)
#endif
#if	! defined(AF_IEEE80211)
#define AF_IEEE80211	(missingaf_ieee80211)
#endif
#if	! defined(AF_IEEE802154)
#define AF_IEEE802154	(missingaf_ieee802154)
#endif
#if	! defined(AF_ISO)
#define AF_ISO		(missingaf_iso)
#endif
#if	! defined(AF_MAX)
#define AF_MAX		(missingaf_max)
#endif
#if	! defined(AF_NATM)
#define AF_NATM		(missingaf_natm)
#endif
#if	! defined(AF_PPP)
#define AF_PPP		(missingaf_ppp)
#endif
#if	! defined(AF_RESERVED_36)
#define AF_RESERVED_36	(missingaf_reserved_36)
#endif
#if	! defined(AF_SIP)
#define AF_SIP		(missingaf_sip)
#endif
#if	! defined(AF_UTUN)
#define AF_UTUN		(missingaf_utun)
#endif
#if	! defined(AF_VSOCK)
#define AF_VSOCK	(missingaf_vsock)
#endif
#ifndef	AF_ALG
#define	AF_ALG		missingaf_alg
#endif
#ifndef	AF_ARP
#define	AF_ARP		missingaf_arp
#endif
#ifndef	AF_ASH
#define	AF_ASH		missingaf_ash
#endif
#ifndef	AF_ATM
#define	AF_ATM		missingaf_atm
#endif
#ifndef	AF_ATMPVC
#define	AF_ATMPVC	missingaf_atmpvc
#endif
#ifndef	AF_ATMSVC
#define	AF_ATMSVC	missingaf_atmsvc
#endif
#ifndef	AF_AX25
#define	AF_AX25		missingaf_ax25
#endif
#ifndef	AF_BLUETOOTH
#define	AF_BLUETOOTH	missingaf_bluetooth
#endif
#ifndef	AF_BRIDGE
#define	AF_BRIDGE	missingaf_bridge
#endif
#ifndef	AF_CAIF
#define	AF_CAIF		missingaf_caif
#endif
#ifndef	AF_CAN
#define	AF_CAN		missingaf_can
#endif
#ifndef	AF_DNSSD
#define	AF_DNSSD	missingaf_dnssd
#endif
#ifndef	AF_DONTZERO
#define	AF_DONTZERO	missingaf_dontÿero
#endif
#ifndef	AF_ECONET
#define	AF_ECONET	missingaf_econet
#endif
#ifndef	AF_FILE
#define	AF_FILE		missingaf_file
#endif
#ifndef	AF_FORCE_NOSLEEP
#define	AF_FORCE_NOSLEEP	missingaf_force_nosleep
#endif
#ifndef	AF_IB
#define	AF_IB		missingaf_ib
#endif
#ifndef	AF_INET6_SDP
#define	AF_INET6_SDP	missingaf_inet6_sdp
#endif
#ifndef	AF_INET_OFFLOAD
#define	AF_INET_OFFLOAD	missingaf_inet_offload
#endif
#ifndef	AF_INET_SDP
#define	AF_INET_SDP	missingaf_inet_sdp
#endif
#ifndef	AF_IRDA
#define	AF_IRDA		missingaf_irda
#endif
#ifndef	AF_IUCV
#define	AF_IUCV		missingaf_iucv
#endif
#ifndef	AF_KCM
#define	AF_KCM		missingaf_kcm
#endif
#ifndef	AF_LLC
#define	AF_LLC		missingaf_llc
#endif
#ifndef	AF_MAX_GAIN
#define	AF_MAX_GAIN	missingaf_max_gain
#endif
#ifndef	AF_MCTP
#define	AF_MCTP		missingaf_mctp
#endif
#ifndef	AF_MIN_GAIN
#define	AF_MIN_GAIN	missingaf_min_gain
#endif
#ifndef	AF_MPLS
#define	AF_MPLS		missingaf_mpls
#endif
#ifndef	AF_NETBEUI
#define	AF_NETBEUI	missingaf_netbeui
#endif
#ifndef	AF_NETGRAPH
#define	AF_NETGRAPH	missingaf_netgraph
#endif
#ifndef	AF_NETLINK
#define	AF_NETLINK	missingaf_netlink
#endif
#ifndef	AF_NETROM
#define	AF_NETROM	missingaf_netrom
#endif
#ifndef	AF_NFC
#define	AF_NFC		missingaf_nfc
#endif
#ifndef	AF_NO_ABTS
#define	AF_NO_ABTS	missingaf_no_abts
#endif
#ifndef	AF_PACKET
#define	AF_PACKET	missingaf_packet
#endif
#ifndef	AF_PHONET
#define	AF_PHONET	missingaf_phonet
#endif
#ifndef	AF_PLTENTER
#define	AF_PLTENTER	missingaf_pltenter
#endif
#ifndef	AF_PLTEXIT
#define	AF_PLTEXIT	missingaf_pltexit
#endif
#ifndef	AF_POLICY
#define	AF_POLICY	missingaf_policy
#endif
#ifndef	AF_PPPOX
#define	AF_PPPOX	missingaf_pppox
#endif
#ifndef	AF_QIPCRTR
#define	AF_QIPCRTR	missingaf_qipcrtr
#endif
#ifndef	AF_RDS
#define	AF_RDS		missingaf_rds
#endif
#ifndef	AF_REGISTERED
#define	AF_REGISTERED	missingaf_registered
#endif
#ifndef	AF_ROSE
#define	AF_ROSE		missingaf_rose
#endif
#ifndef	AF_RXRPC
#define	AF_RXRPC	missingaf_rxrpc
#endif
#ifndef	AF_SCLUSTER
#define	AF_SCLUSTER	missingaf_scluster
#endif
#ifndef	AF_SECURITY
#define	AF_SECURITY	missingaf_security
#endif
#ifndef	AF_SLOW
#define	AF_SLOW		missingaf_slow
#endif
#ifndef	AF_SMC
#define	AF_SMC		missingaf_smc
#endif
#ifndef	AF_SUN_HWCAPVERIFY
#define	AF_SUN_HWCAPVERIFY	missingaf_sun_hwcapverify
#endif
#ifndef	AF_SUN_NOPLM
#define	AF_SUN_NOPLM	missingaf_sun_noplm
#endif
#ifndef	AF_SUN_SETUGID
#define	AF_SUN_SETUGID	missingaf_sun_setugid
#endif
#ifndef	AF_SYS_CONTROL
#define	AF_SYS_CONTROL	missingaf_sys_control
#endif
#ifndef	AF_TIPC
#define	AF_TIPC		missingaf_tipc
#endif
#ifndef	AF_TRILL
#define	AF_TRILL	missingaf_trill
#endif
#ifndef	AF_WANPIPE
#define	AF_WANPIPE	missingaf_wanpipe
#endif
#ifndef	AF_XDP
#define	AF_XDP		missingaf_xdp
#endif


#endif /* USYSDEFSAF_INCLUDE */


