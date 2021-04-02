typedef struct Branch Branch;
typedef struct BranchList BranchList;

struct Branch {
  char* data;//pointeur vers la donnée
  int data_size;//nombre de caracteres de la donnée
  char* tag;//Etiquette
  BranchList* branches;//Liste chainée des branches fils
};

struct BranchList {
  BranchList* next;//Elément suivant de la liste des fils
  Branch* branch;//Fils pointé par cette case de la liste des fils
};

Branch* new_branch(char* data, int data_size, char* tag);

BranchList* new_branchlist();

void freeall(BranchList* list);

void freetree(Branch** b);

void displaytree(Branch* t, int n);

Branch** addBranch(BranchList** sub_branches);

void freeLast(BranchList** sub_branches);
