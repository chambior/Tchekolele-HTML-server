#include "api.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>

//Retourne la concaténation des _Token résultants de searchTree
//de toutes les branches de la BranchList en paramètres
_Token* searchSubTrees(BranchList* branches, char* name);
void subPurgeElement(_Token *r);
void purgeSubTrees(BranchList* branches);
void displayElement(_Token *r);
