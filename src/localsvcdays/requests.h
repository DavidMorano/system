/* requests HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */


/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	REQUESTS_INCLUDE
#define	REQUESTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>


#define	REQUESTS		struct requests_head
#define	REQUESTS_IT		struct requests_item


struct requests_item {
	int		ro ;
	int		rs ;
} ;

struct requests_head {
	vecobj		ents ;
} ;

typedef REQUESTS	requests ;
typedef REQUESTS_IT	requests_it ;

EXTERNC_begin

extern int requests_start(requests *) noex ;
extern int requests_finish(requests *) noex ;
extern int requests_count(requests *) noex ;
extern int requests_del(requests *,int) noex ;
extern int requests_add(requests *,requests_it *) noex ;
extern int requests_get(requests *,int,requests_it *) noex ;

EXTERNC_end


#endif /* REQUESTS_INCLUDE */


