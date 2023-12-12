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
/**
 * \file dybkgnd.c
 * \author Xueshuming <xueshuming@minigui.org>
 * \date 2020/09/03
 *
 * \brief This file implements dynamic background.
 *
 \verbatim

    This file is part of hishell, a developing operating system based on
    MiniGUI. HybridOs will support embedded systems and smart IoT devices.

    Copyright (C) 2002~2020, 2023 Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/blog/minigui-licensing-policy/>.

 \endverbatim
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgeff/mgeff.h>

#include "private/loading.h"

#define WALLPAPER_FILE_TOP          "wallpaper-top.jpg"
#define WALLPAPER_FILE_BOTTOM       "wallpaper-bottom.jpg"
#define LOADING_TEXT       "正在启动系统服务…"

BITMAP bmpTop;
BITMAP bmpBottom;
HWND hLoadingWnd = HWND_INVALID;
int animationValue = 0;

#undef USE_ANIMATION

#ifdef USE_ANIMATION
int start = 100;
int end = 0;
int duration = 600;
enum EffMotionType motionType = OutQuart;
//enum EffMotionType motionType = OutQuint;
#endif

static void loadBitmap(void)
{
    char topRes [MAX_PATH + 1];
    char bottomRes [MAX_PATH + 1];
    char* etc_value = NULL;

    bmpTop.bmWidth = 0;
    bmpBottom.bmWidth = 0;

    if ((etc_value = getenv("HBDINIT_ASSETS_DIR"))) {
        int len = strlen(etc_value);
        if (etc_value[len-1] == '/') {
            sprintf(topRes, "%simg/%s", etc_value, WALLPAPER_FILE_TOP);
            sprintf(bottomRes, "%simg/%s", etc_value, WALLPAPER_FILE_BOTTOM);
        }
        else {
            sprintf(topRes, "%s/img/%s", etc_value, WALLPAPER_FILE_TOP);
            sprintf(bottomRes, "%s/img/%s", etc_value, WALLPAPER_FILE_BOTTOM);
        }
    }
    else {
        sprintf(topRes, "/usr/local/share/hbd-init/assets/img/%s", WALLPAPER_FILE_TOP);
        sprintf(bottomRes, "/usr/local/share/hbd-init/assets/img/%s", WALLPAPER_FILE_BOTTOM);
    }

    LoadBitmapFromFile(HDC_SCREEN, &bmpTop, topRes);
    LoadBitmapFromFile(HDC_SCREEN, &bmpBottom, bottomRes);
}

static void clearBitmap(void)
{
    if (bmpTop.bmWidth > 0) {
        UnloadBitmap(&bmpTop);
    }

    if (bmpBottom.bmWidth > 0) {
        UnloadBitmap(&bmpBottom);
    }
}

#ifdef USE_ANIMATION
static int paintLoading(HDC hdc, int space)
{
    static int lastSpace = -1;
    int ret = 0;

    if (lastSpace == space) {
        return ret;
    }
    lastSpace = space;

    if (bmpTop.bmWidth > 0 && bmpTop.bmHeight > 0 &&
            bmpBottom.bmWidth > 0 && bmpBottom.bmHeight > 0) {
        int x, y, w, h, y2;
        static int wp_w = 0, wp_h = 0, wp_half_h = 0;

        if (wp_w == 0) {
            wp_w = (int)GetGDCapability(hdc, GDCAP_HPIXEL);
            wp_h = (int)GetGDCapability(hdc, GDCAP_VPIXEL);
            wp_half_h = wp_h >> 1;
            start = wp_half_h - bmpTop.bmHeight;;
        }

        SetBrushColor(hdc, RGBA2Pixel(hdc, 0x00, 0x00, 0x0, 0x0));
        FillBox(hdc, 0, 0, wp_w, wp_h);

        w = bmpTop.bmWidth;
        h = bmpTop.bmHeight;

        x = (wp_w - w) >> 1;
        y = wp_half_h - h;
        y2 = wp_half_h;

        FillBoxWithBitmap(hdc, x, y - space, bmpTop.bmWidth, bmpTop.bmHeight,
                &bmpTop);
        FillBoxWithBitmap(hdc, x, y2 + space, bmpBottom.bmWidth,
                bmpBottom.bmHeight, &bmpBottom);
        SyncUpdateDC(hdc);
    }
    else {
        _ERR_PRINTF("Failed to get the size of wallpaper bitmap\n");
        ret = -2;
        goto ret;
    }

ret:
    return ret;
}
#else
static int paintLoading(HDC hdc, int space)
{
    int ret = 0;

    if (bmpTop.bmWidth > 0 && bmpTop.bmHeight > 0 &&
            bmpBottom.bmWidth > 0 && bmpBottom.bmHeight > 0) {
        int x, y, w, h, y2;
        static int wp_w = 0, wp_h = 0, wp_half_h = 0;

        if (wp_w == 0) {
            wp_w = (int)GetGDCapability(hdc, GDCAP_HPIXEL);
            wp_h = (int)GetGDCapability(hdc, GDCAP_VPIXEL);
            wp_half_h = wp_h >> 1;
        }

        w = bmpTop.bmWidth;
        h = bmpTop.bmHeight;

        x = (wp_w - w) >> 1;
        y = wp_half_h - h;
        y2 = wp_half_h;

        FillBoxWithBitmap(hdc, x, y - space, bmpTop.bmWidth, bmpTop.bmHeight,
                &bmpTop);
        FillBoxWithBitmap(hdc, x, y2 + space, bmpBottom.bmWidth,
                bmpBottom.bmHeight, &bmpBottom);

        int fx = 0;
        int fy = y2 + space + bmpBottom.bmHeight;
        RECT rcText = {fx, fy, wp_w, fy + 40};
        SetTextColor (hdc, DWORD2Pixel(hdc, RGBA_lightwhite));
        SetBkMode (hdc, BM_TRANSPARENT);
        SelectFont(hdc, GetSystemFont(SYSLOGFONT_CAPTION));
        DrawText(hdc, LOADING_TEXT, -1, &rcText,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

        SyncUpdateDC(hdc);
    }
    else {
        _ERR_PRINTF("Failed to get the size of wallpaper bitmap\n");
        ret = -2;
        goto ret;
    }

ret:
    return ret;
}
#endif

#ifdef USE_ANIMATION
static void animated_cb(MGEFF_ANIMATION handle, HWND hwnd, int id, int *value)
{
    (void) handle;
    (void) id;
    if (hwnd == HWND_INVALID) {
        return;
    }

    //fprintf(stderr, "####> %s:%d:%s animted callback value=%d\n", __FILE__, __LINE__, __func__, *value);
    animationValue = *value;
    UpdateWindow(hwnd, true);
}

static void doAnimationBack(HWND hwnd)
{
    MGEFF_ANIMATION animation;
    animation = mGEffAnimationCreate((void *)hwnd, (void *)animated_cb, 1,
            MGEFF_INT);
    if (animation) {
        mGEffAnimationSetStartValue(animation, &end);
        mGEffAnimationSetEndValue(animation, &start);
        mGEffAnimationSetDuration(animation, motionType);
        mGEffAnimationSetCurve(animation, motionType);
        mGEffAnimationSyncRun(animation);
        mGEffAnimationDelete(animation);
    }
}

static void animated_end(MGEFF_ANIMATION handle)
{
    (void) handle;
    doAnimationBack(hLoadingWnd);
}

void startAnimation(HWND hwnd)
{
    MGEFF_ANIMATION animation;
    animation = mGEffAnimationCreate((void *)hwnd, (void *)animated_cb, 1,
            MGEFF_INT);
    if (animation) {
        mGEffAnimationSetStartValue(animation, &start);
        mGEffAnimationSetEndValue(animation, &end);
        mGEffAnimationSetDuration(animation, duration);
        mGEffAnimationSetCurve(animation, motionType);
        mGEffAnimationSetFinishedCb(animation, animated_end);
        mGEffAnimationSyncRun(animation);
        mGEffAnimationDelete(animation);
    }
}
#endif

static LRESULT
LoadingGUIWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_CREATE:
            loadBitmap();
        break;

#ifdef USE_ANIMATION
        case MSG_SHOWWINDOW:
            startAnimation(hWnd);
            break;
#endif

        case MSG_PAINT:
            hdc = BeginPaint(hWnd);
            paintLoading(hdc, animationValue);
            EndPaint(hWnd, hdc);
        return 0;

        case MSG_CLOSE:
            hLoadingWnd = HWND_INVALID;
            clearBitmap();
            DestroyMainWindow(hWnd);
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitCreateInfo(PMAINWINCREATE pCreateInfo)
{
    RECT rc = GetScreenRect();

    pCreateInfo->dwStyle = WS_VISIBLE;
    pCreateInfo->dwExStyle = 0;
    pCreateInfo->spCaption = "";
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

bool isLoadingWindowExists()
{
    return hLoadingWnd != HWND_INVALID;
}

int createLoadingWindow()
{
    MAINWINCREATE CreateInfo;
    InitCreateInfo (&CreateInfo);

    hLoadingWnd = CreateMainWindow (&CreateInfo);
    if (hLoadingWnd == HWND_INVALID) {
        return -1;
    }

    return 0;
}

void closeLoadingWindow()
{
    SendMessage(hLoadingWnd, MSG_CLOSE, 0, 0);
}


