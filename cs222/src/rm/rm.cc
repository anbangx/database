#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "rm.h"

RM* RM::_rm = 0;
int tableSize = sizeof(tableInfo);
int columnSize = sizeof(columnInfo);

RM* RM::Instance()
{
    if(!_rm)
        _rm = new RM();
    
    return _rm;
}

RM::RM()
{
	FILE * pfile = fopen("systemCatalog","wb");
	fclose(pfile);

	pfile = fopen("columnInfo","wb");
	fclose(pfile);

	pf = PF_Manager::Instance();
}

RM::~RM()
{
}

bool FileExist1(string fileName)
{
	struct stat stFileInfo;
	if(stat(fileName.c_str(), &stFileInfo) == 0) return true;
	else return false;
}

RC RM::createTable(const string tableName, const vector<Attribute> &attrs)
{
	//Create a table
	PF_FileHandle fileHandle;
	pf->CreateFile(tableName.c_str());

	//Initiate the directory file

	pf->OpenFile(tableName.c_str(),fileHandle);
	void* data = malloc(PF_PAGE_SIZE);
	for(unsigned i = 0; i < PF_PAGE_SIZE/sizeof(int); i++)
	{
		*((int *)data+i) = -1;
	}
	int s = 2048;
	*((int *)data) = s;
	fileHandle.AppendPage(data);

	//Initiate the offset of the free space of the first page
	data = malloc(PF_PAGE_SIZE);
	s = 4;
	*((int *)data) = s;
	*((int*)data+PF_PAGE_SIZE/sizeof(int)-1) = 0;
	fileHandle.AppendPage(data);

	pf->CloseFile(fileHandle);

	free(data);

	//Insert table Info to system catalog
	FILE * pfile = fopen("systemCatalog","a+");

	struct tableInfo tInfo;
	struct tableInfo tPreInfo;
	tPreInfo.tId = 0;

	fseek(pfile, 0, SEEK_END);
	if (ftell(pfile) != 0){
		rewind(pfile);
		while(!feof(pfile))
			fread(&tPreInfo,tableSize,1,pfile);
	}
	tInfo.tId=tPreInfo.tId+1;

	strcpy(tInfo.tableName,tableName.c_str());
	strcpy(tInfo.fileName,tableName.c_str());

	fwrite(&tInfo,tableSize,1,pfile);

	fclose(pfile);

	//Insert table Info to columnInfo
	pfile = fopen("columnInfo","a+");
	struct columnInfo cInfo;
	for(int i=0; i<attrs.size();i++){
		cInfo.tId = tInfo.tId;
		strcpy(cInfo.columnName,attrs[i].name.c_str());
		cInfo.columnType = attrs[i].type;
		cInfo.columnLength = attrs[i].length;

		fwrite(&cInfo,columnSize,1,pfile);
	}

	fflush(pfile);

	fclose(pfile);

	return 0;
}

RC RM::insertTuple(const string tableName, const void *data, RID &rid)
{
	//Find directory file to find the size of free space.
	PF_FileHandle fileHandle;
	pf->OpenFile(tableName.c_str(),fileHandle);
	void* dPage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(0,dPage);
/*	void* testpage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(1,testpage);
	cout << "1.free space = " << *(int*)dPage <<endl;
	int numOfSlots = *((int *)testpage + PF_PAGE_SIZE/sizeof(int) - 1);
	cout << "1. numOfSlots = " << numOfSlots << endl;

	cout << "2.free space = " << *((int*)dPage+1) <<endl;
	testpage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(2,testpage);
	numOfSlots = *((int *)testpage + PF_PAGE_SIZE/sizeof(int) - 1);
	cout << "2. numOfSlots = " << numOfSlots << endl;

	cout << "3.free space = " << *((int*)dPage+2) <<endl;
	testpage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(3,testpage);
	numOfSlots = *((int *)testpage + PF_PAGE_SIZE/sizeof(int) - 1);
	cout << "3. numOfSlots = " << numOfSlots << endl;

	cout << "4.free space = " << *((int*)dPage+3) <<endl << endl;

	free(testpage);*/

	//Read attrs' length from columnTable
	vector<Attribute> attrs;
	this->getAttributes(tableName.c_str(),attrs);
	unsigned int l = 0;
	int s = 0;
	unsigned i=0;
	const void* pData ;
	pData = data;
	for(;i<attrs.size();i++){
		l += 4;

		if(attrs[i].type==TypeVarChar){
			 s = *(int *)pData ; //*((int *)(pData));
			 l += s;
			 pData = (char*)pData + s;
		}
		pData = (char*)pData + 4;
	}

	for(i=0; i < PF_PAGE_SIZE/sizeof(int); i++)
	{
		if(*((int *)dPage+i)>(l))
			break;
		else if(*((int *)dPage+i)==-1)
			break;
	}

	//if there is not enough free space, append new page and update directory
	if(*((int *)dPage+i)==-1){
		void* empty = malloc(PF_PAGE_SIZE);
		s = 4;
		*((int *)empty) = s;  // the address of free space
		*((int*)empty+PF_PAGE_SIZE/sizeof(int)-1) = 0; //slotNum
		fileHandle.AppendPage(empty);

		*((int *)dPage+i) = 2048;
	}

	//Read the first satisfy page.
	void* page = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(i+1,page);

	//Find the offset of free space.
	int free1 = *((int*)page);

	//Write the tuple to the free space.
	memcpy(((char* )page +free1),data,l);

	//Add slot to record offset and length.

	int slotN;
	slotN = *((int*)page+PF_PAGE_SIZE/sizeof(int)-1);
	slotN = slotN + 1;
	*((int*)page+PF_PAGE_SIZE/sizeof(int)-1) = slotN;

	struct slot newSlot;
	newSlot.slotNum = slotN;
	newSlot.length = l;
	newSlot.offset = free1;
	*((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*(slotN)/sizeof(int))) = newSlot;

	//Update free space in record page.
	*((int*)page) += l;

	//Write to the file.
	fileHandle.WritePage(i+1,page);

	//Update directory file and write to file.
	*((int *)dPage + i) -= l;
	fileHandle.WritePage(0,dPage);



	//Set the pageNum and slotNum
	rid.pageNum = i + 1 ;
	rid.slotNum = slotN;

	free(dPage);
	free(page);

	/*void* aPage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(0,aPage);
	void* testpage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(1,testpage);
	cout << "1.free space = " << *(int*)aPage <<endl;
	int numOfSlots = *((int *)testpage + PF_PAGE_SIZE/sizeof(int) - 1);
	cout << "1. numOfSlots = " << numOfSlots << endl;

	cout << "2.free space = " << *((int*)aPage+1) <<endl;
	testpage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(2,testpage);
	numOfSlots = *((int *)testpage + PF_PAGE_SIZE/sizeof(int) - 1);
	cout << "2. numOfSlots = " << numOfSlots << endl;

	cout << "3.free space = " << *((int*)aPage+2) <<endl;
	testpage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(3,testpage);
	numOfSlots = *((int *)testpage + PF_PAGE_SIZE/sizeof(int) - 1);
	cout << "3. numOfSlots = " << numOfSlots << endl;

	cout << "4.free space = " << *((int*)aPage+3) <<endl ;
	testpage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(4,testpage);
	numOfSlots = *((int *)testpage + PF_PAGE_SIZE/sizeof(int) - 1);
	cout << "4. numOfSlots = " << numOfSlots << endl << endl;
	free(testpage);*/

	pf->CloseFile(fileHandle);

	return 0;
}

// Assume the rid does not change after update
RC RM::updateTuple(const string tableName, const void *data, const RID &rid)
{
	//Get the length of data
	//Read attrs' length from columnTable
	vector<Attribute> attrs;
	this->getAttributes(tableName.c_str(),attrs);
	unsigned int l = 0;
	int s = 0;
	unsigned i=0;
	const void* pData ;
	pData = data;
	for(;i<attrs.size();i++){
		l += 4;

		if(attrs[i].type==TypeVarChar){
			 s = *((int *)(pData));
			 l += s;
			 pData = (char*)pData + s;
		}
		pData = (char*)pData + 4;
	}

	//Find the directory page using tableName and rid
	PF_FileHandle fileHandle;
	pf->OpenFile(tableName.c_str(),fileHandle);
	void* dPage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(0,dPage);

	//If the free space of old page is bigger than data,
		//Write data to the offset of freespace
		//Update directory page, head of page and slot offset
	if(*((int *)dPage+rid.pageNum-1)>l){
		//Read the satisfy page.
		void* page = malloc(PF_PAGE_SIZE);
		fileHandle.ReadPage(rid.pageNum,page);

		//Find the offset of free space
		int free1 = *((int*)page);

		//Write the tuple to the free space
		memcpy(((char* )page +free1),data,l);

		//Update tuple offset
		struct slot newSlot = *(struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*rid.slotNum/sizeof(int));
		newSlot.offset = free1;
		newSlot.length = l;
		*((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*rid.slotNum/sizeof(int))) = newSlot;

		//Update free space in record page
		*((int*)page) += l;

		//Write to the file.
		fileHandle.WritePage(rid.pageNum,page);

		//Update directory file and write to file
		*((int *)dPage + rid.pageNum -1) -= l;
		fileHandle.WritePage(0,dPage);

		pf->CloseFile(fileHandle);

		free(page);
		free(dPage);
	}

	//else, append a page
		//change old slot offset to -1, length to new pageNum
		//Insert Tuple by changing slotNum
	else{
		//Initiate the offset of the free space of the first page
		void* initData = malloc(PF_PAGE_SIZE);
		s = 4;
		*((int *)initData) = s;
		*((int*)initData+PF_PAGE_SIZE/sizeof(int)-1) = 0;
		fileHandle.AppendPage(initData);

		//change old slot offset to -1, length to new pageNum
		void* page = malloc(PF_PAGE_SIZE);
		fileHandle.ReadPage(rid.pageNum,page);
		struct slot newSlot = *(struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*rid.slotNum/sizeof(int));
		newSlot.offset = -1;
		newSlot.length = fileHandle.GetNumberOfPages()-1;
		int newPageNum = fileHandle.GetNumberOfPages()-1;
		*((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*rid.slotNum/sizeof(int))) = newSlot;

		fileHandle.WritePage(rid.pageNum,page);
		//Operation in the new appending page
		//Read the satisfy page.
		void* newPage = malloc(PF_PAGE_SIZE);
		fileHandle.ReadPage(newPageNum,newPage); //read new page

		//Find the offset of free space.
		int free1 = *((int*)newPage);

		//Write the tuple to the free space.
		memcpy(((char* )newPage +free1),data,l);

		//update slotNum, offset and length of new slot
		//struct slot newSlot;
		newSlot.slotNum = -1;
		newSlot.length = l;
		newSlot.offset = free1;
		*((struct slot*)((int*)newPage+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)/sizeof(int))) = newSlot;

		//Update free space in record page.
		*((int*)newPage) += l;

		//Write to the file.
		fileHandle.WritePage(newPageNum,newPage);

		//Update directory file and write to file.
		*((int *)dPage + newPageNum ) = -1;
		fileHandle.WritePage(0,dPage);

		pf->CloseFile(fileHandle);

		//pf->OpenFile(tableName.c_str(),fileHandle);

		free(page);
		free(newPage);
		free(dPage);
		free(initData);
	}
	return 0;
}


RC RM::readTuple(const string tableName, const RID &rid, void *data)
{
	//Open the table file
	if(!FileExist1(tableName))
		return -1;

	PF_FileHandle fileHandle;
	pf->OpenFile(tableName.c_str(),fileHandle);

	//check pageNum
	if(rid.pageNum >= fileHandle.GetNumberOfPages()){
		pf->CloseFile(fileHandle);
		return -1;
	}

	//Find the page
	void* page = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(rid.pageNum,page);

	//check slotNum
	int numOfSlot = *((int*)page+PF_PAGE_SIZE/sizeof(int)-1);
	if(rid.slotNum > numOfSlot){
		pf->CloseFile(fileHandle);
		return -1;
	}

	//Find the slot in order to get the offset and length
	struct slot readSlot = *((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*rid.slotNum/sizeof(int)));

	//It has already been deleted.
	if(readSlot.slotNum==-1){
		pf->CloseFile(fileHandle);
		return -1;
	}

	//Copy the record to data
	//if in other page, go to that page based on slot.length,
	//read offset and length in new page, eventually read the tuple
	if(readSlot.offset==-1){
		void* newPage = malloc(PF_PAGE_SIZE);
		fileHandle.ReadPage(readSlot.length,newPage);
		struct slot readNewSlot = *((struct slot*)((int*)newPage+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)/sizeof(int)));
		memcpy(data,((char*)newPage +readNewSlot.offset),readNewSlot.length);
		free(newPage);
	}
	else
		memcpy(data,((char*)page +readSlot.offset),readSlot.length);

	pf->CloseFile(fileHandle);

	free(page);

	return 0;
}

RC RM::readAttribute(const string tableName, const RID &rid, const string attributeName, void *data)
{
	//Read the tuple
	void *tuple = malloc(100);
	memset(tuple, 0, 100);
	this->readTuple(tableName,rid,tuple);
	const void* pTuple;
	pTuple = tuple;

	//Iterate the attributes' name in columnInfo
	//If finding the given attribute,
		//check type
		//If varchar, read 4 byte to get the length, and continue to read the data based on this length
		//else, advance 4 byte
	//else,
		//check type
		//If varchar, read 4 byte, and continue to advance the data based on this length
		//else, advance 4 byte
	int l = 0;
	vector<Attribute> attrs;
	this->getAttributes(tableName,attrs);
	for(int i=0;i<attrs.size();i++){
		if(strcmp(attrs[i].name.c_str(),attributeName.c_str())==0){
			if(attrs[i].type==TypeVarChar){
				l = *(int *)pTuple;
				pTuple = (char *)pTuple + sizeof(int);
				*((char *)pTuple + l) = '\0';
				memcpy(data,pTuple,l+1);
			}
			else
				memcpy(data,pTuple,sizeof(int));
			free(tuple);
			return 0;
		}
		else{
			if(attrs[i].type==TypeVarChar){
				l = *(int *)pTuple;
				pTuple = (char *)pTuple + sizeof(int);
				pTuple = ((char *)pTuple + l);
			}
			else
				pTuple = ((char *)pTuple + sizeof(int));
		}
	}

	free(tuple);

	return -1;

}
RC RM::deleteTuple(const string tableName, const RID &rid)
{
	//Open the file
	PF_FileHandle fileHandle;
	pf->OpenFile(tableName.c_str(),fileHandle);

	//Find the page
	void* page = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(rid.pageNum,page);

	//Find the slot to set offset to -1
	struct slot readSlot = *((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*rid.slotNum/sizeof(int)));
	readSlot.slotNum = -1;
	*((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*rid.slotNum/sizeof(int))) = readSlot;

	//*((int*)page+PF_PAGE_SIZE/sizeof(int)-1) = *((int*)page+PF_PAGE_SIZE/sizeof(int)-1) - 1;
	//Write to file
	fileHandle.WritePage(rid.pageNum,page);
	pf->CloseFile(fileHandle);

	free(page);
	return 0;
}

RC RM::deleteTuples(const string tableName)
{

	vector<Attribute> attrs;
	this->getAttributes(tableName,attrs);
	this->deleteTable(tableName.c_str());
	this->createTable(tableName,attrs);

	return 0;
}

RC RM::getAttributes(const string tableName, vector<Attribute> &attrs)
{
	FILE * pfile = fopen("systemCatalog","r");

	struct tableInfo tInfo;
	int size = sizeof(tableInfo);
	while (!feof(pfile)){
		fread(&tInfo,size,1,pfile);
		if(strcmp(tInfo.tableName,tableName.c_str())==0)
			break;
	}

	fclose(pfile);

	pfile = fopen("columnInfo","r");

	fseek( pfile, 0, SEEK_END );
	int fileSize = ftell( pfile );
	rewind(pfile);
	struct columnInfo cInfo;
	Attribute attr;
	int n = 1;
	while(ftell(pfile)!=fileSize){
		fread(&cInfo,columnSize,1,pfile);
		fseek(pfile, columnSize*n, SEEK_SET );
		if(cInfo.tId==tInfo.tId){
			attr.name = cInfo.columnName;
			attr.type = cInfo.columnType;
			attr.length = cInfo.columnLength;
			attrs.push_back(attr);
		}
		n++;
	}

	fclose(pfile);
	return 0;
}

RC RM::deleteTable(const string tableName)
{
	pf->DestroyFile(tableName.c_str());

	//update systemCatalog
	FILE* pfile = fopen("systemCatalog","r+");

	struct tableInfo tInfo;
	int tId;
	int n = 0;
	while(!feof(pfile)){
		fread(&tInfo,tableSize,1,pfile);
		if(strcmp(tInfo.tableName,tableName.c_str())==0){
			tId = tInfo.tId;
			tInfo.tId = -1;
			n++;
			break;
		}
	}
	fseek(pfile,(n-1)*tableSize,SEEK_SET);
	fwrite(&tInfo,tableSize,1,pfile);
	fclose(pfile);

	//columnInfo
	pfile = fopen("columnInfo","r+");

	fseek( pfile, 0, SEEK_END );
	int fileSize = ftell( pfile );
	rewind(pfile);
	struct columnInfo cInfo;
	n = 0;
	while(ftell(pfile)!=fileSize){
		fread(&cInfo,columnSize,1,pfile);
		if(cInfo.tId==tId){
			cInfo.tId = -1;
			n++;
			fseek(pfile,(n-1)*columnSize,SEEK_SET);
			fwrite(&cInfo,columnSize,1,pfile);
		}
	}
	fclose(pfile);

	return 0;
}

RC RM::reorganizePage(const string tableName, const unsigned pageNumber)
{
	//check if it is an update page
	PF_FileHandle fileHandle;
	pf->OpenFile(tableName.c_str(),fileHandle);
	void* dPage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(0,dPage);
	pf->CloseFile(fileHandle);
	if(*((int*)dPage+pageNumber) == -1)
		return 0;

	//Initiate the new memory
	void* organizePage = malloc(PF_PAGE_SIZE);
	int s = 4;
	*((int *)organizePage) = s;

	//iterate slots
	pf->OpenFile(tableName.c_str(),fileHandle);
	void* page = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(pageNumber,page);

	//copy struct slot from old to new
	int numOfSlot = *((int*)page+PF_PAGE_SIZE/sizeof(int)-1);
	*((int*)organizePage+PF_PAGE_SIZE/sizeof(int)-1) = numOfSlot;
	int sum = 0;
	for(int i = 0; i<numOfSlot; i++){
		//If the offset is not -1
				//set offset to new position
				//copy tuple to new
				//set freespace address of new
		struct slot readSlot = *((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*(i+1)/sizeof(int)));
		if(readSlot.slotNum!=-1){
			memcpy((char*)organizePage+readSlot.offset,(char*)page+readSlot.offset,readSlot.length);
			readSlot.offset = *((int *)organizePage);
			*((int *)organizePage) += readSlot.length;
			sum += readSlot.length;
		}
		*((struct slot*)((int*)organizePage+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*(i+1)/sizeof(int))) = readSlot;
	}

	//update the length of freespace
	*((int*)dPage+pageNumber-1) = 2048 - sum;
	fileHandle.WritePage(0,dPage);

	//Rewrite the page
	fileHandle.WritePage(pageNumber,organizePage);

	pf->CloseFile(fileHandle);

	free(page);
	free(dPage);

	return 0;
}
//struct{
//	int offset;
//	char spaceholder[PF_PAGE_SIZE-8];
//	int slot;
//}PAGE;
//struct PAGE * newPage = (struct Page*)malloc(PF_PAGE_SIZE);
//newPage->offset = 4;
//newPage->slot = 0;


RC RM::reorganizeTable(const string tableName)
{
	//initiate a page in memory to store new info
	void* newPage = malloc(PF_PAGE_SIZE);
	int s = 4;
	*((int *)newPage) = s;
	*((int*)newPage+PF_PAGE_SIZE/sizeof(int)-1) = 0;

	//scan all the slot and store all the exist records
		//rid change
		//copy record
	RM_ScanIterator rmsi;
	vector<Attribute> attrs;
	getAttributes(tableName,attrs);
	vector<string> attributes;
	string attr;
	for(unsigned int i=0;i<attrs.size();i++){
		attr = attrs[i].name;
		attributes.push_back(attr);
	}
	this->scan(tableName, "", NO_OP, NULL, attributes, rmsi);

	void *data_returned = malloc(100);
	RID rid;
	int slotNum = 0;
	struct slot newSlot;
	PF_FileHandle fileHandle;
	string tableName1 = "test";
	pf->CreateFile(tableName1.c_str());

	while(rmsi.getNextTuple(rid, data_returned) != RM_EOF){
		//Read attrs' length from columnTable
		unsigned int l = 0;
		int s = 0;
		unsigned i=0;
		const void* pData ;
		pData = data_returned;
		for(;i<attrs.size();i++){
			l += 4;

			if(attrs[i].type==TypeVarChar){
				 s = *((int *)(pData));
				 l += s;
				 pData = (char*)pData + s;
			}
			pData = (char*)pData + 4;
		}

		slotNum = *((int *)newPage+PF_PAGE_SIZE/sizeof(int))-1;
		slotNum++;
		newSlot.slotNum = slotNum;
		newSlot.offset = *((int *)newPage) + l;
		newSlot.length = l;

		//((struct slot*)((int*)newPage+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*slotNum/sizeof(int))) = newSlot;

		//Find the offset of free space
		int free1 = *((int*)newPage);

		//Write the tuple to the free space
		memcpy(((char* )newPage +free1),data_returned,l);

		//Update free space in record page
		*((int*)newPage) += l;
	}

	//Write to the file
	pf->OpenFile(tableName1.c_str(),fileHandle);
	fileHandle.AppendPage(newPage);
	fileHandle.WritePage(0,newPage);
	pf->CloseFile(fileHandle);
	pf->DestroyFile(tableName1.c_str());

	return 0;
}

RC RM::dropAttribute(const string tableName, const string attributeName)
{
	//Open the file
	PF_FileHandle fileHandle;
	pf->OpenFile(tableName.c_str(),fileHandle);

	//Read the first page
	void* page = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(1,page);

	int numOfSlot = *((int*)page+PF_PAGE_SIZE/sizeof(int)-1);

	//Read the slot
	struct slot readSlot;
	for(int i = 0; i < numOfSlot; i++){
		//change the record length
		 readSlot = *((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*(i+1)/sizeof(int)));
	     readSlot.length -= 4;
	     *((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*(i+1)/sizeof(int))) = readSlot;
	}

	//Write to file
	fileHandle.WritePage(1,page);

	pf->CloseFile(fileHandle);

	pf->OpenFile(tableName.c_str(),fileHandle);
	fileHandle.ReadPage(1,page);
	readSlot = *((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)/sizeof(int)));
	pf->CloseFile(fileHandle);



	return 0;

}

RC RM::addAttribute(const string tableName, const Attribute attr)
{
	//Open the file
	PF_FileHandle fileHandle;
	pf->OpenFile(tableName.c_str(),fileHandle);

	//Read the slotNum and the address of free space
	void* page = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(1,page);

	int numOfSlot = *((int*)page+PF_PAGE_SIZE/sizeof(int)-1);
	int freel = *((int*)page);

	//For each slot,
		//append new record in the end of page
	struct slot readSlot;
	for(int i=0; i<numOfSlot; i++){
		readSlot = *((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*(i+1)/sizeof(int)));
		memcpy((char *)page + freel,(char*)page +sizeof(int), readSlot.length);
	}
	struct columnInfo cInfo;
	/*pfile = fopen("columnInfo","r");
	int n = 0;
	while(!feof(pfile)){
		fread(&cInfo,columnSize,1,pfile);
		n++;
	}
	fclose(pfile);*/

	//update column Table
	FILE* pfile = fopen("systemCatalog","r+");
	struct tableInfo tInfo;
	int tId=0;
	while(!feof(pfile)){
		fread(&tInfo,tableSize,1,pfile);
		if(strcmp(tInfo.tableName,tableName.c_str())==0){
			tId = tInfo.tId;
			break;
		}
	}
	fclose(pfile);

	pfile = fopen("columnInfo","a+");

	cInfo.tId = tId;
	strcpy(cInfo.columnName,attr.name.c_str());
	cInfo.columnType = attr.type;
	cInfo.columnLength = attr.length;
	fwrite(&cInfo,columnSize,1,pfile);
	fclose(pfile);

	pfile = fopen("columnInfo","r");
	int n = 0;
	while(!feof(pfile)){
		fread(&cInfo,columnSize,1,pfile);
		n++;
	}
	fclose(pfile);

	return 0;
}

RC RM::scan(const string tableName,
    const string conditionAttribute,
    const CompOp compOp,                  // comparision type such as "<" and "="
    const void* value,                    // used in the comparison
    const vector<string> &attributeNames, // a list of projected attributes
    RM_ScanIterator &rm_ScanIterator)
{
	rm_ScanIterator.settName(tableName);
	rm_ScanIterator.setcAttr(conditionAttribute);
	rm_ScanIterator.setcompOp(compOp);

	vector<Attribute> attrs;
	getAttributes(tableName,attrs);
	for(unsigned i = 0; i < attrs.size(); i++){
		if(strcmp(attrs[i].name.c_str(),conditionAttribute.c_str())==0){
			if(attrs[i].type==TypeInt){
				int m = *((int *)value);
				rm_ScanIterator.setValue(m);
				break;
			}
			else if(attrs[i].type==TypeReal){
				float n = *((float *)value);
				rm_ScanIterator.setFloat(n);
				break;
			}
			else{
				string s = (char *)value;
				rm_ScanIterator.setString(s);
				break;
			}
		}
	}

	rm_ScanIterator.setattrName(attributeNames);

	return 0;
}

AttrType RM::getTypeFromAttrName(const string tableName,const string attributeName)
{
	vector<Attribute> attrs;
	getAttributes(tableName,attrs);
	for(unsigned i = 0; i < attrs.size(); i++){
		if(strcmp(attrs[i].name.c_str(),attributeName.c_str())==0)
			return attrs[i].type;
	}

	return TypeInt;
}

RM_ScanIterator::RM_ScanIterator()
{
	//Set variable
	tableName = "";

	//Set Rid
	rId.pageNum = 1;
	rId.slotNum = 0;
	conditionAttribute = "";
	compOp = NO_OP;
	value = 0;
	string attr = "";
	vector<string> attributes;
	attributeNames.push_back(attr);
	rm = rm->Instance();
	s= "";
	f=0.0;
}

RM_ScanIterator::~RM_ScanIterator()
{
}

void RM_ScanIterator::resetScan()
{
	//Set variable
	tableName = "";

	//Set Rid
	rId.pageNum = 1;
	rId.slotNum = 0;
	conditionAttribute = "";
	compOp = NO_OP;
	value = 0;
	string attr = "";
	vector<string> attributes;
	attributeNames.push_back(attr);
	rm = rm->Instance();
	s= "";
	f=0.0;
}
 // "data" follows the same format as RM::insertTuple()
RC RM_ScanIterator::getNextTuple(RID &rid, void *data)
{
	//Set pageNum
	rid.pageNum = rId.pageNum;
	rid.slotNum = rId.slotNum;

	PF_Manager *pf;
	PF_FileHandle fileHandle;
	pf->OpenFile(tableName.c_str(),fileHandle);
	void *page = malloc(PF_PAGE_SIZE);
	void *dPage = malloc(PF_PAGE_SIZE);
	fileHandle.ReadPage(0,dPage);
	fileHandle.ReadPage(rId.pageNum,page);

	int pageNum = fileHandle.GetNumberOfPages();
	int numOfSlots = 0;
	int flag = 0;
	numOfSlots = *((int *)page + PF_PAGE_SIZE/sizeof(int) - 1);
	while(true){
		if(rId.slotNum + 1 > numOfSlots){
			rId.pageNum++;
			if(rId.pageNum > pageNum - 1){
				pf->CloseFile(fileHandle);
				return RM_EOF;
			}
			while(*((int*)dPage + rId.pageNum - 1)==-1){
				rId.pageNum++;
				if(rId.pageNum > pageNum - 1){
					pf->CloseFile(fileHandle);
					return RM_EOF;
				}
			}
			fileHandle.ReadPage(rId.pageNum,page);
			numOfSlots = *((int *)page + PF_PAGE_SIZE/sizeof(int) - 1);
			rId.slotNum=1;
		}
		else
			rId.slotNum++;

		struct slot readSlot = *((struct slot*)((int*)page+PF_PAGE_SIZE/sizeof(int)-1-sizeof(slot)*rId.slotNum/sizeof(int)));
		if(readSlot.slotNum!=-1){
			//check if condition is satisfy
			void* c = malloc(100);
			this->rm->readAttribute(tableName, rId, conditionAttribute, c);
			switch(compOp){
			case NO_OP:
				flag=1;
				break;
			case EQ_OP:
				if(this->s=="" && this->f==0.0){
					if(memcmp(c,&value,sizeof(int))==0 )
						flag = 1;
				}
				else if (this->s==""){
					if(*((float*)c) == f)
						flag = 1;
				}
				else
				{
					if(strcmp((char*)c,this->s.c_str())==0)
						flag = 1;
				}
				break;
			case LT_OP:
				if(this->s=="" && this->f==0.0){
					if(memcmp(c,&value,sizeof(int))<0 )
						flag = 1;
				}
				else if (this->s==""){
					if(*((float*)c) < f)
						flag = 1;
				}
				else
				{
					if(strcmp((char*)c,this->s.c_str())<0)
						flag = 1;
				}
				break;
			case GT_OP:
				if(this->s=="" && this->f==0.0){
					if(memcmp(c,&value,sizeof(int))>0 )
						flag = 1;
				}
				else if (this->s==""){
					if(*((float*)c) > f)
						flag = 1;
				}
				else
				{
					if(strcmp((char*)c,this->s.c_str())>0)
						flag = 1;
				}
				break;
			case LE_OP:
				if(this->s=="" && this->f==0.0){
					if(memcmp(c,&value,sizeof(int))<=0 )
						flag = 1;
				}
				else if (this->s==""){
					if(*((float*)c) <= f)
						flag = 1;
				}
				else
				{
					if(strcmp((char*)c,this->s.c_str())<=0)
						flag = 1;
				}
				break;
			case GE_OP:
				if(this->s=="" && this->f==0.0){
					if(memcmp(c,&value,sizeof(int))>=0 )
						flag = 1;
				}
				else if (this->s==""){
					if(*((float*)c) >= f)
						flag = 1;
				}
				else
				{
					if(strcmp((char*)c,this->s.c_str())>=0)
						flag = 1;
				}
				break;
			/*case NE_OP:
				if(this->s=="" && this->f==0.0){
					if(memcmp(c,&value,sizeof(int))!=0 )
						flag = 1;
				}
				else if (this->s==""){
					if(*((float*)c) != f)
						flag = 1;
				}
				else
				{
					if(strcmp((char*)c,this->s.c_str())!=0)
						flag = 1;
				}
				break;*/
			default:
				flag = 0;
				break;
			}
		}
		if(flag == 1)
			break;
	}
	//this->rm->readAttribute(tableName, rId, attributeNames[0], data);
	//vector<Attribute> attrs;
	void* pData = data;
	void* data_returned = malloc(100);
	vector<Attribute> attr;
	int l = 0;
	rm->getAttributes(tableName, attr);
	for(int i = 0; i < attributeNames.size(); i++){
		//check what type of attributeName
			//if int/real, just add to data
			//else, compute and add the length, and add to data
		this->rm->readAttribute(tableName, rId, attributeNames[i], data_returned);
		for(int j = 0; j < attr.size(); j++){
			if(strcmp(attr[j].name.c_str(),attributeNames[i].c_str())==0){
				if(attr[j].type==TypeVarChar){
					l = strlen((char *)data_returned);
					*((int *)pData) = l;
					pData = (char*)pData + sizeof(int);
					memcpy(pData,data_returned,l);
					pData = (char *)pData + l;
				}
				else if(attr[j].type==TypeReal){
					memcpy(pData,data_returned,sizeof(float));
					//cout << attr[j].type << ": " << *(float *)data_returned << endl;
					pData = (char*)pData + sizeof(float);
				}
				else{
					memcpy(pData,data_returned,sizeof(int));
					//cout << attr[j].type << ": " << *(int *)data_returned << endl;
					pData = (char*)pData + sizeof(int);
				}
				break;
			}
		}

	}

	//printData(data);
	pf->CloseFile(fileHandle);
	return 0;

}

void RM_ScanIterator::printData(void* data)
{
	void* pData = data;
	cout << *(int *)pData << endl;

	pData = (char*)pData + sizeof(int);
	cout <<  *(int *)pData << endl;

	pData = (char*)pData + sizeof(int);
	cout <<  *(float *)pData << endl;
}

string RM_ScanIterator::readtName()
{
	return tableName;
}

void RM_ScanIterator::settName(string tableName)
{
	this->tableName = tableName;
}

vector<string> RM_ScanIterator::readattrName()
{
	return attributeNames;
}

void RM_ScanIterator::setattrName( vector<string> attributeNames)
{
	this->attributeNames = attributeNames;
}
string RM_ScanIterator::readcAttr()
{
	return conditionAttribute;
}

void RM_ScanIterator::setcAttr(string conditionAttribute)
{
	this->conditionAttribute = conditionAttribute;
}
CompOp RM_ScanIterator::readcompOp()
{
	return compOp;
}

void RM_ScanIterator::setcompOp(CompOp compOp)
{
	this->compOp = compOp;
}

int RM_ScanIterator::readValue()
{
	return value;
}

void RM_ScanIterator::setValue(int value)
{
	this->value = value;
}

string RM_ScanIterator::readString()
{
	return s;
}

void RM_ScanIterator::setString(string s)
{
	this->s = s;
}

float RM_ScanIterator::readFloat()
{
	return f;
}

void RM_ScanIterator::setFloat(float f)
{
	this->f = f;
}

RC RM_ScanIterator::close()
{
	return -1;
}


