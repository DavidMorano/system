/* usysdefs_pf HEADER */
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

#ifndef	USYSDEFSPF_INCLUDE
#define	USYSDEFSPF_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */


enum missingpfs {
	missingpf_START = 150,
	missingpf_implink,
	missingpf_pup,
	missingpf_chaos,
	missingpf_ns,
	missingpf_nbs,
	missingpf_ecma,
	missingpf_datakit,
	missingpf_ccitt,
	missingpf_sna,
	missingpf_decnet,
	missingpf_dli,
	missingpf_lat,
	missingpf_hylink,
	missingpf_appletalk,
	missingpf_nit,
	missingpf_802,
	missingpf_osi,
	missingpf_x25,
	missingpf_osinet,
	missingpf_gosip,
	missingpf_ipx,
	missingpf_route,
	missingpf_link,
	missingpf_key,
	missingpf_nca,
	missingpf_isdn,
	missingpf_system,
	missingpf_netbios,
	missingpf_ndrv,
	missingpf_cnt,
	missingpf_coip,
	missingpf_e164,
	missingpf_ieee80211,
	missingpf_ieee802154,
	missingpf_iso,
	missingpf_max,
	missingpf_natm,
	missingpf_ppp,
	missingpf_reserved_36,
	missingpf_sip,
	missingpf_utun,
	missingpf_vsock,
	missingpf_alg,
	missingpf_arp,
	missingpf_ash,
	missingpf_atm,
	missingpf_atmpvc,
	missingpf_atmsvc,
	missingpf_ax25,
	missingpf_bluetooth,
	missingpf_bridge,
	missingpf_caif,
	missingpf_can,
	missingpf_dnssd,
	missingpf_dontÿero,
	missingpf_econet,
	missingpf_file,
	missingpf_force_nosleep,
	missingpf_ib,
	missingpf_inet6_sdp,
	missingpf_inet_offload,
	missingpf_inet_sdp,
	missingpf_irda,
	missingpf_iucv,
	missingpf_kcm,
	missingpf_llc,
	missingpf_max_gain,
	missingpf_mctp,
	missingpf_min_gain,
	missingpf_mpls,
	missingpf_netbeui,
	missingpf_netgraph,
	missingpf_netlink,
	missingpf_netrom,
	missingpf_nfc,
	missingpf_no_abts,
	missingpf_packet,
	missingpf_phonet,
	missingpf_pltenter,
	missingpf_pltexit,
	missingpf_policy,
	missingpf_pppox,
	missingpf_qipcrtr,
	missingpf_rds,
	missingpf_registered,
	missingpf_rose,
	missingpf_rxrpc,
	missingpf_scluster,
	missingpf_security,
	missingpf_slow,
	missingpf_smc,
	missingpf_sun_hwcapverify,
	missingpf_sun_noplm,
	missingpf_sun_setugid,
	missingpf_sys_control,
	missingpf_tipc,
	missingpf_trill,
	missingpf_wanpipe,
	missingpf_xdp,
	missingpf_overlast
} ; /* end enum (missingpfs) */


/* aliases */
#if	! defined(PF_UNSPEC)
#define	PF_UNSPEC	0
#endif /* PF_UNSPEC */
#if	! defined(PF_LOCAL)
#ifdef	PF_UNIX
#define	PF_LOCAL	PF_UNIX
#else
#define	PF_LOCAL	1
#endif
#endif /* PF_LOCAL */
#if	! defined(PF_INET4)
#ifdef	PF_INET
#define	PF_INET4	PF_INET
#else
#define	PF_INET4	2
#endif
#endif /* PF_INET4 */
/* missing */
#if	! defined(PF_IMPLINK)
#define	PF_IMPLINK	(missingpf_implink)
#endif
#if	! defined(PF_PUP)
#define	PF_PUP		(missingpf_pup)
#endif
#if	! defined(PF_CHAOS)
#define	PF_CHAOS	(missingpf_chaos)
#endif
#if	! defined(PF_NS)
#define	PF_NS		(missingpf_ns)
#endif
#if	! defined(PF_NBS)
#define	PF_NBS		(missingpf_nbs)
#endif
#if	! defined(PF_ECMA)
#define	PF_ECMA		(missingpf_ecma)
#endif
#if	! defined(PF_DATAKIT)
#define	PF_DATAKIT	(missingpf_datakit)
#endif
#if	! defined(PF_CCITT)
#define	PF_CCITT	(missingpf_ccitt)
#endif
#if	! defined(PF_SNA)
#define	PF_SNA		(missingpf_sna)
#endif
#if	! defined(PF_DECnet)
#define	PF_DECnet	(missingpf_decnet)
#endif
#if	! defined(PF_DLI)
#define	PF_DLI		(missingpf_dli)
#endif
#if	! defined(PF_LAT)
#define	PF_LAT		(missingpf_lat)
#endif
#if	! defined(PF_HYLINK)
#define	PF_HYLINK	(missingpf_hylink)
#endif
#if	! defined(PF_APPLETALK)
#define	PF_APPLETALK	(missingpf_appletalk)
#endif
#if	! defined(PF_NIT)
#define	PF_NIT		(missingpf_nit)
#endif
#if	! defined(PF_802)
#define	PF_802		(missingpf_802)
#endif
#if	! defined(PF_OSI)
#define	PF_OSI		(missingpf_osi)
#endif
#if	! defined(PF_X25)
#define	PF_X25		(missingpf_x25)
#endif
#if	! defined(PF_OSINET)
#define	PF_OSINET	(missingpf_osinet)
#endif
#if	! defined(PF_GOSIP)
#define	PF_GOSIP	(missingpf_gosip)
#endif
#if	! defined(PF_IPX)
#define	PF_IPX		(missingpf_ipx)
#endif
#if	! defined(PF_ROUTE)
#define	PF_ROUTE	(missingpf_route)
#endif
#if	! defined(PF_LINK)
#define	PF_LINK		(missingpf_link)
#endif
#if	! defined(PF_KEY)
#define	PF_KEY		(missingpf_key)
#endif
#if	! defined(PF_NCA)
#define	PF_NCA		(missingpf_nca)
#endif
#if	! defined(PF_ISDN)
#define	PF_ISDN		(missingpf_isdn)
#endif
#if	! defined(PF_SYSTEM)
#define	PF_SYSTEM	(missingpf_system)
#endif
#if	! defined(PF_NETBIOS)
#define	PF_NETBIOS	(missingpf_netbios)
#endif
#if	! defined(PF_NDRV)
#define	PF_NDRV		(missingpf_ndrv)
#endif
#if	! defined(PF_CNT)
#define PF_CNT		(missingpf_cnt)
#endif
#if	! defined(PF_COIP)
#define PF_COIP		(missingpf_coip)
#endif
#if	! defined(PF_E164)
#define PF_E164		(missingpf_e164)
#endif
#if	! defined(PF_IEEE80211)
#define PF_IEEE80211	(missingpf_ieee80211)
#endif
#if	! defined(PF_IEEE802154)
#define PF_IEEE802154	missingpf_ieee802154
#endif
#if	! defined(PF_ISO)
#define PF_ISO		(missingpf_iso)
#endif
#if	! defined(PF_MAX)
#define PF_MAX		(missingpf_max)
#endif
#if	! defined(PF_NATM)
#define PF_NATM		(missingpf_natm)
#endif
#if	! defined(PF_PPP)
#define PF_PPP		(missingpf_ppp)
#endif
#if	! defined(PF_RESERVED_36)
#define PF_RESERVED_36	(missingpf_reserved_36)
#endif
#if	! defined(PF_SIP)
#define PF_SIP		(missingpf_sip)
#endif
#if	! defined(PF_UTUN)
#define PF_UTUN		(missingpf_utun)
#endif
#if	! defined(PF_VSOCK)
#define PF_VSOCK	(missingpf_vsock)
#endif
#ifndef	PF_ALG
#define	PF_ALG		missingpf_alg
#endif
#ifndef	PF_ARP
#define	PF_ARP		missingpf_arp
#endif
#ifndef	PF_ASH
#define	PF_ASH		missingpf_ash
#endif
#ifndef	PF_ATM
#define	PF_ATM		missingpf_atm
#endif
#ifndef	PF_ATMPVC
#define	PF_ATMPVC	missingpf_atmpvc
#endif
#ifndef	PF_ATMSVC
#define	PF_ATMSVC	missingpf_atmsvc
#endif
#ifndef	PF_AX25
#define	PF_AX25		missingpf_ax25
#endif
#ifndef	PF_BLUETOOTH
#define	PF_BLUETOOTH	missingpf_bluetooth
#endif
#ifndef	PF_BRIDGE
#define	PF_BRIDGE	missingpf_bridge
#endif
#ifndef	PF_CAIF
#define	PF_CAIF		missingpf_caif
#endif
#ifndef	PF_CAN
#define	PF_CAN		missingpf_can
#endif
#ifndef	PF_DNSSD
#define	PF_DNSSD	missingpf_dnssd
#endif
#ifndef	PF_DONTZERO
#define	PF_DONTZERO	missingpf_dontÿero
#endif
#ifndef	PF_ECONET
#define	PF_ECONET	missingpf_econet
#endif
#ifndef	PF_FILE
#define	PF_FILE		missingpf_file
#endif
#ifndef	PF_FORCE_NOSLEEP
#define	PF_FORCE_NOSLEEP	missingpf_force_nosleep
#endif
#ifndef	PF_IB
#define	PF_IB		missingpf_ib
#endif
#ifndef	PF_INET6_SDP
#define	PF_INET6_SDP	missingpf_inet6_sdp
#endif
#ifndef	PF_INET_OFFLOAD
#define	PF_INET_OFFLOAD	missingpf_inet_offload
#endif
#ifndef	PF_INET_SDP
#define	PF_INET_SDP	missingpf_inet_sdp
#endif
#ifndef	PF_IRDA
#define	PF_IRDA		missingpf_irda
#endif
#ifndef	PF_IUCV
#define	PF_IUCV		missingpf_iucv
#endif
#ifndef	PF_KCM
#define	PF_KCM		missingpf_kcm
#endif
#ifndef	PF_LLC
#define	PF_LLC		missingpf_llc
#endif
#ifndef	PF_MAX_GAIN
#define	PF_MAX_GAIN	missingpf_max_gain
#endif
#ifndef	PF_MCTP
#define	PF_MCTP		missingpf_mctp
#endif
#ifndef	PF_MIN_GAIN
#define	PF_MIN_GAIN	missingpf_min_gain
#endif
#ifndef	PF_MPLS
#define	PF_MPLS		missingpf_mpls
#endif
#ifndef	PF_NETBEUI
#define	PF_NETBEUI	missingpf_netbeui
#endif
#ifndef	PF_NETGRAPH
#define	PF_NETGRAPH	missingpf_netgraph
#endif
#ifndef	PF_NETLINK
#define	PF_NETLINK	missingpf_netlink
#endif
#ifndef	PF_NETROM
#define	PF_NETROM	missingpf_netrom
#endif
#ifndef	PF_NFC
#define	PF_NFC		missingpf_nfc
#endif
#ifndef	PF_NO_ABTS
#define	PF_NO_ABTS	missingpf_no_abts
#endif
#ifndef	PF_PACKET
#define	PF_PACKET	missingpf_packet
#endif
#ifndef	PF_PHONET
#define	PF_PHONET	missingpf_phonet
#endif
#ifndef	PF_PLTENTER
#define	PF_PLTENTER	missingpf_pltenter
#endif
#ifndef	PF_PLTEXIT
#define	PF_PLTEXIT	missingpf_pltexit
#endif
#ifndef	PF_POLICY
#define	PF_POLICY	missingpf_policy
#endif
#ifndef	PF_PPPOX
#define	PF_PPPOX	missingpf_pppox
#endif
#ifndef	PF_QIPCRTR
#define	PF_QIPCRTR	missingpf_qipcrtr
#endif
#ifndef	PF_RDS
#define	PF_RDS		missingpf_rds
#endif
#ifndef	PF_REGISTERED
#define	PF_REGISTERED	missingpf_registered
#endif
#ifndef	PF_ROSE
#define	PF_ROSE		missingpf_rose
#endif
#ifndef	PF_RXRPC
#define	PF_RXRPC	missingpf_rxrpc
#endif
#ifndef	PF_SCLUSTER
#define	PF_SCLUSTER	missingpf_scluster
#endif
#ifndef	PF_SECURITY
#define	PF_SECURITY	missingpf_security
#endif
#ifndef	PF_SLOW
#define	PF_SLOW		missingpf_slow
#endif
#ifndef	PF_SMC
#define	PF_SMC		missingpf_smc
#endif
#ifndef	PF_SUN_HWCAPVERIFY
#define	PF_SUN_HWCAPVERIFY	missingpf_sun_hwcapverify
#endif
#ifndef	PF_SUN_NOPLM
#define	PF_SUN_NOPLM	missingpf_sun_noplm
#endif
#ifndef	PF_SUN_SETUGID
#define	PF_SUN_SETUGID	missingpf_sun_setugid
#endif
#ifndef	PF_SYS_CONTROL
#define	PF_SYS_CONTROL	missingpf_sys_control
#endif
#ifndef	PF_TIPC
#define	PF_TIPC		missingpf_tipc
#endif
#ifndef	PF_TRILL
#define	PF_TRILL	missingpf_trill
#endif
#ifndef	PF_WANPIPE
#define	PF_WANPIPE	missingpf_wanpipe
#endif
#ifndef	PF_XDP
#define	PF_XDP		missingpf_xdp
#endif


#endif /* USYSDEFSPF_INCLUDE */


