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

#include "token.h"

char * token_t[14] = {
  "BOOL_T", 
  "BBYTE_T",
  "XBYTE_T", 
  "INT_T",
  "DOUBLE_T",
  "SYMBOL_T",
  "OP_T",
  "BITOP_T",
  "LOGIC_T",
  "ASSIGN_T",
  "FLOW_T",
  "FUNC_T",
  "COMMA_T",
  "PAREN_T"
};

void lite_error(file_desc * desc, char * err) {
  fprintf(stderr, "File: %s, Line: %d, Error: %s\n", desc->name, desc->line, err);
  exit(-1);
}

char * slist_print_token(void * obj) {
  token * t = (token *) obj;
  char * s = malloc(sizeof (t->obj) + MAX_STR);
  sprintf(s, "[%s %s] ", token_t[t->type], (char *) t->obj);
  return s;
}

void tokenize(
  file_desc * desc,
  char ** temp,
  int * type,
  slist * tok_queue) {

  if (*type == -1 || strcmp("\0", *temp) == 0) return;

  if (*type == SYMBOL_T) {
    if (strcmp("true", *temp) == 0 ||
      strcmp("false", *temp) == 0)
      *type = BOOL_T;
    else if (strcmp("if", *temp) == 0 ||
      strcmp("elif", *temp) == 0 ||
      strcmp("else", *temp) == 0 ||
      strcmp("end", *temp) == 0 ||
      strcmp("while", *temp) == 0 ||
      strcmp("continue", *temp) == 0 ||
      strcmp("break", *temp) == 0)
      *type = FLOW_T;
    else if (strcmp("in", *temp) == 0 ||
      strcmp("not", *temp) == 0 ||
      strcmp("and", *temp) == 0 ||
      strcmp("or", *temp) == 0 ||
      strcmp("xor", *temp) == 0)
      *type = LOGIC_T;
  }
  
  token * tok = malloc(sizeof (token));
  tok->type = *type;
  tok->obj = *temp;

  //if (*type == FUNC_T || *type == LOGIC_T) {
      tok->s_offset = ftell(desc->file);
  //}

  slist_enqueue(tok_queue, tok);

  *temp = malloc(sizeof (char) *MAX_STR);
  strcpy(*temp, "");
}

void token_parse(
  file_desc * desc,   // Source file
  int tok,            // Character being parsed
  char ** temp,       // Token being constructed
  int * type,         // Type of the token being constructed
  slist * tok_queue)  // Enqueue constructed token
  {

  int token_end = false;
  switch (tok) {
	  case ' ': case '\t':
	    tok = ' ';
      token_end = true;
      break;
    case '+': case '-': case '*': case '/': case '%':
      if (*type == -1) *type = OP_T;
      else token_end = true;
      break;
    case '&': case '|': case '^': case '~':
      if (*type == -1) *type = BITOP_T;
      else token_end = true;
    case '(':
      if (*type == SYMBOL_T) *type = FUNC_T;
    case ')':
      if (*type == -1) *type = PAREN_T;
      else token_end = true;
      break;
    case ',':
      if (*type == -1) *type = COMMA_T;
      else token_end = true;
      break;
    case '!':
      if (*type == -1) *type = LOGIC_T;
      else token_end = true;
      break;
    case '.':
      if (*type == -1) {
        strcpy(*temp, "0");
        *type = DOUBLE_T;
      } else if (*type == INT_T) {
        *type = DOUBLE_T;
      } else  {
        tok = ' ';
        token_end = true;
      }
      break;
    case '<': case '>':
      if (*type == -1) *type = LOGIC_T;
      else if (*type == LOGIC_T &&
        ((strcmp("<", *temp) == 0
            && tok == '<') ||
        (strcmp(">", *temp) == 0
            && tok == '>')))
        *type = BITOP_T;
      else
        token_end = true;
	    break;
    case '=':
      if (*type == -1) *type = ASSIGN_T;
      else {
        if (*type == OP_T && 
          (
            strcmp("+", *temp) == 0 || 
            strcmp("-", *temp) == 0 || 
            strcmp("*", *temp) == 0 || 
            strcmp("/", *temp) == 0 || 
            strcmp("%", *temp) == 0
          ))
          *type = ASSIGN_T;
        else if (*type == LOGIC_T &&
          (
            strcmp("<", *temp) == 0 || 
            strcmp(">", *temp) == 0 || 
            strcmp("!", *temp) == 0
          ))
          *type = LOGIC_T;
        else if (*type == ASSIGN_T && strcmp("=", *temp) == 0)
          *type = LOGIC_T;
        else token_end = true;
      }
      break;
    default:
      // ASCII codes for big and small letters plus underscore
      if ((97 <= tok && tok <= 122) || (65 <= tok && tok <= 90) || tok == '_') {
        // If no token is being parsed
        // start constructing SYMBOL_T
        if (*type == -1) *type = SYMBOL_T;
        else {
          if (*type == INT_T && strcmp("0", *temp) == 0) {
            if (tok == 'b') *type = BBYTE_T;
            else if (tok == 'x') *type = XBYTE_T;
          } else if (*type == XBYTE_T) {
            if (strlen(*temp) == 4) token_end = true;
            else if (tok < 'a' || tok > 'f') {
              lite_error(desc, "Invalid hexadecimal representation of byte");
            }
          // A SYMBOL_T can not start with digits
          } else if (*type == INT_T || *type == DOUBLE_T)
            lite_error(desc, "Invalid symbol");
          // anything else comes, SYMBOL_T has been parsed
          else if (*type != SYMBOL_T)
            token_end = true;
        }
      // ASCII codes for digits
      } else if (48 <= tok && tok <= 57) {
        // If no token is being parsed
        // start parsing INT_T
        if (*type == -1) *type = INT_T;
		    else {
		      // If what we are parsing are neither
		      // INT_T, DOUBLE_T or SYMBOL_T,
		      // the parsing is over
		      if (*type == XBYTE_T && strlen(*temp) == 4) {
		        token_end = true;
		      } else if (*type == BBYTE_T) {
		        if (strlen(*temp) == 10) token_end = true;
		        else if (tok != '0' || tok != '1')
		          lite_error(desc, "Invalid binary representation of byte");
		      } else if (*type != INT_T &&
			      *type != DOUBLE_T &&
			      *type != SYMBOL_T)
			      token_end = true;
		    }
      } else {
        // Anything else is invalid
        // TODO: Deal with UTF-8 bytes
        lite_error(desc, "Invalid character");
      }
      break;
  }
  if (token_end == true) {
    tokenize(desc, temp, type, tok_queue);
    *type = -1;
    if (tok != ' ') ungetc(tok, desc->file);
  } else {
    strcat(*temp, (char *) &tok);
  }
}

