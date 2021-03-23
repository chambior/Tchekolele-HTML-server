
#include "api_sub_functions.h"

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

  if(strncmp(branch->tag, name, strlen(name))){
    result = malloc(sizeof(_Token));
    result->node = branch;
    result->next = searchSubTrees(branch->branches, name);
  }
  else result = searchSubTrees(branch->branches, name);

  return result;
}

char *getElementTag(void *node,int *len){
  Branch* branch = (Branch*) node;
  *len = strlen(branch->tag);
  return branch->tag;
}


//QUESTION POUR LE PROF:
/*
On nous a dit de ne pas recopier les données. Donc on ne passe qu'un pointeur
vers le début de la chaine de caractères DANS la requête complète
Donc on a aucun marqueur pour la FIN de la chaine, alors comment retourner len ?

On a rajouté un champ "data_size" dans la structure d'un noeud en attendant
*/
char *getElementValue(void *node,int *len){
  Branch* branch = (Branch*) node;
  *len = branch->data_size;
  return branch->data;
}


//Pourquoi y'a un ** ?
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

//Mdr on verra + tard
int parseur(char *req, int len);
