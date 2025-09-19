/* cmdmap HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manage the map of keys to commands */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services (RNS).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	CMDMAP_INCLUDE
#define	CMDMAP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<vecobj.h>


#define	CMDMAP		struct cmdmap_head
#define	CMDMAP_FL	struct cmdmap_flags
#define	CMDMAP_ENT	struct cmdmap_entry
#define	CMDMAP_MAGIC	0x24182139


struct cmdmap_entry {
	int		key ;
	int		cmd ;
} ;

struct cmdmap_flags {
	uint		sorted:1 ;
} ;

struct cmdmap_head {
	vecobj		*mlp ;		/* map-list-pointer */
	CMDMAP_FL	fl ;
	uint		magic ;
} ;

typedef	CMDMAP		cmdmap ;
typedef	CMDMAP_FL	cmdmap_fl ;
typedef	CMDMAP_ENT	cmdmap_ent ;

EXTERNC_begin

extern int cmdmap_start(cmdmap *,const cmdmap_ent *) noex ;
extern int cmdmap_finish(cmdmap *) noex ;
extern int cmdmap_load(cmdmap *,int,int) noex ;
extern int cmdmap_lookup(cmdmap *,int) noex ;

EXTERNC_end


#endif /* CMDMAP_INCLUDE */


