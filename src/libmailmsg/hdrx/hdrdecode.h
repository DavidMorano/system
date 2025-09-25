/* hdrdecode HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* mail-header value string decoder */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HDRDECODE_INCLUDE
#define	HDRDECODE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |wchar_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<b64decoder.h>
#include	<qpdecoder.h>
#include	<chartrans.h>


#define	HDRDECODE_MAGIC		0x13f3c202
#define	HDRDECODE		struct hdrdecode_head
#define	HDRDECODE_FL		struct hdrdecode_flags
#define	HDRDECODE_CSLEN		32


struct hdrdecode_flags {
	uint		space:1 ;
	uint		ct:1 ;
} ;

struct hdrdecode_head {
	b64decoder	*b64p ;
	qpdecoder	*qpp ;
	chartrans	*ctp ;
	cchar		*pr ;
	HDRDECODE_FL	fl ;
	uint		magic ;
	char		cs[HDRDECODE_CSLEN+1] ;
} ;

typedef HDRDECODE	hdrdecode ;
typedef	HDRDECODE_FL	hdrdecode_fl ;

EXTERNC_begin

extern int hdrdecode_start(hdrdecode *,cchar *) noex ;
extern int hdrdecode_proc(hdrdecode *,wchar_t *,int,cchar *,int) noex ;
extern int hdrdecode_finish(hdrdecode *) noex ;

EXTERNC_end


#endif /* HDRDECODE_INCLUDE */


