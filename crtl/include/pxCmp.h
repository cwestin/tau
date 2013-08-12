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

/** @file pxCmp.h
   Provides several utility functions for comparing various native types.

   The signature of the utility functions conforms to that required for
   pxHmMap's use.
 */

#ifndef PXCMP_H
#define PXCMP_H


/**
   Compare two ints.

   @param pL pointer to the left-hand value
   @param pR pointer to the right-hand value
   @returns a value less than zero if the left-hand value is less than the
     right-hand value, zero if the two values are the same, or a value greater
     than zero if the left-hand value is greater than the right-hand value
 */
int pxCmpInt(const void *pL, const void *pR);

/**
   Compare two structure pointers.

   @param pL pointer to the left-hand value
   @param pR pointer to the right-hand value
   @returns a value less than zero if the left-hand value is less than the
     right-hand value, zero if the two values are the same, or a value greater
     than zero if the left-hand value is greater than the right-hand value
 */
int pxCmpStructStar(const void *pL, const void *pR);

#endif // PXCMP_H
