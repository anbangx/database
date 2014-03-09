
#ifndef _ix_h_
#define _ix_h_

#include <vector>
#include <string>
#include <algorithm>
#include <map>

#include "../pf/pf.h"
#include "../rm/rm.h"

# define IX_EOF (-1)  // end of the index scan

using namespace std;

template <class KeyType>
class leafNode{
public:
	int sizeOfMap;
	map<KeyType,int> myMap;
	int nextLeaf;
	int d;
};

template <class KeyType>
class stemNode{
public:
	int sizeOfMap;
	map<KeyType,int> idxMap;
	int end;
	int d;
protected:

};

class vectorOfRidNode{
public:
	vector<RID> v;
	int size;
	int nextPage;
};

class IX_IndexHandle;

class IX_Manager {
 public:

	int index;
	vector<int> vRealRoot;
	vector<AttrType> vRealType;
	map<string,int> mapRoot;
	int realRoot;
	AttrType realType;

  static IX_Manager* Instance();

  RC CreateIndex(const string tableName,       // create new index
		 const string attributeName);
  RC DestroyIndex(const string tableName,      // destroy an index
		  const string attributeName);
  RC OpenIndex(const string tableName,         // open an index
	       const string attributeName,
	       IX_IndexHandle &indexHandle);
  RC CloseIndex(IX_IndexHandle &indexHandle);  // close index
  
 protected:
  IX_Manager   ();                             // Constructor
  ~IX_Manager  ();                             // Destructor
 
 private:
  static IX_Manager *_ix_manager;

};


class IX_IndexHandle {
 public:
	int depth;
	int midValue;
	float midValue_float;
	string midValue_string;
	int nextRoot;
	int newRoot;
	int nowRoot;
	int root;


	int realRoot;
	AttrType realType;

  IX_IndexHandle  ();                           // Constructor
  ~IX_IndexHandle ();                           // Destructor

  // The following two functions are using the following format for the passed key value.
  //  1) data is a concatenation of values of the attributes
  //  2) For int and real: use 4 bytes to store the value;
  //     For varchar: use 4 bytes to store the length of characters, then store the actual characters.
  RC InsertEntry(void *key, const RID &rid);  // Insert new index entry
  RC DeleteEntry(void *key, const RID &rid);  // Delete index entry

  void insertIX(int root, void *key, const RID &rid, int *newchildentry);
  void insertIX_float(int node, void *key, const RID &rid, int *newchildentry);
  void insertIX_string(int node, void *key, const RID &rid, int *newchildentry);
  void deleteIX(int *parent, int node, void *key, const RID &rid, int *oldchildentry);
  void deleteIX_float(int *parent, int node, void *key, const RID &rid, int *oldchildentry);
  void deleteIX_string(int *parent, int node, void *key, const RID &rid, int *oldchildentry);

  void removeOldchildentryFromNode(int node, int oldchildentry);// no change
  void replaceKeyValue(int parent, int sibling);// no change
  void findLeafSibling(int parent, int node, int *sibling); //no change
  void findStemSibling(int parent, int node, int *sibling); // no change
  void redistributeNS_Leaf(int node, int sibling); // no change
  void redistributeNS_Stem(int node, int sibling, int parent); //no change
  void mergeLeafNodes(int node, int sibling); // no change
  void mergeStemNodes(int node, int sibling, int parent); // no change

  void removeOldchildentryFromNode_f(int node, int oldchildentry);// no change
  void replaceKeyValue_f(int parent, int sibling);// no change
  void findLeafSibling_f(int parent, int node, int *sibling); //no change
  void findStemSibling_f(int parent, int node, int *sibling); // no change
  void redistributeNS_Leaf_f(int node, int sibling); // no change
  void redistributeNS_Stem_f(int node, int sibling, int parent); //no change
  void mergeLeafNodes_f(int node, int sibling); // no change
  void mergeStemNodes_f(int node, int sibling, int parent); // no change

  void removeOldchildentryFromNode_s(int node, int oldchildentry);// no change
  void replaceKeyValue_s(int parent, int sibling);// no change
  void findLeafSibling_s(int parent, int node, int *sibling); //no change
  void findStemSibling_s(int parent, int node, int *sibling); // no change
  void redistributeNS_Leaf_s(int node, int sibling); // no change
  void redistributeNS_Stem_s(int node, int sibling, int parent); //no change
  void mergeLeafNodes_s(int node, int sibling); // no change
  void mergeStemNodes_s(int node, int sibling, int parent); // no change

  map<int,int> insertRidToVectorOfRid(map<int,int> myMap, int key, RID rid);
  map<float,int> insertRidToVectorOfRid(map<float,int> myMap, float key, RID rid);
  map<string,int> insertRidToVectorOfRid(map<string,int> myMap, string key, RID rid);

  RID readRidFromVectorOfRid(map<int,int>::iterator it, int pageOfVector, int n);
  RID readRidFromVectorOfRid(map<float,int>::iterator it, int pageOfVector, int n);
  RID readRidFromVectorOfRid(map<string,int>::iterator it, int pageOfVector, int n);

  void createNewVectorNode(vector<RID> v, int nextPage, vectorOfRidNode *vNode);
  int addToVectorNodePage(vectorOfRidNode *v);
  void writeToVectorNodePage(vectorOfRidNode * vNode, int pageNum);
  void readFromVectorNodePage(vectorOfRidNode *vNode, int pageNum);
  void readVectorFromMem(vectorOfRidNode * vNode,void* page);
  void writeVectorNodeToPage(vectorOfRidNode *vNode, void* page);

  void createNewLeafNode(map<int,int> myMap, int nextLeaf, int d,leafNode<int> *newchildNode);
  void createNewLeafNode(map<float,int> myMap, int nextLeaf, int d,leafNode<float> *newchildNode);
  void createNewLeafNode(map<string,int> myMap, int nextLeaf, int d,leafNode<string> *newchildNode);

  void createNewStemNode(map<int,int> idxMap, int end, int d,stemNode<int> *newchildNode);
  void createNewStemNode(map<float,int> idxMap, int end, int d,stemNode<float> *newchildNode);
  void createNewStemNode(map<string,int> idxMap, int end, int d,stemNode<string> *newchildNode);

  RC removeEntryFromLeaf(int *key,int *node);
  RC removeEntryFromLeaf(float *key, int *node);
  RC removeEntryFromLeaf(string *key, int *node);

  void writeLeafToMem(leafNode<int> * lNode,void* page);
  void writeLeafToMem(leafNode<float> * lNode,void* page);
  void writeLeafToMem(leafNode<string> * lNode,void* page);

  void writeStemToMem(stemNode<int> * sNode,void* page);
  void writeStemToMem(stemNode<float> * sNode,void* page);
  void writeStemToMem(stemNode<string> * sNode,void* page);

  void readLeafFromMem(leafNode<int> * lNode,void* page);
  void readLeafFromMem(leafNode<float> * lNode,void* page);
  void readLeafFromMem(leafNode<string> * lNode,void* page);

  void readStemFromMem(stemNode<int> * sNode,void* page);
  void readStemFromMem(stemNode<float> * sNode,void* page);
  void readStemFromMem(stemNode<string> * sNode,void* page);

  void createNewNode(int node, int newchildentry,int midValue,  int *newNode);
  void createNewNode(int node, int newchildentry,float midValue,  int *newNode);
  void createNewNode(int node, int newchildentry, string midValue, int *newNode);

  void splitLeafNode(int node, int *midValue, int *newchildentry);
  void splitLeafNode(int node, float *midValue, int *newchildentry);
  void splitLeafNode(int node, string *midValue, int *newchildentry);

  void splitStemNode(int node, int *midValue, int *newchildentry);
  void splitStemNode(int node, float *midValue, int *newchildentry);
  void splitStemNode(int node, string *midValue, int *newchildentry);

  void getFirstValue(int pageNum, int *value);
  void getFirstValue(int pageNum, float *value);
  void getFirstValue(int pageNum, string *value);

  int addToLeafNodePage(leafNode<int> * lNode);
  int addToLeafNodePage(leafNode<float> * lNode);
  int addToLeafNodePage(leafNode<string> * lNode);

  int addToStemNodePage(stemNode<int> *stemNode);
  int addToStemNodePage(stemNode<float> *stemNode);
  int addToStemNodePage(stemNode<string> *sNode);

  void writeToLeafNodePage(leafNode<int> * lNode,int pageNum);
  void writeToLeafNodePage(leafNode<float> * lNode,int pageNum);
  void writeToLeafNodePage(leafNode<string> * lNode, int pageNum);

  void writeToStemNodePage(stemNode<int> *stemNode,  int pageNum);
  void writeToStemNodePage(stemNode<float> *stemNode,  int pageNum);
  void writeToStemNodePage(stemNode<string> *sNode, int pageNum);

  void readFromLeafNodePage(leafNode<int> * lNode, int pageNum);
  void readFromLeafNodePage(leafNode<float> * lNode, int pageNum);
  void readFromLeafNodePage(leafNode<string> * lNode, int pageNum);

  void readFromStemNodePage(stemNode<int> *stemNode, int pageNum);
  void readFromStemNodePage(stemNode<float> *stemNode, int pageNum);
  void readFromStemNodePage(stemNode<string> *sNode, int pageNum);

  int getRoot();

  RC ifNextDuplicate(map<int,int> myMap, int key, int pageOfVector, int n);
  RC ifNextDuplicate(map<float,int> myMap, float key, int pageOfVector, int n);
  RC ifNextDuplicate(map<string,int> myMap, string key, int pageOfVector, int n);

  int getNumOfVectorPage(map<int,int>::iterator it);
  int getNumOfVectorPage(map<float,int>::iterator it);
  int getNumOfVectorPage(map<string,int>::iterator it);

  int getNumOfVector(map<int,int>::iterator it, int pageOfVector);
  int getNumOfVector(map<float,int>::iterator it, int pageOfVector);
  int getNumOfVector(map<string,int>::iterator it, int pageOfVector);

  PF_FileHandle readfHandle();
  void setfHandle(PF_FileHandle pfile);
  string readfName();
  void setfName(string fileName);
  string readtName();
  void settName(string tableName);
  AttrType type;
  PF_FileHandle pf_fileHandle;

 private:


  string fileName;
  string tableName;


};


class IX_IndexScan {
 public:
  IX_IndexScan();  								// Constructor
  ~IX_IndexScan(); 								// Destructor

  // for the format of "value", please see IX_IndexHandle::InsertEntry()
  RC OpenScan(const IX_IndexHandle &indexHandle, // Initialize index scan
	      CompOp      compOp,
	      void        *value);           

  RC GetNextEntry(RID &rid);  // Get next matching entry
  RC CloseScan();             // Terminate index scan

  RC GetNextEntry_Int(RID &rid);
  RC GetNextEntry_float(RID &rid);
  RC GetNextEntry_String(RID &rid);

  int locate(int *node);
  int locate_float(int *node);
  int locate_string(int *node);

  IX_IndexHandle getIdxHandle();
  void setIdxHandle(IX_IndexHandle indexHandle);
  CompOp getCompOp();
  void setCompOp(CompOp compOp);
  int getValue();
  void setValue(int cmpValue);
  float getfValue();
  void setfValue(float val_float);

 protected:
 // IX_IndexHandle &indexHandle;
  CompOp compOp;
  int val;
  float val_f;
  string val_s;
  IX_IndexHandle idxHandle;
  int currentPage;
  bool isVirgin;
  int currentKey;
  int nextKey;
  float nextKey_f;
  float currentKey_f;
  string currentKey_s;
  string nextKey_s;

  int pageOfVector;
  int n;


};

// print out the error message for a given return code
void IX_PrintError (RC rc);


#endif
