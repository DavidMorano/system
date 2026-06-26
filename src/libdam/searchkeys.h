/* searchkeys HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* provides management for search-keys */
/* version %I% last-modified %G% */


/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	SEARCHKEYS_INCLUDE
#define	SEARCHKEYS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strpack.h>		/* LIBDAM */
#include	<xwords.h>		/* LIBDAM */


#define	SEARCHKEYS		struct searchkeys_head
#define	SEARCHKEYS_POP		struct searchkeys_popper
#define	SEARCHKEYS_CUR		struct searchkeys_cursor
#define	SEARCHKEYS_KW		struct searchkeys_kword
#define	SEARCHKEYS_PH		struct searchkeys_kphrase
#define	SEARCHKEYS_MAGIC	0x00885543


struct searchkeys_cursor {
	int		i ;
	int		j ;
} ; /* end struct */

struct searchkeys_kword {
	cchar		*kp ;
	int		kl ;
} ; /* end struct */

struct searchkeys_kphrase {
	SEARCHKEYS_KW	*kwords ;
	int		nwords ;
} ; /* end struct */

struct searchkeys_popper {
	int		*nmatch ;
	uint		magval ;
	int		cphrases ;
	int		f_prefix ;
} ; /* end struct */

struct searchkeys_head {
	strpack		*spp ;		/* String-Pack-Pointer */
	SEARCHKEYS_PH	*kphrases ;
	uint		magval ;
	int		nphrases ;
} ; /* end struct */

typedef	SEARCHKEYS	searchkeys ;
typedef	SEARCHKEYS_POP	searchkeys_pop ;
typedef	SEARCHKEYS_CUR	searchkeys_cur ;
typedef	SEARCHKEYS_KW	searchkeys_kw ;
typedef	SEARCHKEYS_PH	searchkeys_ph ;

EXTERNC_begin

extern int searchkeys_start(searchkeys *,mainv) noex ;
extern int searchkeys_popbegin(searchkeys *,searchkeys_pop *,int) noex ;
extern int searchkeys_process(searchkeys *,searchkeys_pop *,cchar *,int) noex ;
extern int searchkeys_processxw(searchkeys *,searchkeys_pop *,xwords *) noex ;
extern int searchkeys_popend(searchkeys *,searchkeys_pop *) noex ;
extern int searchkeys_curbegin(searchkeys *,searchkeys_cur *) noex ;
extern int searchkeys_curenum(searchkeys *,searchkeys_cur *,cchar **) noex ;
extern int searchkeys_curend(searchkeys *,searchkeys_cur *) noex ;
extern int searchkeys_finish(searchkeys *) noex ;

EXTERNC_end


#endif /* SEARCHKEYS_INCLUDE */


