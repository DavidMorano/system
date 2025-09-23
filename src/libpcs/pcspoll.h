/* pcspoll HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* load management and interface for the PCSPOLLS object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-03, David A­D­ Morano
	This module was originally written.

	- 2008-10-07, David A­D­ Morano
	This module was modified to allow for the main part of it
	to be a loadable module.

*/

/* Copyright © 1998,2008 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSPOLL_INCLUDE
#define	PCSPOLL_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<modload.h>
#include	<pcsconf.h>
#include	<localmisc.h>

#include	"pcspolls.h"


#define	PCSPOLL		struct pcspoll_head
#define	PCSPOLL_INFO	struct pcspoll_information
#define	PCSPOLL_CA	struct pcspoll_calls
#define	PCSPOLL_FL	struct pcspoll_flags
#define	PCSPOLL_MAGIC	0x97677246


struct pcspoll_information {
	int		dummy ;
} ;

struct pcspoll_flags {
	uint		loaded:1 ;
} ;

struct pcspoll_head {
	void		*obj ;		/* object pointer */
	modload		loader ;
	PCSPOLL_CA	call ;
	PCSPOLL_FL	fl ;
	uint		magic ;
} ;

typedef	PCSPOLL		pcspoll ;
typedef	PCSPOLL_INFO	pcspoll_info ;
typedef	PCSPOLL_CA	pcspoll_ca ;
typedef	PCSPOLL_FL	pcspoll_fl ;

EXTERNC_begin

struct pcspoll_calls {
	int	(*start)(void *,PCSCONF *,cchar *) noex ;
	int	(*info)(void *,PCSPOLLS_INFO *) noex ;
	int	(*cmd)(void *,int) noex ;
	int	(*finish)(void *) noex ;
} ;

extern int	pcspoll_start(PCSPOLL *,PCSCONF *,cchar *) noex ;
extern int	pcspoll_info(PCSPOLL *,pcspoll_info *) noex ;
extern int	pcspoll_cmd(PCSPOLL *,int) noex ;
extern int	pcspoll_finish(PCSPOLL *) noex ;

EXTERNC_end


#endif /* PCSPOLL_INCLUDE */


