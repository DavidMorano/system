/* usysdefs_netdb HEADER */
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

#ifndef	USYSDEFSNETDB_INCLUDE
#define	USYSDEFSNETDB_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/utsname.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* |S_IS{x}| + S_IF{x}| */
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* |SIG{x}| */
#include	<unistd.h>		/* |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* |O_{x}| */
#include	<netdb.h>		/* |NI_MAX{x}| */


/* network-related name resolving service (some system do not have) */
#ifndef	NETDB_INTERAL			/* indicated to see |errno| */
#define	NETDB_INTERAL	-1
#endif
#ifndef	NETDB_SUCCESS			/* indicates success */
#define	NETDB_SUCCESS	0
#endif

#ifndef	NETDB_TRYAGAIN
#define	NETDB_TRYAGAIN		TRY_AGAIN
#endif
#ifndef	NETDB_NOTFOUND
#define	NETDB_NOTFOUND		HOST_NOT_FOUND
#endif
#ifndef	NETDB_NOADDRESS
#define	NETDB_NOADDRESS		NO_ADDRESS
#endif
#ifndef	NETDB_NORECOVERY
#define	NETDB_NORECOVERY	NO_RECOVERY
#endif

#ifdef	__cplusplus

struct netdberrs {
    static constexpr int	internal	= NETDB_INTERNAL ;
    static constexpr int	success		= NETDB_SUCCESS ;
    static constexpr int	tryagain	= NETDB_TRYAGAIN ;
    static constexpr int	notfound	= NETDB_NOTFOUND ;
    static constexpr int	noaddress	= NETDB_NOADDRESS ;
    static constexpr int	norecovery	= NETDB_NORECOVERY ;
} ; /* end struct (netdberrs) */

extern const netdberrs		netdberr ;

#endif /* __cplusplus */


#endif /* USYSDEFSNETDB_INCLUDE */


