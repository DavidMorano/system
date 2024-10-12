/* mailfiles HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */


#ifndef	MAILFILES_INCLUDE
#define	MAILFILES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecitem.h>


#define	MAILFILES		VECITEM
#define	MAILFILES_ENT		struct mailfiles_entry


struct mailfiles_entry {
	char	*mailfname ;
	time_t	lasttime ;
	off_t	lastsize ;
	int	f_changed ;
} ;

typedef	MAILFILES		mailfiles ;
typedef	MAILFILES_ENT		mailfiles_ent ;

EXTERNC_begin

extern int mailfiles_init(mailfiles *) noex ;
extern int mailfiles_free(mailfiles *) noex ;
extern int mailfiles_add(mailfiles *,cchar *,int) noex ;
extern int mailfiles_addpath(mailfiles *,cchar *,int) noex ;
extern int mailfiles_get(mailfiles *,int,mailfiles_ent **) noex ;
extern int mailfiles_check(mailfiles *) noex ;
extern int mailfiles_count(mailfiles *) noex ;

EXTERNC_end


#endif /* MAILFILES_INCLUDE */



