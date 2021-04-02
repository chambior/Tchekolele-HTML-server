#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct_arbre.h"

Branch* new_branch(char* data, int data_size, char* tag){
    Branch* branch = (Branch*) malloc(sizeof(Branch));
    branch->tag = (char*) malloc(strlen(tag));
    strcpy(branch->tag, tag);
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

void freetree(Branch** b){
  if(*b){
    if((*b)->branches) freeall((*b)->branches);
    if((*b)->tag) free((*b)->tag);
    free((*b));
    *b = NULL;
  }
}

void displaytree(Branch* t, int n){
    for(int i = 0; i < n; i++) printf("  ");
    printf("%d-%s: %*.*s\n", n, t->tag, t->data_size, t->data_size, t->data);
    BranchList* bl = t->branches;
    while(bl){
        displaytree(bl->branch, n+1);
        bl = bl->next;
    }
}


Branch** addBranch(BranchList** sub_branches){
    if(!(*sub_branches)){
        (*sub_branches) = new_branchlist();
        return &((*sub_branches)->branch);
    }
    else if((*sub_branches)->branch){
    	BranchList* sb = (*sub_branches);
    	while(sb->next) sb = sb->next;
    	sb->next = new_branchlist();
    	return &(sb->next->branch);
    }
    else return &((*sub_branches)->branch);
}

void freeLast(BranchList** sub_branches){
    BranchList* sb = *sub_branches;
    if(sb->next){
        while(sb->next->next) sb = sb->next;
        freeall(sb->next);
        sb->next = NULL;
    }
    else{
        freeall(sb);
        *sub_branches = NULL;
    }
}
