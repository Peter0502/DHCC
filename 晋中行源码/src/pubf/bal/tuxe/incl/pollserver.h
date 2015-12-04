#ifndef __POLLSERVER_H
#define __PLLLSERVER_H

/******************************************************************************\
 *  
 *
 *
 *
\******************************************************************************/


#define OK								0
#define FAIL							-1
#define ENOSVC							-2

#define POLL_NORMAL						0
#define POLL_TEST						1

#define POLL_SERVICE_PREFIX				"PC"

#define SERVICENAMELEN       			30

#define LIST_INITSIZE					50

#define DEFAULT_GRANULARITY				1000000L

#define POLLSVR_POLLTHRESHOLD		    10

/******* Type Definition **********/

typedef struct {
	char 				service[SERVICENAMELEN];	/* polling service  	*/
	unsigned long		lservice;					/* polling service id 	*/
	unsigned long		interval;					/* interval by ms 		*/
	struct timeval 		lasttime;					/* Last polling time	*/
	struct timeval		nexttime;					/* Next polling time	*/
	long				count;						/* polling count		*/
} T_PollSvc;

#endif
