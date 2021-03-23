#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct_arbre.h"

Branch* new_branch(char* data, int data_size, char* tag){
  Branch* branch = (Branch*) malloc(sizeof(Branch));

  branch->tag = (char*) malloc(strlen(tag));
  strcpy(tag, branch->tag);

  branch->branches = NULL;
  branch->data = data;
  branch->data_size = data_size;
  return branch;
}

BranchList* new_branchlist(){
  BranchList* branchlist = (BranchList*) malloc(sizeof(BranchList));
  branchlist->branch = NULL;
  branchlist->next = NULL;
  return branchlist;
}
