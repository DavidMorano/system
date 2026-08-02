/* nlsmsg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* information to create and read an NLS-connect message */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Name:
	nlsmsg

  	Description:
	This header file contains some pre-processor defines used to
	create an NLS connection message and to parse the result.

	Definitions:
	NLPS		Network-Listener-Process-Server
	NLS		Network-Listener-Service

*******************************************************************************/

#ifndef	NLSMSG_INCLUDE
#define	NLSMSG_INCLUDE


/* this is the NLS dial-out connection-request string-prefix */
#ifndef	NLSDIALSTR
#define	NLSDIALSTR	"NLPS:002:002:"
#endif

/* these are some kind of NLS codes */
/* these are UNIX® System V standard */
#ifndef	NLSSTART
#define	NLSSTART	0
#define	NLSFORMAT	2
#define	NLSUNKNOWN	3
#define	NLSDISABLED	4
#endif /* NLSSTART */

/* these are environment variables passed down to a NLPS sub-servier */
/* these are UNIX® System V standard */
#ifndef	NLSADDR
#define	NLSADDR		"NLSADDR"
#define	NLSOPT		"NLSOPT"
#define	NLSUDATA	"NLSUDATA"
#define	NLSPROVIDER	"NLSPROVIDER"
#endif /* NLSADDR */

#ifdef	__cplusplus
struct nlsaddrs {
    static constexpr char addr[]	= NLSADDR ;
    static constexpr char opt[]		= NLSOPT ;
    static constexpr char udata[]	= NLSUDATA ;
    static constexpr char provider[]	= NLSPROVIDER ;
} ; /* end struct */
#endif /* __cplusplus */

#ifdef	__cplusplus
extern const nlsaddrs	nlsaddr ;
#endif /* __cplusplus */


#endif /* NLSMSG_INCLUDE */


