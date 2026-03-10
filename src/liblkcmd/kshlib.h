/* kshlib HEADER */
/* charset=ISO8859-1 */
/* lang=C89 */

/* library initialization for KSH built-in command libraries */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-11-01, David A­D­ Morano
        Written to have a place for the various KSH initialization subroutines.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	KSHLIB_INCLUDE
#define	KSHLIB_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<sesmsg.hh>


#define	KSHLIB_NOTE		struct kshlib_note
#define	KSHLIB_NENTS		30
#define	KSHLIB_INTPOLL		5
#define	KSHLIB_INTSESCHECK	(1*3600)
#define	KSHLIB_SESHOUR		18 /* 6:00pm */
#define	KSHLIB_SESDNAME		"/var/tmp/sessions"

#define	KSHLIB_RMKSH	(1<<0)		/* run-mode KSH */
#define	KSHLIB_RMMAIN	(1<<1)		/* run-mode MAIN */

enum kshlibcmds {
	kshlibcmd_noteoff,
	kshlibcmd_noteon,
	kshlibcmd_notecount,
	kshlibcmd_notestate,
	kshlibcmd_overlast
} ; /* end enum */

struct kshlib_note {
	time_t		stime ;
	uint		type ;
	int		nlen ;
	char		nbuf[SESMSG_NBUFLEN+1] ;
	char		user[SESMSG_USERLEN+1] ;
} ; /* end struct */

EXTERNC_begin

extern void	lib_init(int,void *) noex ;
extern void	lib_fini(void) noex ;

extern int	lib_initenviron(void *) noex ;
extern int	lib_initmemalloc(int) noex ;
extern int	lib_mainbegin(cchar **,cint *) noex ;
extern int	lib_mainend(void) noex ;
extern int	lib_kshbegin(void *,cint *) noex ;
extern int	lib_kshend(void) noex ;
extern int	lib_runmode(void) noex ;
extern int	lib_serial(void) noex ;

extern int	lib_sigreset(int) noex ;
extern int	lib_sigquit(void) noex ;
extern int	lib_sigintr(void) noex ;
extern int	lib_sigterm(void) noex ;
extern int	lib_issig(int) noex ;

extern int	lib_proghave(cchar *) noex ;
extern int	lib_progaddr(cchar *,void *) noex ;
extern int	lib_progcall(cchar *,int,cchar **,cchar **,void *) noex ;
extern int	lib_progcalla(const void *,int,cchar **,cchar **,void *) noex ;

extern int	lib_callcmd(cchar *,int,cchar **,cchar **,void *) noex ;
extern int	lib_callfunc(const void *,int,cchar **,cchar **,void *) noex ;

extern int	lib_noteadm(int,...) noex ;
extern int	lib_noteread(KSHLIB_NOTE *,int) noex ;
extern int	lib_notedel(int) noex ;

EXTERNC_end


#endif /* KSHLIB_INCLUDE */


