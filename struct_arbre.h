typedef struct Branch Branch;
typedef struct BranchList BranchList;

struct Branch {
  char* data;
  int data_size;
  char* tag;
  BranchList* branches;
};

struct BranchList {
  BranchList* next;
  Branch* branch;
};

Branch* new_branch(char* data, int data_size, char* tag);

BranchList* new_branchlist();

void freeall(BranchList* list);

void freetree(Branch* b);
