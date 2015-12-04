#ifndef _GLOBAL_H
#define _GLOBAL_H

/** Global Variables **/
T_infGlobal  g_global;              /* global information */
T_infDate    g_date;                /* date information  */
T_BalInfo    g_balinfo;		        /* bal information   */
int	     balerrno;		            /* bal errno         */

#ifdef FOR_APCTL
char**		__argv;		   /* __argv = argv  */
int			__argc;		   /* __argc == argc */
#endif

#endif
