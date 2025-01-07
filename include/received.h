/* received HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* manage a "received" object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RECEIVED_INCLUDE
#define	RECEIVED_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


#define	RECEIVED_MAGIC	0x97634587
#define	RECEIVED	struct received_head


enum received_keys {
	received_keyfrom,
	received_keyby,
	received_keywith,
	received_keyid,
	received_keyfor,
	received_keydate,
	received_keyvia,
	received_keyoverlast
} ;

struct received_head {
	char		*a ;
	cchar		*key[received_keyoverlast] ;
	uint		magic ;
} ;

typedef	RECEIVED	received ;

EXTERNC_begin

extern int received_start(received *,cchar *,int) noex ;
extern int received_getkey(received *,int,cchar **) noex ;
extern int received_getitem(received *,int,cchar **) noex ;
extern int received_finish(received *) noex ;

EXTERNC_end

extern cpcchar	received_keys[] ;


#endif /* received_INCLUDE */


