/* received HEADER */
/* charset=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
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

#ifdef	__cplusplus
enum receivedmems {
	receivedmem_finish,
	receivedmem_overlast
} ;
struct received ;
struct received_co {
	received	*op = nullptr ;
	int		w = -1 ;
	void operator () (received *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (received_co) */
struct received : received_head {
	received_co	finish ;
	received() noex {
	    finish(this,receivedmem_finish) ;
	    magic = 0 ;
	} ;
	received(const received &) = delete ;
	received &operator = (const received &) = delete ;
	int start(cchar *,int = -1) noex ;
	int getkey(int,cchar **) noex ;
	int getitem(int,cchar **) noex ;
	void dtor() noex ;
	destruct received() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (received) */
#else	/* __cplusplus */
typedef	RECEIVED	received ;
#endif /* __cplusplus */

EXTERNC_begin

extern int received_start(received *,cchar *,int) noex ;
extern int received_getkey(received *,int,cchar **) noex ;
extern int received_getitem(received *,int,cchar **) noex ;
extern int received_finish(received *) noex ;

EXTERNC_end

extern cpcchar	received_keys[] ;


#endif /* received_INCLUDE */


