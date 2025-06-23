/* commandment HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* COMMANDMENT object-load management */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-03-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	COMMANDMENT_INCLUDE
#define	COMMANDMENT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>
#include	<commandments.h>


#define	COMMANDMENT_MAGIC	0x99447242
#define	COMMANDMENT		struct commandment_head
#define	COMMANDMENT_FL		struct commandment_flags
#define	COMMANDMENT_CUR		struct commandment_cursor


struct commandment_flags {
        uint		modload:1 ;
} ;

struct commandment_cursor {
	void		*scp ;
	uint		magic ;
} ;

struct commandment_head {
	modload		*mlp ;		/* module-load-pointer */
	void		*obj ;		/* object pointer */
	void		*callp ;
	COMMANDMENT_FL	fl ;
	uint		magic ;
	int		objsz ;
	int		cursz ;
} ;

typedef	COMMANDMENT		commandment ;
typedef	COMMANDMENT_FL		commandment_fl ;
typedef	COMMANDMENT_CUR		commandment_cur ;

EXTERNC_begin

extern int	commandment_open(commandment *,cchar *,cchar *) noex ;
extern int	commandment_audit(commandment *) noex ;
extern int	commandment_count(commandment *) noex ;
extern int	commandment_max(commandment *) noex ;
extern int	commandment_read(commandment *,char *,int,int) noex ;
extern int	commandment_get(commandment *,int,char *,int) noex ;
extern int	commandment_curbegin(commandment *,commandment_cur *) noex ;
extern int	commandment_curend(commandment *,commandment_cur *) noex ;
extern int	commandment_curenum(commandment *,commandment_cur *,
			uint *,char *,int) noex ;
extern int	commandment_close(commandment *) noex ;

EXTERNC_end


#endif /* COMMANDMENT_INCLUDE */


