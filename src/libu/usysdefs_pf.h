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
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>


enum missingpfs {
	missingpf_unspec,
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
	missingpf_iso,
	missingpf_max,
	missingpf_natm,
	missingpf_ppp,
	missingpf_reserved_36,
	missingpf_sip,
	missingpf_utun,
	missingpf_vsock,
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
#define	PF_IMPLINK	(- missingpf_implink)
#endif
#if	! defined(PF_PUP)
#define	PF_PUP		(- missingpf_pup)
#endif
#if	! defined(PF_CHAOS)
#define	PF_CHAOS	(- missingpf_chaos)
#endif
#if	! defined(PF_NS)
#define	PF_NS		(- missingpf_ns)
#endif
#if	! defined(PF_NBS)
#define	PF_NBS		(- missingpf_nbs)
#endif
#if	! defined(PF_ECMA)
#define	PF_ECMA		(- missingpf_ecma)
#endif
#if	! defined(PF_DATAKIT)
#define	PF_DATAKIT	(- missingpf_datakit)
#endif
#if	! defined(PF_CCITT)
#define	PF_CCITT	(- missingpf_ccitt)
#endif
#if	! defined(PF_SNA)
#define	PF_SNA		(- missingpf_sna)
#endif
#if	! defined(PF_DECnet)
#define	PF_DECnet	(- missingpf_decnet)
#endif
#if	! defined(PF_DLI)
#define	PF_DLI		(- missingpf_dli)
#endif
#if	! defined(PF_LAT)
#define	PF_LAT		(- missingpf_lat)
#endif
#if	! defined(PF_HYLINK)
#define	PF_HYLINK	(- missingpf_hylink)
#endif
#if	! defined(PF_APPLETALK)
#define	PF_APPLETALK	(- missingpf_appletalk)
#endif
#if	! defined(PF_NIT)
#define	PF_NIT		(- missingpf_nit)
#endif
#if	! defined(PF_802)
#define	PF_802		(- missingpf_802)
#endif
#if	! defined(PF_OSI)
#define	PF_OSI		(- missingpf_osi)
#endif
#if	! defined(PF_X25)
#define	PF_X25		(- missingpf_x25)
#endif
#if	! defined(PF_OSINET)
#define	PF_OSINET	(- missingpf_osinet)
#endif
#if	! defined(PF_GOSIP)
#define	PF_GOSIP	(- missingpf_gosip)
#endif
#if	! defined(PF_IPX)
#define	PF_IPX		(- missingpf_ipx)
#endif
#if	! defined(PF_ROUTE)
#define	PF_ROUTE	(- missingpf_route)
#endif
#if	! defined(PF_LINK)
#define	PF_LINK		(- missingpf_link)
#endif
#if	! defined(PF_KEY)
#define	PF_KEY		(- missingpf_key)
#endif
#if	! defined(PF_NCA)
#define	PF_NCA		(- missingpf_nca)
#endif
#if	! defined(PF_ISDN)
#define	PF_ISDN		(- missingpf_isdn)
#endif
#if	! defined(PF_SYSTEM)
#define	PF_SYSTEM	(- missingpf_system)
#endif
#if	! defined(PF_NETBIOS)
#define	PF_NETBIOS	(- missingpf_netbios)
#endif
#if	! defined(PF_NDRV)
#define	PF_NDRV		(- missingpf_ndrv)
#endif
#if	! defined(PF_CNT)
#define PF_CNT		(- missingpf_cnt)
#endif
#if	! defined(PF_COIP)
#define PF_COIP		(- missingpf_coip)
#endif
#if	! defined(PF_E164)
#define PF_E164		(- missingpf_e164)
#endif
#if	! defined(PF_IEEE80211)
#define PF_IEEE80211	(- missingpf_ieee80211)
#endif
#if	! defined(PF_ISO)
#define PF_ISO		(- missingpf_iso)
#endif
#if	! defined(PF_MAX)
#define PF_MAX		(- missingpf_max)
#endif
#if	! defined(PF_NATM)
#define PF_NATM		(- missingpf_natm)
#endif
#if	! defined(PF_PPP)
#define PF_PPP		(- missingpf_ppp)
#endif
#if	! defined(PF_RESERVED_36)
#define PF_RESERVED_36	(- missingpf_reserved_36)
#endif
#if	! defined(PF_SIP)
#define PF_SIP		(- missingpf_sip)
#endif
#if	! defined(PF_UTUN)
#define PF_UTUN		(- missingpf_utun)
#endif
#if	! defined(PF_VSOCK)
#define PF_VSOCK	(- missingpf_vsock)
#endif


#endif /* USYSDEFSPF_INCLUDE */


