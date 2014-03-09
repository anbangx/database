
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <map>
#include <sys/stat.h>
#include <fstream>
#include <iostream>

#include "ix.h"

using namespace std;

IX_Manager* IX_Manager::_ix_manager = 0;
//IX_Manager *_ix_manager = IX_Manager::Instance();
PF_Manager *pf = PF_Manager::Instance();
RM *rm1 = RM::Instance();
int D = 4;

IX_Manager* IX_Manager::Instance()
{
    if(!_ix_manager)
    _ix_manager = new IX_Manager();

    return _ix_manager;
}

IX_Manager::IX_Manager()
{
	index = -1;
	realRoot = -1;
	realType = TypeInt;
}

IX_Manager::~IX_Manager()
{

}

RC IX_Manager::CreateIndex(const string tableName,const string attributeName)
{
	index++;
	string name = tableName +"_idx_" + attributeName;
	mapRoot.insert(pair<string,int>(name,index));
	vRealRoot.push_back(-1);
	vRealType.push_back(TypeInt);
	//realRoot = -1;
	return pf->CreateFile(name.c_str());
}

RC IX_Manager::DestroyIndex(const string tableName,const string attributeName)
{
	vRealRoot[index] = -1;
	vRealType[index] = TypeInt;
	//realRoot = -1;
	//realType = TypeInt;

	string name = tableName +"_idx_" + attributeName;
	return pf->DestroyFile(name.c_str());


}

RC IX_Manager::OpenIndex(const string tableName,const string attributeName,IX_IndexHandle &indexHandle)
{
	PF_FileHandle pf_fileHandle;
	string name = tableName +"_idx_" + attributeName;
	if(pf->OpenFile(name.c_str(),pf_fileHandle)!=0)
			return -1;

	map<string,int>::iterator it = mapRoot.find(name);
	index = (*it).second;

	indexHandle.type = rm1->getTypeFromAttrName(tableName,attributeName);
	indexHandle.setfHandle(pf_fileHandle);

	indexHandle.realRoot = vRealRoot[index]; //realRoot;
	indexHandle.root = vRealRoot[index];
	indexHandle.realType = vRealType[index];

	return 0;
}

RC IX_Manager::CloseIndex(IX_IndexHandle &indexHandle)
{
	PF_FileHandle fileHandle = indexHandle.readfHandle();
	pf->CloseFile(fileHandle);

	vRealRoot[index] = indexHandle.getRoot(); //realRoot
	vRealType[index] = indexHandle.type;
	//realType = indexHandle.type;
	return 0;
}

IX_IndexHandle::IX_IndexHandle()
{
	pf_fileHandle.file = NULL;//pf_fileHandle.setFile(0);
	fileName = "";
	tableName = "";
	root = -1;
	type = TypeInt;

	depth=0;
	midValue = -1;
	midValue_float = -1;
	midValue_string = "";
	nextRoot = -1;
	newRoot = -1;
	nowRoot = -1;

	realRoot = -1;
	realType = TypeInt;
}

IX_IndexHandle::~IX_IndexHandle()
{
	depth=0;
	midValue = -1;
	midValue_float = -1;
	midValue_string = "";
	nextRoot = -1;
	newRoot = -1;
	nowRoot = -1;

}

RC IX_IndexHandle::InsertEntry(void *key, const RID &rid)
{
	//check key type
	void* typedKey;
	switch(this->type){
	case TypeInt:
		typedKey = new int;
		*(int*)typedKey = *(int*)key;

		//root is empty
		if(root == -1){


			leafNode<int> *lNode = new leafNode<int>();

			lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, *(int*)typedKey,rid);
			lNode->nextLeaf = -1;
			lNode->d = D;
			lNode->sizeOfMap = 1;

			//write this node to page file
			int page = this->addToLeafNodePage(lNode);
			root = page;
			newRoot = page;

			delete lNode;

			return 0;
		}

		//Depth = 0
		if(depth==0){
			leafNode<int> *lNode = new leafNode<int>();

			//read node from page file
			this->readFromLeafNodePage(lNode,root);

			lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, *(int*)typedKey,rid);
			this->writeToLeafNodePage(lNode,root);
			this->readFromLeafNodePage(lNode,root);

			//If L has space
			if(lNode->myMap.size()<=lNode->d){
				delete lNode;
				return 0;
			}


			//Once in a while, the leaf is full
			else{
				leafNode<int> *newchildNode = new leafNode<int>();

				//split L: first d entries stay, rest move to new node L2
				map<int,int>::iterator it = lNode->myMap.begin();
				for(int i = 0; i < (lNode->d)/2; i++)
					it++;

				int keyValue = (*it).first;
				map<int,int> tempLeafMap;
				for (; it != lNode->myMap.end(); it++ )
					tempLeafMap.insert(pair<int,int>((*it).first,(*it).second));

				//createNewLeafNode parameter: myMap, nextLeaf, d, newchildNode
				this->createNewLeafNode(tempLeafMap,-1,D,newchildNode);

				//write to file
				int newpageNum = this->addToLeafNodePage(newchildNode);

				it=lNode->myMap.begin();
				for(int i = 0; i < (lNode->d)/2; i++)
					it++;
				lNode->myMap.erase(it,lNode->myMap.end());

				//set sibling pointer in L2
				this->createNewLeafNode(lNode->myMap,newpageNum,D,lNode);
				this->writeToLeafNodePage(lNode, root);

				//make new node with (pointer to L, *newchildNode)
				stemNode<int> *sNode = new stemNode<int>();
				map<int,int> tempStemMap;
				tempStemMap.insert(pair<int,int>(keyValue,root));
				this->createNewStemNode(tempStemMap,newpageNum,D,sNode);

				//write to file
				int pageNum = this->addToStemNodePage(sNode);

				//make the tree's root-node pointer point to the new node
				root = pageNum;
				depth = 1;

				delete newchildNode;
				delete lNode;
				delete sNode;
				return 0;
			}
		}
		else{
			int newchildentry = -1;
			this->insertIX(root,key,rid,&newchildentry);

			return 0;
		}
		return 0;
	case TypeReal:
		typedKey = new float;
		*(float*)typedKey = *(float*)key;

		//root is empty
		if(root == -1){

			leafNode<float> *lNode = new leafNode<float>();

			lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, *(float*)typedKey,rid);
			lNode->nextLeaf = -1;
			lNode->d = D;
			lNode->sizeOfMap = 1;

			//write this node to page file
			int page = this->addToLeafNodePage(lNode);
			root = page;
			newRoot = page;

			delete lNode;

			return 0;
		}

		//Depth = 0
		if(depth==0){
			leafNode<float> *lNode = new leafNode<float>();

			//read node from page file
			this->readFromLeafNodePage(lNode,root);

			lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, *(float*)typedKey,rid);
			this->writeToLeafNodePage(lNode,root);
			this->readFromLeafNodePage(lNode,root);

			//If L has space
			if(lNode->myMap.size()<=lNode->d){
				delete lNode;
				return 0;
			}


			//Once in a while, the leaf is full
			else{
				leafNode<float> *newchildNode = new leafNode<float>();

				//split L: first d entries stay, rest move to new node L2
				map<float,int>::iterator it = lNode->myMap.begin();
				for(int i = 0; i < (lNode->d)/2; i++)
					it++;

				float keyValue = (*it).first;
				map<float,int> tempLeafMap;
				for (; it != lNode->myMap.end(); it++ )
					tempLeafMap.insert(pair<float,int>((*it).first,(*it).second));

				//createNewLeafNode parameter: myMap, nextLeaf, d, newchildNode
				this->createNewLeafNode(tempLeafMap,-1,D,newchildNode);

				//write to file
				int newpageNum = this->addToLeafNodePage(newchildNode);

				it=lNode->myMap.begin();
				for(int i = 0; i < (lNode->d)/2; i++)
					it++;
				lNode->myMap.erase(it,lNode->myMap.end());

				//set sibling pointer in L2
				this->createNewLeafNode(lNode->myMap,newpageNum,D,lNode);
				this->writeToLeafNodePage(lNode, root);

				//make new node with (pointer to L, *newchildNode)
				stemNode<float> *sNode = new stemNode<float>();
				map<float,int> tempStemMap;
				tempStemMap.insert(pair<float,int>(keyValue,root));
				this->createNewStemNode(tempStemMap,newpageNum,D,sNode);

				//write to file
				int pageNum = this->addToStemNodePage(sNode);

				//make the tree's root-node pointer point to the new node
				root = pageNum;
				depth = 1;

				delete newchildNode;
				delete lNode;
				delete sNode;
				return 0;
			}
		}
		else{
			int newchildentry = -1;
			this->insertIX_float(root,key,rid,&newchildentry);

			return 0;
		}
	    return 0;
	case TypeVarChar:
			 int len = *(int*)key;
			 char * str = (char*)malloc(len + 1);
			 memcpy(str, (char*)key + 4, len);
			 str[len] = '\0';
			 string typedKey(str);
			 free(str);

			 //root is empty
			if(root == -1){


				leafNode<string> *lNode = new leafNode<string>();

				lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, typedKey,rid);
				lNode->nextLeaf = -1;
				lNode->d = D;
				lNode->sizeOfMap = 1;

				//write this node to page file
				int page = this->addToLeafNodePage(lNode);
				root = page;
				newRoot = page;

				delete lNode;

				return 0;
			}

			//Depth = 0
			if(depth==0){
				leafNode<string> *lNode = new leafNode<string>();

				//read node from page file
				this->readFromLeafNodePage(lNode,root);

				lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, typedKey,rid);
				this->writeToLeafNodePage(lNode,root);
				this->readFromLeafNodePage(lNode,root);

				//If L has space
				if(lNode->myMap.size()<=lNode->d){
					delete lNode;
					return 0;
				}


				//Once in a while, the leaf is full
				else{
					leafNode<string> *newchildNode = new leafNode<string>();

					//split L: first d entries stay, rest move to new node L2
					map<string,int>::iterator it = lNode->myMap.begin();
					for(int i = 0; i < (lNode->d)/2; i++)
						it++;

					string keyValue = (*it).first;
					map<string,int> tempLeafMap;
					for (; it != lNode->myMap.end(); it++ )
						tempLeafMap.insert(pair<string,int>((*it).first,(*it).second));

					//createNewLeafNode parameter: myMap, nextLeaf, d, newchildNode
					this->createNewLeafNode(tempLeafMap,-1,D,newchildNode);

					//write to file
					int newpageNum = this->addToLeafNodePage(newchildNode);

					it=lNode->myMap.begin();
					for(int i = 0; i < (lNode->d)/2; i++)
						it++;
					lNode->myMap.erase(it,lNode->myMap.end());

					//set sibling pointer in L2
					this->createNewLeafNode(lNode->myMap,newpageNum,D,lNode);
					this->writeToLeafNodePage(lNode, root);

					//make new node with (pointer to L, *newchildNode)
					stemNode<string> *sNode = new stemNode<string>();
					map<string,int> tempStemMap;
					tempStemMap.insert(pair<string,int>(keyValue,root));
					this->createNewStemNode(tempStemMap,newpageNum,D,sNode);

					//write to file
					int pageNum = this->addToStemNodePage(sNode);

					//make the tree's root-node pointer point to the new node
					root = pageNum;
					depth = 1;

					delete newchildNode;
					delete lNode;
					delete sNode;
					return 0;
				}
			}
			else{
				int newchildentry = -1;
				this->insertIX_string(root,key,rid,&newchildentry);

				return 0;
			}
	}
	return 0;
}

void IX_IndexHandle::insertIX(int node, void *key, const RID &rid, int *newchildentry)
{

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(node,page);
	char flag = *(char *)page;
	delete page;

	//non-leaf node
	if(flag == '0'){

		stemNode<int> *sNode = new stemNode<int>();
		this->readFromStemNodePage(sNode,node);

		//choose subtree
		map<int,int>::iterator it=sNode->idxMap.begin();
		for(; it!=sNode->idxMap.end();it++){
			if((*it).first > *(int*)key)
				break;
		}

		//recursively, insert entry
		if(it!=sNode->idxMap.end())
			insertIX((*it).second,key,rid,&(*newchildentry));
		else
			insertIX(sNode->end,key,rid,&(*newchildentry));

		//int midValue = -1;
		//usual case, didn't split child
		if(*newchildentry==-1){
			delete sNode;
			return;
		}

		else{  //we split child, must insert &newchildentry in N

			//put newchildentry on it
			int value;

			//check if leaf or stem
			void* page = new unsigned char[PF_PAGE_SIZE];
			pf_fileHandle.ReadPage(*newchildentry,page);
			char check = *(char *)page;
			delete page;

			if(check == '1')
				this->getFirstValue(*newchildentry,&value);
			else
				value = midValue;

			map<int,int> tempStemMap = sNode->idxMap;
			if(it == sNode->idxMap.end()){
				tempStemMap.insert(pair<int,int>(value,sNode->end));
				this->createNewStemNode(tempStemMap,*newchildentry,D,sNode);
			}
			else{
				tempStemMap.insert(pair<int,int>(value,(*it).second));
				map<int,int>::iterator del = tempStemMap.find((*it).first);
				tempStemMap.erase(del);
				tempStemMap.insert(pair<int,int>((*it).first,*newchildentry));
				this->createNewStemNode(tempStemMap,sNode->end,D,sNode);
			}

			this->writeToStemNodePage(sNode,node);
			this->readFromStemNodePage(sNode,node);

			//if N has space
			if(sNode->idxMap.size() <= sNode->d){
				//set newchildentry to null, return
				*newchildentry = -1;

				return;
			}
			else
				//split N
				this->splitStemNode(node,&midValue,&(*newchildentry));

			//root node was just split
			if(node == root){
				int newNode = -1;
				this->createNewNode(node,*newchildentry,midValue,&newNode);

				//make the tree's root-node pointer point to the new node
				root = newNode;
				this->readFromStemNodePage(sNode,root);

			}
			delete sNode;
			return;
		}
	}

	//leaf-node
	if(flag == '1'){

		leafNode<int> *lNode = new leafNode<int>();
		this->readFromLeafNodePage(lNode,node);

		lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, *(int*)key,rid);
		lNode->sizeOfMap = lNode->myMap.size();
		this->writeToLeafNodePage(lNode,node);
		this->readFromLeafNodePage(lNode,node);

		//if L has space
		if(lNode->myMap.size() <= lNode->d){
			*newchildentry = -1;

			delete lNode;

			return;
		}
		else{
			this->splitLeafNode(node,&midValue,&(*newchildentry));

			delete lNode;

			return;
		}
	}
}

void IX_IndexHandle::insertIX_float(int node, void *key, const RID &rid, int *newchildentry)
{
	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(node,page);
	char flag = *(char *)page;
	delete page;

	//non-leaf node
	if(flag == '0'){

		stemNode<float> *sNode = new stemNode<float>();
		this->readFromStemNodePage(sNode,node);

		//choose subtree
		map<float,int>::iterator it=sNode->idxMap.begin();
		for(; it!=sNode->idxMap.end();it++){
			if((*it).first > *(float*)key)
				break;
		}

		//recursively, insert entry
		if(it!=sNode->idxMap.end())
			insertIX_float((*it).second,key,rid,&(*newchildentry));
		else
			insertIX_float(sNode->end,key,rid,&(*newchildentry));

		//int midValue = -1;
		//usual case, didn't split child
		if(*newchildentry==-1){
			delete sNode;
			return;
		}

		else{  //we split child, must insert &newchildentry in N

			//put newchildentry on it
			float value;

			//check if leaf or stem
			void* page = new unsigned char[PF_PAGE_SIZE];
			pf_fileHandle.ReadPage(*newchildentry,page);
			char check = *(char *)page;
			delete page;

			if(check == '1')
				this->getFirstValue(*newchildentry,&value);
			else
				value = midValue_float;
			map<float,int> tempStemMap = sNode->idxMap;

			if(it == sNode->idxMap.end()){
				tempStemMap.insert(pair<float,int>(value,sNode->end));
				this->createNewStemNode(tempStemMap,*newchildentry,D,sNode);
			}
			else{
				tempStemMap.insert(pair<float,int>(value,(*it).second));
				map<float,int>::iterator del = tempStemMap.find((*it).first);
				tempStemMap.erase(del);
				tempStemMap.insert(pair<float,int>((*it).first,*newchildentry));
				this->createNewStemNode(tempStemMap,sNode->end,D,sNode);
			}

			this->writeToStemNodePage(sNode,node);
			this->readFromStemNodePage(sNode,node);

			//if N has space
			if(sNode->idxMap.size() <= sNode->d){
				//set newchildentry to null, return
				*newchildentry = -1;

				return;
			}
			else{
				//split N
				this->splitStemNode(node,&midValue_float,&(*newchildentry));

			}

			//root node was just split
			if(node == root){
				int newNode = -1;
				this->createNewNode(node,*newchildentry,midValue_float,&newNode);

				//make the tree's root-node pointer point to the new node
				root = newNode;
				this->readFromStemNodePage(sNode,root);

			}
			delete sNode;
			return;
		}
	}

	//leaf-node
	if(flag == '1'){

		leafNode<float> *lNode = new leafNode<float>();
		this->readFromLeafNodePage(lNode,node);

		lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, *(float*)key,rid);
		lNode->sizeOfMap = lNode->myMap.size();
		this->writeToLeafNodePage(lNode,node);
		this->readFromLeafNodePage(lNode,node);

		//if L has space
		if(lNode->myMap.size() <= lNode->d){
			*newchildentry = -1;

			delete lNode;

			return;
		}
		else{
			this->splitLeafNode(node,&midValue_float,&(*newchildentry));

			delete lNode;

			return;
		}
	}
}

void IX_IndexHandle::insertIX_string(int node, void *key, const RID &rid, int *newchildentry)
{

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(node,page);
	char flag = *(char *)page;
	delete page;

	int len = *(int*)key;
	char * str = (char*)malloc(len + 1);
	memcpy(str, (char*)key + 4, len);
	str[len] = '\0';
	string typedKey(str);
	free(str);

	//non-leaf node
	if(flag == '0'){

		stemNode<string> *sNode = new stemNode<string>();
		this->readFromStemNodePage(sNode,node);

		//choose subtree
		map<string,int>::iterator it=sNode->idxMap.begin();
		for(; it!=sNode->idxMap.end();it++){
			if((*it).first > typedKey)
				break;
		}

		//recursively, insert entry
		if(it!=sNode->idxMap.end())
			insertIX_string((*it).second,key,rid,&(*newchildentry));
		else
			insertIX_string(sNode->end,key,rid,&(*newchildentry));

		//int midValue = -1;
		//usual case, didn't split child
		if(*newchildentry==-1){
			delete sNode;
			return;
		}

		else{  //we split child, must insert &newchildentry in N

			//put newchildentry on it
			string value;

			//check if leaf or stem
			void* page = new unsigned char[PF_PAGE_SIZE];
			pf_fileHandle.ReadPage(*newchildentry,page);
			char check = *(char *)page;
			delete page;

			if(check == '1')
				this->getFirstValue(*newchildentry,&value);
			else
				value = midValue_string;
			map<string,int> tempStemMap = sNode->idxMap;

			if(it == sNode->idxMap.end()){
				tempStemMap.insert(pair<string,int>(value,sNode->end));
				this->createNewStemNode(tempStemMap,*newchildentry,D,sNode);
			}
			else{
				tempStemMap.insert(pair<string,int>(value,(*it).second));
				map<string,int>::iterator del = tempStemMap.find((*it).first);
				tempStemMap.erase(del);
				tempStemMap.insert(pair<string,int>((*it).first,*newchildentry));
				this->createNewStemNode(tempStemMap,sNode->end,D,sNode);
			}

			this->writeToStemNodePage(sNode,node);
			this->readFromStemNodePage(sNode,node);

			//if N has space
			if(sNode->idxMap.size() <= sNode->d){
				//set newchildentry to null, return
				*newchildentry = -1;
				return;
			}
			else
				//split N
				this->splitStemNode(node,&midValue_string,&(*newchildentry));

			//root node was just split
			if(node == root){
				int newNode = -1;
				this->createNewNode(node,*newchildentry,midValue_string,&newNode);

				//make the tree's root-node pointer point to the new node
				root = newNode;
				this->readFromStemNodePage(sNode,root);

			}
			delete sNode;
			return;
		}
	}

	//leaf-node
	if(flag == '1'){

		leafNode<string> *lNode = new leafNode<string>();
		this->readFromLeafNodePage(lNode,node);

		lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, typedKey,rid);
		lNode->sizeOfMap = lNode->myMap.size();
		this->writeToLeafNodePage(lNode,node);
		this->readFromLeafNodePage(lNode,node);

		//if L has space
		if(lNode->myMap.size() <= lNode->d){
			*newchildentry = -1;
			delete lNode;

			return;
		}
		else{
			this->splitLeafNode(node,&midValue_string,&(*newchildentry));
			delete lNode;

			return;
		}
	}
}

RC IX_IndexHandle::DeleteEntry(void *key, const RID &rid)
{
	//check key type
	void* typedKey;
	switch(this->type){
	case TypeInt:
		typedKey = new int;
		*(int*)typedKey = *(int*)key;

		if(depth == 0){
			if(root == -1)
				return -1;
			else{
				//remove entry
				removeEntryFromLeaf((int *)key,&root);
				leafNode<int> *rootNode = new leafNode<int>();
				this->readFromLeafNodePage(rootNode,root);

				if(rootNode->sizeOfMap == 0)
					root = -1;

				delete rootNode;
				return 0;
			}
		}
		else{
			int oldchildentry = -1;
			stemNode<int> *rootNode = new stemNode<int>();
			this->readFromStemNodePage(rootNode,root);
			map<int,int>::iterator it = rootNode->idxMap.begin();
			nextRoot = (*it).second;
			delete rootNode;

			this->deleteIX(&root,root,key,rid,&oldchildentry);
		}
		nowRoot = root;
		return 0;

	case TypeReal:
		typedKey = new float;
		*(float*)typedKey = *(float*)key;

		if(depth == 0){

			if(root == -1)
				return -1;
			else{
				//remove entry
				removeEntryFromLeaf((float *)key,&root);
				leafNode<float> *rootNode = new leafNode<float>();
				this->readFromLeafNodePage(rootNode,root);
				if(rootNode->sizeOfMap == 0)
					root = -1;

				delete rootNode;
				return 0;
			}
		}
		else{
			int oldchildentry = -1;
			stemNode<float> *rootNode = new stemNode<float>();
			this->readFromStemNodePage(rootNode,root);
			map<float,int>::iterator it = rootNode->idxMap.begin();
			nextRoot = (*it).second;
			delete rootNode;

			this->deleteIX_float(&root,root,key,rid,&oldchildentry);
		}
		nowRoot = root;
		return 0;
	case TypeVarChar:
		int len = *(int*)key;
		char * str = (char*)malloc(len + 1);
		memcpy(str, (char*)key + 4, len);
		str[len] = '\0';
		string typedKey(str);
		free(str);

		if(depth == 0){

			if(root == -1)
				return -1;
			else{
				//remove entry
				removeEntryFromLeaf(&typedKey,&root);
				leafNode<string> *rootNode = new leafNode<string>();
				this->readFromLeafNodePage(rootNode,root);
				if(rootNode->sizeOfMap == 0)
					root = -1;

				delete rootNode;
				return 0;
			}
		}
		else{
			int oldchildentry = -1;
			stemNode<string> *rootNode = new stemNode<string>();
			this->readFromStemNodePage(rootNode,root);
			map<string,int>::iterator it = rootNode->idxMap.begin();
			nextRoot = (*it).second;
			delete rootNode;

			this->deleteIX_string(&root,root,key,rid,&oldchildentry);
		}
		nowRoot = root;
		return 0;
	}
	return 0;
}


void IX_IndexHandle::deleteIX(int *parent, int node, void *key, const RID &rid, int *oldchildentry)
{

	//check leaf or stem
	void* page = new signed char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(node,page);
	char flag = *(char *)page;
	delete page;

	//non-leaf
	if(flag == '0'){
		stemNode<int> *sNode = new stemNode<int>();
		this->readFromStemNodePage(sNode,node);

		//choose subtree
		map<int,int>::iterator it=sNode->idxMap.begin();
		for(; it!=sNode->idxMap.end();it++){
			if((*it).first > *(int*)key)
				break;
		}

		if(it == sNode->idxMap.end()){
			//remove entry from last leaf node
			int end = sNode->end;
			removeEntryFromLeaf((int *)key,&end);
		}
		else{ //recursively, delete entry
			int second =(*it).second;
			this->deleteIX(&node,second,key,rid,&(*oldchildentry));
		}

		//usual case: child not deleted
		if(*oldchildentry == -1){
			delete sNode;
			return;
		}

		else{

			this->removeOldchildentryFromNode(node,*oldchildentry);
			this->readFromStemNodePage(sNode,node);

			if(root == node){
				if(sNode->sizeOfMap == 0)
					root = nextRoot;
				if(root == newRoot){
					depth = 0;
				}

				return;
			}

			//next, check for underflow
			//usual case
			if(sNode->idxMap.size() >= sNode->d){
				*oldchildentry = -1;
				delete sNode;
				return;
			}
			else{
				//get a sibling s of N
				int sibling;
				this->findStemSibling(*parent,node,&sibling);

				stemNode<int> *siblingNode = new stemNode<int>();
				this->readFromStemNodePage(siblingNode,sibling);

				//if S has extra entries
				int size = siblingNode->idxMap.size();
				int d = (siblingNode->d)/2;
				if(size-1 >= d){
					//redistribute evenly between N and S through parent
					this->redistributeNS_Stem(node, sibling, *parent);
					//set oldchildentry to null
					*oldchildentry = -1;

					/*//test
					stemNode<int> *testNode = new stemNode<int>();
					this->readFromStemNodePage(testNode,node);
					for(map<int,int>::iterator it1 = testNode->idxMap.begin();it1!=testNode->idxMap.end();it1++)
							cout << "stem: more space:" << (*it1).first << "--" << (*it1).second <<endl;

					//test
					testNode = new stemNode<int>();
					this->readFromStemNodePage(testNode,*parent);
					for(map<int,int>::iterator it1 = testNode->idxMap.begin();it1!=testNode->idxMap.end();it1++)
							cout << (*it1).first <<endl;
					delete testNode;*/

					delete sNode;
					return;
				}

				else{  //merge N and S
					*oldchildentry = sibling;
					this->mergeStemNodes(node,sibling,*parent);

					/*//test
					stemNode<int> *testNode = new stemNode<int>();
					this->readFromStemNodePage(testNode,node);
					for(map<int,int>::iterator it1 = testNode->idxMap.begin();it1!=testNode->idxMap.end();it1++)
							cout << "stem: merge:" << (*it1).first << "--" << (*it1).second <<endl;
					delete testNode;

					//test
					stemNode<int> *stestNode = new stemNode<int>();
					this->readFromStemNodePage(stestNode,*parent);
					for(map<int,int>::iterator it1 = stestNode->idxMap.begin();it1!=stestNode->idxMap.end();it1++)
							cout << (*it1).first <<endl;
					delete stestNode;*/

					delete sNode;
					return;
				}
			}
		}

	}

	//leaf-node
	if(flag == '1'){

		//remove entry
		removeEntryFromLeaf((int *)key,&node);

		leafNode<int> *lNode = new leafNode<int>();
		this->readFromLeafNodePage(lNode,node);

		//if L has entries to spare
		if(lNode->myMap.size()>=(lNode->d)/2){

			*oldchildentry = -1;

			delete lNode;
			return;
		}
		else{    //once in a while, the leaf becomes underfull
			//get a sibling s of N
			int sibling;
			this->findLeafSibling(*parent, node, &sibling);

			leafNode<int> *siblingNode = new leafNode<int>();
			this->readFromLeafNodePage(siblingNode,sibling);

			//if S has extra entries
			if(siblingNode->myMap.size() - 1 >= (siblingNode->d)/2){

				//redistribute evenly between N and S through parent
				this->redistributeNS_Leaf(node, sibling);

				//replace key value in parent entry by new low-key value in S
				replaceKeyValue(*parent, sibling);

				//set oldchildentry to null
				*oldchildentry = -1;

				/*//test
				leafNode<int> *testNode = new leafNode<int>();
				this->readFromLeafNodePage(testNode,node);
				for(map<int,int>::iterator it1 = testNode->myMap.begin();it1!=testNode->myMap.end();it1++)
						cout << "leaf: more space:" << (*it1).first << "--" << (*it1).second <<endl;
				delete testNode;*/

				delete lNode;
				delete siblingNode;
				return;
			}
			else{    //merge L and S
				//set oldchildentry = &(current entry in parent for S)
				*oldchildentry = sibling;
				this->mergeLeafNodes(node, sibling);

				/*//test
				leafNode<int> *testNode = new leafNode<int>();
				this->readFromLeafNodePage(testNode,node);
				for(map<int,int>::iterator it1 = testNode->myMap.begin();it1!=testNode->myMap.end();it1++)
						cout << "leaf: merge:" << (*it1).first << "--" << (*it1).second <<endl;
				delete testNode;*/

				delete lNode;
				return;
			}
		}
	}
}

void IX_IndexHandle::deleteIX_float(int *parent, int node, void *key, const RID &rid, int *oldchildentry)
{
	//check leaf or stem
	void* page = new signed char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(node,page);
	char flag = *(char *)page;
	delete page;

	//non-leaf
	if(flag == '0'){

		stemNode<float> *sNode = new stemNode<float>();
		this->readFromStemNodePage(sNode,node);

		//choose subtree
		map<float,int>::iterator it=sNode->idxMap.begin();
		for(; it!=sNode->idxMap.end();it++){
			if((*it).first > *(float*)key)
				break;
		}

		if(it == sNode->idxMap.end()){
			//remove entry from last leaf node
			int end = sNode->end;
			removeEntryFromLeaf((float *)key,&end);
		}
		else{ //recursively, delete entry
			int second =(*it).second;
			this->deleteIX_float(&node,second,key,rid,&(*oldchildentry));
		}

		//usual case: child not deleted
		if(*oldchildentry == -1){
			delete sNode;
			return;
		}

		else{

			this->removeOldchildentryFromNode_f(node,*oldchildentry);
			this->readFromStemNodePage(sNode,node);

			if(root == node){
				if(sNode->sizeOfMap == 0)
					root = nextRoot;
				if(root == newRoot){
					depth = 0;
				}

				return;
			}

			//next, check for underflow
			//usual case
			if(sNode->idxMap.size() >= sNode->d){
				*oldchildentry = -1;
				delete sNode;
				return;
			}
			else{
				//get a sibling s of N
				int sibling;
				this->findStemSibling_f(*parent,node,&sibling);

				stemNode<float> *siblingNode = new stemNode<float>();
				this->readFromStemNodePage(siblingNode,sibling);

				//if S has extra entries
				int size = siblingNode->idxMap.size();
				int d = (siblingNode->d)/2;
				if(size-1 >= d){
					//redistribute evenly between N and S through parent
					this->redistributeNS_Stem_f(node, sibling, *parent);
					//set oldchildentry to null
					*oldchildentry = -1;

					delete sNode;
					return;
				}

				else{  //merge N and S
					*oldchildentry = sibling;
					this->mergeStemNodes_f(node,sibling,*parent);

					delete sNode;
					return;
				}
			}
		}

	}

	//leaf-node
	if(flag == '1'){

		//remove entry
		removeEntryFromLeaf((float *)key,&node);

		leafNode<float> *lNode = new leafNode<float>();
		this->readFromLeafNodePage(lNode,node);

		//if L has entries to spare
		if(lNode->myMap.size()>=(lNode->d)/2){

			*oldchildentry = -1;

			delete lNode;
			return;
		}
		else{    //once in a while, the leaf becomes underfull
			//get a sibling s of N
			int sibling;
			this->findLeafSibling_f(*parent, node, &sibling);

			leafNode<float> *siblingNode = new leafNode<float>();
			this->readFromLeafNodePage(siblingNode,sibling);

			//if S has extra entries
			if(siblingNode->myMap.size() - 1 >= (siblingNode->d)/2){

				//redistribute evenly between N and S through parent
				this->redistributeNS_Leaf_f(node, sibling);

				//replace key value in parent entry by new low-key value in S
				replaceKeyValue_f(*parent, sibling);

				//set oldchildentry to null
				*oldchildentry = -1;

				delete lNode;
				delete siblingNode;
				return;
			}
			else{    //merge L and S
				//set oldchildentry = &(current entry in parent for S)
				*oldchildentry = sibling;
				this->mergeLeafNodes_f(node, sibling);

				delete lNode;
				return;
			}
		}
	}
}

void IX_IndexHandle::deleteIX_string(int *parent, int node, void *key, const RID &rid, int *oldchildentry)
{

	//check leaf or stem
	void* page = new signed char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(node,page);
	char flag = *(char *)page;
	delete page;

	int len = *(int*)key;
	char * str = (char*)malloc(len + 1);
	memcpy(str, (char*)key + 4, len);
	str[len] = '\0';
	string typedKey(str);
	free(str);

	//non-leaf
	if(flag == '0'){
		stemNode<string> *sNode = new stemNode<string>();
		this->readFromStemNodePage(sNode,node);

		//choose subtree
		map<string,int>::iterator it=sNode->idxMap.begin();
		for(; it!=sNode->idxMap.end();it++){
			if((*it).first > typedKey)
				break;
		}

		if(it == sNode->idxMap.end()){
			//remove entry from last leaf node
			int end = sNode->end;
			removeEntryFromLeaf(&typedKey,&end);
		}
		else{ //recursively, delete entry
			int second =(*it).second;
			this->deleteIX_string(&node,second,key,rid,&(*oldchildentry));
		}

		//usual case: child not deleted
		if(*oldchildentry == -1){
			delete sNode;
			return;
		}

		else{

			this->removeOldchildentryFromNode_s(node,*oldchildentry);
			this->readFromStemNodePage(sNode,node);

			if(root == node){
				if(sNode->sizeOfMap == 0)
					root = nextRoot;
				if(root == newRoot){
					depth = 0;
				}

				return;
			}

			//next, check for underflow
			//usual case
			if(sNode->idxMap.size() >= sNode->d){
				*oldchildentry = -1;
				delete sNode;
				return;
			}
			else{
				//get a sibling s of N
				int sibling;
				this->findStemSibling_f(*parent,node,&sibling);

				stemNode<string> *siblingNode = new stemNode<string>();
				this->readFromStemNodePage(siblingNode,sibling);

				//if S has extra entries
				int size = siblingNode->idxMap.size();
				int d = (siblingNode->d)/2;
				if(size-1 >= d){
					//redistribute evenly between N and S through parent
					this->redistributeNS_Stem_s(node, sibling, *parent);
					//set oldchildentry to null
					*oldchildentry = -1;

					delete sNode;
					return;
				}

				else{  //merge N and S
					*oldchildentry = sibling;
					this->mergeStemNodes_s(node,sibling,*parent);

					delete sNode;
					return;
				}
			}
		}

	}

	//leaf-node
	if(flag == '1'){

		//remove entry
		removeEntryFromLeaf(&typedKey,&node);

		leafNode<string> *lNode = new leafNode<string>();
		this->readFromLeafNodePage(lNode,node);

		//if L has entries to spare
		if(lNode->myMap.size()>=(lNode->d)/2){

			*oldchildentry = -1;

			delete lNode;
			return;
		}
		else{    //once in a while, the leaf becomes underfull
			//get a sibling s of N
			int sibling;
			this->findLeafSibling_s(*parent, node, &sibling);

			leafNode<string> *siblingNode = new leafNode<string>();
			this->readFromLeafNodePage(siblingNode,sibling);

			//if S has extra entries
			if(siblingNode->myMap.size() - 1 >= (siblingNode->d)/2){

				//redistribute evenly between N and S through parent
				this->redistributeNS_Leaf_s(node, sibling);

				//replace key value in parent entry by new low-key value in S
				replaceKeyValue_s(*parent, sibling);

				//set oldchildentry to null
				*oldchildentry = -1;

				delete lNode;
				delete siblingNode;
				return;
			}
			else{    //merge L and S
				//set oldchildentry = &(current entry in parent for S)
				*oldchildentry = sibling;
				this->mergeLeafNodes_s(node, sibling);

				delete lNode;
				return;
			}
		}
	}
}

map<int,int> IX_IndexHandle::insertRidToVectorOfRid(map<int,int> myMap, int key, RID rid)
{
	map<int,int>::iterator it = myMap.find(key);
	vector<RID> v;
	vectorOfRidNode *vNode;
	//unfind
	if(it == myMap.end()){
		v.push_back(rid);
		vNode = new vectorOfRidNode();
		this->createNewVectorNode(v,-1,vNode);
		int pageNum = this->addToVectorNodePage(vNode);
		myMap.insert(pair<int,int>(key,pageNum));
	}
	//find
	else{
		vNode = new vectorOfRidNode();
		this->readFromVectorNodePage(vNode,(*it).second);
		vectorOfRidNode *vNextNode;
		if(vNode->size == 400){
			if(vNode->nextPage == -1){
				v.push_back(rid);
				vectorOfRidNode *vNewNode = new vectorOfRidNode();
				this->createNewVectorNode(v,-1,vNewNode);
				int pageNum = this->addToVectorNodePage(vNewNode);
				vNode->nextPage = pageNum;
				this->createNewVectorNode(vNode->v,vNode->nextPage,vNode);
				this->writeToVectorNodePage(vNode,(*it).second);

				return myMap;
			}
			while(vNode->nextPage != -1){
				vNextNode = new vectorOfRidNode();
				this->readFromVectorNodePage(vNextNode,vNode->nextPage);
				if(vNextNode->size == 400){
					if(vNextNode->nextPage == -1){
						v.push_back(rid);
						vectorOfRidNode *vNewNode = new vectorOfRidNode();
						this->createNewVectorNode(v,-1,vNewNode);
						int pageNum = this->addToVectorNodePage(vNewNode);
						this->createNewVectorNode(vNextNode->v,pageNum,vNextNode);
						this->writeToVectorNodePage(vNextNode,vNode->nextPage);

						return myMap;
					}
					else
						vNode = vNextNode;
				}

				else{
					v = vNextNode->v;
					v.push_back(rid);
					this->createNewVectorNode(v,vNextNode->nextPage,vNextNode);
					this->writeToVectorNodePage(vNextNode,vNode->nextPage);

					return myMap;
				}
			}
		}
		v = vNode->v;
		v.push_back(rid);
		this->createNewVectorNode(v,vNode->nextPage,vNode);
		this->writeToVectorNodePage(vNode,(*it).second);
	}

	return myMap;
}

RID IX_IndexHandle::readRidFromVectorOfRid(map<int,int>::iterator it, int pageOfVector, int n)
{
	//n begin with 1
	int page = (*it).second;
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,page);
	for(int i = 0; i < pageOfVector; i++)
		this->readFromVectorNodePage(vNode,vNode->nextPage);

	vector<RID> v = vNode->v;

	return v[n];
}

void IX_IndexHandle::removeOldchildentryFromNode(int node, int oldchildentry)
{
	stemNode<int> *sNode = new stemNode<int>();
	this->readFromStemNodePage(sNode,node);

	//find the corresponding entry
	map<int,int>::iterator it;
	for(it = sNode->idxMap.begin(); it!=sNode->idxMap.end(); it++){
		if((*it).second == oldchildentry)
			break;
	}

	it--;
	map<int,int>::iterator pre = it;
	it++;

	sNode->idxMap.find((*it).first)->second = (*pre).second;
	sNode->idxMap.erase(pre);

	this->createNewStemNode(sNode->idxMap,sNode->end,D,sNode);
	this->writeToStemNodePage(sNode,node);
	delete sNode;
	return;
}

void IX_IndexHandle::replaceKeyValue(int parent, int sibling)
{
	leafNode<int> *siblingNode = new leafNode<int>();
	this->readFromLeafNodePage(siblingNode,sibling);

	stemNode<int> *pNode = new stemNode<int>();
	this->readFromStemNodePage(pNode,parent);

	map<int,int>::iterator it=siblingNode->myMap.begin();

	//find entry in parent for node for sibling
	map<int,int>::iterator pit = pNode->idxMap.begin();
	for(;pit!=pNode->idxMap.end();pit++){
		if((*pit).second == sibling)
			break;
	}
	pit--;

	//problem
	pNode->idxMap.insert(pair<int,int>((*it).first,(*pit).second));
	pNode->idxMap.erase((*pit).first);

	this->createNewStemNode(pNode->idxMap,pNode->end,D,pNode);
	this->writeToStemNodePage(pNode,parent);

	delete siblingNode;
	delete pNode;
	return;
}

void IX_IndexHandle::findLeafSibling(int parent, int node, int *sibling)
{
	stemNode<int> *pNode = new stemNode<int>();
	this->readFromStemNodePage(pNode,parent);

	map<int,int>::iterator it;
	for(it = pNode->idxMap.begin(); it!=pNode->idxMap.end(); it++){
		if((*it).second == node)
			break;
	}

	it++;
	if(it == pNode->idxMap.end())
		*sibling = pNode->end;
	else
		*sibling = (*it).second;

	delete pNode;
	return;
}

void IX_IndexHandle::findStemSibling(int parent, int node, int *sibling)
{
	stemNode<int> *pNode = new stemNode<int>();
	this->readFromStemNodePage(pNode,parent);

	map<int,int>::iterator it;
	for(it = pNode->idxMap.begin(); it!=pNode->idxMap.end(); it++){
		if((*it).second == node)
			break;
	}

	it++;
	if(it == pNode->idxMap.end()){
		*sibling = pNode->end;
		return;
	}

	*sibling = (*it).second;

	delete pNode;
	return;
}

void IX_IndexHandle::redistributeNS_Leaf(int node, int sibling)
{
	leafNode<int> *lNode = new leafNode<int>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<int> *siblingNode = new leafNode<int>();
	this->readFromLeafNodePage(siblingNode,sibling);

	int numsOfEntries = (lNode->myMap.size() + siblingNode->myMap.size())/2;
	int numsOfMoves = numsOfEntries - lNode->myMap.size();

	map<int,int>::iterator it;
	map<int,int>::iterator temp;
	it = siblingNode->myMap.begin();

	for(int i = 0;i < numsOfMoves; i++){
		lNode->myMap.insert(pair<int,int>((*it).first,(*it).second));
		//map<int,int> tempMap = lNode->myMap;
		//tempMap = this->insertRidToVectorOfRid(tempMap, (*it).first,(*it).second);

		temp = it;
		temp++;
		siblingNode->myMap.erase(it);
		it = temp;
	}

	this->createNewLeafNode(lNode->myMap,lNode->nextLeaf,D,lNode);
	this->writeToLeafNodePage(lNode,node);

	this->createNewLeafNode(siblingNode->myMap,siblingNode->nextLeaf,D,siblingNode);
	this->writeToLeafNodePage(siblingNode,sibling);

	leafNode<int> *testNode = new leafNode<int>();
	this->readFromLeafNodePage(testNode,node);

	delete testNode;

	delete lNode;
	delete siblingNode;

	return;
}

void IX_IndexHandle::redistributeNS_Stem(int node, int sibling, int parent)
{
	stemNode<int> *sNode = new stemNode<int>();
	this->readFromStemNodePage(sNode,node);

	stemNode<int> *siblingNode = new stemNode<int>();
	this->readFromStemNodePage(siblingNode,sibling);

	stemNode<int> *pNode = new stemNode<int>();
	this->readFromStemNodePage(pNode,parent);

	int numsOfEntries = (sNode->idxMap.size() + siblingNode->idxMap.size())/2;
	int numsOfMoves = numsOfEntries - sNode->idxMap.size();

 	map<int,int>::iterator it;
	map<int,int>::iterator temp;
	it = siblingNode->idxMap.begin();

	for(int i = 0;i < numsOfMoves-1; i++){
		sNode->idxMap.insert(pair<int,int>((*it).first,(*it).second));
		temp = it;
		temp++;
		siblingNode->idxMap.erase(it);
		it = temp;
	}

	map<int,int>::iterator pit = pNode->idxMap.begin();
	for(;pit!=pNode->idxMap.end();pit++){
		if((*pit).second == node)
			break;
	}
	sNode->idxMap.insert(pair<int,int>((*pit).first,sNode->end));
	this->createNewStemNode(sNode->idxMap,(*it).second,D,sNode);
	this->writeToStemNodePage(sNode,node);

	int second = (*pit).second;
	pNode->idxMap.erase(pNode->idxMap.find((*pit).first));
	pNode->idxMap.insert(pair<int,int>((*it).first,second));
	this->createNewStemNode(pNode->idxMap,pNode->end,D,sNode);
	this->writeToStemNodePage(pNode,parent);

	siblingNode->idxMap.erase(it);
	this->createNewStemNode(siblingNode->idxMap,siblingNode->end,D,siblingNode);
	this->writeToStemNodePage(siblingNode,sibling);


	delete sNode;
	delete siblingNode;
	delete pNode;
	return;
}

void IX_IndexHandle::mergeLeafNodes(int node, int sibling)
{
	leafNode<int> *lNode = new leafNode<int>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<int> *siblingNode = new leafNode<int>();
	this->readFromLeafNodePage(siblingNode,sibling);

	//move all the entries from S to node on the left
	//discard empty node
	map<int,int>::iterator it=siblingNode->myMap.begin();
	map<int,int>::iterator temp;
	for(; it!=siblingNode->myMap.end();it++){
		/*vector<RID> v = (*it).second;
		int full = 0;
		for(int j = 0; j < v.size(); j++)
			lNode->myMap = this->insertRidToVectorOfRid(lNode->myMap, (*it).first,v[j],&full);*/

		lNode->myMap.insert(pair<int,int>((*it).first,(*it).second));
	}

	//discard empty, adjust sibling pointers
	this->createNewLeafNode(lNode->myMap,siblingNode->nextLeaf,D,lNode);
	this->writeToLeafNodePage(lNode,node);

	delete lNode;
	delete siblingNode;

	return;
}

void IX_IndexHandle::mergeStemNodes(int node, int sibling, int parent)
{
	stemNode<int> *sNode = new stemNode<int>();
	this->readFromStemNodePage(sNode,node);

	stemNode<int> *siblingNode = new stemNode<int>();
	this->readFromStemNodePage(siblingNode,sibling);

	stemNode<int> *pNode = new stemNode<int>();
	this->readFromStemNodePage(pNode,parent);

	//pull splitting key from parent down into node on the left
	for(map<int,int>::iterator pit=pNode->idxMap.begin();pit!=pNode->idxMap.end();pit++){
		if((*pit).second == node){
			sNode->idxMap.insert(pair<int,int>((*pit).first,sNode->end));
			break;
		}
	}

	//move all the entries from S to node on the left
	//discard empty node
	map<int,int>::iterator it=siblingNode->idxMap.begin();
	map<int,int>::iterator temp;
	for(; it!=siblingNode->idxMap.end();it++)
		sNode->idxMap.insert(pair<int,int>((*it).first,(*it).second));/**/

	this->createNewStemNode(sNode->idxMap,siblingNode->end,D,sNode);
	this->writeToStemNodePage(sNode,node);

	delete sNode;
	delete siblingNode;
	delete pNode;

	return;
}

//delete
RC IX_IndexHandle::removeEntryFromLeaf(int *key, int *node)
{
	leafNode<int> *lNode = new leafNode<int>();
	this->readFromLeafNodePage(lNode,*node);

	map<int,int>::iterator it;
	it = lNode->myMap.find(*(int*)key);

	//check if the value exists
	if(it == lNode->myMap.end())
		return -1;

	lNode->myMap.erase(it);

	this->createNewLeafNode(lNode->myMap,lNode->nextLeaf,D,lNode);

	this->writeToLeafNodePage(lNode,*node);

	delete lNode;
	return 0;
}

void IX_IndexHandle::createNewVectorNode(vector<RID> v, int nextPage, vectorOfRidNode *vNode)
{
	vNode->size = v.size();
	vNode->v = v;
	vNode->nextPage = nextPage;

	return;
}

void IX_IndexHandle::createNewLeafNode(map<int,int> myMap, int nextLeaf, int d,leafNode<int> *newchildNode)
{
	newchildNode->sizeOfMap = myMap.size();
	newchildNode->myMap = myMap;
	newchildNode->nextLeaf = nextLeaf;
	newchildNode->d = d;

	return;
}

void IX_IndexHandle::createNewStemNode(map<int,int> idxMap, int end, int d,stemNode<int> *newchildNode)
{
	newchildNode->sizeOfMap = idxMap.size();
	newchildNode->idxMap = idxMap;
	newchildNode->end = end;
	newchildNode->d = d;

	return;
}

void IX_IndexHandle::splitLeafNode(int node, int *midValue, int *newchildentry)
{
	leafNode<int> *lNode = new leafNode<int>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<int> *newchildNode = new leafNode<int>();

	//split L: first d entries stay, rest move to new node L2
	map<int,int>::iterator it=lNode->myMap.begin();
	for(int i = 0; i < (lNode->d)/2; i++)
		it++;

	*midValue = (*it).first;
	map<int,int> tempLeafMap;

	for (; it != lNode->myMap.end(); it++ )
		tempLeafMap.insert(pair<int,int>((*it).first,(*it).second));
	this->createNewLeafNode(tempLeafMap,lNode->nextLeaf,D,newchildNode);

	//write to file
	*newchildentry = this->addToLeafNodePage(newchildNode);

	it=lNode->myMap.begin();
	for(int i = 0; i < (lNode->d)/2; i++)
		it++;
	lNode->myMap.erase(it,lNode->myMap.end());

	//set sibling pointer in L2
	lNode->nextLeaf = *newchildentry;

	this->writeToLeafNodePage(lNode,node);

	delete lNode;
	delete newchildNode;
	return;
}

void IX_IndexHandle::createNewNode(int node, int newchildentry, int midValue, int *newNode)
{
	map<int,int> tempMap;
	tempMap.insert(pair<int,int>(midValue,node));
	stemNode<int> *sNode = new stemNode<int>();
	this->createNewStemNode(tempMap,newchildentry,D,sNode);

	*newNode = this->addToStemNodePage(sNode);

	delete sNode;
	return;
}

void IX_IndexHandle::splitStemNode(int node, int *midValue, int *newchildentry)
{
	stemNode<int> *sNode = new stemNode<int>();
	this->readFromStemNodePage(sNode,node);

	//first d key values and d+1 nodepointers stay,
	map<int,int>::iterator it=sNode->idxMap.begin();
	for(int i = 0; i < (sNode->d)/2; i++)
		it++;

	*midValue = (*it).first;
	it++;

	//last d keys and d+1 pointers move to new node, N2
	stemNode<int> *newNode = new stemNode<int>();
	map<int,int> tempMap;
	for(; it!=sNode->idxMap.end(); it++)
		tempMap.insert(pair<int,int>((*it).first,(*it).second));
	this->createNewStemNode(tempMap,sNode->end,D,newNode);
	*newchildentry = this->addToStemNodePage(newNode);

	it = sNode->idxMap.find(*midValue);
	int end = (*it).second;
	it = sNode->idxMap.begin();
	for(int i = 0; i < (sNode->d)/2; i++)
		it++;
	sNode->idxMap.erase(it,sNode->idxMap.end());

	this->createNewStemNode(sNode->idxMap,end,D,sNode);
	this->writeToStemNodePage(sNode,node);

	delete sNode;
	delete newNode;
	return;
}

void IX_IndexHandle::getFirstValue(int pageNum, int *value)
{
	leafNode<int> *lNode = new leafNode<int>();
	this->readFromLeafNodePage(lNode,pageNum);
	map<int,int>::iterator it=lNode->myMap.begin();
	*value = (*it).first;

	delete lNode;
	return;
}

void IX_IndexHandle::writeVectorNodeToPage(vectorOfRidNode *vNode, void* page)
{
	*((int *)(char *)page) = vNode->size;
	*((int *)((char *)page+4)) = vNode->nextPage;
	int offset = 0;
	vector<RID> v = vNode->v;
	for(vector<RID>::iterator it = v.begin(); it!=v.end(); it++){
		*((RID*)((char *)page+8+offset)) = *it;
		offset += sizeof(RID);
	}

	return;
}

void IX_IndexHandle::writeLeafToMem(leafNode<int> * lNode,void* page)
{
	map<int,int>::iterator it=lNode->myMap.begin();
	*((int *)((char *)page+1)) = lNode->myMap.size();
	int offset = 0;
	for(; it!=lNode->myMap.end(); it++){
		*((int *)((char *)page+1+4+offset)) = (*it).first;
		offset += sizeof(int);
		*((int *)((char *)page+1+4+offset)) = (*it).second;
		offset += sizeof(int);
	}
	*((int *)((char *)page+1+4+offset)) = lNode->nextLeaf;
	*((int *)((char *)page+1+4+offset+4)) = lNode->d;

	return;
}

void IX_IndexHandle::writeStemToMem(stemNode<int> * sNode,void* page)
{
	map<int,int>::iterator it=sNode->idxMap.begin();
	*((int *)((char *)page+1)) = sNode->idxMap.size();
	int offset = 0;
	for(; it!=sNode->idxMap.end(); it++){
		*((int *)((char *)page+1+4+offset)) = (*it).first;
		offset += sizeof(int);
		*((int *)((char *)page+1+4+offset)) = (*it).second;
		offset += sizeof(int);
	}
	*((int *)((char *)page+1+4+offset)) = sNode->end;
	*((int *)((char *)page+1+4+offset+4)) = sNode->d;

	return;
}

void IX_IndexHandle::readVectorFromMem(vectorOfRidNode * vNode,void* page)
{
	vNode->size = *((int *)((char *)page));
	vNode->nextPage = *((int *)((char *)page+4));
	int offset = 0;
	vector<RID> v;
	RID rId;
	for(int i = 0; i < vNode->size; i++){
		rId = *((RID *)((char *)page+8+offset));
		offset += sizeof(RID);
		v.push_back(rId);
	}
	vNode->v = v;

	return;
}

void IX_IndexHandle::readLeafFromMem(leafNode<int> * lNode,void* page)
{
	lNode->sizeOfMap = *((int *)((char *)page+1));
	int offset = 0;
	int key = 0;
	int pointer = 0;
	for(int i = 0; i < lNode->sizeOfMap; i++){
		key = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		pointer = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		lNode->myMap.insert(pair<int,int>(key,pointer));
	}

	lNode->nextLeaf = *((int *)((char *)page+1+4+offset));
	lNode->d = *((int *)((char *)page+1+4+offset+4));
}

void IX_IndexHandle::readStemFromMem(stemNode<int> * sNode,void* page)
{
	sNode->sizeOfMap = *((int *)((char *)page+1));
	map<int,int>::iterator it;
	int offset = 0;
	int key = 0;
	int lPointer = -1;
	for(int i = 0; i < sNode->sizeOfMap; i++){
		key = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		lPointer = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		sNode->idxMap.insert(pair<int,int>(key,lPointer));
	}

	sNode->end = *((int *)((char *)page+1+4+offset));
	sNode->d = *((int *)((char *)page+1+4+offset+4));
}

int IX_IndexHandle::addToLeafNodePage(leafNode<int> * lNode){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '1';
	writeLeafToMem(lNode,page);
	pf_fileHandle.AppendPage(page);
	int pageNum = pf_fileHandle.GetNumberOfPages()-1;
	delete page;

	return pageNum;
}

void IX_IndexHandle::writeToVectorNodePage(vectorOfRidNode * vNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	writeVectorNodeToPage(vNode,page);
	pf_fileHandle.WritePage(pageNum,page);
	delete page;

	return;
}

void IX_IndexHandle::writeToLeafNodePage(leafNode<int> * lNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '1';
	writeLeafToMem(lNode,page);
	pf_fileHandle.WritePage(pageNum,page);
	delete page;

	return;
}

void IX_IndexHandle::readFromVectorNodePage(vectorOfRidNode *vNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(pageNum, page);
	readVectorFromMem(vNode,page);
	delete page;

	return;
}

void IX_IndexHandle::readFromLeafNodePage(leafNode<int> * lNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(pageNum, page);
	readLeafFromMem(lNode,page);
	delete page;

	return;
}

int IX_IndexHandle::addToVectorNodePage(vectorOfRidNode *v){
	void* page = new unsigned char[PF_PAGE_SIZE];
	this->writeVectorNodeToPage(v,page);
	pf_fileHandle.AppendPage(page);
	int pageNum = pf_fileHandle.GetNumberOfPages()-1;
	delete page;

	return pageNum;
}
int IX_IndexHandle::addToStemNodePage(stemNode<int> *sNode){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '0';
	writeStemToMem(sNode,page);
	pf_fileHandle.AppendPage(page);
	int pageNum = pf_fileHandle.GetNumberOfPages()-1;
	delete page;

	return pageNum;
}

void IX_IndexHandle::writeToStemNodePage(stemNode<int> *sNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '0';
	writeStemToMem(sNode,page);
	pf_fileHandle.WritePage(pageNum,page);
	delete page;

	return;
}

void IX_IndexHandle::readFromStemNodePage(stemNode<int> *sNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(pageNum, page);
	readStemFromMem(sNode,page);
	delete page;

	return;
}

RC IX_IndexHandle::ifNextDuplicate(map<int,int> myMap, int key, int pageOfVector, int n)
{
	//n+1 should be mext one index
	map<int,int>::iterator it = myMap.find(key);
	if(it == myMap.end())
		return -1;
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);
	if((pageOfVector == this->getNumOfVectorPage(it)-1) && n == vNode->size)
		return -1;

	else
		return 0;
}

int IX_IndexHandle::getNumOfVectorPage(map<int,int>::iterator it)
{
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);

	int numOfPage = 1;
	while(vNode->nextPage!=-1){
		numOfPage++;
		this->readFromVectorNodePage(vNode,vNode->nextPage);
	}


	return numOfPage;
}

int IX_IndexHandle::getNumOfVector(map<int,int>::iterator it, int pageOfVector)
{
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);
	vectorOfRidNode *nowNode = vNode;
	for(int i = 0; i < pageOfVector; i++)
		this->readFromVectorNodePage(nowNode,nowNode->nextPage);

	return nowNode->size;
}

//overload - float
map<float,int> IX_IndexHandle::insertRidToVectorOfRid(map<float,int> myMap, float key, RID rid)
{
	map<float,int>::iterator it = myMap.find(key);
	vector<RID> v;
	vectorOfRidNode *vNode;
	//unfind
	if(it == myMap.end()){
		v.push_back(rid);
		vNode = new vectorOfRidNode();
		this->createNewVectorNode(v,-1,vNode);
		int pageNum = this->addToVectorNodePage(vNode);
		myMap.insert(pair<float,int>(key,pageNum));
	}
	//find
	else{
		vNode = new vectorOfRidNode();
		this->readFromVectorNodePage(vNode,(*it).second);
		vectorOfRidNode *vNextNode;
		if(vNode->size == 400){
			if(vNode->nextPage == -1){
				v.push_back(rid);
				vectorOfRidNode *vNewNode = new vectorOfRidNode();
				this->createNewVectorNode(v,-1,vNewNode);
				int pageNum = this->addToVectorNodePage(vNewNode);
				vNode->nextPage = pageNum;
				this->createNewVectorNode(vNode->v,vNode->nextPage,vNode);
				this->writeToVectorNodePage(vNode,(*it).second);

				return myMap;
			}
			while(vNode->nextPage != -1){
				vNextNode = new vectorOfRidNode();
				this->readFromVectorNodePage(vNextNode,vNode->nextPage);
				if(vNextNode->size == 400){
					if(vNextNode->nextPage == -1){
						v.push_back(rid);
						vectorOfRidNode *vNewNode = new vectorOfRidNode();
						this->createNewVectorNode(v,-1,vNewNode);
						int pageNum = this->addToVectorNodePage(vNewNode);
						this->createNewVectorNode(vNextNode->v,pageNum,vNextNode);
						this->writeToVectorNodePage(vNextNode,vNode->nextPage);

						return myMap;
					}
					else
						vNode = vNextNode;
				}

				else{
					v = vNextNode->v;
					v.push_back(rid);
					this->createNewVectorNode(v,vNextNode->nextPage,vNextNode);
					this->writeToVectorNodePage(vNextNode,vNode->nextPage);

					return myMap;
				}
			}
		}
		v = vNode->v;
		v.push_back(rid);
		this->createNewVectorNode(v,vNode->nextPage,vNode);
		this->writeToVectorNodePage(vNode,(*it).second);
	}

	return myMap;
}

RID IX_IndexHandle::readRidFromVectorOfRid(map<float,int>::iterator it, int pageOfVector, int n)
{
	//n begin with 1
	int page = (*it).second;
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,page);
	for(int i = 0; i < pageOfVector; i++)
		this->readFromVectorNodePage(vNode,vNode->nextPage);

	vector<RID> v = vNode->v;

	return v[n];
}

RC IX_IndexHandle::removeEntryFromLeaf(float *key, int *node)
{
	leafNode<float> *lNode = new leafNode<float>();
	this->readFromLeafNodePage(lNode,*node);

	map<float,int>::iterator it;
	it = lNode->myMap.find(*(float*)key);

	//check if the value exists
	if(it == lNode->myMap.end())
		return -1;

	lNode->myMap.erase(it);

	this->createNewLeafNode(lNode->myMap,lNode->nextLeaf,D,lNode);

	this->writeToLeafNodePage(lNode,*node);

	delete lNode;
	return 0;
}

void IX_IndexHandle::createNewLeafNode(map<float,int> myMap, int nextLeaf, int d,leafNode<float> *newchildNode)
{
	newchildNode->sizeOfMap = myMap.size();
	newchildNode->myMap = myMap;
	newchildNode->nextLeaf = nextLeaf;
	newchildNode->d = d;

	return;
}

void IX_IndexHandle::createNewStemNode(map<float,int> idxMap, int end, int d,stemNode<float> *newchildNode)
{
	newchildNode->sizeOfMap = idxMap.size();
	newchildNode->idxMap = idxMap;
	newchildNode->end = end;
	newchildNode->d = d;

	return;
}

void IX_IndexHandle::splitLeafNode(int node, float *midValue, int *newchildentry)
{
	leafNode<float> *lNode = new leafNode<float>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<float> *newchildNode = new leafNode<float>();

	//split L: first d entries stay, rest move to new node L2
	map<float,int>::iterator it=lNode->myMap.begin();
	for(int i = 0; i < (lNode->d)/2; i++)
		it++;

	*midValue = (*it).first;
	map<float,int> tempLeafMap;

	for (; it != lNode->myMap.end(); it++ )
		tempLeafMap.insert(pair<float,int>((*it).first,(*it).second));
	this->createNewLeafNode(tempLeafMap,lNode->nextLeaf,D,newchildNode);

	//write to file
	*newchildentry = this->addToLeafNodePage(newchildNode);

	it=lNode->myMap.begin();
	for(int i = 0; i < (lNode->d)/2; i++)
		it++;
	lNode->myMap.erase(it,lNode->myMap.end());

	//set sibling pointer in L2
	lNode->nextLeaf = *newchildentry;

	this->writeToLeafNodePage(lNode,node);

	delete lNode;
	delete newchildNode;
	return;
}

void IX_IndexHandle::createNewNode(int node, int newchildentry, float midValue, int *newNode)
{
	map<float,int> tempMap;
	tempMap.insert(pair<float,int>(midValue,node));
	stemNode<float> *sNode = new stemNode<float>();
	this->createNewStemNode(tempMap,newchildentry,D,sNode);

	*newNode = this->addToStemNodePage(sNode);

	delete sNode;
	return;
}

void IX_IndexHandle::splitStemNode(int node, float *midValue, int *newchildentry)
{
	stemNode<float> *sNode = new stemNode<float>();
	this->readFromStemNodePage(sNode,node);

	//first d key values and d+1 nodepointers stay,
	map<float,int>::iterator it=sNode->idxMap.begin();
	for(int i = 0; i < (sNode->d)/2; i++)
		it++;

	*midValue = (*it).first;
	it++;

	//last d keys and d+1 pointers move to new node, N2
	stemNode<float> *newNode = new stemNode<float>();
	map<float,int> tempMap;
	for(; it!=sNode->idxMap.end(); it++)
		tempMap.insert(pair<float,int>((*it).first,(*it).second));
	this->createNewStemNode(tempMap,sNode->end,D,newNode);
	*newchildentry = this->addToStemNodePage(newNode);

	it = sNode->idxMap.find(*midValue);
	int end = (*it).second;
	it = sNode->idxMap.begin();
	for(int i = 0; i < (sNode->d)/2; i++)
		it++;
	sNode->idxMap.erase(it,sNode->idxMap.end());

	this->createNewStemNode(sNode->idxMap,end,D,sNode);
	this->writeToStemNodePage(sNode,node);

	delete sNode;
	delete newNode;
	return;
}

void IX_IndexHandle::getFirstValue(int pageNum, float *value)
{
	leafNode<float> *lNode = new leafNode<float>();
	this->readFromLeafNodePage(lNode,pageNum);
	map<float,int>::iterator it=lNode->myMap.begin();
	*value = (*it).first;

	delete lNode;
	return;
}

void IX_IndexHandle::writeLeafToMem(leafNode<float> * lNode,void* page)
{
	map<float,int>::iterator it=lNode->myMap.begin();
	*((int *)((char *)page+1)) = lNode->myMap.size();
	int offset = 0;
	for(; it!=lNode->myMap.end(); it++){
		*((float *)((char *)page+1+4+offset)) = (*it).first;
		offset += sizeof(float);
		*((int *)((char *)page+1+4+offset)) = (*it).second;
		offset += sizeof(int);
	}
	*((int *)((char *)page+1+4+offset)) = lNode->nextLeaf;
	*((int *)((char *)page+1+4+offset+4)) = lNode->d;

	return;
}

void IX_IndexHandle::writeStemToMem(stemNode<float> * sNode,void* page)
{
	map<float,int>::iterator it=sNode->idxMap.begin();
	*((int *)((char *)page+1)) = sNode->idxMap.size();
	int offset = 0;
	for(; it!=sNode->idxMap.end(); it++){
		*((float *)((char *)page+1+4+offset)) = (*it).first;
		offset += sizeof(float);
		*((int *)((char *)page+1+4+offset)) = (*it).second;
		offset += sizeof(int);
	}
	*((int *)((char *)page+1+4+offset)) = sNode->end;
	*((int *)((char *)page+1+4+offset+4)) = sNode->d;

	return;
}

void IX_IndexHandle::readLeafFromMem(leafNode<float> * lNode,void* page)
{
	lNode->sizeOfMap = *((int *)((char *)page+1));
	int offset = 0;
	float key = 0;
	int pointer = 0;
	for(int i = 0; i < lNode->sizeOfMap; i++){
		key = *((float *)((char *)page+1+4+offset));
		offset += sizeof(float);
		pointer = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		lNode->myMap.insert(pair<float,int>(key,pointer));
	}

	lNode->nextLeaf = *((int *)((char *)page+1+4+offset));
	lNode->d = *((int *)((char *)page+1+4+offset+4));
}

void IX_IndexHandle::readStemFromMem(stemNode<float> * sNode,void* page)
{
	sNode->sizeOfMap = *((int *)((char *)page+1));
	map<float,int>::iterator it;
	int offset = 0;
	int key = 0;
	int lPointer = -1;
	for(int i = 0; i < sNode->sizeOfMap; i++){
		key = *((float *)((char *)page+1+4+offset));
		offset += sizeof(float);
		lPointer = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		sNode->idxMap.insert(pair<float,int>(key,lPointer));
	}

	sNode->end = *((int *)((char *)page+1+4+offset));
	sNode->d = *((int *)((char *)page+1+4+offset+4));
}

int IX_IndexHandle::addToLeafNodePage(leafNode<float> * lNode){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '1';
	writeLeafToMem(lNode,page);
	pf_fileHandle.AppendPage(page);
	int pageNum = pf_fileHandle.GetNumberOfPages()-1;
	delete page;

	return pageNum;
}

void IX_IndexHandle::writeToLeafNodePage(leafNode<float> * lNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '1';
	writeLeafToMem(lNode,page);
	pf_fileHandle.WritePage(pageNum,page);
	delete page;

	return;
}

void IX_IndexHandle::readFromLeafNodePage(leafNode<float> * lNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(pageNum, page);
	readLeafFromMem(lNode,page);
	delete page;

	return;
}

int IX_IndexHandle::addToStemNodePage(stemNode<float> *sNode){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '0';
	writeStemToMem(sNode,page);
	pf_fileHandle.AppendPage(page);
	int pageNum = pf_fileHandle.GetNumberOfPages()-1;
	delete page;

	stemNode<float> * testNode = new stemNode<float>();
	this->readFromStemNodePage(testNode,2);
	delete testNode;

	return pageNum;
}

void IX_IndexHandle::writeToStemNodePage(stemNode<float> *sNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '0';
	writeStemToMem(sNode,page);
	pf_fileHandle.WritePage(pageNum,page);
	delete page;

	return;
}

void IX_IndexHandle::readFromStemNodePage(stemNode<float> *sNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(pageNum, page);
	readStemFromMem(sNode,page);
	delete page;

	return;
}

RC IX_IndexHandle::ifNextDuplicate(map<float,int> myMap, float key, int pageOfVector, int n)
{
	//n+1 should be mext one index
	map<float,int>::iterator it = myMap.find(key);
	if(it == myMap.end())
		return -1;
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);
	if((pageOfVector == this->getNumOfVectorPage(it)-1) && n == vNode->size)
		return -1;

	else
		return 0;
}

int IX_IndexHandle::getNumOfVectorPage(map<float,int>::iterator it)
{
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);

	int numOfPage = 1;
	while(vNode->nextPage!=-1){
		numOfPage++;
		this->readFromVectorNodePage(vNode,vNode->nextPage);
	}


	return numOfPage;
}

int IX_IndexHandle::getNumOfVector(map<float,int>::iterator it, int pageOfVector)
{
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);
	vectorOfRidNode *nowNode = vNode;
	for(int i = 0; i < pageOfVector; i++)
		this->readFromVectorNodePage(nowNode,nowNode->nextPage);

	return nowNode->size;
}

void IX_IndexHandle::removeOldchildentryFromNode_f(int node, int oldchildentry)
{
	stemNode<float> *sNode = new stemNode<float>();
	this->readFromStemNodePage(sNode,node);

	//find the corresponding entry
	map<float,int>::iterator it;
	for(it = sNode->idxMap.begin(); it!=sNode->idxMap.end(); it++){
		if((*it).second == oldchildentry)
			break;
	}

	it--;
	map<float,int>::iterator pre = it;
	it++;

	sNode->idxMap.find((*it).first)->second = (*pre).second;
	sNode->idxMap.erase(pre);

	this->createNewStemNode(sNode->idxMap,sNode->end,D,sNode);
	this->writeToStemNodePage(sNode,node);
	delete sNode;
	return;
}

void IX_IndexHandle::replaceKeyValue_f(int parent, int sibling)
{
	leafNode<float> *siblingNode = new leafNode<float>();
	this->readFromLeafNodePage(siblingNode,sibling);

	stemNode<float> *pNode = new stemNode<float>();
	this->readFromStemNodePage(pNode,parent);

	map<float,int>::iterator it=siblingNode->myMap.begin();

	//find entry in parent for node for sibling
	map<float,int>::iterator pit = pNode->idxMap.begin();
	for(;pit!=pNode->idxMap.end();pit++){
		if((*pit).second == sibling)
			break;
	}
	pit--;

	//problem
	pNode->idxMap.insert(pair<float,int>((*it).first,(*pit).second));
	pNode->idxMap.erase((*pit).first);

	this->createNewStemNode(pNode->idxMap,pNode->end,D,pNode);
	this->writeToStemNodePage(pNode,parent);

	delete siblingNode;
	delete pNode;
	return;
}

void IX_IndexHandle::findLeafSibling_f(int parent, int node, int *sibling)
{
	stemNode<float> *pNode = new stemNode<float>();
	this->readFromStemNodePage(pNode,parent);

	map<float,int>::iterator it;
	for(it = pNode->idxMap.begin(); it!=pNode->idxMap.end(); it++){
		if((*it).second == node)
			break;
	}

	it++;
	if(it == pNode->idxMap.end())
		*sibling = pNode->end;
	else
		*sibling = (*it).second;

	delete pNode;
	return;
}

void IX_IndexHandle::findStemSibling_f(int parent, int node, int *sibling)
{
	stemNode<float> *pNode = new stemNode<float>();
	this->readFromStemNodePage(pNode,parent);

	map<float,int>::iterator it;
	for(it = pNode->idxMap.begin(); it!=pNode->idxMap.end(); it++){
		if((*it).second == node)
			break;
	}

	it++;
	if(it == pNode->idxMap.end()){
		*sibling = pNode->end;
		return;
	}

	*sibling = (*it).second;

	delete pNode;
	return;
}

void IX_IndexHandle::redistributeNS_Leaf_f(int node, int sibling)
{
	leafNode<float> *lNode = new leafNode<float>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<float> *siblingNode = new leafNode<float>();
	this->readFromLeafNodePage(siblingNode,sibling);

	int numsOfEntries = (lNode->myMap.size() + siblingNode->myMap.size())/2;
	int numsOfMoves = numsOfEntries - lNode->myMap.size();

	map<float,int>::iterator it;
	map<float,int>::iterator temp;
	it = siblingNode->myMap.begin();

	for(int i = 0;i < numsOfMoves; i++){
		lNode->myMap.insert(pair<float,int>((*it).first,(*it).second));
		//map<int,int> tempMap = lNode->myMap;
		//tempMap = this->insertRidToVectorOfRid(tempMap, (*it).first,(*it).second);

		temp = it;
		temp++;
		siblingNode->myMap.erase(it);
		it = temp;
	}

	this->createNewLeafNode(lNode->myMap,lNode->nextLeaf,D,lNode);
	this->writeToLeafNodePage(lNode,node);

	this->createNewLeafNode(siblingNode->myMap,siblingNode->nextLeaf,D,siblingNode);
	this->writeToLeafNodePage(siblingNode,sibling);

	/*leafNode<float> *testNode = new leafNode<float>();
	this->readFromLeafNodePage(testNode,node);
	for(map<float,int>::iterator it1 = testNode->myMap.begin();it1!=testNode->myMap.end();it1++)
		cout << (*it1).first <<endl;
	delete testNode;*/

	delete lNode;
	delete siblingNode;

	return;
}

void IX_IndexHandle::redistributeNS_Stem_f(int node, int sibling, int parent)
{
	stemNode<float> *sNode = new stemNode<float>();
	this->readFromStemNodePage(sNode,node);

	stemNode<float> *siblingNode = new stemNode<float>();
	this->readFromStemNodePage(siblingNode,sibling);

	stemNode<float> *pNode = new stemNode<float>();
	this->readFromStemNodePage(pNode,parent);

	int numsOfEntries = (sNode->idxMap.size() + siblingNode->idxMap.size())/2;
	int numsOfMoves = numsOfEntries - sNode->idxMap.size();

 	map<float,int>::iterator it;
	map<float,int>::iterator temp;
	it = siblingNode->idxMap.begin();

	for(int i = 0;i < numsOfMoves-1; i++){
		sNode->idxMap.insert(pair<float,int>((*it).first,(*it).second));
		temp = it;
		temp++;
		siblingNode->idxMap.erase(it);
		it = temp;
	}

	map<float,int>::iterator pit = pNode->idxMap.begin();
	for(;pit!=pNode->idxMap.end();pit++){
		if((*pit).second == node)
			break;
	}
	sNode->idxMap.insert(pair<float,int>((*pit).first,sNode->end));
	this->createNewStemNode(sNode->idxMap,(*it).second,D,sNode);
	this->writeToStemNodePage(sNode,node);

	int second = (*pit).second;
	pNode->idxMap.erase(pNode->idxMap.find((*pit).first));
	pNode->idxMap.insert(pair<float,int>((*it).first,second));
	this->createNewStemNode(pNode->idxMap,pNode->end,D,sNode);
	this->writeToStemNodePage(pNode,parent);

	siblingNode->idxMap.erase(it);
	this->createNewStemNode(siblingNode->idxMap,siblingNode->end,D,siblingNode);
	this->writeToStemNodePage(siblingNode,sibling);


	delete sNode;
	delete siblingNode;
	delete pNode;
	return;
}

void IX_IndexHandle::mergeLeafNodes_f(int node, int sibling)
{
	leafNode<float> *lNode = new leafNode<float>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<float> *siblingNode = new leafNode<float>();
	this->readFromLeafNodePage(siblingNode,sibling);

	//move all the entries from S to node on the left
	//discard empty node
	map<float,int>::iterator it=siblingNode->myMap.begin();
	map<float,int>::iterator temp;
	for(; it!=siblingNode->myMap.end();it++)
		lNode->myMap.insert(pair<float,int>((*it).first,(*it).second));

	//discard empty, adjust sibling pointers
	this->createNewLeafNode(lNode->myMap,siblingNode->nextLeaf,D,lNode);
	this->writeToLeafNodePage(lNode,node);

	delete lNode;
	delete siblingNode;

	return;
}

void IX_IndexHandle::mergeStemNodes_f(int node, int sibling, int parent)
{
	stemNode<float> *sNode = new stemNode<float>();
	this->readFromStemNodePage(sNode,node);

	stemNode<float> *siblingNode = new stemNode<float>();
	this->readFromStemNodePage(siblingNode,sibling);

	stemNode<float> *pNode = new stemNode<float>();
	this->readFromStemNodePage(pNode,parent);

	//pull splitting key from parent down into node on the left
	for(map<float,int>::iterator pit=pNode->idxMap.begin();pit!=pNode->idxMap.end();pit++){
		if((*pit).second == node){
			sNode->idxMap.insert(pair<float,int>((*pit).first,sNode->end));
			break;
		}
	}

	//move all the entries from S to node on the left
	//discard empty node
	map<float,int>::iterator it=siblingNode->idxMap.begin();
	map<float,int>::iterator temp;
	for(; it!=siblingNode->idxMap.end();it++)
		sNode->idxMap.insert(pair<float,int>((*it).first,(*it).second));

	this->createNewStemNode(sNode->idxMap,siblingNode->end,D,sNode);
	this->writeToStemNodePage(sNode,node);

	delete sNode;
	delete siblingNode;
	delete pNode;

	return;
}

//overload - string
map<string,int> IX_IndexHandle::insertRidToVectorOfRid(map<string,int> myMap, string key, RID rid)
{
	map<string,int>::iterator it = myMap.find(key);
	vector<RID> v;
	vectorOfRidNode *vNode;
	//unfind
	if(it == myMap.end()){
		v.push_back(rid);
		vNode = new vectorOfRidNode();
		this->createNewVectorNode(v,-1,vNode);
		int pageNum = this->addToVectorNodePage(vNode);
		myMap.insert(pair<string,int>(key,pageNum));
	}
	//find
	else{
		vNode = new vectorOfRidNode();
		this->readFromVectorNodePage(vNode,(*it).second);
		vectorOfRidNode *vNextNode;
		//200
		if(vNode->size == 20){
			if(vNode->nextPage == -1){
				v.push_back(rid);
				vectorOfRidNode *vNewNode = new vectorOfRidNode();
				this->createNewVectorNode(v,-1,vNewNode);
				int pageNum = this->addToVectorNodePage(vNewNode);
				vNode->nextPage = pageNum;
				this->createNewVectorNode(vNode->v,vNode->nextPage,vNode);
				this->writeToVectorNodePage(vNode,(*it).second);

				return myMap;
			}
			while(vNode->nextPage != -1){
				vNextNode = new vectorOfRidNode();
				this->readFromVectorNodePage(vNextNode,vNode->nextPage);
				if(vNextNode->size == 20){
					if(vNextNode->nextPage == -1){
						v.push_back(rid);
						vectorOfRidNode *vNewNode = new vectorOfRidNode();
						this->createNewVectorNode(v,-1,vNewNode);
						int pageNum = this->addToVectorNodePage(vNewNode);
						this->createNewVectorNode(vNextNode->v,pageNum,vNextNode);
						this->writeToVectorNodePage(vNextNode,vNode->nextPage);

						return myMap;
					}
					else
						vNode = vNextNode;
				}

				else{
					v = vNextNode->v;
					v.push_back(rid);
					this->createNewVectorNode(v,vNextNode->nextPage,vNextNode);
					this->writeToVectorNodePage(vNextNode,vNode->nextPage);

					return myMap;
				}
			}
		}
		v = vNode->v;
		v.push_back(rid);
		this->createNewVectorNode(v,vNode->nextPage,vNode);
		this->writeToVectorNodePage(vNode,(*it).second);
	}

	return myMap;
}

RID IX_IndexHandle::readRidFromVectorOfRid(map<string,int>::iterator it, int pageOfVector, int n)
{
	//n begin with 1
	int page = (*it).second;
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,page);
	for(int i = 0; i < pageOfVector; i++)
		this->readFromVectorNodePage(vNode,vNode->nextPage);

	vector<RID> v = vNode->v;

	return v[n];
}

int IX_IndexHandle::addToLeafNodePage(leafNode<string> * lNode){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '1';
	writeLeafToMem(lNode,page);
	pf_fileHandle.AppendPage(page);
	int pageNum = pf_fileHandle.GetNumberOfPages()-1;
	delete page;

	return pageNum;
}

void IX_IndexHandle::writeLeafToMem(leafNode<string> * lNode,void* page)
{
	map<string,int>::iterator it=lNode->myMap.begin();
	*((int *)((char *)page+1)) = lNode->myMap.size();
	int offset = 0;
	for(; it!=lNode->myMap.end(); it++){
		string s = (*it).first;
		*((int *)((char *)page+1+4+offset)) = s.length();
		offset += sizeof(int);
		memcpy((char *)page+1+4+offset,s.c_str(),s.length());
		offset += s.length();
		*((int *)((char *)page+1+4+offset)) = (*it).second;
		offset += sizeof(int);
	}
	*((int *)((char *)page+1+4+offset)) = lNode->nextLeaf;
	*((int *)((char *)page+1+4+offset+4)) = lNode->d;

	return;
}

void IX_IndexHandle::readLeafFromMem(leafNode<string> * lNode,void* page)
{
	lNode->sizeOfMap = *((int *)((char *)page+1));
	int offset = 0;
	int length = 0;
	int pointer = 0;
	char *c = (char *)malloc(PF_PAGE_SIZE);
	for(int i = 0; i < lNode->sizeOfMap; i++){
		length = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		memcpy(c,(char *)page+1+4+offset,length);
		c[length] = '\0';
		string key(c);
		offset += length;
		pointer = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		lNode->myMap.insert(pair<string,int>(key,pointer));
	}
	free(c);

	lNode->nextLeaf = *((int *)((char *)page+1+4+offset));
	lNode->d = *((int *)((char *)page+1+4+offset+4));
}

void IX_IndexHandle::readFromLeafNodePage(leafNode<string> * lNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(pageNum, page);
	readLeafFromMem(lNode,page);
	delete page;

	return;
}

void IX_IndexHandle::writeToLeafNodePage(leafNode<string> * lNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '1';
	writeLeafToMem(lNode,page);
	pf_fileHandle.WritePage(pageNum,page);
	delete page;

	return;
}

void IX_IndexHandle::createNewLeafNode(map<string,int> myMap, int nextLeaf, int d,leafNode<string> *newchildNode)
{
	newchildNode->sizeOfMap = myMap.size();
	newchildNode->myMap = myMap;
	newchildNode->nextLeaf = nextLeaf;
	newchildNode->d = d;

	return;
}

void IX_IndexHandle::createNewStemNode(map<string,int> idxMap, int end, int d,stemNode<string> *newchildNode)
{
	newchildNode->sizeOfMap = idxMap.size();
	newchildNode->idxMap = idxMap;
	newchildNode->end = end;
	newchildNode->d = d;

	return;
}

int IX_IndexHandle::addToStemNodePage(stemNode<string> *sNode){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '0';
	writeStemToMem(sNode,page);
	pf_fileHandle.AppendPage(page);
	int pageNum = pf_fileHandle.GetNumberOfPages()-1;
	delete page;

	return pageNum;
}

void IX_IndexHandle::writeToStemNodePage(stemNode<string> *sNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	*(char *)page = '0';
	writeStemToMem(sNode,page);
	pf_fileHandle.WritePage(pageNum,page);
	delete page;

	return;
}

void IX_IndexHandle::readFromStemNodePage(stemNode<string> *sNode, int pageNum){

	void* page = new unsigned char[PF_PAGE_SIZE];
	pf_fileHandle.ReadPage(pageNum, page);
	readStemFromMem(sNode,page);
	delete page;

	return;
}

void IX_IndexHandle::writeStemToMem(stemNode<string> * sNode,void* page)
{
	map<string,int>::iterator it=sNode->idxMap.begin();
	*((int *)((char *)page+1)) = sNode->idxMap.size();
	int offset = 0;
	for(; it!=sNode->idxMap.end(); it++){
		string s = (*it).first;
		*((int *)((char *)page+1+4+offset)) = s.length();
		offset += sizeof(int);
		memcpy((char *)page+1+4+offset,s.c_str(),s.length());
		offset += s.length();
		*((int *)((char *)page+1+4+offset)) = (*it).second;
		offset += sizeof(int);
	}
	*((int *)((char *)page+1+4+offset)) = sNode->end;
	*((int *)((char *)page+1+4+offset+4)) = sNode->d;

	return;
}

void IX_IndexHandle::readStemFromMem(stemNode<string> * sNode,void* page)
{
	sNode->sizeOfMap = *((int *)((char *)page+1));
	map<string,int>::iterator it;
	int offset = 0;
	int length = 0;
	int lPointer = -1;
	char *c = (char *)malloc(PF_PAGE_SIZE);
	for(int i = 0; i < sNode->sizeOfMap; i++){
		length = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		memcpy(c,(char *)page+1+4+offset,length);
		c[length] = '\0';
		string key(c);
		offset += length;
		lPointer = *((int *)((char *)page+1+4+offset));
		offset += sizeof(int);
		sNode->idxMap.insert(pair<string,int>(key,lPointer));
	}
	free(c);

	sNode->end = *((int *)((char *)page+1+4+offset));
	sNode->d = *((int *)((char *)page+1+4+offset+4));
}

void IX_IndexHandle::splitStemNode(int node, string *midValue, int *newchildentry)
{
	stemNode<string> *sNode = new stemNode<string>();
	this->readFromStemNodePage(sNode,node);

	//first d key values and d+1 nodepointers stay,
	map<string,int>::iterator it=sNode->idxMap.begin();
	for(int i = 0; i < (sNode->d)/2; i++)
		it++;

	*midValue = (*it).first;
	it++;

	//last d keys and d+1 pointers move to new node, N2
	stemNode<string> *newNode = new stemNode<string>();
	map<string,int> tempMap;
	for(; it!=sNode->idxMap.end(); it++)
		tempMap.insert(pair<string,int>((*it).first,(*it).second));
	this->createNewStemNode(tempMap,sNode->end,4,newNode);
	*newchildentry = this->addToStemNodePage(newNode);

	it = sNode->idxMap.find(*midValue);
	int end = (*it).second;
	it = sNode->idxMap.begin();
	for(int i = 0; i < (sNode->d)/2; i++)
		it++;
	sNode->idxMap.erase(it,sNode->idxMap.end());

	this->createNewStemNode(sNode->idxMap,end,D,sNode);
	this->writeToStemNodePage(sNode,node);

	delete sNode;
	delete newNode;
	return;
}

void IX_IndexHandle::getFirstValue(int pageNum, string *value)
{
	leafNode<string> *lNode = new leafNode<string>();
	this->readFromLeafNodePage(lNode,pageNum);
	map<string,int>::iterator it=lNode->myMap.begin();
	*value = (*it).first;

	delete lNode;
	return;
}

void IX_IndexHandle::splitLeafNode(int node, string *midValue, int *newchildentry)
{
	leafNode<string> *lNode = new leafNode<string>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<string> *newchildNode = new leafNode<string>();

	//split L: first d entries stay, rest move to new node L2
	map<string,int>::iterator it=lNode->myMap.begin();
	for(int i = 0; i < (lNode->d)/2; i++)
		it++;

	*midValue = (*it).first;
	map<string,int> tempLeafMap;

	for (; it != lNode->myMap.end(); it++ )
		tempLeafMap.insert(pair<string,int>((*it).first,(*it).second));
	this->createNewLeafNode(tempLeafMap,lNode->nextLeaf,D,newchildNode);

	//write to file
	*newchildentry = this->addToLeafNodePage(newchildNode);

	it=lNode->myMap.begin();
	for(int i = 0; i < (lNode->d)/2; i++)
		it++;
	lNode->myMap.erase(it,lNode->myMap.end());

	//set sibling pointer in L2
	this->createNewLeafNode(lNode->myMap,*newchildentry,D,lNode);
	this->writeToLeafNodePage(lNode,node);

	delete lNode;
	delete newchildNode;
	return;
}

void IX_IndexHandle::createNewNode(int node, int newchildentry, string midValue, int *newNode)
{
	map<string,int> tempMap;
	tempMap.insert(pair<string,int>(midValue,node));
	stemNode<string> *sNode = new stemNode<string>();
	this->createNewStemNode(tempMap,newchildentry,D,sNode);

	*newNode = this->addToStemNodePage(sNode);

	delete sNode;
	return;
}

RC IX_IndexHandle::ifNextDuplicate(map<string,int> myMap, string key, int pageOfVector, int n)
{
	//n+1 should be mext one index
	map<string,int>::iterator it = myMap.find(key);
	if(it == myMap.end())
		return -1;
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);
	if((pageOfVector == this->getNumOfVectorPage(it)-1) && n == vNode->size)
		return -1;

	else
		return 0;
}

int IX_IndexHandle::getNumOfVectorPage(map<string,int>::iterator it)
{
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);

	int numOfPage = 1;
	while(vNode->nextPage!=-1){
		numOfPage++;
		this->readFromVectorNodePage(vNode,vNode->nextPage);
	}


	return numOfPage;
}

int IX_IndexHandle::getNumOfVector(map<string,int>::iterator it, int pageOfVector)
{
	vectorOfRidNode *vNode = new vectorOfRidNode();
	this->readFromVectorNodePage(vNode,(*it).second);
	vectorOfRidNode *nowNode = vNode;
	for(int i = 0; i < pageOfVector; i++)
		this->readFromVectorNodePage(nowNode,nowNode->nextPage);

	return nowNode->size;
}

//delete - string
RC IX_IndexHandle::removeEntryFromLeaf(string *key, int *node)
{
	leafNode<string> *lNode = new leafNode<string>();
	this->readFromLeafNodePage(lNode,*node);

	map<string,int>::iterator it;
	it = lNode->myMap.find(*key);

	//check if the value exists
	if(it == lNode->myMap.end())
		return -1;

	lNode->myMap.erase(it);

	this->createNewLeafNode(lNode->myMap,lNode->nextLeaf,D,lNode);

	this->writeToLeafNodePage(lNode,*node);

	delete lNode;
	return 0;
}

void IX_IndexHandle::removeOldchildentryFromNode_s(int node, int oldchildentry)
{
	stemNode<string> *sNode = new stemNode<string>();
	this->readFromStemNodePage(sNode,node);

	//find the corresponding entry
	map<string,int>::iterator it;
	for(it = sNode->idxMap.begin(); it!=sNode->idxMap.end(); it++){
		if((*it).second == oldchildentry)
			break;
	}

	it--;
	map<string,int>::iterator pre = it;
	it++;

	sNode->idxMap.find((*it).first)->second = (*pre).second;
	sNode->idxMap.erase(pre);

	this->createNewStemNode(sNode->idxMap,sNode->end,D,sNode);
	this->writeToStemNodePage(sNode,node);
	delete sNode;
	return;
}

void IX_IndexHandle::replaceKeyValue_s(int parent, int sibling)
{
	leafNode<string> *siblingNode = new leafNode<string>();
	this->readFromLeafNodePage(siblingNode,sibling);

	stemNode<string> *pNode = new stemNode<string>();
	this->readFromStemNodePage(pNode,parent);

	map<string,int>::iterator it=siblingNode->myMap.begin();

	//find entry in parent for node for sibling
	map<string,int>::iterator pit = pNode->idxMap.begin();
	for(;pit!=pNode->idxMap.end();pit++){
		if((*pit).second == sibling)
			break;
	}
	pit--;

	//problem
	pNode->idxMap.insert(pair<string,int>((*it).first,(*pit).second));
	pNode->idxMap.erase((*pit).first);

	this->createNewStemNode(pNode->idxMap,pNode->end,D,pNode);
	this->writeToStemNodePage(pNode,parent);

	delete siblingNode;
	delete pNode;
	return;
}

void IX_IndexHandle::findLeafSibling_s(int parent, int node, int *sibling)
{
	stemNode<string> *pNode = new stemNode<string>();
	this->readFromStemNodePage(pNode,parent);

	map<string,int>::iterator it;
	for(it = pNode->idxMap.begin(); it!=pNode->idxMap.end(); it++){
		if((*it).second == node)
			break;
	}

	it++;
	if(it == pNode->idxMap.end())
		*sibling = pNode->end;
	else
		*sibling = (*it).second;

	delete pNode;
	return;
}

void IX_IndexHandle::findStemSibling_s(int parent, int node, int *sibling)
{
	stemNode<string> *pNode = new stemNode<string>();
	this->readFromStemNodePage(pNode,parent);

	map<string,int>::iterator it;
	for(it = pNode->idxMap.begin(); it!=pNode->idxMap.end(); it++){
		if((*it).second == node)
			break;
	}

	it++;
	if(it == pNode->idxMap.end()){
		*sibling = pNode->end;
		return;
	}

	*sibling = (*it).second;

	delete pNode;
	return;
}

void IX_IndexHandle::redistributeNS_Leaf_s(int node, int sibling)
{
	leafNode<string> *lNode = new leafNode<string>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<string> *siblingNode = new leafNode<string>();
	this->readFromLeafNodePage(siblingNode,sibling);

	int numsOfEntries = (lNode->myMap.size() + siblingNode->myMap.size())/2;
	int numsOfMoves = numsOfEntries - lNode->myMap.size();

	map<string,int>::iterator it;
	map<string,int>::iterator temp;
	it = siblingNode->myMap.begin();

	for(int i = 0;i < numsOfMoves; i++){
		lNode->myMap.insert(pair<string,int>((*it).first,(*it).second));

		temp = it;
		temp++;
		siblingNode->myMap.erase(it);
		it = temp;
	}

	this->createNewLeafNode(lNode->myMap,lNode->nextLeaf,D,lNode);
	this->writeToLeafNodePage(lNode,node);

	this->createNewLeafNode(siblingNode->myMap,siblingNode->nextLeaf,D,siblingNode);

	delete lNode;
	delete siblingNode;

	return;
}

void IX_IndexHandle::redistributeNS_Stem_s(int node, int sibling, int parent)
{
	stemNode<string> *sNode = new stemNode<string>();
	this->readFromStemNodePage(sNode,node);

	stemNode<string> *siblingNode = new stemNode<string>();
	this->readFromStemNodePage(siblingNode,sibling);

	stemNode<string> *pNode = new stemNode<string>();
	this->readFromStemNodePage(pNode,parent);

	int numsOfEntries = (sNode->idxMap.size() + siblingNode->idxMap.size())/2;
	int numsOfMoves = numsOfEntries - sNode->idxMap.size();

 	map<string,int>::iterator it;
	map<string,int>::iterator temp;
	it = siblingNode->idxMap.begin();

	for(int i = 0;i < numsOfMoves-1; i++){
		sNode->idxMap.insert(pair<string,int>((*it).first,(*it).second));
		temp = it;
		temp++;
		siblingNode->idxMap.erase(it);
		it = temp;
	}

	map<string,int>::iterator pit = pNode->idxMap.begin();
	for(;pit!=pNode->idxMap.end();pit++){
		if((*pit).second == node)
			break;
	}
	sNode->idxMap.insert(pair<string,int>((*pit).first,sNode->end));
	this->createNewStemNode(sNode->idxMap,(*it).second,D,sNode);
	this->writeToStemNodePage(sNode,node);

	int second = (*pit).second;
	pNode->idxMap.erase(pNode->idxMap.find((*pit).first));
	pNode->idxMap.insert(pair<string,int>((*it).first,second));
	this->createNewStemNode(pNode->idxMap,pNode->end,D,sNode);
	this->writeToStemNodePage(pNode,parent);

	siblingNode->idxMap.erase(it);
	this->createNewStemNode(siblingNode->idxMap,siblingNode->end,D,siblingNode);
	this->writeToStemNodePage(siblingNode,sibling);


	delete sNode;
	delete siblingNode;
	delete pNode;
	return;
}

void IX_IndexHandle::mergeLeafNodes_s(int node, int sibling)
{
	leafNode<string> *lNode = new leafNode<string>();
	this->readFromLeafNodePage(lNode,node);

	leafNode<string> *siblingNode = new leafNode<string>();
	this->readFromLeafNodePage(siblingNode,sibling);

	//move all the entries from S to node on the left
	//discard empty node
	map<string,int>::iterator it=siblingNode->myMap.begin();
	map<string,int>::iterator temp;
	for(; it!=siblingNode->myMap.end();it++)
		lNode->myMap.insert(pair<string,int>((*it).first,(*it).second));

	//discard empty, adjust sibling pointers
	this->createNewLeafNode(lNode->myMap,siblingNode->nextLeaf,D,lNode);
	this->writeToLeafNodePage(lNode,node);

	delete lNode;
	delete siblingNode;

	return;
}

void IX_IndexHandle::mergeStemNodes_s(int node, int sibling, int parent)
{
	stemNode<string> *sNode = new stemNode<string>();
	this->readFromStemNodePage(sNode,node);

	stemNode<string> *siblingNode = new stemNode<string>();
	this->readFromStemNodePage(siblingNode,sibling);

	stemNode<string> *pNode = new stemNode<string>();
	this->readFromStemNodePage(pNode,parent);

	//pull splitting key from parent down into node on the left
	for(map<string,int>::iterator pit=pNode->idxMap.begin();pit!=pNode->idxMap.end();pit++){
		if((*pit).second == node){
			sNode->idxMap.insert(pair<string,int>((*pit).first,sNode->end));
			break;
		}
	}

	//move all the entries from S to node on the left
	//discard empty node
	map<string,int>::iterator it=siblingNode->idxMap.begin();
	map<string,int>::iterator temp;
	for(; it!=siblingNode->idxMap.end();it++)
		sNode->idxMap.insert(pair<string,int>((*it).first,(*it).second));

	this->createNewStemNode(sNode->idxMap,siblingNode->end,D,sNode);
	this->writeToStemNodePage(sNode,node);

	delete sNode;
	delete siblingNode;
	delete pNode;

	return;
}

PF_FileHandle IX_IndexHandle::readfHandle()
{
	return pf_fileHandle;
}

void IX_IndexHandle::setfHandle(PF_FileHandle pf_fileHandle)
{
	this->pf_fileHandle = pf_fileHandle;
}

string IX_IndexHandle::readfName()
{
	return fileName;
}

void IX_IndexHandle::setfName(string fileName)
{
	this->fileName = fileName;
}

string IX_IndexHandle::readtName()
{
	return tableName;
}

void IX_IndexHandle::settName(string tableName)
{
	this->tableName = tableName;
}

int IX_IndexHandle::getRoot()
{
	return root;
}

IX_IndexScan::IX_IndexScan()
{
	//indexHandle == NULL;

	compOp = EQ_OP;
	val = -1;
	val_f = -1;
	PF_FileHandle pf_fileHandle;
	pf_fileHandle.file = NULL;//.setFile(0);
	idxHandle.setfHandle(pf_fileHandle);
	currentPage = -1;
	isVirgin = true;
	currentKey = -1;
	nextKey = -1;
	currentKey_f = -1;
	currentKey_s = "";
	nextKey_f = -1;
	nextKey_s = "";

	pageOfVector = 0;
	n = 0;
}

IX_IndexScan::~IX_IndexScan()
{
}

// for the format of "value", please see IX_IndexHandle::InsertEntry()
// Initialize index scan
RC IX_IndexScan::OpenScan(const IX_IndexHandle &indexHandle, CompOp compOp,void *value)
{
	PF_FileHandle fileHandle = indexHandle.pf_fileHandle;
	if(fileHandle.file == NULL)//(fileHandle.readFile()==0)
		return -1;
	this->idxHandle = indexHandle;

	this->compOp = compOp;
	int root = this->idxHandle.realRoot; //this->idxHandle.getRoot();
	//this->idxHandle.type
	if(this->compOp != NO_OP){
		switch(this->idxHandle.realType){
		case TypeInt:
			this->val = *((int*)value);
			break;
		case TypeReal:
			this->val_f = *((float*)value);
			break;
		case TypeVarChar:
			int len = *(int*)value;
			char * str = (char*)malloc(len + 1);
			memcpy(str, (char*)value + 4, len);
			str[len] = '\0';
			string typedKey(str);
			free(str);

			this->val_s = typedKey;
			break;
		}
	}
	//this->idxHandle.type
	switch(this->idxHandle.realType){
	case TypeInt:{
		currentKey = val;
		this->currentPage = this->locate(&root);
		break;
	}
	case TypeReal:{
		currentKey_f = val_f;
		this->currentPage = this->locate_float(&root);
		break;
	}
	case TypeVarChar:{
		currentKey_s = val_s;
		this->currentPage = this->locate_string(&root);
		break;
	}
	}

	return 0;
}

int IX_IndexScan::locate(int *node)
{
	void* page = new unsigned char[PF_PAGE_SIZE];
	idxHandle.readfHandle().ReadPage(*node,page);
	char flag = *(char *)page;
	delete page;

	if(flag == '0'){
		stemNode<int> *sNode = new stemNode<int>();
		this->idxHandle.readFromStemNodePage(sNode, *node);
		map<int,int>::iterator it = sNode->idxMap.begin();
		map<int,int>::iterator last = sNode->idxMap.end();
		last--;
		if (compOp == LT_OP || compOp == LE_OP || compOp == NO_OP)
		{
			int resultPage = locate(&((*it).second));
			delete sNode;
			return resultPage;
		}
		else if ((*last).first <= currentKey)
		{
			int resultPage = locate(&(sNode->end));
			delete sNode;
			return resultPage;
		}
		else
		{
			for(; it!=sNode->idxMap.end();it++){
				if((*it).first > currentKey)    break;
			}
			int resultPage = locate(&((*it).second));
			delete sNode;
			return resultPage;
		}
	}
	else
		return *node;
}

int IX_IndexScan::locate_float(int *node)
{
	void* page = new unsigned char[PF_PAGE_SIZE];
	idxHandle.readfHandle().ReadPage(*node,page);
	char flag = *(char *)page;
	delete page;

	if(flag == '0'){
		stemNode<float> *sNode = new stemNode<float>();
		this->idxHandle.readFromStemNodePage(sNode, *node);
		map<float,int>::iterator it = sNode->idxMap.begin();
		map<float,int>::iterator last = sNode->idxMap.end();
		last--;
		if (compOp == LT_OP || compOp == LE_OP || compOp == NO_OP)
		{
			int resultPage = locate_float(&((*it).second));
			delete sNode;
			return resultPage;
		}
		else if ((*last).first <= val_f)
		{
			int resultPage = locate_float(&(sNode->end));
			delete sNode;
			return resultPage;
		}
		else
		{
			for(; it!=sNode->idxMap.end();it++){
				if((*it).first > val_f)    break;
			}
			int resultPage = locate_float(&((*it).second));
			delete sNode;
			return resultPage;
		}
	}
	else
		return *node;
}

int IX_IndexScan::locate_string(int *node)
{
	void* page = new unsigned char[PF_PAGE_SIZE];
	idxHandle.readfHandle().ReadPage(*node,page);
	char flag = *(char *)page;
	delete page;

	if(flag == '0'){
		stemNode<string> *sNode = new stemNode<string>();
		this->idxHandle.readFromStemNodePage(sNode, *node);
		map<string,int>::iterator it = sNode->idxMap.begin();
		map<string,int>::iterator last = sNode->idxMap.end();
		last--;
		if (compOp == LT_OP || compOp == LE_OP || compOp == NO_OP)
		{
			int resultPage = locate_string(&((*it).second));
			delete sNode;
			return resultPage;
		}
		else if ((*last).first <= currentKey_s)
		{
			int resultPage = locate_string(&(sNode->end));
			delete sNode;
			return resultPage;
		}
		else
		{
			for(; it!=sNode->idxMap.end();it++){
				if((*it).first > currentKey_s)    break;
			}
			int resultPage = locate_string(&((*it).second));
			delete sNode;
			return resultPage;
		}
	}
	else
		return *node;
}

RC IX_IndexScan::GetNextEntry(RID &rid)
{
	//this->idxHandle.getRoot()
	if(this->idxHandle.realRoot == -1){
		cout << "There is no such entry!" << endl;
		return IX_EOF;
	}
	//this->idxHandle.type
	//this->idxHandle.realType = TypeInt;
	switch(this->idxHandle.realType){
	case TypeInt:
		return GetNextEntry_Int(rid);
		break;
	case TypeReal:
		return GetNextEntry_float(rid);
		break;
	case TypeVarChar:
		//return GetNextEntry_Int(rid); //for project4
		return GetNextEntry_String(rid);
		break;
	}
	return 0;
}

RC IX_IndexScan::GetNextEntry_Int(RID &rid)
{
	if (currentPage == -1)
		{
			cout << "There is no such entry!" << endl;
			return -2;
		}
		leafNode<int> *lNode = new leafNode<int>();

		idxHandle.readFromLeafNodePage(lNode,currentPage);

		map<int,int>::iterator it = lNode->myMap.begin();
		vector<RID> ridVector;
		if (isVirgin)
		{
			isVirgin = false;
			switch(compOp){
				case NO_OP:
					currentKey = (*it).first;
					rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
					it++;
					if(it != lNode->myMap.end())
						nextKey = (*it).first;
					delete lNode;
					return 0;
				case LT_OP:
					if ((*it).first >= val)
					{
						delete lNode;
						return IX_EOF;
					}
					else
					{
						currentKey = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey = (*it).first;
						delete lNode;
						return 0;
					}
				case LE_OP:
					if ((*it).first > val)
					{
						delete lNode;
						return IX_EOF;
					}
					else
					{
						currentKey = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey = (*it).first;
						delete lNode;
						return 0;
					}
				case EQ_OP:
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first == val)
						{
							currentKey = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey = (*it).first;
							delete lNode;
							return 0;
						}
					}
					delete lNode;
					return IX_EOF;
				case GT_OP:
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first == val)
						{
							currentKey = (*it).first;
							it++;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							GetNextEntry(rid);
							return 0;
						}
						if ((*it).first > val)
						{
							currentKey = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey = (*it).first;
							delete lNode;
							return 0;
						}
					}
					while (lNode->nextLeaf != -1)
					{
						currentPage = lNode->nextLeaf;
						GetNextEntry(rid);
					}
					delete lNode;
					return IX_EOF;
				case GE_OP:
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first >= val)
						{
							currentKey = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey = (*it).first;
							delete lNode;
							return 0;
						}
					}
					while (lNode->nextLeaf != -1)
					{
						currentPage = lNode->nextLeaf;
						GetNextEntry(rid);
					}
					delete lNode;
					return IX_EOF;
			}
		}
		else
		{
			if(idxHandle.ifNextDuplicate(lNode->myMap,currentKey,pageOfVector,n+1) == -1){
				pageOfVector = 0;
				n = 0;
				it = lNode->myMap.find(currentKey);

				if(it == lNode->myMap.end()&&currentKey!=-1){
					switch(compOp){
					case NO_OP:
					case LT_OP:
					case LE_OP:
					it = lNode->myMap.find(nextKey);
					break;

					case EQ_OP:
						this->currentPage = this->locate(&(this->idxHandle.nowRoot));
						lNode = new leafNode<int>();
						idxHandle.readFromLeafNodePage(lNode,currentPage);
						for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
						{
							if ((*it).first == currentKey)
							{
								currentKey = (*it).first;
								rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
								it++;
								if(it != lNode->myMap.end())
									nextKey = (*it).first;
								delete lNode;
								return 0;
							}
						}
						delete lNode;
						return IX_EOF;
					case GT_OP:{
						this->currentPage = this->locate(&(this->idxHandle.nowRoot));
						lNode = new leafNode<int>();
						idxHandle.readFromLeafNodePage(lNode,currentPage);
						for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
						{
							if ((*it).first > currentKey)
							{
								currentKey = (*it).first;
								rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
								it++;
								if(it != lNode->myMap.end())
									nextKey = (*it).first;
								delete lNode;
								return 0;
							}
						}
						while (lNode->nextLeaf != -1)
						{
							currentPage = lNode->nextLeaf;
							GetNextEntry(rid);
						}
						delete lNode;
						return IX_EOF;
					}

					case GE_OP:
						this->currentPage = this->locate(&(this->idxHandle.nowRoot));
						lNode = new leafNode<int>();
						idxHandle.readFromLeafNodePage(lNode,currentPage);
						for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
						{
							if ((*it).first >= currentKey)
							{
								currentKey = (*it).first;
								rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
								it++;
								if(it != lNode->myMap.end())
									nextKey = (*it).first;
								delete lNode;
								return 0;
							}
						}
						while (lNode->nextLeaf != -1)
						{
							currentPage = lNode->nextLeaf;
							GetNextEntry(rid);
						}
						delete lNode;
						return IX_EOF;
				}
				}
				else
					it++;
				if (it == lNode->myMap.end())
				{
					currentPage = lNode->nextLeaf;
					if (currentPage == -1)
					{
						delete lNode;
						return IX_EOF;
					}
					lNode = new leafNode<int>();
					this->idxHandle.readFromLeafNodePage(lNode,currentPage);
					it = lNode->myMap.begin();
				}
				switch(compOp){
					case NO_OP:
						currentKey = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey = (*it).first;
						delete lNode;
						return 0;
					case LT_OP:
						if ((*it).first >= val)
						{
							delete lNode;
							return IX_EOF;
						}
						else
						{
							currentKey = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey = (*it).first;
							delete lNode;
							return 0;
						}
					case LE_OP:
						if ((*it).first > val)
						{
							delete lNode;
							return IX_EOF;
						}
						else
						{
							currentKey = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey = (*it).first;
							delete lNode;
							return 0;
						}
					case EQ_OP:
						return IX_EOF;
					case GT_OP:
						currentKey = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey = (*it).first;
						delete lNode;
						return 0;
					case GE_OP:
						currentKey = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey = (*it).first;
						delete lNode;
						return 0;
				}
			}
			else{
				it = lNode->myMap.find(currentKey);
				n++;
				if(n == idxHandle.getNumOfVector(it,pageOfVector)){
					if(idxHandle.getNumOfVector(it,pageOfVector) < 400)
						return IX_EOF;
					pageOfVector++;
					n = 0;
				}
				if(pageOfVector > idxHandle.getNumOfVectorPage(it) - 1)
					return IX_EOF;
				rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);

				return 0;
			}

		}
		return 0;
}

RC IX_IndexScan::GetNextEntry_float(RID &rid)
{
	if (currentPage == -1)
	{
		cout << "There is no such entry!" << endl;
		return -2;
	}
	leafNode<float> *lNode = new leafNode<float>();
	idxHandle.readFromLeafNodePage(lNode,currentPage);

	map<float,int>::iterator it = lNode->myMap.begin();
	if (isVirgin)
	{
		isVirgin = false;
		switch(compOp){
			case NO_OP:
				currentKey_f = (*it).first;
				rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
				it++;
				if(it != lNode->myMap.end())
					nextKey_f = (*it).first;
				delete lNode;
				return 0;
			case LT_OP:
				if ((*it).first >= val_f)
				{
					delete lNode;
					return IX_EOF;
				}
				else
				{
					currentKey_f = (*it).first;
					rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
					it++;
					if(it != lNode->myMap.end())
						nextKey_f = (*it).first;
					delete lNode;
					return 0;
				}
			case LE_OP:
				if ((*it).first > val_f)
				{
					delete lNode;
					return IX_EOF;
				}
				else
				{
					currentKey_f = (*it).first;
					rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
					it++;
					if(it != lNode->myMap.end())
						nextKey_f = (*it).first;
					delete lNode;
					return 0;
				}
			case EQ_OP:
				for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
				{
					if ((*it).first == val_f)
					{
						currentKey_f = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_f = (*it).first;
						delete lNode;
						return 0;
					}
				}
				delete lNode;
				return IX_EOF;
			case GT_OP:
				for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
				{
					if ((*it).first == val_f)
					{
						currentKey_f = (*it).first;
						it++;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						GetNextEntry(rid);
						return 0;
					}
					if ((*it).first > val_f)
					{
						currentKey_f = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_f = (*it).first;
						delete lNode;
						return 0;
					}
				}
				while (lNode->nextLeaf != -1)
				{
					currentPage = lNode->nextLeaf;
					GetNextEntry(rid);
				}
				delete lNode;
				return IX_EOF;
			case GE_OP:
				for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
				{
					if ((*it).first >= val_f)
					{
						currentKey_f = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_f = (*it).first;
						delete lNode;
						return 0;
					}
				}
				while (lNode->nextLeaf != -1)
				{
					currentPage = lNode->nextLeaf;
					GetNextEntry(rid);
				}
				delete lNode;
				return IX_EOF;
		}
	}
	else
	{
		if(idxHandle.ifNextDuplicate(lNode->myMap,currentKey_f,pageOfVector,n+1) == -1){
			pageOfVector = 0;
			n = 0;
			it = lNode->myMap.find(currentKey_f);
			if(it == lNode->myMap.end()&&currentKey_f!=-1){
				switch(compOp){
				case NO_OP:
				case LT_OP:
				case LE_OP:
				it = lNode->myMap.find(nextKey_f);
				break;

				case EQ_OP:
					this->currentPage = this->locate_float(&(this->idxHandle.nowRoot));
					lNode = new leafNode<float>();
					idxHandle.readFromLeafNodePage(lNode,currentPage);
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first == currentKey_f)
						{
							currentKey_f = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey_f = (*it).first;
							delete lNode;
							return 0;
						}
					}
					delete lNode;
					return IX_EOF;
				case GT_OP:{
					this->currentPage = this->locate_float(&(this->idxHandle.nowRoot));
					lNode = new leafNode<float>();
					idxHandle.readFromLeafNodePage(lNode,currentPage);
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first > currentKey_f)
						{
							currentKey_f = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey_f = (*it).first;
							delete lNode;
							return 0;
						}
					}
					while (lNode->nextLeaf != -1)
					{
						currentPage = lNode->nextLeaf;
						GetNextEntry(rid);
					}
					delete lNode;
					return IX_EOF;
				}

				case GE_OP:
					this->currentPage = this->locate_float(&(this->idxHandle.nowRoot));
					lNode = new leafNode<float>();
					idxHandle.readFromLeafNodePage(lNode,currentPage);
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first >= currentKey_f)
						{
							currentKey_f = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey_f = (*it).first;
							delete lNode;
							return 0;
						}
					}
					while (lNode->nextLeaf != -1)
					{
						currentPage = lNode->nextLeaf;
						GetNextEntry(rid);
					}
					delete lNode;
					return IX_EOF;
			}
			}
			else
				it++;
			if (it == lNode->myMap.end())
			{
				currentPage = lNode->nextLeaf;
				if (currentPage == -1)
				{
					delete lNode;
					return IX_EOF;
				}
				lNode = new leafNode<float>();
				this->idxHandle.readFromLeafNodePage(lNode,currentPage);
				it = lNode->myMap.begin();
			}
			switch(compOp){
				case NO_OP:
					currentKey_f = (*it).first;
					rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
					it++;
					if(it != lNode->myMap.end())
						nextKey_f = (*it).first;
					delete lNode;
					return 0;
				case LT_OP:
					if ((*it).first >= val_f)
					{
						delete lNode;
						return IX_EOF;
					}
					else
					{
						currentKey_f = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_f = (*it).first;
						delete lNode;
						return 0;
					}
				case LE_OP:
					if ((*it).first > val_f)
					{
						delete lNode;
						return IX_EOF;
					}
					else
					{
						currentKey_f = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_f = (*it).first;
						delete lNode;
						return 0;
					}
				case EQ_OP:
					return IX_EOF;
				case GT_OP:
					currentKey_f = (*it).first;
					rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
					it++;
					if(it != lNode->myMap.end())
						nextKey_f = (*it).first;
					delete lNode;
					return 0;
				case GE_OP:
					currentKey_f = (*it).first;
					rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
					it++;
					if(it != lNode->myMap.end())
						nextKey_f = (*it).first;
					delete lNode;
					return 0;
			}
		}
		else{
			it = lNode->myMap.find(currentKey_f);
			n++;
			if(n == idxHandle.getNumOfVector(it,pageOfVector)){
				if(idxHandle.getNumOfVector(it,pageOfVector) < 400)
					return IX_EOF;
				pageOfVector++;
				n = 0;
			}
			if(pageOfVector > idxHandle.getNumOfVectorPage(it) - 1)
				return IX_EOF;
			rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);

			return 0;
		}

	}
	return 0;
}

RC IX_IndexScan::GetNextEntry_String(RID &rid)
{
	if (currentPage == -1)
		{
			cout << "There is no such entry!" << endl;
			return -2;
		}
		leafNode<string> *lNode = new leafNode<string>();
		idxHandle.readFromLeafNodePage(lNode,currentPage);

		map<string,int>::iterator it = lNode->myMap.begin();
		vector<RID> ridVector;
		if (isVirgin)
		{
			isVirgin = false;
			switch(compOp){
				case NO_OP:
					currentKey_s = (*it).first;
					rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
					it++;
					if(it != lNode->myMap.end())
						nextKey_s = (*it).first;
					delete lNode;
					return 0;
				case LT_OP:
					if ((*it).first >= val_s)
					{
						delete lNode;
						return IX_EOF;
					}
					else
					{
						currentKey_s = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_s = (*it).first;
						delete lNode;
						return 0;
					}
				case LE_OP:
					if ((*it).first > val_s)
					{
						delete lNode;
						return IX_EOF;
					}
					else
					{
						currentKey_s = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_s = (*it).first;
						delete lNode;
						return 0;
					}
				case EQ_OP:
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first == val_s)
						{
							currentKey_s = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey_s = (*it).first;
							delete lNode;
							return 0;
						}
					}
					delete lNode;
					return IX_EOF;
				case GT_OP:
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first == val_s)
						{
							currentKey_s = (*it).first;
							it++;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							GetNextEntry(rid);
							return 0;
						}
						if ((*it).first > val_s)
						{
							currentKey_s = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey_s = (*it).first;
							delete lNode;
							return 0;
						}
					}
					while (lNode->nextLeaf != -1)
					{
						currentPage = lNode->nextLeaf;
						GetNextEntry(rid);
					}
					delete lNode;
					return IX_EOF;
				case GE_OP:
					for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
					{
						if ((*it).first >= val_s)
						{
							currentKey_s = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey_s = (*it).first;
							delete lNode;
							return 0;
						}
					}
					while (lNode->nextLeaf != -1)
					{
						currentPage = lNode->nextLeaf;
						GetNextEntry(rid);
					}
					delete lNode;
					return IX_EOF;
			}
		}
		else
		{
			if(idxHandle.ifNextDuplicate(lNode->myMap,currentKey_s,pageOfVector,n+1) == -1){
				pageOfVector = 0;
				n = 0;
				it = lNode->myMap.find(currentKey_s);
				if(it == lNode->myMap.end()&&currentKey_s!=""){
					switch(compOp){
					case NO_OP:
					case LT_OP:
					case LE_OP:
					it = lNode->myMap.find(nextKey_s);
					break;

					case EQ_OP:
						this->currentPage = this->locate_string(&(this->idxHandle.nowRoot));
						lNode = new leafNode<string>();
						idxHandle.readFromLeafNodePage(lNode,currentPage);
						for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
						{
							if ((*it).first == currentKey_s)
							{
								currentKey_s = (*it).first;
								rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
								it++;
								if(it != lNode->myMap.end())
									nextKey_s = (*it).first;
								delete lNode;
								return 0;
							}
						}
						delete lNode;
						return IX_EOF;
					case GT_OP:{
						this->currentPage = this->locate_string(&(this->idxHandle.nowRoot));
						lNode = new leafNode<string>();
						idxHandle.readFromLeafNodePage(lNode,currentPage);
						for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
						{
							if ((*it).first > currentKey_s)
							{
								currentKey_s = (*it).first;
								rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
								it++;
								if(it != lNode->myMap.end())
									nextKey_s = (*it).first;
								delete lNode;
								return 0;
							}
						}
						while (lNode->nextLeaf != -1)
						{
							currentPage = lNode->nextLeaf;
							GetNextEntry(rid);
						}
						delete lNode;
						return IX_EOF;
					}

					case GE_OP:
						this->currentPage = this->locate_string(&(this->idxHandle.nowRoot));
						lNode = new leafNode<string>();
						idxHandle.readFromLeafNodePage(lNode,currentPage);
						for (it = lNode->myMap.begin(); it != lNode->myMap.end(); it++)
						{
							if ((*it).first >= currentKey_s)
							{
								currentKey_s = (*it).first;
								rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
								it++;
								if(it != lNode->myMap.end())
									nextKey_s = (*it).first;
								delete lNode;
								return 0;
							}
						}
						while (lNode->nextLeaf != -1)
						{
							currentPage = lNode->nextLeaf;
							GetNextEntry(rid);
						}
						delete lNode;
						return IX_EOF;
				}
				}
				else
					it++;
				if (it == lNode->myMap.end())
				{
					currentPage = lNode->nextLeaf;
					if (currentPage == -1)
					{
						delete lNode;
						return IX_EOF;
					}
					lNode = new leafNode<string>();
					this->idxHandle.readFromLeafNodePage(lNode,currentPage);
					it = lNode->myMap.begin();
				}
				switch(compOp){
					case NO_OP:
						currentKey_s = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_s = (*it).first;
						delete lNode;
						return 0;
					case LT_OP:
						if ((*it).first >= val_s)
						{
							delete lNode;
							return IX_EOF;
						}
						else
						{
							currentKey_s = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey_s = (*it).first;
							delete lNode;
							return 0;
						}
					case LE_OP:
						if ((*it).first > val_s)
						{
							delete lNode;
							return IX_EOF;
						}
						else
						{
							currentKey_s = (*it).first;
							rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
							it++;
							if(it != lNode->myMap.end())
								nextKey_s = (*it).first;
							delete lNode;
							return 0;
						}
					case EQ_OP:
						return IX_EOF;
					case GT_OP:
						currentKey_s = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_s = (*it).first;
						delete lNode;
						return 0;
					case GE_OP:
						currentKey_s = (*it).first;
						rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);
						it++;
						if(it != lNode->myMap.end())
							nextKey_s = (*it).first;
						delete lNode;
						return 0;
				}
			}
			else{
				it = lNode->myMap.find(currentKey_s);
				n++;
				if(n == idxHandle.getNumOfVector(it,pageOfVector)){
					if(idxHandle.getNumOfVector(it,pageOfVector) < 20)
						return IX_EOF;
					pageOfVector++;
					n = 0;
				}
				if(pageOfVector > idxHandle.getNumOfVectorPage(it) - 1)
					return IX_EOF;
				rid = idxHandle.readRidFromVectorOfRid(it,pageOfVector,n);

				return 0;
			}

		}
		return 0;
}

RC IX_IndexScan::CloseScan()
{
	compOp = EQ_OP;
	val = -1;
	val_f = -1;
	PF_FileHandle pf_fileHandle;
	pf_fileHandle.file = NULL; //setFile(0);
	idxHandle.setfHandle(pf_fileHandle);
	currentPage = -1;
	isVirgin = true;
	currentKey = -1;
	nextKey = -1;
	currentKey_f = -1;
	currentKey_s = "";
	nextKey_f = -1;
	nextKey_s = "";

	pageOfVector = 0;
	n = 0;
	return 0;
}

IX_IndexHandle IX_IndexScan::getIdxHandle()
{
	return idxHandle;
}

void IX_IndexScan::setIdxHandle(IX_IndexHandle idxHandle)
{
	this->idxHandle = idxHandle;
}

CompOp IX_IndexScan::getCompOp()
{
	return compOp;
}

void IX_IndexScan::setCompOp(CompOp compOp)
{
	this->compOp = compOp;
}

int IX_IndexScan::getValue()
{
	return val;
}

void IX_IndexScan::setValue(int val)
{
	this->val = val;
}

float IX_IndexScan::getfValue()
{
	return val_f;
}

void IX_IndexScan::setfValue(float val_f)
{
	this->val_f = val_f;
}
