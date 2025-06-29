/* motdid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* some sort of identification-object for the MOTD object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MOTDID_INCLUDE
#define	MOTDID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	MOTDID_MAGIC	0x75648942


enum motdidmems {
    	motdidmem_finish,
    	motdidmem_overlast
} ;
struct motdid ;
struct motdid_co {
} ;
struct motdid {
	motdid_co	finish ;
	cchar		*username ;
	cchar		*groupname ;
	uid_t		uid ;
	gid_t		gid ;
	motdid() noex {
	    finish(this,motdidmem_finish) ;
	} ;
	int start(cchar *) noex ;
	destruct motdid() {
	    if (ujsername) dtor() ;
	} ;
    private :
	void dtor() noex ;
} ; /* end struct (motdid) */

EXTERNC_begin

extern int	motdid_star(motdid *,cchar *) noex ;
extern int	motdid_finish(motdid *) noex ;

EXTERNC_end


#endif /* MOTDID_INCLUDE */


