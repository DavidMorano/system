/* timestr HEADER */
/* lang=C++20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TIMESTR_INCLUDE
#define	TIMESTR_INCLUDE


EXTERNC_begin

extern char *timestr_date(time_t,int,char *) noex ;
extern char *timestr_std(time_t,char *) noex ;
extern char *timestr_edate(time_t,char *) noex ;
extern char *timestr_gmtstd(time_t,char *) noex ;
extern char *timestr_msg(time_t,char *) noex ;
extern char *timestr_hdate(time_t,char *) noex ;
extern char *timestr_log(time_t,char *) noex ;
extern char *timestr_gmlog(time_t,char *) noex ;
extern char *timestr_logz(time_t,char *) noex ;
extern char *timestr_gmlogz(time_t,char *) noex ;
extern char *timestr_elapsed(time_t,char *) noex ;
extern char *timestr_gmlog(time_t,char *) noex ;
extern char *timestr_scandate(time_t,char *) noex ;

EXTERNC_end


#endif /* TIMESTR_INCLUDE */


