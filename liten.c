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

#include "litelang.h"
#include "token.h"
#include "exec.h"

void liten(FILE * in, char * file_name, slist * sym_table, token ** ret) {
    
    // *ret = NULL;
    
    slist * block_stack = slist_init();

    slist * local_table = slist_init();
    slist_push(sym_table, local_table);
    
    file_desc * desc = malloc(sizeof(file_desc));
    desc->file = in;
    desc->name = file_name;
    desc->line = 0;
    
    int tok;
    char * temp = malloc(sizeof(char)*MAX_STR);
    strcpy(temp, "\0");
    int type = -1;
    slist * tok_queue = slist_init();
    
    while (1) {
        if (in == stdin) {
            printf(":> ");
        } else if (tok == EOF) {
	    break;
        }
        tok = getc(in);
        while (1) {
	    if (tok == '\n' || tok == EOF) {
		desc->line++;
                tokenize(desc, &temp, &type, tok_queue);
                type = -1;
		// slist_print(tok_queue, slist_print_token);
                // TODO: Check line for syntax errors
		if (tok_queue->len == 0) break;
		if (block_stack->len == 0) {
		    rpn(desc, tok_queue, sym_table, block_stack, ret);
		} else {
		    char * temp_str = ((token *) slist_get(tok_queue, 0)->obj)->obj;
		    block * temp = (block *) slist_get(block_stack, -1)->obj;
		    if (strcmp("end", temp_str) == 0) {
			rpn(desc, tok_queue, sym_table, block_stack, ret);
		    } else if (strcmp("if", temp_str) == 0 ||
			strcmp("while", temp_str) == 0) {
			if (temp->value == 1)
			    rpn(desc, tok_queue, sym_table, block_stack, ret);
			else {
			    block * temp1 = malloc(sizeof(block));
			    temp1->value = 0;
			    if (strcmp("if", temp_str) == 0) {
				temp1->type = IF_B;
			    } else if (strcmp("while", temp_str) == 0) {
				temp1->type = WHILE_B;
			    }
			    slist_push(block_stack, temp1);
			}
		    } else if (strcmp("elif", temp_str) == 0) {
			if (temp->type != IF_B) lite_error(desc, "'elif' exists in 'if' block");
			if (temp->value == 1)
			    temp->value = 0;
			else
			    rpn(desc, tok_queue, sym_table, block_stack, ret);
		    } else if (strcmp("else", temp_str) == 0) {			
			if (temp->type != IF_B) lite_error(desc, "'else' exists in 'if' block");
			if (temp->value == 1)
			    temp->value = 0;
			else
			    temp->value = 1;
		    } else {
			if (temp->value == 1)
			    rpn(desc, tok_queue, sym_table, block_stack, ret);
		    }
		}
                tok_queue = slist_init();
            } else {
                token_parse(desc, tok, &temp, &type, tok_queue);
            }
            if (in == stdin && tok == '\n') {
		break;
	    } 
            tok = getc(in);
        }
    }
    
    return;
}
