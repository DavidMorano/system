/* libmailmsg HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* string-copy variant */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LIBMAILMSG_INCLUDE
#define	LIBMAILMSG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<mailmsghdrx.h>		/* composite group */
#include	<mailmsgmatx.h>		/* composite group */
#include	<mailmsgx.h>		/* composite group */
#include	<mailbox.h>
#include	<msgentry.h>

#include	<mimetypes.h>
#include	<mbcache.h>
#include	<emainfo.h>
#include	<mhcom.h>
#include	<mcmsg.h>

#include	<hdrx.h>		/* composite group */
#include	<contentencodings.h>
#include	<contypevals.h>
#include	<headkeymat.h>
#include	<comparse.h>
#include	<whitelist.h>
#include	<splitaddr.h>
#include	<stackaddr.h>


EXTERNC_begin

extern int	mailmsgheadkey(cchar *,int,cchar **) noex ;

EXTERNC_end


#endif /* LIBMAILMSG_INCLUDE */


