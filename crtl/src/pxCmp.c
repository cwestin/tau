/*
  tau - http://github.com/cwestin/tau
  Copyright 2013 Chris Westin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#ifndef PXCMP_H
#include "pxCmp.h"
#endif


int pxCmpInt(const void *pL, const void *pR)
{
    if (*(const int *)pL < *(const int *)pR)
        return -1;
    if (*(const int *)pL > *(const int *)pR)
        return 1;

    return 0;
}

int pxCmpStructStar(const void *pL, const void *pR)
{
    if (*(const struct x **)pL < *(const struct x **)pR)
        return -1;
    if (*(const struct x **)pL > *(const struct x **)pR)
        return 1;

    return 0;
}
