////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Massachusetts Institute of Technology              //
// All rights reserved.                                                   //
//                                                                        //
// Redistribution and use in source and binary forms, with or without     //
// modification, are permitted provided that the following conditions are //
// met:                                                                   //
//                                                                        //
//     * Redistributions of source code must retain the above copyright   //
//       notice, this list of conditions and the following disclaimer.    //
//                                                                        //
//     * Redistributions in binary form must reproduce the above          //
//       copyright notice, this list of conditions and the following      //
//       disclaimer in the documentation and/or other materials provided  //
//       with the distribution.                                           //
//                                                                        //
//     * Neither the name of the Massachusetts Institute of Technology    //
//       nor the names of its contributors may be used to endorse or      //
//       promote products derived from this software without specific     //
//       prior written permission.                                        //
//                                                                        //
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS    //
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT      //
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  //
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   //
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  //
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT       //
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  //
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  //
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT    //
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  //
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   //
////////////////////////////////////////////////////////////////////////////

/* 
 * File:   litelang.h
 * Author: Dagvadorj Galbadrakh <dagvadorj@gmail.com>
 *
 * Created on September 27, 2010, 10:26 PM
 */

#ifndef LITELANG_H
#define LITELANG_H

#include <stdio.h>  // printf, sprintf
#include <stdlib.h> // NULL, free, malloc
#include <string.h> // strlen, strcmp
#include "slist.h"
#define MAX_STR 20

typedef struct block_s {
  int type;
  long s_offset;
  int value;
  int repeat;
} block;

typedef struct {
  FILE * file;
  char * name;
  int line;
} file_desc;

typedef struct token_s {
  int type;
  char * obj;
  file_desc * desc;
  long s_offset;
  int num_params; // for function
  slist * params;
} token;

void liten(FILE * in, char * file_name, slist * sym_table, token ** ret);

#endif	/* LITELANG_H */

