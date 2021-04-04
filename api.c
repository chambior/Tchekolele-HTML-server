#include "api_sub_functions.h"
#include <stdio.h>

void *getRootTree(){
  extern void * root;
  return root;
}



_Token* searchSubTrees(BranchList* branches, char* name){
  _Token * result;
  _Token * sub_result;
  if(branches==NULL) result = NULL;
  else if(branches->next == NULL){
    result = searchTree(branches->branch, name);
  }
  else{
    result = searchTree(branches->branch, name);
    sub_result = result;
    if(result == NULL) result = searchSubTrees(branches->next, name);
    else{
      while(sub_result->next != NULL) sub_result->next = sub_result;
      sub_result->next = searchSubTrees(branches->next, name);
    }
  }
  return result;
}

_Token *searchTree(void *start,char *name){
  _Token * result;
  Branch * branch;

  if(start == NULL) branch = (Branch *) getRootTree();
  else branch = (Branch *) start;

  //printf("name %s\n", name);//debug
  //printf("branch->tag %s\n", branch->tag);//debug

  if(strncmp(branch->tag, name, strlen(name))==0){
    result = malloc(sizeof(_Token));
    result->node = branch;
    result->next = searchSubTrees(branch->branches, name);
  }
  else result = searchSubTrees(branch->branches, name);
  //printf("api %x\n", result);
  return result;
}

char *getElementTag(void *node,int *len){
  Branch* branch = (Branch*) node;
  *len = strlen(branch->tag);
  return branch->tag;
}

char *getElementValue(void *node,int *len){
  Branch* branch = (Branch*) node;
  *len = branch->data_size;
  return branch->data;
}


void purgeElement(_Token **r){
  subPurgeElement(*r);
  *r = NULL;
}

void subPurgeElement(_Token *r){
  if(r->next != NULL){
    subPurgeElement(r->next);
  }
  free(r);
}

void purgeTree(void *root){
  Branch* branch = (Branch*) root;
  purgeSubTrees(branch->branches);
  free(branch);
}

void purgeSubTrees(BranchList* branches){
  if(branches!=NULL){
    purgeTree(branches->branch);
    purgeSubTrees(branches->next);
    free(branches);
  }
}

int parseur(char *req, int len){
    printf("parsing de la requête %s\n", req);
    Branch* branch = getRootTree();


    branch = new_branch(req, 0, "parseur");
	BranchList* sub_branches = new_branchlist();

    int i = HTTP_message(req, addBranch(&sub_branches));
	if(!i) freeLast(&sub_branches);

    branch->branches = sub_branches;
	branch->data_size = i;
    displaytree(branch, 0);
    printf("Fin displaytree sans erreur\n");

    extern void * root;
    root = branch;

	return i;
}

void displayElement(_Token *r){
    while(r){
        printf("%s: %*.*s\n",
        ((Branch*)(r->node))->tag,
        ((Branch*)(r->node))->data_size,
        ((Branch*)(r->node))->data_size,
        ((Branch*)(r->node))->data);
        r = r->next;
    }
}
