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
  printf("branch1\n");
  BranchList* branchlist = (BranchList*) malloc(sizeof(BranchList));
  printf("branch2\n");
  branchlist->branch = NULL;
  printf("branch3\n");
  branchlist->next = NULL;
  return branchlist;
}

void freeall(BranchList* list){
  while(list){
    if(list->branch){
      if(list->branch->tag) free(list->branch->tag);
      if(list->branch->branches) freeall(list->branch->branches);
    }
    BranchList* oldlist = list;
    list = list->next;
    free(oldlist);
  }
}

void freetree(Branch* b){
  if(b){
    if(b->branches) freeall(b->branches);
    if(b->tag) freeall(b->tag);
    free(b);
  }
}
