/*
 * @file features.c
 * @author Vincent Wei (https://github.com/VincentWei)
 * @date 2021/09/20
 * @brief The functions for testing features of FooBar.
 *
 * Copyright (C) 2021 FMSoft <https://www.fmsoft.cn>
 *
 * This file is a part of FooBar, which contains the examples of my course:
 * _the Best Practices of C Language_.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "hbdinit_features.h"

bool
hbdinit_is_feature_enabled(enum hbdinit_feature feature)
{
    switch (feature) {
        case HBDINIT_FEATURE_HTML:
            if (HBDINIT_ENABLE_HTML)
                return true;
            break;
        case HBDINIT_FEATURE_XGML:
            if (HBDINIT_ENABLE_XGML)
                return true;
            break;
        case HBDINIT_FEATURE_XML:
            if (HBDINIT_ENABLE_XML)
                return true;
            break;
        case HBDINIT_FEATURE_LCMD:
            if (HBDINIT_ENABLE_LCMD)
                return true;
            break;
        case HBDINIT_FEATURE_LSQL:
            if (HBDINIT_ENABLE_LSQL)
                return true;
            break;
        case HBDINIT_FEATURE_RSQL:
            if (HBDINIT_ENABLE_RSQL)
                return true;
            break;
        case HBDINIT_FEATURE_HTTP:
            if (HBDINIT_ENABLE_HTTP)
                return true;
            break;
        case HBDINIT_FEATURE_HIBUS:
            if (HBDINIT_ENABLE_HIBUS)
                return true;
            break;
        case HBDINIT_FEATURE_MQTT:
            if (HBDINIT_ENABLE_MQTT)
                return true;
            break;
        case HBDINIT_FEATURE_SSL:
            if (HBDINIT_ENABLE_SSL)
                return true;
            break;
    }

    return false;
}

