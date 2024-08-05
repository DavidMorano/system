/* contentencodings HEADER */
/* lang=C20 */


/* revision history:

	= 1998-02-03, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	CONTENTENCODINGS
#define	CONTENTENCODINGS


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


#define	CF_NONE		0
#define	CE_7BIT		1
#define	CE_8BIT		2
#define	CE_BINARY	3
#define	CE_BASE64	4
#define	CE_QUOTED	5
#define	CE_OVERLAST	6


enum contentencodings {
	contentencoding_none,
	contentencoding_7bit,
	contentencoding_8bit,
	contentencoding_binary,
	contentencoding_base64,
	contentencoding_quoted,
	contentencoding_overlast
} ;

extern cchar	*contentencodings[] ;


#endif /* CONTENTENCODINGS */


