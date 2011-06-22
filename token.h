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
 * File:   token.h
 * Author: Dagvadorj Galbadrakh
 *
 * Created on October 1, 2010, 2:21 PM
 */

#ifndef TOKEN_H
#define	TOKEN_H

#include "litelang.h"

enum {
  IF_B = 0,
  WHILE_B,
  FUNC_B
};

enum {
  BOOL_T = 0,   // true, false
  BBYTE_T,
  XBYTE_T,
  INT_T,
  DOUBLE_T,
  SYMBOL_T,

  OP_T,         // + - * / %
  BITOP_T,      // & | ~ ^ >> <<
  LOGIC_T,	    // < > <= >= == != not and or xor
  ASSIGN_T,	    // = += -= *= /= %=
  FLOW_T,	      // if elif else while continue break end

  FUNC_T,       // return() is built-in
  COMMA_T,	    // ,
  PAREN_T 	    // ( )
};

enum {
  false = 0,
  true
};

char * slist_print_token(void * obj);

void token_parse(
  file_desc * desc,
  int tok,
  char ** temp,
  int * type,
  slist * tok_queue);

void tokenize(
  file_desc * desc,
  char ** temp,
  int * type,
  slist * tok_queue);

#endif	/* TOKEN_H */

