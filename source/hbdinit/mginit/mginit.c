///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
** mginit.c: A simple server for MiniGUI-Processes runtime mode.
**
** Copyright (C) 2003 ~ 2020, 2023 FMSoft (http://www.fmsoft.cn).
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgeff/mgeff.h>

#include "hbdinit_features.h"

#include "private/loading.h"

#ifdef _MGRM_PROCESSES

// Customer Require Id
#define FIXED_FORMAT_REQID          (MAX_SYS_REQID + 1)
#define UNFIXED_FORMAT_REQID        (MAX_SYS_REQID + 2)

// Customer sub require Id
#define REQ_SUBMIT_STATUSBAR_ZNODE  0   // status bar send znode index to server
#define REQ_GET_TOPMOST_TITLE       1   // get topmost normal window title
#define REQ_SUBMIT_TOGGLE           2   // toggle the application
#define REQ_SHOW_PAGE               3   // show target page
#define REQ_SUBMIT_TOPMOST          4   // set the window to topmost

// Customer compositor name
#define MIME_COMPOSITOR         "mine-compositor"

typedef struct tagRequestInfo 
{
    int id;                     // sub request ID
    HWND hWnd;                  // the window handle of the sending window
    unsigned int iData0;
    unsigned int iData1;
    unsigned int iData2;
    unsigned int iData3;
} RequestInfo;

typedef struct tagReplyInfo 
{
    int id;                     // sub request ID
    unsigned int iData0;
    unsigned int request1;
    unsigned int request2;
    unsigned int request3;
} ReplyInfo;

typedef struct tagSysConfig 
{
    int iSystemConfigClientID;  // clientID of system manager process
    int iDyBKGndClientID;       // clientID of dynamic background process
    int iBrowserClientID;       // clientID of browser process
    pid_t iSystemConfigPid;     // pid of system manager process
    pid_t iDyBKGndPid;          // pid of dynamic background process
    pid_t iBrowserPid;          // pid of browser process
    int iStatusBarZNode;        // z node index of status bar
    HWND hWndStatusBar;         // the handle of status bar, it is invlid in sysmgr process
    int iTopmostClientID;       // topmost client ID of topmost process
} SysConfig;

static BOOL quit = FALSE;
static int nr_clients = 0;
static SysConfig m_SysConfig;
extern CompositorOps mine_compositor;
extern void mine_compositor_toggle_state(BOOL toggle);

static void on_new_del_client (int op, int cli)
{
    if (op == LCO_NEW_CLIENT) {
        nr_clients ++;
        _MG_PRINTF ("A new client: %d.\n", mgClients[cli].pid);
    }
    else if (op == LCO_DEL_CLIENT) {
        _MG_PRINTF ("A client left: %d.\n", mgClients[cli].pid);
        nr_clients --;
        if (nr_clients == 0) {
            _MG_PRINTF ("There is no any client.\n");
        }
        else if (nr_clients < 0) {
            _ERR_PRINTF ("Serious error: nr_clients less than zero.\n");
        }
    }
    else
        _ERR_PRINTF ("Serious error: incorrect operations.\n");

    if (isLoadingWindowExists() && nr_clients > 0) {
        closeLoadingWindow();
    }
}

static pid_t exec_app (const char* file_name, const char* app_name)
{
    pid_t pid = 0;

    if ((pid = vfork ()) > 0) {
        _MG_PRINTF ("new child, pid: %d.\n", pid);
    }
    else if (pid == 0) {
        execl (file_name, app_name, NULL);
        perror ("execl");
        _exit (1);
    }
    else {
        perror ("vfork");
    }

    return pid;
}

static void child_wait (int sig)
{
    (void) sig;
    int pid;
    int status;

    while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED (status))
            _MG_PRINTF ("--pid=%d--status=%x--rc=%d---\n", pid, status, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            _MG_PRINTF ("--pid=%d--signal=%d--\n", pid, WTERMSIG (status));
    }
}

static int fix_format_request(int cli, int clifd, void* buff, size_t len)
{
    (void) cli;
    (void) len;
    RequestInfo * requestInfo = (RequestInfo *)buff;
    ReplyInfo replyInfo;

    if(requestInfo->id == REQ_SUBMIT_TOGGLE) {
        int cur_clientId = 0; 
        int idx_topmost = 0; 

        // reply client
        replyInfo.id = REQ_SUBMIT_TOGGLE;
        replyInfo.iData0 = (int)TRUE;
        ServerSendReply(clifd, &replyInfo, sizeof(replyInfo));

        idx_topmost = ServerGetTopmostZNodeOfType(NULL, ZOF_TYPE_NORMAL, &cur_clientId); 
        if(idx_topmost > 0)
            mine_compositor_toggle_state(TRUE);
    }
    return 0;
}

int MiniGUIMain (int argc, const char* argv[])
{
    MSG msg;
    struct sigaction siga;

    siga.sa_handler = child_wait;
    siga.sa_flags  = 0;
    memset (&siga.sa_mask, 0, sizeof(sigset_t));
    sigaction (SIGCHLD, &siga, NULL);

    memset(&m_SysConfig, 0, sizeof(SysConfig));

    OnNewDelClient = on_new_del_client;

    if (!ServerStartup (0 , 0 , 0)) {
        _ERR_PRINTF("Can not start the server of MiniGUI-Processes: mginit.\n");
        return 1;
    }

    // register a compositor
    if(!ServerRegisterCompositor(MIME_COMPOSITOR, &mine_compositor))
    {
        return 4;
    }

    // register request handler
    if(!RegisterRequestHandler(FIXED_FORMAT_REQID, fix_format_request))
    {
        return 2;
    }

#ifdef USE_ANIMATION
    mGEffInit();
#endif

    if (createLoadingWindow()) {
        _ERR_PRINTF("Can not create loading window.\n");
#ifdef USE_ANIMATION
        mGEffDeinit();
#endif
        return 1;
    }

    if (argc > 1) {
        if (exec_app (argv[1], argv[1]) == 0)
            return 3;
    }

    while (!quit && GetMessage (&msg, HWND_DESKTOP)) {
        DispatchMessage (&msg);
    }

    // unregister customer compositor
    ServerUnregisterCompositor(MIME_COMPOSITOR);

#ifdef USE_ANIMATION
    mGEffDeinit();
#endif
    return 0;
}

#else   /* defined _MGRM_PROCESSES */

int main(void)
{
    _WRN_PRINTF ("This test program is the server for MiniGUI-Prcesses runmode.\n"
           "    But your MiniGUI was not configured as MiniGUI-Processes\n");
    return 0;
}

#endif  /* not defined _MGRM_PROCESSES */

