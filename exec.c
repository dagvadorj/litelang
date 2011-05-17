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

#include "exec.h"

enum { Operator = 0, Operand, Function, Comma, LeftParen, RightParen };

typedef struct symbol_s {
	char * symbol;
	token * tok;
} symbol;

symbol * fetch_sym(slist * sym_table, char * sym) {
	int i = -1, j;
	slist_node * iter, * iter_node;
	slist * node;
	symbol * iter_sym;
	while ((iter = slist_get(sym_table, i)) != NULL) {
		node = (slist *) iter->obj;
		j = -1;
		while ((iter_node = slist_get(node, j)) != NULL) {
			iter_sym = (symbol *) iter_node->obj;
			if (strcmp(sym, iter_sym->symbol) == 0) {
				return iter_node->obj;
			}
			j--;
		}
		i--;
	}
	return NULL;
}

int op_prec(file_desc * desc, token * tok1, token * tok2) {
	int t1, t2;

	switch (tok1->type) {
	case ASSIGN_T:
		t1 = 0;
		break;
	case FLOW_T:
		t1 = 1;
		break;
	case LOGIC_T:
		t1 = 2;
		break;
	case OP_T:
		t1 = 3;
		break;
	case BITOP_T:
		t1 = 4;
		break;
	default:
		lite_error(desc, "Operation not supported");
		break;
	}

	switch (tok2->type) {
	case ASSIGN_T:
		t2 = 0;
		break;
	case FLOW_T:
		t2 = 1;
		break;
	case LOGIC_T:
		t2 = 2;
		break;
	case OP_T:
		t2 = 3;
		break;
	case BITOP_T:
		t2 = 4;
		break;
	default:
		lite_error(desc, "Operation not supported");
		break;
	}

	if (t1 != t2)
		return t1-t2;

	if (tok1->type == OP_T && tok2->type == OP_T) {

		if (strcmp("+", tok1->obj) == 0 || strcmp("-", tok1->obj) == 0)
			t1 = 0;
		else
			t1 = 1;

		if (strcmp("+", tok2->obj) == 0 || strcmp("-", tok2->obj) == 0)
			t2 = 0;
		else
			t2 = 1;

		return t1-t2;
	}

	if (tok1->type == BITOP_T && tok2->type == BITOP_T) {

		if (strcmp("&", tok1->obj) == 0 || strcmp("|", tok1->obj) == 0
				|| strcmp("^", tok1->obj) == 0)
			t1 = 0;
		else
			t1 = 1;

		if (strcmp("&", tok2->obj) == 0 || strcmp("|", tok2->obj) == 0
				|| strcmp("^", tok2->obj) == 0)
			t2 = 0;
		else
			t2 = 1;

		return t1-t2;
	}
	
	if (tok1->type == LOGIC_T && tok2->type == LOGIC_T) {
		if (strcmp("<", tok1->obj) == 0 || strcmp(">", tok1->obj) == 0
				|| strcmp(">=", tok1->obj) == 0
				|| strcmp("<=", tok1->obj) == 0
				|| strcmp("==", tok1->obj) == 0
				|| strcmp("!=", tok1->obj) == 0)
			t1 = 1;
		else
			t1 = 0;

		if (strcmp("<", tok2->obj) == 0 || strcmp(">", tok2->obj) == 0
				|| strcmp(">=", tok2->obj) == 0
				|| strcmp("<=", tok2->obj) == 0
				|| strcmp("==", tok2->obj) == 0
				|| strcmp("!=", tok2->obj) == 0)
			t2 = 1;
		else
			t2 = 0;

		return t1-t2;
	}

	return 0;
}

int token_type(file_desc * desc, token * tok) {
	int type = tok->type;
	switch (type) {
	case BOOL_T:
	case INT_T:
	case DOUBLE_T:
	case SYMBOL_T:
	case STR_T:
		return Operand;
		break;
	case OP_T:
	case BITOP_T:
	case LOGIC_T:
	case ASSIGN_T:
	case FLOW_T:
		return Operator;
		break;
	case FUNC_T:
		return Function;
		break;
	case COMMA_T:
		return Comma;
		break;
	case PAREN_T:
		if (strcmp("(", tok->obj) == 0)
			return LeftParen;
		else if (strcmp(")", tok->obj) == 0)
			return RightParen;
		break;
	default:
		lite_error(desc, "Syntax error");
		break;
	}
	return -1;
}

void exec_op(file_desc * desc, slist * out_queue, token * tok, slist * sym_table) {

	token * iter_tok1, * iter_tok2, * res_tok;
	symbol * sym;

	if ((iter_tok2 = (token *) slist_pop(out_queue)) == NULL)
		lite_error(desc, "Not enough arguments");
	if ((iter_tok1 = (token *) slist_pop(out_queue)) == NULL)
		lite_error(desc, "Not enough arguments");

	if (iter_tok1->type == SYMBOL_T) {
		if ((sym = fetch_sym(sym_table, iter_tok1->obj)) == NULL)
			lite_error(desc, "Symbol doesn't exist");
		else {
			free(iter_tok1);
			iter_tok1 = ((symbol *) sym)->tok;
		}
	}

	if (iter_tok2->type == SYMBOL_T) {
		if ((sym = fetch_sym(sym_table, iter_tok2->obj)) == NULL)
			lite_error(desc, "Symbol doesn't exist");
		else {
			free(iter_tok2);
			iter_tok2 = ((symbol *) sym)->tok;
		}
	}

	if (strcmp("/", tok->obj) == 0 || strcmp("%", tok->obj) == 0) {
		if (atoi(iter_tok2->obj) == 0)
			lite_error(desc, "Division by zero");
	}
	
	res_tok = malloc(sizeof(token));
	res_tok->obj = malloc(sizeof(char *));

	if (iter_tok1->type == DOUBLE_T || iter_tok2->type == DOUBLE_T) {
		res_tok->type = DOUBLE_T;
		if (strcmp("+", tok->obj) == 0)
			sprintf(res_tok->obj, "%f", atof(iter_tok1->obj)
					+ atof(iter_tok2->obj));
		else if (strcmp("-", tok->obj) == 0)
			sprintf(res_tok->obj, "%f", atof(iter_tok1->obj)
					- atof(iter_tok2->obj));
		else if (strcmp("*", tok->obj) == 0)
			sprintf(res_tok->obj, "%f", atof(iter_tok1->obj)
					* atof(iter_tok2->obj));
		else if (strcmp("/", tok->obj) == 0)
			sprintf(res_tok->obj, "%f", atof(iter_tok1->obj)
					/ atof(iter_tok2->obj));
		else if (strcmp("%", tok->obj) == 0)
			lite_error(desc, "Arithmetic 'modulo' operation is valid only for integers");
	} else {
		res_tok->type = INT_T;
		if (strcmp("+", tok->obj) == 0)
			sprintf(res_tok->obj, "%d", atoi(iter_tok1->obj)
					+ atoi(iter_tok2->obj));
		else if (strcmp("-", tok->obj) == 0)
			sprintf(res_tok->obj, "%d", atoi(iter_tok1->obj)
					- atoi(iter_tok2->obj));
		else if (strcmp("*", tok->obj) == 0)
			sprintf(res_tok->obj, "%d", atoi(iter_tok1->obj)
					* atoi(iter_tok2->obj));
		else if (strcmp("/", tok->obj) == 0)
			sprintf(res_tok->obj, "%d", atoi(iter_tok1->obj)
					/ atoi(iter_tok2->obj));
		else if (strcmp("%", tok->obj) == 0)
			sprintf(res_tok->obj, "%d", atoi(iter_tok1->obj)
					% atoi(iter_tok2->obj));
	}
	slist_push(out_queue, res_tok);
}

void exec_bitop(file_desc * desc, slist * out_queue, token * tok, slist * sym_table) {
}

void exec_flow(file_desc * desc, slist * out_queue, token * tok, slist * sym_table,
		slist * block_stack) {

	token * iter_tok;
	symbol * sym;
	block * temp;

	if (strcmp("if", tok->obj) == 0) {
		if ((iter_tok = (token *) slist_pop(out_queue)) == NULL)
			lite_error(desc, "Not enough arguments");

		if (iter_tok->type == SYMBOL_T) {
			if ((sym = fetch_sym(sym_table, iter_tok->obj)) == NULL)
				lite_error(desc, "Symbol doesn't exist");
			else {
				free(iter_tok);
				iter_tok = ((symbol *) sym)->tok;
			}
		}

		if (iter_tok->type != BOOL_T)
			lite_error(desc, "Control flow 'if' operation takes boolean argument");
		
		if (block_stack->len == 0) {
		    temp = malloc(sizeof(block));
		    temp->type = IF_B;
		    temp->value = (strcmp("true", iter_tok->obj) == 0) ? 1 : 0;
		    slist_push(block_stack, temp);
		    return;
		}
		
		block * temp1 = (block *) slist_get(block_stack,-1)->obj;
		
		temp = malloc(sizeof(block));
		temp->type = IF_B;
		if (temp1->value == 0) {
		    temp->value = 0;
		} else {
		    temp->value = (strcmp("true", iter_tok->obj) == 0) ? 1 : 0;
		}
		slist_push(block_stack, temp);
		
	} else if (strcmp("while", tok->obj) == 0) {
		if ((iter_tok = (token *) slist_pop(out_queue)) == NULL)
			lite_error(desc, "Not enough arguments");

		if (iter_tok->type == SYMBOL_T) {
			if ((sym = fetch_sym(sym_table, iter_tok->obj)) == NULL)
				lite_error(desc, "Symbol doesn't exist");
			else {
				free(iter_tok);
				iter_tok = ((symbol *) sym)->tok;
			}
		}

		if (iter_tok->type != BOOL_T)
			lite_error(desc, "Control flow 'while' operation takes boolean argument");
		if (block_stack->len == 0) {
			temp = malloc(sizeof(block));
			temp->type = WHILE_B;
			temp->value = (strcmp("true", iter_tok->obj) == 0) ? 1 : 0;
			temp->s_offset = tok->s_offset-strlen(tok->obj)-1;
			temp->repeat = 0;
			slist_push(block_stack, temp);
			return;
		}
		
		temp = (block *) slist_get(block_stack,-1)->obj;
		if (temp->type == WHILE_B && temp->repeat == 1) {
			temp->value = (strcmp("true", iter_tok->obj) == 0) ? 1 : 0;
		} else {
			block * temp = malloc(sizeof(block));
			temp->type = WHILE_B;
			temp->value = (strcmp("true", iter_tok->obj) == 0) ? 1 : 0;
			temp->s_offset = tok->s_offset-strlen(tok->obj)-1;
			temp->repeat = 0;
			slist_push(block_stack, temp);
		}
	} else if (strcmp("elif", tok->obj) == 0) {
		temp = (block *) slist_get(block_stack,-1)->obj;

		if (temp->type != IF_B)
			lite_error(desc, "'elif' must exist in 'if' block");

		if ((iter_tok = (token *) slist_pop(out_queue)) == NULL)
			lite_error(desc, "Not enough arguments");

		if (iter_tok->type == SYMBOL_T) {
			if ((sym = fetch_sym(sym_table, iter_tok->obj)) == NULL)
				lite_error(desc, "Symbol doesn't exist");
			else {
				free(iter_tok);
				iter_tok = ((symbol *) sym)->tok;
			}
		}

		if (iter_tok->type != BOOL_T)
			lite_error(desc, "Control flow 'if' operation takes boolean argument");

		temp->value = (strcmp("true", iter_tok->obj) == 0) ? 1 : 0;
	} else if (strcmp("end", tok->obj) == 0) {
		if (block_stack->len == 0) {
		    fseek(desc->file, 1L, SEEK_END);
		    return;
		}
		temp = (block *) slist_get(block_stack,-1)->obj;
		switch (temp->type) {
		case IF_B: case FUNC_B:
			free(slist_pop(block_stack));
			break;
		case WHILE_B:
			if (temp->value == 0) {
			    free(slist_pop(block_stack));
			} else {
			    fseek(desc->file, temp->s_offset, SEEK_SET);
			    temp->repeat = 1;
			}
			break;
		default:
			break;
		}
	}
}

void exec_logic(file_desc * desc, slist * out_queue, token * tok, slist * sym_table) {
	
	token * iter_tok1, * iter_tok2, * res_tok;
	symbol * sym;

	if ((iter_tok2 = (token *) slist_pop(out_queue)) == NULL)
		lite_error(desc, "Not enough arguments");

	if (iter_tok2->type == SYMBOL_T) {
		if ((sym = fetch_sym(sym_table, iter_tok2->obj)) == NULL)
			lite_error(desc, "Symbol doesn't exist");
		else {
			free(iter_tok2);
			iter_tok2 = ((symbol *) sym)->tok;
		}
	}
	
	res_tok = malloc(sizeof(token));
	res_tok->type = BOOL_T;
	res_tok->obj = malloc(sizeof(char *));
	int val = false;

	if (strcmp("not", tok->obj) == 0) {
		if (iter_tok2->type != BOOL_T)
			lite_error(desc, "Logic 'not' operation takes boolean argument");
		if (strcmp("true", iter_tok2->obj) == 0)
			val = true;
		sprintf(res_tok->obj, "%s", (val == true) ? "false" : "true");
		slist_push(out_queue, res_tok);
		return;
	}

	if ((iter_tok1 = (token *) slist_pop(out_queue)) == NULL)
		lite_error(desc, "Not enough arguments");

	if (iter_tok1->type == SYMBOL_T) {
		if ((sym = fetch_sym(sym_table, iter_tok1->obj)) == NULL)
			lite_error(desc, "Symbol doesn't exist");
		else {
			free(iter_tok1);
			iter_tok1 = ((symbol *) sym)->tok;
		}
	}

	if (strcmp("<", tok->obj) == 0) {
		if ((iter_tok1->type != INT_T && iter_tok1->type != DOUBLE_T)
				|| (iter_tok1->type != INT_T && iter_tok1->type
						!= DOUBLE_T))
			lite_error(desc, "Logic 'compare' operation takes numeric arguments");
		if (atof(iter_tok1->obj) < atof(iter_tok2->obj))
			val = true;
	} else if (strcmp(">", tok->obj) == 0) {
		if ((iter_tok1->type != INT_T && iter_tok1->type != DOUBLE_T)
				|| (iter_tok1->type != INT_T && iter_tok1->type
						!= DOUBLE_T))
			lite_error(desc, "Logic 'compare' operation takes numeric arguments");
		if (atof(iter_tok1->obj) > atof(iter_tok2->obj))
			val = true;
	} else if (strcmp("<=", tok->obj) == 0) {
		if ((iter_tok1->type != INT_T && iter_tok1->type != DOUBLE_T)
				|| (iter_tok1->type != INT_T && iter_tok1->type
						!= DOUBLE_T))
			lite_error(desc, "Logic 'compare' operation takes numeric arguments");
		if (atof(iter_tok1->obj) <= atof(iter_tok2->obj))
			val = true;
	} else if (strcmp("=>", tok->obj) == 0) {
		if ((iter_tok1->type != INT_T && iter_tok1->type != DOUBLE_T)
				|| (iter_tok1->type != INT_T && iter_tok1->type
						!= DOUBLE_T))
			lite_error(desc, "Logic 'compare' operation takes numeric arguments");
		if (atof(iter_tok1->obj) >= atof(iter_tok2->obj))
			val = true;
	} else if (strcmp("and", tok->obj) == 0) {
		if (iter_tok1->type != BOOL_T && iter_tok2->type != BOOL_T)
			lite_error(desc, "Logic 'and' operation takes boolean arguments");
		if (strcmp(iter_tok1->obj, iter_tok2->obj) == 0)
			val = true;
	} else if (strcmp("or", tok->obj) == 0) {
		if (iter_tok1->type != BOOL_T && iter_tok2->type != BOOL_T)
			lite_error(desc, "Logic 'or' operation takes boolean arguments");
		if (strcmp("true", iter_tok1->obj) == 0 || strcmp("true",
				iter_tok2->obj) == 0)
			val = true;
	} else if (strcmp("xor", tok->obj) == 0) {
		if (iter_tok1->type != BOOL_T && iter_tok2->type != BOOL_T)
			lite_error(desc, "Logic 'xor' operation takes boolean arguments");
		if (strcmp(iter_tok1->obj, iter_tok2->obj) != 0)
			val = true;
	} else if (strcmp("==", tok->obj) == 0 || strcmp("!=", tok->obj) == 0) {
		if ((iter_tok1->type == INT_T || iter_tok1->type == DOUBLE_T)
				&& (iter_tok2->type != INT_T || iter_tok2->type
						!= DOUBLE_T)) {
			if (strcmp("==", tok->obj) == 0 && atof(iter_tok1->obj)
					== atof(iter_tok2->obj))
				val = true;
			else if (strcmp("!=", tok->obj) == 0 && atof(iter_tok1->obj)
					!= atof(iter_tok2->obj))
				val = true;
		} else if (iter_tok1->type == BOOL_T && iter_tok2->type == BOOL_T) {
			if (strcmp(iter_tok1->obj, iter_tok2->obj) == 0) val = true;
		} else {
			lite_error(desc, "Invalid arguments passed  for logic operation");
		}
	}
	sprintf(res_tok->obj, "%s", (val == true) ? "true" : "false");
	slist_push(out_queue, res_tok);
}

void exec_assign(file_desc * desc, slist * out_queue, token * tok, slist * sym_table) {
	
	token * iter_tok1, * iter_tok2;
	symbol * sym1= NULL;
	symbol * sym2= NULL;

	if ((iter_tok2 = (token *) slist_pop(out_queue)) == NULL)
		lite_error(desc, "Not enough arguments");
	if ((iter_tok1 = (token *) slist_pop(out_queue)) == NULL)
		lite_error(desc, "Not enough arguments");

	if (iter_tok1->type != SYMBOL_T)
		lite_error(desc, "Assignation must be made to a symbol");

	if (iter_tok2->type == SYMBOL_T) {
		if ((sym2 = fetch_sym(sym_table, iter_tok2->obj)) == NULL)
			lite_error(desc, "Symbol doesn't exist");
		else {
			free(iter_tok2);
			iter_tok2 = ((symbol *) sym2)->tok;
		}
	}

	if ((sym1 = fetch_sym(sym_table, iter_tok1->obj)) == NULL) {
		if (strcmp("=", tok->obj) != 0)
			lite_error(desc, "Symbol doesn't exist");
	}

	if (strcmp("=", tok->obj) == 0) {
		if (sym1 != NULL) {
			sym1->tok = iter_tok2;
		} else {
			sym1 = malloc(sizeof(symbol));
			sym1->symbol = iter_tok1->obj;
			sym1->tok = iter_tok2;
			slist_node * temp = (slist_node *) slist_get(sym_table, -1);
			slist * local = (slist *) temp->obj;
			slist_push(local, sym1);
		}
		slist_push(out_queue, iter_tok1);
		return;
	}

	if (strcmp("%=", tok->obj) == 0 && (sym1->tok->type != INT_T
			|| iter_tok2->type != INT_T)) {
		lite_error(desc, "Arithmetic 'modulo' operation is valid only for integers");
	}

	if (strcmp("%=", tok->obj) == 0 || strcmp("/=", tok->obj) == 0) {
		if (strcmp("0", iter_tok2->obj) == 0)
			lite_error(desc, "Division by zero");
	}

	if (strcmp("+=", tok->obj) == 0 || strcmp("-=", tok->obj) == 0 || strcmp(
			"*=", tok->obj) == 0 || strcmp("/=", tok->obj) == 0)
		if ((sym1->tok->type != INT_T && iter_tok2->type != DOUBLE_T)
				|| (sym1->tok->type != INT_T && iter_tok2->type
						!= DOUBLE_T))
			lite_error(desc, 
				"Arithmetic 'modulo' operation is valid only for integers and floating-point numbers");

	if (strcmp("+=", tok->obj) == 0) {
		if (sym1->tok->type == DOUBLE_T || iter_tok2->type == DOUBLE_T) {
			sym1->tok->type = DOUBLE_T;
			sprintf(sym1->tok->obj, "%f", atof(sym1->tok->obj)
					+ atof(iter_tok2->obj));
		} else {
			sym1->tok->type = INT_T;
			sprintf(sym1->tok->obj, "%d", atoi(sym1->tok->obj)
					+ atoi(iter_tok2->obj));
		}
	} else if (strcmp("-=", tok->obj) == 0) {
		if (sym1->tok->type == DOUBLE_T || iter_tok2->type == DOUBLE_T) {
			sym1->tok->type = DOUBLE_T;
			sprintf(sym1->tok->obj, "%f", atof(sym1->tok->obj)
					- atof(iter_tok2->obj));
		} else {
			sym1->tok->type = INT_T;
			sprintf(sym1->tok->obj, "%d", atoi(sym1->tok->obj)
					- atoi(iter_tok2->obj));
		}
	} else if (strcmp("*=", tok->obj) == 0) {
		if (sym1->tok->type == DOUBLE_T || iter_tok2->type == DOUBLE_T) {
			sym1->tok->type = DOUBLE_T;
			sprintf(sym1->tok->obj, "%f", atof(sym1->tok->obj)
					* atof(iter_tok2->obj));
		} else {
			sym1->tok->type = INT_T;
			sprintf(sym1->tok->obj, "%d", atoi(sym1->tok->obj)
					* atoi(iter_tok2->obj));
		}
	} else if (strcmp("/=", tok->obj) == 0) {
		if (sym1->tok->type == DOUBLE_T || iter_tok2->type == DOUBLE_T) {
			sym1->tok->type = DOUBLE_T;
			sprintf(sym1->tok->obj, "%f", atof(sym1->tok->obj)
					/ atof(iter_tok2->obj));
		} else {
			sym1->tok->type = INT_T;
			sprintf(sym1->tok->obj, "%d", atoi(sym1->tok->obj)
					/ atoi(iter_tok2->obj));
		}
	} else if (strcmp("%=", tok->obj) == 0) {
		sym1->tok->type = INT_T;
		sprintf(sym1->tok->obj, "%d", atoi(sym1->tok->obj)
				% atoi(iter_tok2->obj));
	}
	slist_push(out_queue, iter_tok1);
}

void exec_func(file_desc * desc, slist * out_queue, token * tok, slist * sym_table, slist * block_stack, token ** ret) {
	
	token * iter_tok;
	symbol * iter_sym;
	
	if (strcmp("print", tok->obj) == 0) {
		while ((iter_tok = slist_dequeue(out_queue)) != NULL) {
			if (iter_tok->type == SYMBOL_T) {
				if ((iter_sym = fetch_sym(sym_table, iter_tok->obj)) == NULL)
					lite_error(desc, "Symbol doesn't exist");
				iter_tok = iter_sym->tok;
			}
			printf("%s", iter_tok->obj);
		}
		printf("\n");
	} else if (strcmp("input", tok->obj) == 0) {
		char * temp_str = malloc(sizeof(char)*MAX_STR);
		int temp_size= MAX_STR;
		getline(&temp_str, &temp_size, stdin);
		token * temp = malloc(sizeof(token));
		temp->type = STR_T;
		temp->obj = temp_str;
		slist_push(out_queue, temp);
	} else if (strcmp("list", tok->obj) == 0) {
		slist * list = slist_init();
		token * temp = (token *) slist_dequeue(out_queue);
		while (temp != NULL) {
			// TODO: Make list cpy
			slist_enqueue(list, temp);
			temp = (token *) slist_dequeue(out_queue);
		}
	} else if (strcmp("include", tok->obj) == 0) {
		if ((iter_tok = slist_dequeue(out_queue)) != NULL) {
			if (iter_tok->type == SYMBOL_T) {
				if ((iter_sym = fetch_sym(sym_table, iter_tok->obj)) == NULL)
					lite_error(desc, "Symbol doesn't exist");
				iter_tok = iter_sym->tok;
			}
			FILE * temp_file = fopen(iter_tok->obj, "r");
			liten(temp_file, iter_tok->obj, sym_table, ret);
			fclose(temp_file);
		} else {
			lite_error(desc, "Module to be included is not specified");
		}
	} else if (strcmp("return", tok->obj) == 0) {
		if ((iter_tok = slist_dequeue(out_queue)) == NULL) {
		    lite_error("Returns nothing");
		}
		if (iter_tok->type == SYMBOL_T) {
			if ((iter_sym = fetch_sym(sym_table, iter_tok->obj)) == NULL)
				lite_error(desc, "Symbol doesn't exist");
			iter_tok = iter_sym->tok;
		}
		*ret = iter_tok;
		fseek(desc->file, 1L, SEEK_END);
	} else if (strcmp("func", tok->obj) == 0) {
		block * func_block = malloc(sizeof(block));
		func_block->value = 0;
		func_block->type = FUNC_B;
		slist_push(block_stack, func_block);
		slist * func_params = slist_init();
		
		token * temp;
		int param = tok->num_params;
		while (param > 0) {
			temp = (token *) slist_pop(out_queue);
			if (temp == NULL) lite_error(desc, "Not enough parameters");
			if (temp->type != SYMBOL_T) lite_error(desc, "Invalid function argument");
			iter_sym = malloc(sizeof(block));
			iter_sym->symbol = temp->obj;
			slist_push(func_params, iter_sym);
			param--;
		}
		
		tok->s_offset = ftell(desc->file);
		tok->params = func_params;
		tok->desc = desc;
		slist_push(out_queue, tok);
	} else {
		if ((iter_sym = fetch_sym(sym_table, tok->obj)) == NULL)
			lite_error(desc, "Symbol doesn't exist");
		tok = iter_sym->tok;
		FILE * func_file = fopen(tok->desc->name, "r");
		slist * func_params = tok->params;
		symbol * iter_sym;
		token * temp;
		int param = tok->num_params;
		while (param > 0) {
			temp = (token *) slist_pop(out_queue);
			if (temp == NULL) lite_error(desc, "Not enough parameters");
			if (temp->type == SYMBOL_T) {
				if ((iter_sym = fetch_sym(sym_table, temp->obj)) == NULL)
					lite_error(desc, "Symbol doesn't exist");
				temp = iter_sym->tok;
			}
			iter_sym = (symbol *) slist_get(func_params, param-1)->obj;
			iter_sym->tok = temp;
			param--;
		}
		slist_push(sym_table, func_params);
		fseek(func_file, tok->s_offset, SEEK_SET);
		token * temp_ret;
		liten(func_file, tok->desc->name, sym_table, &temp_ret);
		fclose(func_file);
		if (temp_ret != NULL) {
		    slist_push(out_queue, temp_ret);
		}
	}
}

void exec(file_desc * desc, slist * out_queue, token * tok, slist * sym_table, slist * block_stack, token ** ret) {
	switch (tok->type) {
	case OP_T:
		exec_op(desc, out_queue, tok, sym_table);
		break;
	case BITOP_T:
		exec_bitop(desc, out_queue, tok, sym_table);
		break;
	case LOGIC_T:
		exec_logic(desc, out_queue, tok, sym_table);
		break;
	case ASSIGN_T:
		exec_assign(desc, out_queue, tok, sym_table);
		break;
	case FUNC_T:
		exec_func(desc, out_queue, tok, sym_table, block_stack, ret);
		break;
	case FLOW_T:
		exec_flow(desc, out_queue, tok, sym_table, block_stack);
		break;
	default:
		break;
	}
}

// Transforms to Reverse Polish Notation while 
// implementing Djikstra's Shunting-yard algorithm
// for execution
void rpn(file_desc * desc, slist * tok_queue, slist * sym_table, slist * block_stack, token ** ret) {

	slist * op_stack = slist_init();
	slist * out_queue = slist_init();
	slist * param_counter = slist_init();
	
	slist_node * iter_node;
	token * iter_tok;

	token * tok = (token *) slist_dequeue(tok_queue);
	int tok_type;
	int * count_param;
	while (tok != NULL) {
		tok_type = token_type(desc, tok);
		if (tok_type == Operand) 		// Operand
			slist_push(out_queue, tok);
		else if (tok_type == Operator) { 	// Operator
			iter_node = slist_get(op_stack, -1);
			iter_tok = (iter_node != NULL) ? (token *) iter_node->obj : NULL;
			if (iter_tok != NULL && token_type(desc, iter_tok) == Operator
					&& op_prec(desc, tok, iter_tok) <= 0) {
				iter_tok = (token *) slist_pop(op_stack);
				if (iter_tok != NULL)
					exec(desc, out_queue, iter_tok, sym_table, block_stack, ret);
			}
			slist_push(op_stack, tok);
		} else if (tok_type == Function) {	// Function
			slist_push(op_stack, tok);
			iter_node = slist_get(tok_queue, 1);
			iter_tok = (iter_node != NULL) ? (token *) iter_node->obj : NULL;
			if (iter_tok == NULL) lite_error(desc, "Syntax error");
			
			count_param = malloc(sizeof(int));
			*count_param = 1;
			if (token_type(desc, iter_tok) == RightParen) *count_param = 0;			
			slist_push(param_counter, count_param);
		} else if (tok_type == Comma) { 	// Comma
			iter_node = slist_get(op_stack, -1);
			iter_tok = (iter_node != NULL) ? (token *) iter_node->obj : NULL;
			while (iter_tok != NULL && token_type(desc, iter_tok) != LeftParen) {
				tok = (token *) slist_pop(op_stack);
				exec(desc, out_queue, iter_tok, sym_table, block_stack, ret);
				iter_node = slist_get(op_stack, -1);
				iter_tok = (iter_node != NULL) ? (token *) iter_node->obj
						: NULL;
			}
			if (token_type(desc, iter_tok) != LeftParen) {
				free(slist_pop(op_stack));
				lite_error(desc, "Syntax error: Parentheses don't match");
			}
			count_param = (int *) slist_get(param_counter, -1)->obj;
			*count_param = *count_param + 1;
		} else if (tok_type == LeftParen) {	// Left parenthesis
			slist_push(op_stack, tok);
		} else if (tok_type == RightParen) { 	// Right parenthesis
			iter_node = slist_get(op_stack, -1);
			iter_tok = (iter_node != NULL) ? (token *) iter_node->obj : NULL;
			while (iter_tok != NULL && token_type(desc, iter_tok) != LeftParen) {
				tok = (token *) slist_pop(op_stack);
				exec(desc, out_queue, iter_tok, sym_table, block_stack, ret);
				iter_node = slist_get(op_stack, -1);
				iter_tok = (iter_node != NULL) ? (token *) iter_node->obj
						: NULL;
			}
			if (token_type(desc, iter_tok) != LeftParen) {
				free(slist_pop(op_stack));
				lite_error(desc, "Syntax error: Parentheses don't match");
			}
			free(slist_pop(op_stack));
			iter_node = slist_get(op_stack, -1);
			iter_tok = (iter_node != NULL) ? (token *) iter_node->obj : NULL;
			if (token_type(desc, iter_tok) == Function) {
				count_param = (int *) slist_pop(param_counter);
				iter_tok = slist_pop(op_stack);
				iter_tok->num_params = *count_param;
				exec(desc, out_queue, iter_tok, sym_table, block_stack, ret);
			} else {
				free(slist_pop(param_counter));
			}
		}
		tok = (token *) slist_dequeue(tok_queue);
	}
	iter_node = slist_get(op_stack, -1);
	iter_tok = (iter_node != NULL) ? (token *) iter_node->obj : NULL;
	if (iter_tok != NULL && token_type(desc, iter_tok) == LeftParen)
		lite_error(desc, "Syntax error: Parentheses don't match");
	tok = (token *) slist_pop(op_stack);
	while (tok != NULL) {
		exec(desc, out_queue, tok, sym_table, block_stack, ret);
		tok = (token *) slist_pop(op_stack);
	}
}