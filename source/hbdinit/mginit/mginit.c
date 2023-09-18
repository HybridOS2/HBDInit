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
** Copyright (C) 2003 ~ 2020 FMSoft (http://www.fmsoft.cn).
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

#ifdef _MGRM_PROCESSES

static BOOL quit = FALSE;
static int nr_clients = 0;

HWND hLoadingWnd = HWND_INVALID;

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

    if ((hLoadingWnd != HWND_INVALID) && nr_clients > 0) {
        SendMessage(hLoadingWnd, MSG_CLOSE, 0, 0);
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

static unsigned int old_tick_count;

static pid_t pid_scrnsaver = 0;

static int my_event_hook (PMSG msg)
{
    old_tick_count = GetTickCount ();

    if (pid_scrnsaver) {
        kill (pid_scrnsaver, SIGINT);
        ShowCursor (TRUE);
        pid_scrnsaver = 0;
    }

    if (msg->message == MSG_KEYDOWN) {
        switch (msg->wParam) {
        case SCANCODE_ESCAPE:
            if (nr_clients == 0) {
                quit = TRUE;
            }
            break;

        case SCANCODE_F1:
           exec_app ("./edit", "edit");
           break;
        case SCANCODE_F2:
           exec_app ("./timeeditor", "timeeditor");
           break;
        case SCANCODE_F3:
           exec_app ("./propsheet", "propsheet");
           break;
        case SCANCODE_F4:
           exec_app ("./bmpbkgnd", "bmpbkgnd");
           break;
    }
    }

    return HOOK_GOON;
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

static void InitLoadingGUI (void)
{
}

static void TermLoadingGUI (void)
{
}

static void OnLoadingPaint (HWND hWnd, HDC hdc)
{
    RECT rc;
    GetWindowRect(hWnd, &rc);
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, 0, 0, RECTW(rc), RECTH(rc));
}

static LRESULT LoadingGUIWinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_CREATE:
            InitLoadingGUI ();
        break;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            OnLoadingPaint (hWnd, hdc);
            EndPaint (hWnd, hdc);
        return 0;

        case MSG_CLOSE:
            hLoadingWnd = HWND_INVALID;
            TermLoadingGUI ();
            DestroyMainWindow (hWnd);
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

void InitCreateInfo (PMAINWINCREATE pCreateInfo)
{
    RECT rc = GetScreenRect();

    pCreateInfo->dwStyle = WS_VISIBLE;
    pCreateInfo->dwExStyle = 0;
    pCreateInfo->spCaption = "abcdefg";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = 0;
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = LoadingGUIWinProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = pCreateInfo->lx + RECTW(rc);
    pCreateInfo->by = pCreateInfo->ty + RECTH(rc);
    pCreateInfo->iBkColor = COLOR_black;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int MiniGUIMain (int argc, const char* argv[])
{
    MAINWINCREATE CreateInfo;

    MSG msg;
    struct sigaction siga;

    siga.sa_handler = child_wait;
    siga.sa_flags  = 0;
    memset (&siga.sa_mask, 0, sizeof(sigset_t));
    sigaction (SIGCHLD, &siga, NULL);

    OnNewDelClient = on_new_del_client;

    if (!ServerStartup (0 , 0 , 0)) {
        _ERR_PRINTF("Can not start the server of MiniGUI-Processes: mginit.\n");
        return 1;
    }

    SetServerEventHook (my_event_hook);

    if (argc > 1) {
        if (exec_app (argv[1], argv[1]) == 0)
            return 3;
    }

    InitCreateInfo (&CreateInfo);

    hLoadingWnd = CreateMainWindow (&CreateInfo);
    if (hLoadingWnd == HWND_INVALID)
        return -1;

    old_tick_count = GetTickCount ();

    while (!quit && GetMessage (&msg, HWND_DESKTOP)) {
        DispatchMessage (&msg);
    }

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

