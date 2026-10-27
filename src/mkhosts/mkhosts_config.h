/* mkhosts_config HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* MKHOSTS program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	MKHOSTSCONFIG_INCLUDE
#define	MKHOSTSCONFIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* POSIX® */
#include	<stdlib.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


#define	VERSION		"0a"
#define	WHATINFO	"@(#)mkhosts "

#define	VARPROGRAMROOT1	"PROGRAMROOT_MKHOSTS"
#define	VARPROGRAMROOT2	"PROGRAMROOT"

#ifndef	PROGRAMROOT
#define	PROGRAMROOT	"/usr/add-on/local"
#endif

#define	CONFIGFILE1	"etc/mkhosts/conf"
#define	CONFIGFILE2	"etc/mkhosts/mkhosts.conf"
#define	CONFIGFILE3	"etc/mkhosts.conf"

#define	HELPFILE	"lib/mkhosts/helpfile"


#endif /* MKHOSTSCONFIG_INCLUDE */


