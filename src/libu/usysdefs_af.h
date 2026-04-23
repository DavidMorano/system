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
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>


enum missingafs {
	missingaf_unspec,
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
	missingaf_iso,
	missingaf_max,
	missingaf_natm,
	missingaf_ppp,
	missingaf_reserved_36,
	missingaf_sip,
	missingaf_utun,
	missingaf_vsock,
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
#define	AF_IMPLINK	(- missingaf_implink)
#endif
#if	! defined(AF_PUP)
#define	AF_PUP		(- missingaf_pup)
#endif
#if	! defined(AF_CHAOS)
#define	AF_CHAOS	(- missingaf_chaos)
#endif
#if	! defined(AF_NS)
#define	AF_NS		(- missingaf_ns)
#endif
#if	! defined(AF_NBS)
#define	AF_NBS		(- missingaf_nbs)
#endif
#if	! defined(AF_ECMA)
#define	AF_ECMA		(- missingaf_ecma)
#endif
#if	! defined(AF_DATAKIT)
#define	AF_DATAKIT	(- missingaf_datakit)
#endif
#if	! defined(AF_CCITT)
#define	AF_CCITT	(- missingaf_ccitt)
#endif
#if	! defined(AF_SNA)
#define	AF_SNA		(- missingaf_sna)
#endif
#if	! defined(AF_DECnet)
#define	AF_DECnet	(- missingaf_decnet)
#endif
#if	! defined(AF_DLI)
#define	AF_DLI		(- missingaf_dli)
#endif
#if	! defined(AF_LAT)
#define	AF_LAT		(- missingaf_lat)
#endif
#if	! defined(AF_HYLINK)
#define	AF_HYLINK	(- missingaf_hylink)
#endif
#if	! defined(AF_APPLETALK)
#define	AF_APPLETALK	(- missingaf_appletalk)
#endif
#if	! defined(AF_NIT)
#define	AF_NIT		(- missingaf_nit)
#endif
#if	! defined(AF_802)
#define	AF_802		(- missingaf_802)
#endif
#if	! defined(AF_OSI)
#define	AF_OSI		(- missingaf_osi)
#endif
#if	! defined(AF_X25)
#define	AF_X25		(- missingaf_x25)
#endif
#if	! defined(AF_OSINET)
#define	AF_OSINET	(- missingaf_osinet)
#endif
#if	! defined(AF_GOSIP)
#define	AF_GOSIP	(- missingaf_gosip)
#endif
#if	! defined(AF_IPX)
#define	AF_IPX		(- missingaf_ipx)
#endif
#if	! defined(AF_ROUTE)
#define	AF_ROUTE	(- missingaf_route)
#endif
#if	! defined(AF_LINK)
#define	AF_LINK		(- missingaf_link)
#endif
#if	! defined(AF_KEY)
#define	AF_KEY		(- missingaf_key)
#endif
#if	! defined(AF_NCA)
#define	AF_NCA		(- missingaf_nca)
#endif
#if	! defined(AF_ISDN)
#define	AF_ISDN		(- missingaf_isdn)
#endif
#if	! defined(AF_SYSTEM)
#define	AF_SYSTEM	(- missingaf_system)
#endif
#if	! defined(AF_NETBIOS)
#define	AF_NETBIOS	(- missingaf_netbios)
#endif
#if	! defined(AF_NDRV)
#define	AF_NDRV		(- missingaf_ndrv)
#endif
#if	! defined(AF_CNT)
#define AF_CNT		(- missingaf_cnt)
#endif
#if	! defined(AF_COIP)
#define AF_COIP		(- missingaf_coip)
#endif
#if	! defined(AF_E164)
#define AF_E164		(- missingaf_e164)
#endif
#if	! defined(AF_IEEE80211)
#define AF_IEEE80211	(- missingaf_ieee80211)
#endif
#if	! defined(AF_ISO)
#define AF_ISO		(- missingaf_iso)
#endif
#if	! defined(AF_MAX)
#define AF_MAX		(- missingaf_max)
#endif
#if	! defined(AF_NATM)
#define AF_NATM		(- missingaf_natm)
#endif
#if	! defined(AF_PPP)
#define AF_PPP		(- missingaf_ppp)
#endif
#if	! defined(AF_RESERVED_36)
#define AF_RESERVED_36	(- missingaf_reserved_36)
#endif
#if	! defined(AF_SIP)
#define AF_SIP		(- missingaf_sip)
#endif
#if	! defined(AF_UTUN)
#define AF_UTUN		(- missingaf_utun)
#endif
#if	! defined(AF_VSOCK)
#define AF_VSOCK	(- missingaf_vsock)
#endif


#endif /* USYSDEFSAF_INCLUDE */


