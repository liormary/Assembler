/*
 * label_help.h
 * Header file for label_help.c
 */

#ifndef LABEL_HELP_H_
#define LABEL_HELP_H_

int check_for_label(char**,symbol_p*,int);
void save_label(char**,symbol_p * ,int,int);
void save_label_in_external_table (external_ptr *,char *,int);
int exists_label(char * ,symbol_p  ,int ,int,int);
int compare_reserved_words(char*);
int reserved_words_check(char *,int);
int is_legal_label(char *,symbol_p *,int,int);
int get_label_length(char *);
int updt_symb_tab_with_entry(char **,symbol_p sp,int);
void skip_on_label(char **);
void save_label_diff_code_in_immed(char **,int,comm_ptr,symbol_p ,int);

#endif /* LABEL_HELP_H_ */
