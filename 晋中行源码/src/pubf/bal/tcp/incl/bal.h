#ifndef _BAL_H
#define _BAL_H

/* Define FOR_APCTL to be compatible to APCTL */
#define FOR_APCTL

#include <sys/types.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <netdb.h>


#include "tcp.h"

#include "pollevent.h"
#include "apctl.h"
#include "const.h"
#include "err.h"
#include "def.h"
#include "extern.h"

#endif
