/* usys_xtistructs HEADER (X/Open Transport Interface) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® XTI support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	System:
	usys_xtistructs

	Description:
	This file contains some support for the X/Open Transport
	Interface (XTI) related operations.

*******************************************************************************/

#ifndef	USTSXTISTRUCTS_INCLUDE
#define	USTSXTISTRUCTS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#if	defined(SYSHAS_XTI) && (SYSHAS_XTI > 0)

/* congratulations: you have it! */

#else /* defined(SYSHAS_XTI) && (SYSHAS_XTI > 0) */

struct netbuf {
        unsigned int maxlen;
        unsigned int len;
        void *buf;
} ; /* end struct */

struct t_info {
        int addr;        /* max size of protocol address         */
        int options;     /* max size of protocol options         */
        int tsdu;        /* max size of max transport service    */
                                /* data unit    */
        int etsdu;       /* max size of max expedited tsdu       */
        int connect;     /* max data for connection primitives   */
        int discon;      /* max data for disconnect primitives   */
        int servtype;    /* provider service type                */
        int flags;       /* other info about transport providers */
} ; /* end struct */

struct t_bind {
        struct netbuf   addr;
        unsigned int    qlen;
} ; /* end struct */

struct t_call {
        struct netbuf   addr;           /*  address             */
        struct netbuf   opt;            /* options              */
        struct netbuf   udata;          /* user data            */
        int             sequence;       /* sequence number      */
} ; /* end struct */

/* type-aliases for XTI structures */
#define	NETBUF		struct netbuf
#define	UTINFO		struct t_info
#define	UTBIND		struct t_bind
#define	UTCALL		struct t_call


#endif /* defined(SYSHAS_XTI) && (SYSHAS_XTI > 0) */


#endif /* USTSXTISTRUCTS_INCLUDE */


