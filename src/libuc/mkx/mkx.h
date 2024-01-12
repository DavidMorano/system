/* mkx HEADER */
/* lang=C20 */

/* make a character (8-bit) out of an integer */
/* version %I% last-modified %G% */


/* revision history:

  = 2000-05-14, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
  = 2018-11-27, David A-D- Morano
	I created an inline version for the C language. We will
	likely do something for C++ at some other time.

  = 2023-10-23, David A-D- Morano
	At some point I added an inline (here) for the C++ language.

*/

/* Copyright (c) 1998,2018,23023 David A-D- Morano.  All rights reserved. */

#ifndef	MKX_INCLUDE
#define	MKX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	<mkgecosname.h>
#include	<mkcexsync.h>
#include	<mkuuid.h>
#include	<mkutmpid.h>
#include	<mkquoted.h>
#include	<mkprogenv.h>
#include	<mkpathxw.h>
#include	<mkpathx.h>
#include	<mkpathrooted.h>
#include	<mknpathxw.h>
#include	<mknpathx.h>
#include	<mkmagic.h>
#include	<mkfnamesuf.h>
#include	<mkfname.h>
#include	<mkfdfname.h>
#include	<mkdisplayable.h>
#include	<mkdisphdr.h>
#include	<mknpath.h>
#include	<mkpath.h>
#include	<mkchar.h>


EXTERNC_begin

extern int	mklogid(char *,int,cchar *,int,int) noex ;
extern int	mkplogid(char *,int,cchar *,int) noex ;
extern int	mksublogid(char *,int,cchar *,int) noex ;
extern int	mkaddrbest(char *,int,cchar *,int) noex ;
extern int	mkaddrname(char *,int,cchar *,int) noex ;
extern int	mkdisphdr(char *,int,cchar *,int) noex ;
extern int	mkrealname(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MKX_INCLUDE */


