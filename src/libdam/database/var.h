/* var HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* VAR management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VAR_INCLUDE
#define	VAR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<stddef.h>		/* language types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>

#include	<vars.h>


#define	VAR_MAGIC	0x99447246
#define	VAR		struct var_head
#define	VAR_CUR		struct var_cursor
#define	VAR_INFO	struct var_information
#define	VAR_CALLS	struct var_callsubs

#define	VARINFO_DAT	struct varinfo_data


struct var_information {
	time_t		wtime ;
	time_t		mtime ;
	uint		nvars ;
	uint		nskip ;
} ;

struct var_cursor {
	void		*scp ;		/* SO-cursor pointer */
	uint		magic ;
} ;

EXTERNC_begin
    struct var_callsubs {
	int	(*open)(void *,cchar *) noex ;
	int	(*opena)(void *,cchar **) noex ;
	int	(*count)(void *) noex ;
	int	(*curbegin)(void *,void *) noex ;
	int	(*fetch)(void *,cchar *,int,void *,char *,int) noex ;
	int	(*enumerate)(void *,void *,char *,int,char *,int) noex ;
	int	(*curend)(void *,void *) noex ;
	int	(*info)(void *,VARS_INFO *) noex ;
	int	(*audit)(void *) noex ;
	int	(*close)(void *) noex ;
    } ; /* end struct (var_callsubs) */
EXTERNC_end

struct var_head {
	modload		loader ;
	VAR_CALLS	call ;
	void		*obj ;		/* object pointer */
	uint		magic ;
	int		objsz ;		/* object size */
	int		cursz ;		/* cursor size */
} ;

struct varinfo_data {
	size_t		sz ;
	time_t		mtime ;
} ;

typedef	VAR		var ;
typedef VAR_CUR		var_cur ;
typedef VAR_INFO	var_info ;
typedef VAR_CALLS	var_calls ;

typedef VARINFO_DAT		varinfo_dat ;

EXTERNC_begin

extern int	var_open(var *,cchar *) noex ;
extern int	var_opena(var *,mainv) noex ;
extern int	var_count(var *) noex ;
extern int	var_curbegin(var *,var_cur *) noex ;
extern int	var_fetch(var *,cchar *,int,var_cur *,char *,int) noex ;
extern int	var_enum(var *,var_cur *,char *,int,char *,int) noex ;
extern int	var_curend(var *,var_cur *) noex ;
extern int	var_getinfo(var *,var_info *) noex ;
extern int	var_audit(var *) noex ;
extern int	var_close(var *) noex ;

extern int	varinfo_get(varinfo_dat *,cchar *,int) noex ;
extern int	varunlink(cchar *,int) noex ;

EXTERNC_end


#endif /* VAR_INCLUDE */


