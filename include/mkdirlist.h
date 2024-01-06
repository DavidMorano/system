/* mkdirlist SUPPORT */
/* lang=C++20 */

/* create a list of the newsgroup directories */
/* version %I% last-modified %G% */

#ifndef	MKDIRLIST_INCLUDE
#define	MKDIRLIST_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<usystem.h>
#include	<vechand.h>
#include	<localmisc.h>


#define	MKDIRLIST_MAGIC		0x8987372A
#define	MKDIRLIST		struct mkdirlist_head
#define	MKDIRLIST_ENT		struct mkdirlist_ent
#define	MKDIRLIST_OFL		struct mkdirlist_eflags


struct mkdirlist_head {
	VECHAND		dirs ;
	uint		magic ;
} ;

struct mkdirlist_eflags {
	uint		excl:3 ;	/* ??? */
	uint		new:1 ;		/* new newsgroup */
	uint		options:1 ;	/* directory has options file */
	uint		subscribe:1 ;	/* subscribed in user file? */
	uint		seen:1 ;	/* already written out to user file */
	uint		show:1 ;	/* display to user */
	uint		link:1 ;	/* has been linked to another */
} ;

struct mkdirlist_ent {
	cchar		*name ;
	MKDIRLIST_ENT	*link ;
	MKDIRLIST_OFL	f ;
	int		nlen ;
	mode_t		mode ;
	time_t		mtime ;
	time_t		utime ;
	uino_t		ino ;
	dev_t		dev ;
	int		narticles ;
	int		order ;
} ;

typedef MKDIRLIST	mkdirlist ;
typedef MKDIRLIST_ENT	mkdirlist_ent ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int mkdirlist_start(MKDIRLIST *,cchar *,cchar *) noex ;
extern int mkdirlist_link(MKDIRLIST *) noex ;
extern int mkdirlist_defshow(MKDIRLIST *) noex ;
extern int mkdirlist_sort(MKDIRLIST *) noex ;
extern int mkdirlist_get(MKDIRLIST *,int,MKDIRLIST_ENT **) noex ;
extern int mkdirlist_ung(MKDIRLIST *,cchar *,time_t,int,int) noex ;
extern int mkdirlist_showdef(MKDIRLIST *) noex ;
extern int mkdirlist_show(MKDIRLIST *,cchar *,int) noex ;
extern int mkdirlist_audit(MKDIRLIST *) noex ;
extern int mkdirlist_finish(MKDIRLIST *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* MKDIRLIST_INCLUDE */


