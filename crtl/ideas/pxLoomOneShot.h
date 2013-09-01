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
/*
  Devices like channels need a way to cause a loom thread to wait until
  the target recipient pick up (copy in the channel's case) the message the
  target has been sent. A monitor would be the classic thing to use, but it
  it seemed like it would be possible to create something even lighter weight.

  In this experiment, the loom stack frame itself provides the object. This
  saves over allocating both a loom stack frame and a monitor object (which
  would also maintain a list of processes that are waiting for the monitor to
  be signaled. The OneShot only supports the caller as a waiter, although the
  trigger can be given to any number of recipients to use. However, once used,
  it is gone, and cannot be used again.
 */
#ifndef PXLOOMONESHOT_H
#define PXLOOMONESHOT_H


struct pxAlloc;
struct pxPoke *pxLoomOneShotCreate(struct pxAlloc *pAlloc);

#define PXLOOMONESHOT_WAIT(pLoomFrame, pLoom, pPoke) { \
        pxLoomContinuation *const pLC = \
            PXINTERFACE_getInterface(pPoke, pxLoomContinuation); \
        PXLOOMFRAME_CALL(pLoomFrame, pLoom, pLC); \
    }

#endif // PXLOOMONESHOT_H
