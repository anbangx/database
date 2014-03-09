#include <math.h>

# include "qe.h"

using namespace std;

HashJoin* HashJoin::_hashjoin = 0;
PF_Manager *pf1 = PF_Manager::Instance();
const unsigned bufsize = 200;


string convertIntToString(int number)
{
	char buf[bufsize];
	sprintf (buf, "%d", number);
	return buf;
}

class hashBlock{
public:
	int index;
	void* block;
	void* groupData; //groupData
	Attribute aggAttr;
	Attribute gAttr;
	AggregateOp op;
	void* max;
	void* min;
	void* sum;
	void* avg;
	float count;


	hashBlock(int index, Attribute aggAttr, Attribute gAttr, AggregateOp op, void* groupData, void* data){
		this->index = index;
		this->block = malloc(200);
		this->groupData = malloc(200);
		memcpy(this->groupData,groupData,200);
		this->aggAttr = aggAttr;
		this->gAttr = gAttr;
		this->op = op;
		this->max = malloc(20);
		*(float *)(this->max) = (float)(*(int *)data);
		this->min = malloc(20);
		*(float *)(this->min) = (float)(*(int *)data);
		this->avg = malloc(20);
		*(float *)(this->avg) = (float)(*(int *)data);
		this->sum = malloc(20);
		*(float *)(this->sum) = (float)(*(int *)data); //memcpy(this->sum,(float)(*(int *)data,20);

		count = 1;

		/*switch(aggAttr.type){
		case TypeInt:
			*(int *)this->max = *(int *)data;
			*(int *)this->min = *(int *)data;
			*(int *)this->sum = *(int *)data;
			*(int *)this->avg = *(int *)data;
			count = 1;
			break;
		case TypeReal:
			*(float *)this->max = *(float *)data;
			*(float *)this->min = *(float *)data;
			*(float *)this->sum = *(float *)data;
			*(float *)this->avg = *(float *)data;
			count = 1;
			break;
		}*/

		switch(gAttr.type){
		case TypeInt:
			*(int *)this->groupData = *(int *)groupData;
			break;
		case TypeReal:
			*(float *)this->groupData = *(float *)groupData;
			break;
		case TypeVarChar:
			*(char *)this->groupData = *(char *)groupData;
			break;
		}
	}

	~hashBlock(){
		/*free(block);
		free(groupData); //groupData
		free(max);
		free(min);
		free(sum);
		free(avg);*/
	}

	void update(void *data){
		count++;
		switch(op){
		case MAX:
			switch(aggAttr.type){
			case TypeInt:
				if(*(float *)max < (float)(*(int *)data))
					*(float *)max = (float)(*(int *)data);
				break;
			case TypeReal:
				if(*(float *)max < *(float *)data)
					*(float *)max = *(float *)data;
				break;
			}
			break;
		case MIN:
			switch(aggAttr.type){
			case TypeInt:
				if(*(float *)min > (float)(*(int *)data))
					*(float *)min = (float)(*(int *)data);
				break;
			case TypeReal:
				if(*(float *)min > *(float *)data)
					*(float *)min = *(float *)data;
				break;
			}
			break;
		case SUM:
			switch(aggAttr.type){
			case TypeInt:
				*(float *)sum = *(float *)sum + (float)(*(int *)data);
				break;
			case TypeReal:
				*(float *)sum = *(float *)sum + *(float *)data;
				break;
			}
			break;
		case AVG:
			switch(aggAttr.type){
			case TypeInt:
				*(float *)sum = *(float *)sum + (float)(*(int *)data);
				*(float *)avg = *(float *)sum/float(count); //thinking
				break;
			case TypeReal:
				*(float *)sum = *(float *)sum + *(float *)data;
				*(float *)avg = (*(float *)sum)/float(count);
				break;
			}
			break;
		}
	}
};

class partitioningPage{
public:
	int freeSpace;
	vector<int> offset;
	vector<int> length;
	int size;
	void* currentPage;
	int index;
	int count;
	int leftOrRight; // 0 = left, 1 = right

	partitioningPage(void* page, int index, int leftOrRight)
	{
		this->freeSpace = 4;
		this->size = 0;
		this->currentPage = page;
		writeToMem(currentPage);

		this->index = index;
		this->leftOrRight = leftOrRight;

		count = 0;
	}
	~partitioningPage(){}

	void insertData(void* data, int lengthOfData){
		readFromMem(currentPage);

		offset.push_back(freeSpace);
		length.push_back(lengthOfData);
		size++;
		memcpy((char *)currentPage + freeSpace, data, lengthOfData);
		/*if(leftOrRight == 1){
		cout << "B: " << *(int *)((char *)currentPage + freeSpace) << endl;
		cout << "C: " << *(float *)((char *)currentPage + freeSpace + 4) << endl;
		cout << "D: " << *(int *)((char *)currentPage + freeSpace + 8) << endl << endl;
		}*/

		freeSpace += lengthOfData;

		writeToMem(currentPage);

		//if full, flush into file
		if(size == 40)
			flushToFile();
	}

	void insertLastData(void* data, int lengthOfData){
			readFromMem(currentPage);

			offset.push_back(freeSpace);
			length.push_back(lengthOfData);
			size++;
			memcpy((char *)currentPage + freeSpace, data, lengthOfData);

			freeSpace += lengthOfData;

			writeToMem(currentPage);

			flushToFile();
	}

	void writeToMem(void* page){
		*(int *)page = freeSpace;
		*(int *)((char *)page + 4096 - sizeof(int)) = size;
		for(int i = 0; i < size; i++){
		    *(int *)((char *)page + 4096 - sizeof(int) - (i+1)*2*sizeof(int)) = offset[i];
			*(int *)((char *)page + 4096 - sizeof(int) - (i*2+1)*sizeof(int)) = length[i];
		}
	}

	void readFromMem(void* page){
		freeSpace = *(int *)page;
		size = *(int *)((char *)page + 4096 - sizeof(int));
		offset.clear();
		length.clear();
		for(int i = 0; i < size; i++){
			offset.push_back(*(int *)((char *)page + 4096 - sizeof(int) - (i+1)*2*sizeof(int)));
			length.push_back(*(int *)((char *)page + 4096 - sizeof(int) - (i*2+1)*sizeof(int)));
		}
	}

	void flushToFile(){
		PF_FileHandle fileHandle;

		string str = convertIntToString(index);
		string fileName;
		if(leftOrRight == 0)
			fileName = "R" + str;
		else
			fileName = "S" + str;

		pf1->OpenFile(fileName.c_str(), fileHandle);
		fileHandle.AppendPage(currentPage);
		pf1->CloseFile(fileHandle);

		//initiate new partitioning page
		offset.clear();
		length.clear();
		this->freeSpace = 4;
		this->size = 0;
		void* page = malloc(PF_PAGE_SIZE);
		memcpy(this->currentPage,page,PF_PAGE_SIZE);
		writeToMem(currentPage);
		free(page);
	}

	void readFromFile(int pageNum){
		PF_FileHandle fileHandle;

		string str = convertIntToString(index);
		string fileName;
		if(leftOrRight == 0)
			fileName = "R" + str;
		else
			fileName = "S" + str;

		pf1->OpenFile(fileName.c_str(), fileHandle);
		fileHandle.ReadPage(pageNum,currentPage);
		pf1->CloseFile(fileHandle);

		//set class parameter
		readFromMem(currentPage);
	}

	void readDataFromPage(int pageNum, int idx, void* data){
		void* page = malloc(PF_PAGE_SIZE);
		PF_FileHandle fileHandle;

		string str = convertIntToString(index);
		string fileName;
		if(leftOrRight == 0)
			fileName = "R" + str;
		else
			fileName = "S" + str;

		pf1->OpenFile(fileName.c_str(), fileHandle);
		fileHandle.ReadPage(pageNum,page);
		pf1->CloseFile(fileHandle);

		memcpy(data,(char *)page + offset[idx],length[idx]);
		free(page);
	}
};

class joinPage{
public:
	int freeSpace;
	vector<int> offset;
	vector<int> length;
	int size;
	void* currentPage;
	int index;

	joinPage(void* page, int index)
	{
		this->freeSpace = 4;
		this->size = 0;
		this->currentPage = page;
		writeToMem(currentPage);

		this->index = index;
	}
	~joinPage(){}

	void insertData(void* data, int lengthOfData){
		readFromMem(currentPage);

		offset.push_back(freeSpace);
		length.push_back(lengthOfData);
		size++;

		memcpy((char *)currentPage + freeSpace, data, lengthOfData);

		freeSpace += lengthOfData;

		writeToMem(currentPage);
	}

	void writeToMem(void* page){
		*(int *)page = freeSpace;
		*(int *)((char *)page + 4096 - sizeof(int)) = size;
		for(int i = 0; i < size; i++){
		    *(int *)((char *)page + 4096 - sizeof(int) - (i+1)*2*sizeof(int)) = offset[i];
			*(int *)((char *)page + 4096 - sizeof(int) - (i*2+1)*sizeof(int)) = length[i];
		}
	}

	void readFromMem(void* page){
		freeSpace = *(int *)page;
		size = *(int *)((char *)page + 4096 - sizeof(int));
		for(int i = 0; i < size; i++){
			offset[i] = *(int *)((char *)page + 4096 - sizeof(int) - (i+1)*2*sizeof(int));
			length[i] = *(int *)((char *)page + 4096 - sizeof(int) - (i*2+1)*sizeof(int));
		}
	}

	void readDataFromPage(int idx, void* data){
		memcpy(data,(char *)currentPage + offset[idx],length[idx]);
	}

};

class ScanS{
public:
	int numOfPages_R;
	int i;
	int x;
	int y;

	ScanS(int i){

		PF_FileHandle fileHandle;
		string str = convertIntToString(i);
		string fileName = "S" + str;
		pf1->OpenFile(fileName.c_str(),fileHandle);
		this->numOfPages_R  = fileHandle.GetNumberOfPages();
		pf1->CloseFile(fileHandle);


		this->i = i;
		this->x = 0;
		this->y = 0;

	}

	~ScanS(){}

	RC getNextTuple(void* data){

		if(x == numOfPages_R)
			return -1;

		//scan S'
		partitioningPage *pPage;
		void* page = malloc(PF_PAGE_SIZE);
		pPage = new partitioningPage(page, i, 1);

		//read currentPage from file
		pPage->readFromFile(x);

		//cout << "size: " << pPage->size << endl;
		while(pPage->size == 0){
			x++;
			pPage->readFromFile(x);
			//cout << "size: " << pPage->size << endl;
			if(x == numOfPages_R)
				return -1;
		}
		//read one tuple
		pPage->readDataFromPage(x,y,data);

		y++;
		if(y == pPage->size){
			x++;
			y = 0;
		}

		free(page);
		return 0;
	}
};



Filter::Filter(Iterator *input,const Condition &condition) // Iterator of input R // Selection condition
{
	this->iter = input;
	this->cond = condition;
};
Filter::~Filter(){};

RC Filter::getNextTuple(void *data)
{
	while(iter->getNextTuple(data) == 0){
		if(this->compareAttriVal(data) == 0)
			return 0;
	}

	return -1;
};
// For attribute in vector<Attribute>, name it as rel.attr
void Filter::getAttributes(vector<Attribute> &attrs) const
{
	attrs.clear();
	iter->getAttributes(attrs);
};

RC Filter::compareAttriVal(void *data)
{
	vector<Attribute> attrs;
	this->getAttributes(attrs);
	void *pData = data;
	Value value = cond.rhsValue;
	for(unsigned int i = 0; i < attrs.size(); i++){
		if(strcmp(attrs[i].name.c_str(), cond.lhsAttr.c_str()) == 0){
			switch(value.type){
			case TypeInt:{
				int comVal = *(int *)value.data;
				int val = *(int *)pData;
				switch(cond.op){
				case EQ_OP:{
					if(val == comVal)
						return 0;
					else
						return -1;
				}
				case LT_OP:{
					if(val < comVal)
						return 0;
					else
						return -1;
				}
				case GT_OP:{
					if(val > comVal)
						return 0;
					else
						return -1;
				}
				case LE_OP:{
					if(val <= comVal)
						return 0;
					else
						return -1;
				}
				case GE_OP:{
					if(val >= comVal)
						return 0;
					else
						return -1;
				}
				case NO_OP:
					return 0;
				}
				return 0;
			}

			case TypeReal:{
				float comVal = *(float *)value.data;
				float val = *(float *)pData;
				switch(cond.op){
				case EQ_OP:{
					if(val == comVal)
						return 0;
					else
						return -1;
				}
				case LT_OP:{
					if(val < comVal)
						return 0;
					else
						return -1;
				}
				case GT_OP:{
					if(val > comVal)
						return 0;
					else
						return -1;
				}
				case LE_OP:{
					if(val <= comVal)
						return 0;
					else
						return -1;
				}
				case GE_OP:{
					if(val >= comVal)
						return 0;
					else
						return -1;
				}
				case NO_OP:
					return 0;
				}
				return 0;
			}
			case TypeVarChar:{
				int comVal = *(int *)value.data;
				int val = *(int *)pData;
				switch(cond.op){
				case EQ_OP:{
					if(val == comVal)
						return 0;
					else
						return -1;
				}
				case LT_OP:{
					if(val < comVal)
						return 0;
					else
						return -1;
				}
				case GT_OP:{
					if(val > comVal)
						return 0;
					else
						return -1;
				}
				case LE_OP:{
					if(val <= comVal)
						return 0;
					else
						return -1;
				}
				case GE_OP:{
					if(val >= comVal)
						return 0;
					else
						return -1;
				}
				case NO_OP:
					return 0;
				}
				return 0;
			}
			}
		}
		else{
			switch(attrs[i].type){
			case TypeInt:
				pData = (char *)pData + sizeof(int);
				break;
			case TypeReal:
				pData = (char *)pData + sizeof(float);
				break;
			case TypeVarChar:
				int len = *(int *)pData;
				pData = (char *)pData + sizeof(int);
				pData = (char *)pData + len;
				break;
			}
		}
	}
	return 0;
}

// Iterator of input R // vector containing attribute names
Project::Project(Iterator *input, const vector<string> &attrNames)
{
	this->iter = input;
	this->attrNames = attrNames;
};
Project::~Project(){};

RC Project::getNextTuple(void *data)
{
	while(iter->getNextTuple(data)==0){
		projectSpecificAttrs(data);
		return 0;
	}

	return QE_EOF;
};
// For attribute in vector<Attribute>, name it as rel.attr
void Project::getAttributes(vector<Attribute> &attrs) const
{
	attrs.clear();
	vector<Attribute> originalAttrs;
	iter->getAttributes(originalAttrs);
	unsigned int j = 0;
	for(unsigned int i = 0; i < originalAttrs.size(); i++){
		if(strcmp(originalAttrs[i].name.c_str(), attrNames[j].c_str()) == 0){
			attrs.push_back(originalAttrs[i]);
			j++;
		}
		if(j == attrNames.size())
			break;
	}
};

void Project::projectSpecificAttrs(void* data)
{
	vector<Attribute> attrs;
	iter->getAttributes(attrs);
	void* pData = data;
	void* temp = malloc(200);
	void* pTemp = temp;
	int size = 0;
	unsigned int j = 0;
	for(unsigned int i = 0; i < attrs.size(); i++){
		if(strcmp(attrs[i].name.c_str(), attrNames[j].c_str()) == 0){
			switch(attrs[i].type){
			case TypeInt:{
				*(int *)pTemp = *(int *)pData;
				pTemp = (char *)pTemp + sizeof(int);
				pData = (char *)pData + sizeof(int);
				size += sizeof(int);
				break;
			}
			case TypeReal:{
				*(float *)pTemp = *(float *)pData;
				pTemp = (char *)pTemp + sizeof(float);
				pData = (char *)pData + sizeof(float);
				size += sizeof(float);
				break;
			}
			case TypeVarChar:{
				int len = *(int *)pData;
				*(int *)pTemp = *(int *)pData;
				pTemp = (char *)pTemp + sizeof(int);
				pData = (char *)pData + sizeof(int);
				size += sizeof(int);
				memcpy((char *)pTemp, (char *)pData, len);
				pTemp = (char *)pTemp + len;
				pData = (char *)pData + len;
				size += len;
				break;
			}

			}
			j++;
			if(j == attrNames.size())
				break;
		}
		else{
			switch(attrs[i].type){
			case TypeInt:
				pData = (char *)pData + sizeof(int);
				break;
			case TypeReal:
				pData = (char *)pData + sizeof(float);
				break;
			case TypeVarChar:
				int len = *(int *)pData;
				pData = (char *)pData + sizeof(int);
				pData = (char *)pData + len;
				break;
			}
		}
	}

	memcpy(data,temp,size);
	//free(temp);
	//free(pData);
	//free(pTemp);
}

// Iterator of input R // TableScan Iterator of input S // Join condition // Number of pages can be used to do join (decided by the optimizer)
NLJoin::NLJoin(Iterator *leftIn, TableScan *rightIn, const Condition &condition, const unsigned numPages)
{
	this->lIter = leftIn;
	this->rIter = rightIn;
	this->cond = condition;
	this->flag = 0;
	this->lData = malloc(200);
};
NLJoin::~NLJoin(){};

RC NLJoin::getNextTuple(void *data)
{

	void* rData = malloc(200);
	int lSize = 0;
	int rSize = 0;

	while(true){
		if(flag == 0){
			if(lIter->getNextTuple(lData)==-1){
				free(lData);
				free(rData);
				return QE_EOF;
			}
		}

		while(rIter->getNextTuple(rData) == 0){
			if(compareAttrFromLeftAndRight(lData,rData,&lSize,&rSize) == 0){
				memcpy(data,lData,lSize);
				memcpy((char *)data + lSize,rData,rSize);

				flag = 1;
				//free(lData);
				free(rData);
				return 0;
			}
		}

		flag = 0;
		rIter->setIterator();
	}
	return QE_EOF;
};

// For attribute in vector<Attribute>, name it as rel.attr
void NLJoin::getAttributes(vector<Attribute> &attrs) const
{
	attrs.clear();
	unsigned int i;

	vector<Attribute> lAttrs;
	vector<Attribute> rAttrs;
	lIter->getAttributes(lAttrs);
	rIter->getAttributes(rAttrs);

	for(i = 0; i < lAttrs.size(); i++)
		attrs.push_back(lAttrs[i]);

	for(i = 0; i < rAttrs.size(); i++)
		attrs.push_back(rAttrs[i]);

	return;
};

RC NLJoin::compareAttrFromLeftAndRight(void* lData, void* rData, int* lSize, int* rSize)
{
	vector<Attribute> lAttrs;
	lIter->getAttributes(lAttrs);
	vector<Attribute> rAttrs;
	rIter->getAttributes(rAttrs);
	Value leftVal;
	Value rightVal;
	void* plData = lData;
	void* prData = rData;
	*lSize = 0;
	*rSize = 0;
	int len = 0;

	//get specific attr from left
	for(unsigned int i = 0; i < lAttrs.size(); i++){
		if(strcmp(lAttrs[i].name.c_str(), cond.lhsAttr.c_str()) == 0){
			leftVal.type = lAttrs[i].type;
			switch(leftVal.type){
			case TypeInt:{
				leftVal.data = malloc(200);
				*(int *)leftVal.data = *(int *)plData;
				*lSize += sizeof(int);
				plData = (char *)plData + sizeof(int);
				break;
			}
			case TypeReal:{
				leftVal.data = malloc(200);
				*(float *)leftVal.data = *(float *)plData;
				*lSize += sizeof(float);
				plData = (char *)plData + sizeof(float);
				break;
			}
			case TypeVarChar:{
				leftVal.data = malloc(200);
				len = *(int*)plData;
				*lSize += sizeof(int);
				memcpy((char *)leftVal.data, (char*)plData + 4, len);
				memset((char *)leftVal.data + len,'\0',1);
				*lSize += len;

				plData = (char *)plData + sizeof(int);
				plData = (char *)plData + len;
				break;
			}

			break;
			}
		}
		else{
			switch(lAttrs[i].type){
			case TypeInt:
				plData = (char *)plData + sizeof(int);
				*lSize += sizeof(int);
				break;
			case TypeReal:
				plData = (char *)plData + sizeof(float);
				*lSize += sizeof(float);
				break;
			case TypeVarChar:
				len = *(int *)(char *)plData;
				plData = (char *)plData + sizeof(int);
				*lSize += sizeof(int);
				plData = (char *)plData + len;
				*lSize += len;
				break;
			}
		}
	}

	//get specific attr from right
	for(unsigned int i = 0; i < rAttrs.size(); i++){
		if(strcmp(rAttrs[i].name.c_str(), cond.rhsAttr.c_str()) == 0){
			rightVal.type = rAttrs[i].type;
			switch(rightVal.type){
			case TypeInt:{
				rightVal.data = malloc(200);
				*(int *)rightVal.data = *(int *)prData;
				*rSize += sizeof(int);
				prData = (char *)prData + sizeof(int);
				break;
			}
			case TypeReal:{
				rightVal.data = malloc(200);
				*(float *)rightVal.data = *(float *)prData;
				*rSize += sizeof(float);
				prData = (char *)prData + sizeof(float);
				break;
			}
			case TypeVarChar:{
				rightVal.data = malloc(200);
				len = *(int*)prData;
				*rSize += sizeof(int);
				memcpy((char *)rightVal.data, (char*)prData + 4, len);
				memset((char *)rightVal.data + len,'\0',1);
				*rSize += len;

				prData = (char *)prData + sizeof(int);
				prData = (char *)prData + len;
			}
			break;
			}
		}
		else{
			switch(rAttrs[i].type){
			case TypeInt:
				prData = (char *)prData + sizeof(int);
				*rSize += sizeof(int);
				break;
			case TypeReal:
				prData = (char *)prData + sizeof(float);
				*rSize += sizeof(float);
				break;
			case TypeVarChar:
				len = *(int *)(char *)prData;
				prData = (char *)prData + sizeof(int);
				*rSize += sizeof(int);
				prData = (char *)prData + len;
				*rSize += len;
				break;
			}
		}
	}

	//check if leftVal == rightVal
	switch(leftVal.type){
	case TypeInt:
		if(*(int *)leftVal.data == *(int *)rightVal.data)
			return 0;
		break;
	case TypeReal:
		if(*(float *)leftVal.data == *(float *)rightVal.data)
			return 0;
		break;
	case TypeVarChar:
		string lVal = (char *)leftVal.data;
		string rVal = (char *)rightVal.data;
		if(lVal == rVal)
			return 0;
		break;
	}

	return -1;
}

// Iterator of input R // IndexScan Iterator of input S // Join condition // Number of pages can be used to do join (decided by the optimizer)
INLJoin::INLJoin(Iterator *leftIn, IndexScan *rightIn, const Condition &condition, const unsigned numPages)
{
	this->lIter = leftIn;
	this->rIter = rightIn;
	//string str = (char*)cond.rhsValue.data;
	this->rIter->setIterator(NO_OP,cond.rhsValue.data);
	this->cond = condition;
	this->flag = 0;
	this->lData = malloc(200);
};

INLJoin::~INLJoin(){};

RC INLJoin::getNextTuple(void *data)
{

	//void* lData = malloc(200);
	void* rData = malloc(200);
	int lSize = 0;
	int rSize = 0;

	while(true){
		if(flag == 0){
			if(lIter->getNextTuple(lData)==-1){
				free(lData);
				free(rData);
				return QE_EOF;
			}
		}

		while(rIter->getNextTuple(rData) == 0){
			if(compareAttrFromLeftAndRight(lData,rData,&lSize,&rSize) == 0){
				memcpy(data,lData,lSize);
				memcpy((char *)data + lSize,rData,rSize);

				flag = 1;
				free(rData);
				return 0;
			}
		}

		flag = 0;
		rIter->setIterator(NO_OP,cond.rhsValue.data);
	}
	return QE_EOF;
};
// For attribute in vector<Attribute>, name it as rel.attr
void INLJoin::getAttributes(vector<Attribute> &attrs) const
{
	attrs.clear();
	unsigned int i;

	vector<Attribute> lAttrs;
	vector<Attribute> rAttrs;
	lIter->getAttributes(lAttrs);
	rIter->getAttributes(rAttrs);

	for(i = 0; i < lAttrs.size(); i++)
		attrs.push_back(lAttrs[i]);

	for(i = 0; i < rAttrs.size(); i++)
		attrs.push_back(rAttrs[i]);

	return;
};
RC INLJoin::compareAttrFromLeftAndRight(void* lData, void* rData, int* lSize, int* rSize)
{
	vector<Attribute> lAttrs;
	lIter->getAttributes(lAttrs);
	vector<Attribute> rAttrs;
	rIter->getAttributes(rAttrs);
	Value leftVal;
	Value rightVal;
	void* plData = lData;
	void* prData = rData;
	*lSize = 0;
	*rSize = 0;
	int len = 0;

	//get specific attr from left
	for(unsigned int i = 0; i < lAttrs.size(); i++){
		if(strcmp(lAttrs[i].name.c_str(), cond.lhsAttr.c_str()) == 0){
			leftVal.type = lAttrs[i].type;
			switch(leftVal.type){
			case TypeInt:{
				leftVal.data = malloc(200);
				*(int *)leftVal.data = *(int *)plData;
				*lSize += sizeof(int);
				plData = (char *)plData + sizeof(int);
				break;
			}
			case TypeReal:{
				leftVal.data = malloc(200);
				*(float *)leftVal.data = *(float *)plData;
				*lSize += sizeof(float);
				plData = (char *)plData + sizeof(float);
				break;
			}
			case TypeVarChar:{
				leftVal.data = malloc(200);
				len = *(int*)plData;
				*lSize += sizeof(int);
				memcpy((char *)leftVal.data, (char*)plData + 4, len);
				memset((char *)leftVal.data + len,'\0',1);
				*lSize += len;

				plData = (char *)plData + sizeof(int);
				plData = (char *)plData + len;
				break;
			}

			break;
			}
		}
		else{
			switch(lAttrs[i].type){
			case TypeInt:
				plData = (char *)plData + sizeof(int);
				*lSize += sizeof(int);
				break;
			case TypeReal:
				plData = (char *)plData + sizeof(float);
				*lSize += sizeof(float);
				break;
			case TypeVarChar:
				len = *(int *)(char *)plData;
				plData = (char *)plData + sizeof(int);
				*lSize += sizeof(int);
				plData = (char *)plData + len;
				*lSize += len;
				break;
			}
		}
	}

	//get specific attr from right
	for(unsigned int i = 0; i < rAttrs.size(); i++){
		if(strcmp(rAttrs[i].name.c_str(), cond.rhsAttr.c_str()) == 0){
			rightVal.type = rAttrs[i].type;
			switch(rightVal.type){
			case TypeInt:{
				rightVal.data = malloc(200);
				*(int *)rightVal.data = *(int *)prData;
				*rSize += sizeof(int);
				prData = (char *)prData + sizeof(int);
				break;
			}
			case TypeReal:{
				rightVal.data = malloc(200);
				*(float *)rightVal.data = *(float *)prData;
				*rSize += sizeof(float);
				prData = (char *)prData + sizeof(float);
				break;
			}
			case TypeVarChar:{
				rightVal.data = malloc(200);
				len = *(int*)prData;
				*rSize += sizeof(int);
				memcpy((char *)rightVal.data, (char*)prData + 4, len);
				memset((char *)rightVal.data + len,'\0',1);
				*rSize += len;

				prData = (char *)prData + sizeof(int);
				prData = (char *)prData + len;
			}
			break;
			}
		}
		else{
			switch(rAttrs[i].type){
			case TypeInt:
				prData = (char *)prData + sizeof(int);
				*rSize += sizeof(int);
				break;
			case TypeReal:
				prData = (char *)prData + sizeof(float);
				*rSize += sizeof(float);
				break;
			case TypeVarChar:
				len = *(int *)(char *)prData;
				prData = (char *)prData + sizeof(int);
				*rSize += sizeof(int);
				prData = (char *)prData + len;
				*rSize += len;
				break;
			}
		}
	}

	//check if leftVal == rightVal
	switch(leftVal.type){
	case TypeInt:
		if(*(int *)leftVal.data == *(int *)rightVal.data)
			return 0;
		break;
	case TypeReal:
		if(*(float *)leftVal.data == *(float *)rightVal.data)
			return 0;
		break;
	case TypeVarChar:
		string lVal = (char *)leftVal.data;
		string rVal = (char *)rightVal.data;
		if(lVal == rVal)
			return 0;
		break;
	}

	return -1;
}

HashJoin::HashJoin(Iterator *leftIn, Iterator *rightIn, const Condition &condition, const unsigned numPages)
{
	this->lIter = leftIn;
	this->rIter = rightIn;
	this->cond = condition;
	this->numPages = numPages;
	this->I = 0;
	this->k = 0;
	rData = malloc(bufsize);
	this->indexProbe = -1;

	this->partitioningPhase();

	//scan S' and probe for matching R tuple
	this->scanS = new ScanS(I);

	this->inMemoryHashTableForR();
	this->scanS->getNextTuple(rData);
}

HashJoin::~HashJoin()
{
}

RC HashJoin::getNextTuple(void *data)
{
	//get attrs of this table
	vector<Attribute> lAttrs;
	lIter->getAttributes(lAttrs);
	vector<Attribute> rAttrs;
	rIter->getAttributes(rAttrs);

	void* page;
	//int index = -1;
	int lLength = 0;
	int rLength = 0;
	void *lData = malloc(bufsize);

	while(true){
		//Probing phase

		while(true){
			//get one tuple from S'
			//&& vJoinPage[indexProbe].size
			if(indexProbe != -1 && k == vJoinPage[indexProbe].size){
				indexProbe = 0;
				while(true){
					if(vJoinPage[indexProbe].size != 0)
						break;
					indexProbe++;
				}
				k = 0;
				RC rc = this->scanS->getNextTuple(rData);
				if(rc == -1){
					string str = convertIntToString(I);
					string fileName = "R" + str;
					string fileName2 = "S" + str;

					pf1->DestroyFile(fileName.c_str());
					pf1->DestroyFile(fileName2.c_str());

					I++;
					if(I == numPages - 1)
						return -1;

					this->inMemoryHashTableForR();
					this->scanS = new ScanS(I);
					this->scanS->getNextTuple(rData);
				}
			}
			else{
				//get right.C of one tuple
				Value rValue = this->projectSpecificAttrs(rData, 1);

				//hash(right.c)
				switch(rValue.type){
				case TypeInt:
					indexProbe = this->hashFunction(rValue,numPages - 2);
					break;
				case TypeReal:
					indexProbe = this->hashFunction(rValue,numPages - 2);
					break;
				case TypeVarChar:
					indexProbe = this->hashFunctionForVarChar(rValue,numPages - 2);
					break;
				}


				//get length of data
				rLength = this->getLengthOfData(rData,rAttrs);

				//check if matching
				int flag = 0;
				for(; k < vJoinPage[indexProbe].size; k++){
					vJoinPage[indexProbe].readDataFromPage(k,lData);
					Value lValue = this->projectSpecificAttrs(lData, 0);
					//get length of data
					lLength = this->getLengthOfData(lData,lAttrs);
					switch(lValue.type){
					case TypeInt:
						//cout << "l.A: " << *(int *)lData << endl;
						//cout << "l.B: " << *(int *)((char *)lData + 4) << endl;
						//cout << "r.B: " << *(int *)rValue.data << endl << endl;
						//cout << "r.D: " << *(int *)((char *)lData + 7) << endl;

						if(*(int *)rValue.data == *(int *)lValue.data){
							flag = 1;
							break;
						}
						break;
					case TypeReal:
						if(*(int *)rValue.data == *(int *)lValue.data){
							flag = 1;
							break;
						}
						break;
					case TypeVarChar:
						string lVal = (char *)lValue.data;
						string rVal = (char *)rValue.data;
						if(lVal == rVal){
							flag = 1;
							break;
						}
						break;
					}
					if(flag == 1){
						memcpy(data,lData,lLength);
						memcpy((char *)data + lLength,rData,rLength);

						free(lData);
						//free(rData);
						k++;
						return 0;
					}
				}
			}
		}
	}

	free(page);
	return QE_EOF;
}

void HashJoin::inMemoryHashTableForR()
{
	void* page;
	unsigned int numOfPages_L = 0;

	vector<Attribute> lAttrs;
	lIter->getAttributes(lAttrs);
	int index = -1;
	Value value;
	int lLength = 0;
	void* tuple;
	partitioningPage *pPage;

	//initiate vector of join page
	vJoinPage.clear();
	for(unsigned int i = 0; i < numPages - 2; i++){
		page = malloc(PF_PAGE_SIZE);
		joinPage *jPage = new joinPage(page, i);
		vJoinPage.push_back(*jPage);
		delete jPage;
	}

	//build in-memory hash table for R', using h2 (num: numPages - 1)
	//get numOfPages
	PF_FileHandle fileHandle;
	string str = convertIntToString(I);
	string fileName = "R" + str;
	pf1->OpenFile(fileName.c_str(),fileHandle);
	numOfPages_L = fileHandle.GetNumberOfPages();
	pf1->CloseFile(fileHandle);

	page = malloc(PF_PAGE_SIZE);
	pPage = new partitioningPage(page, I, 0);

	//read page into input buffer one by one
	for(unsigned int x = 0; x < numOfPages_L; x++){
		//read currentPage from file
		pPage->readFromFile(x);

		for(int y = 0; y < pPage->size; y++){
			//read one tuple
			tuple = malloc(bufsize);
			pPage->readDataFromPage(x,y,tuple);
/*
			int offset = 0;
			int length = 0;
			void *c = malloc(200);
			cout << "x: " << x << endl;
			// Print leftSecond.A
			cout << "leftSecond.A " << *(int *)((char *)tuple + offset) << endl;
			offset += sizeof(int);

			// Print left.B
			cout << "leftSecond.B " << *(int *)((char *)tuple + offset) << endl;
			offset += sizeof(int);

			// Print leftSecond.C
			length = *(int *)((char *)tuple + offset);
			offset += sizeof(int);
			cout << "leftSecond.C.length " << length << endl;

			memset(c, 0, 200);
			memcpy(c, (char *)tuple + offset, length);
			*((char *)c + length) = 0;
			offset += length;
			cout << "leftSecond.C " << (char *)c << endl;

			cout << endl;*/

			//get left.C of one tuple
			value = this->projectSpecificAttrs(tuple, 0);

			//hash(left.c)
			switch(value.type){
			case TypeInt:
				index = this->hashFunction(value,numPages - 2);
				break;
			case TypeReal:
				index = this->hashFunction(value,numPages - 2);
				break;
			case TypeVarChar:
				index = this->hashFunctionForVarChar(value,numPages - 2);
				break;
			}

			//get length of data
			lLength = this->getLengthOfData(tuple,lAttrs);

			//add it to buffer page (num: index)
			vJoinPage[index].insertData(tuple,lLength);
		}
	}
	return;
	//free(tuple);
}

void HashJoin::getAttributes(vector<Attribute> &attrs) const
{
	attrs.clear();
	unsigned int i;

	vector<Attribute> lAttrs;
	vector<Attribute> rAttrs;
	lIter->getAttributes(lAttrs);
	rIter->getAttributes(rAttrs);

	for(i = 0; i < lAttrs.size(); i++)
		attrs.push_back(lAttrs[i]);

	for(i = 0; i < rAttrs.size(); i++)
		attrs.push_back(rAttrs[i]);

	return;
}

int HashJoin::getLengthOfData(void* data, vector<Attribute> &attrs){
	void* pData = data;
	int lengthOfData = 0;
	for(unsigned int i = 0; i < attrs.size(); i++){
		switch(attrs[i].type){
		case TypeInt:
			lengthOfData += sizeof(int);
			pData = (char *)pData + sizeof(int);
			break;
		case TypeReal:
			lengthOfData += sizeof(float);
			pData = (char *)pData + sizeof(float);
			break;
		case TypeVarChar:
			int len = *(int *)(char *)pData;
			pData = (char *)pData + sizeof(int);
			lengthOfData += sizeof(int);
			pData = (char *)pData + len;
			lengthOfData += len;
			break;
		}
	}

	return lengthOfData;
}

Value HashJoin::projectSpecificAttrs(void* data, int leftOrRight)
{
	void* pData = data;
	Value value;
	vector<Attribute> attrs;
	string hsAttr;

	if(leftOrRight == 0){
		hsAttr = cond.lhsAttr;
		lIter->getAttributes(attrs);
	}
	else{
		hsAttr = cond.rhsAttr;
		rIter->getAttributes(attrs);
	}


	for(unsigned int i = 0; i < attrs.size(); i++){
		if(strcmp(attrs[i].name.c_str(), hsAttr.c_str()) == 0){
			switch(attrs[i].type){
			case TypeInt:{
				value.type = TypeInt;
				value.data = malloc(bufsize);
				*(int *)value.data = *(int *)pData;
				pData = (char *)pData + sizeof(int);
				break;
			}
			case TypeReal:{
				value.type = TypeReal;
				value.data = malloc(bufsize);
				*(float *)value.data = *(float *)pData;
				pData = (char *)pData + sizeof(float);
				break;
			}
			case TypeVarChar:{
				value.type = TypeVarChar;
				value.data = malloc(bufsize);
				int len = *(int*)pData;
				pData = (char *)pData + sizeof(int);
				memcpy((char *)value.data, (char*)pData, len);
				memset((char *)value.data + len,'\0',1);
				pData = (char *)pData + len;
				break;
			}
			}
			break;
		}
		else{
			switch(attrs[i].type){
			case TypeInt:
				pData = (char *)pData + sizeof(int);
				break;
			case TypeReal:
				pData = (char *)pData + sizeof(float);
				break;
			case TypeVarChar:
				int len = *(int *)pData;
				pData = (char *)pData + sizeof(int);
				pData = (char *)pData + len;
				break;
			}
		}
	}

	return value;
}

int HashJoin::hashFunction(Value value, int numOfPages)
{
	int index;
	void* typedVal;
	//f(l.C)
	switch(value.type){
	case TypeInt:
		typedVal = new int;
		*(int *)typedVal = *(int *)value.data;
		index = (*(int *)typedVal) % numOfPages;
		break;
	case TypeReal:
		typedVal = new float;
		*(float *)typedVal = *(float *)value.data;
		float temp = fmod(*(float *)typedVal,numOfPages);
		index = floor(temp);
		break;
	/*case TypeVarChar:
		 int len = *(int*)value.data;
		 char * str = (char*)malloc(len + 1);
		 memcpy(str, (char*)value.data + 4, len);
		 str[len] = '\0';
		 string typedVal(str);
		 free(str);
		 break;*/
	}

	return index;
}

int HashJoin::hashFunctionForVarChar(Value value, int numOfPages)
{
	string str = (char *)value.data;
	int sum = 0;
	for (unsigned int k = 0; k < str.length(); k++)
		sum = sum + int(str[k]);
	return  sum % numOfPages;
/*
	char* str = (char *)value.data;
    int i, l = (strlen(str)+1) / 2;
    unsigned int ret = 0;
    unsigned short *s = (unsigned short*)str;
    for (i = 0; i < l; i++)
    {
        ret ^= (s[i] << (i & 0x0F));
    }
    return ret % numOfPages;*/

	/*string str = (char *)value.data;
    int seed = 131;
    unsigned long hash = 0;
    for(int i = 0; i < str.length(); i++)
    {
	   hash = (hash * seed) + str[i];
    }
    return hash % numOfPages;*/
}

void HashJoin::partitioningPhase()
{
	void* lData = malloc(bufsize);
	void* rData1 = malloc(bufsize);
	int index = -1;

	//initiate vector of partitioning page
	vector<partitioningPage> vPartitionPage;
	void* page;

	for(unsigned int i = 0; i < numPages - 1; i++){
		page = malloc(PF_PAGE_SIZE);
		partitioningPage *pPage = new partitioningPage(page, i, 0);
		vPartitionPage.push_back(*pPage);
		delete pPage;
	}

	//get attrs of this table
	vector<Attribute> lAttrs;
	lIter->getAttributes(lAttrs);
	vector<Attribute> rAttrs;
	rIter->getAttributes(rAttrs);

	int lLength = 0;
	int rLength = 0;
	Value value;

	//left R
	//Partition R into k partition, here k = 4
	//initiate partitioning file
	for(unsigned int i = 0; i < numPages - 1; i++){

		string str = convertIntToString(i);
		string fileName = "R" + str;

		pf1->CreateFile(fileName.c_str());
	}

	//for each tuple r in R
	while(lIter->getNextTuple(lData) == 0){
		//print lData
		void *c = malloc(200);
        int offset = 0;
/*
        // Print left.A
        cout << "left.A " << *(int *)((char *)lData + offset) << endl;
        offset += sizeof(int);

        // Print left.B
        cout << "left.B " << *(int *)((char *)lData + offset) << endl;
        offset += sizeof(int);

        // Print left.C
        int length = *(int *)((char *)lData + offset);
        offset += sizeof(int);
        cout << "left.C.length " << length << endl;

        memcpy(c, (char *)lData + offset, length);
        *((char *)c + length) = 0;
        offset += length;
        cout << "left.C " << (char *)c << endl;

        // Print right.B
        cout << "right.B " << *(int *)((char *)lData + offset) << endl;
        offset += sizeof(int);

        // Print right.C
        length = *(int *)((char *)lData + offset);
        *((char *)c + length) = 0;
        offset += sizeof(int);
        cout << "right.C.length " << length << endl;

        memset(c, 0, 200);
        memcpy(c, (char *)lData + offset, length);
        offset += length;
        cout << "right.C " << (char *)c << endl;

        // Print right.D
        cout << "right.D " << *(int *)((char *)lData + offset) << endl;
        offset += sizeof(int);
        //end print
         */

		//get left.C of one tuple
		value = this->projectSpecificAttrs(lData, 0);

		//hash(left.c)
		switch(value.type){
		case TypeInt:
			index = this->hashFunction(value,numPages - 1);
			break;
		case TypeReal:
			index = this->hashFunction(value,numPages - 1);
			break;
		case TypeVarChar:
			index = this->hashFunctionForVarChar(value,numPages - 1);
			break;
		}


		//get length of data
		lLength = this->getLengthOfData(lData,lAttrs);

		//add it to buffer page (num: index)
		vPartitionPage[index].insertData(lData,lLength);
	}

	//flush the last page
	for(unsigned int i = 0; i < vPartitionPage.size(); i++)
		vPartitionPage[i].flushToFile();

	//right S
	//initiate vector of partitioning page
	vPartitionPage.clear();
	for(unsigned int i = 0; i < numPages - 1; i++){
		page = malloc(PF_PAGE_SIZE);
		partitioningPage *pPage = new partitioningPage(page, i, 1);
		vPartitionPage.push_back(*pPage);
		delete pPage;
	}

	//Partition S into k partition, here k = 4
	//initiate partitioning file
	for(unsigned int i = 0; i < numPages - 1; i++){

		string str = convertIntToString(i);
		string fileName = "S" + str;

		pf1->CreateFile(fileName.c_str());
	}

	//for each tuple s in S
	while(rIter->getNextTuple(rData1) == 0){

		void *c = malloc(200);
		int offset = 0;
		int length = 0;
/*		// Print leftSecond.A
		cout << "leftSecond.A " << *(int *)((char *)rData1 + offset) << endl;
		offset += sizeof(int);

		// Print left.B
		cout << "leftSecond.B " << *(int *)((char *)rData1 + offset) << endl;
		offset += sizeof(int);

		// Print leftSecond.C
		length = *(int *)((char *)rData1 + offset);
		offset += sizeof(int);
		cout << "leftSecond.C.length " << length << endl;

		memset(c, 0, 200);
		memcpy(c, (char *)rData1 + offset, length);
		*((char *)c + length) = 0;
		offset += length;
		cout << "leftSecond.C " << (char *)c << endl;

		cout << endl;
		//end print
*/
		//get right.C of one tuple
		value = this->projectSpecificAttrs(rData1, 1);

		//hash(right.c)
		switch(value.type){
		case TypeInt:
			index = this->hashFunction(value,numPages - 1);
			break;
		case TypeReal:
			index = this->hashFunction(value,numPages - 1);
			break;
		case TypeVarChar:
			index = this->hashFunctionForVarChar(value,numPages - 1);
			break;
		}


		//get length of data
		rLength = this->getLengthOfData(rData1,rAttrs);

		//add it to buffer page (num: index)
		vPartitionPage[index].insertData(rData1,rLength);
	}

	//flush the last page
	for(unsigned int i = 0; i < vPartitionPage.size(); i++)
		vPartitionPage[i].flushToFile();
}

Aggregate::Aggregate(Iterator *input, Attribute aggAttr, AggregateOp op){
	this->input = input;
	this->aggAttr = aggAttr;
	this->op = op;
	this->flag = 0;

	Attribute gA;
	gA.name = "";
	this->gAttr = gA;
	this->countForHashBlock = 0;
}

Aggregate::Aggregate(Iterator *input, Attribute aggAttr, Attribute gAttr, AggregateOp op){
	this->input = input;
	this->aggAttr = aggAttr;
	this->gAttr = gAttr;
	this->op = op;
	this->countForHashBlock = 0;

	void* data = malloc(200);
	void* groupData = malloc(200);
	unsigned int i;

	//do hashing on group attribute
	switch(gAttr.type){
	case TypeInt:
		while(this->input->getNextTuple(data)!=0){
			memcpy(groupData,data,200);
			projectSpecificAttrs(data);
			projectSpecificAttrsForGroupAttr(groupData);
			for(i = 0; i < vhashBlock.size(); i++){
				if(*(int *)vhashBlock[i].groupData == *(int *)groupData){
					//exist, so update
					vhashBlock[i].update(data);
					break;
				}
			}

			if(i == vhashBlock.size()){
				//not exist, so create a new one
				hashBlock *newhashBlock = new hashBlock(i, aggAttr, gAttr, op, groupData, data);
				vhashBlock.push_back(*newhashBlock);
				//delete newhashBlock;
			}
		}
		break;
	case TypeReal:
		while(this->input->getNextTuple(data) != QE_EOF){
			//cout << "A: " << *(int *)data << endl;
			memcpy(groupData,data,200);
			projectSpecificAttrs(data);
			//cout << "l.B " << *(int *)data <<endl;
			projectSpecificAttrsForGroupAttr(groupData);
			//cout << "l.C " << *(float *)groupData <<endl;

			for(i = 0; i < vhashBlock.size(); i++){
				if(*(float *)vhashBlock[i].groupData == *(float *)groupData){
					//exist, so update
					vhashBlock[i].update(data);
					break;
				}
			}
			if(i == vhashBlock.size()){
				//not exist, so create a new one
				hashBlock *newhashBlock = new hashBlock(i, aggAttr, gAttr, op, groupData, data);
				vhashBlock.push_back(*newhashBlock);
				//delete newhashBlock;
			}

		}
		break;
	case TypeVarChar:
			while(this->input->getNextTuple(data) != QE_EOF){
				/*void *c = malloc(100);
				int offset = 0;
				int length = *(int *)((char *)groupData + offset);
				offset += sizeof(int);
				cout << "left.C.length " << length << endl;

				memset(c, 0, 100);
				memcpy(c, (char *)groupData + offset, length);
				*((char *)c + length) = 0;
				offset += length;
				cout << "left.C " << (char *)c << endl;*/

				memcpy(groupData,data,200);
				projectSpecificAttrs(data);
				//cout << "l.B " << *(int *)data <<endl;
				projectSpecificAttrsForGroupAttr(groupData);
				// Print left.C
				void *c = malloc(100);
				int offset = 0;
				int length = *(int *)((char *)groupData + offset);
				offset += sizeof(int);
				//cout << "left.C.length " << length << endl;

				memset(c, 0, 100);
				memcpy(c, (char *)groupData + offset, length);
				*((char *)c + length) = 0;
				*((char *)groupData + offset + length) = 0;
				offset += length;
				//cout << "left.C " << (char *)c << endl;

				for(i = 0; i < vhashBlock.size(); i++){
					string str1 = ((char *)groupData +4);;//(char *)c
					string str2 = ((char *)vhashBlock[i].groupData +4);//strcmp((char *)groupData, (char *)(vhashBlock[i].groupData))
					//cout << "str1: " << str1 << endl;
					//cout << "str2: " << str2 << endl;
					if(strcmp(str1.c_str(),str2.c_str()) == 0){
						//exist, so update
						vhashBlock[i].update(data);
						break;
					}
				}
				if(i == vhashBlock.size()){
					//not exist, so create a new one
					hashBlock *newhashBlock = new hashBlock(i, aggAttr, gAttr, op, groupData, data);
					vhashBlock.push_back(*newhashBlock);
					//delete newhashBlock;
				}

			}
			break;
	}

	free(data);
	free(groupData);
}

Aggregate::~Aggregate()
{

}

RC Aggregate::getNextTuple(void *data)
{
	if(gAttr.name == ""){
		if(flag == 0){
			switch(op){
			case MAX:
				return this->getMax(data);
			case MIN:
				return this->getMin(data);
			case SUM:
				return this->getSum(data);
			case AVG:
				return this->getAvg(data);
			case COUNT:
				return this->getCount(data);
			}
		}
	}
	else{
		if(this->countForHashBlock == vhashBlock.size())
			return QE_EOF;

		int length = 0;
		switch(gAttr.type){
		case TypeInt:
			memcpy(data,vhashBlock[countForHashBlock].groupData,4);
			break;
		case TypeReal:
			memcpy(data,vhashBlock[countForHashBlock].groupData,4);
			break;
		case TypeVarChar:
			int offset = 0;
			length = *(int *)((char *)vhashBlock[countForHashBlock].groupData + offset);
			memcpy(data,vhashBlock[countForHashBlock].groupData,4 + length);

/*
			void *c = malloc(100);

			offset += sizeof(int);
			cout << "left.C.length " << length << endl;

			memset(c, 0, 100);
			memcpy(c, (char *)vhashBlock[countForHashBlock].groupData + offset, length);
			*((char *)c + length) = 0;
			offset += length;
			cout << "left.C " << (char *)c << endl;*/
			break;
		}
		//int offset2 = 0;
		//void* c1 = malloc(100);
		//int length1 = 0;
		switch(op){
		case MAX:
			memcpy((char *)data + 4 + length,vhashBlock[countForHashBlock].max,4);

			countForHashBlock++;
			return 0;
		case MIN:
			memcpy((char *)data + 4 + length,vhashBlock[countForHashBlock].min,4);
			//cout << "min: " << *(float *)vhashBlock[countForHashBlock].min << endl;
/*
			length1 = *(int *)((char *)data + offset2);

			offset2 += sizeof(int);
			cout << "left.C.length " << length1 << endl;

			memset(c1, 0, 100);
			memcpy(c1, (char *)(char *)data + offset2, length1);
			*((char *)c1 + length1) = 0;
			offset2 += length1;
			cout << "left.C " << (char *)c1 << endl;*/

			countForHashBlock++;
			return 0;
		case SUM:
			memcpy((char *)data + 4 + length,vhashBlock[countForHashBlock].sum,4);
			countForHashBlock++;
			return 0;
		case AVG:
			memcpy((char *)data + 4 + length,vhashBlock[countForHashBlock].avg,4);
			countForHashBlock++;
			return 0;
		case COUNT:
			*(float *)((char *)data + 4 + length) = vhashBlock[countForHashBlock].count;
			countForHashBlock++;
			return 0;
		}
	}

	return QE_EOF;
}

void Aggregate::getAttributes(vector<Attribute> &attrs) const
{
	string aggregateOp;
	switch(op){
	case MIN:
		aggregateOp = "MIN";
		break;
	case MAX:
		aggregateOp = "MAX";
		break;
	case SUM:
		aggregateOp = "SUM";
		break;
	case AVG:
		aggregateOp = "AVG";
		break;
	case COUNT:
		aggregateOp = "COUNT";
		break;
	}
	Attribute attr;
	string name = aggregateOp + "(" + aggAttr.name + ")";
	attr.name = name;
	attr.type = aggAttr.type;
	attr.length = aggAttr.length;

	attrs.push_back(attr);

	return;
}

void Aggregate::projectSpecificAttrs(void* data)
{
	vector<Attribute> attrs;
	input->getAttributes(attrs);
	void* pData = data;
	void* temp = malloc(200);
	void* pTemp = temp;
	int size = 0;
	for(unsigned int i = 0; i < attrs.size(); i++){
		if(strcmp(attrs[i].name.c_str(), aggAttr.name.c_str()) == 0){
			switch(attrs[i].type){
			case TypeInt:{
				*(int *)pTemp = *(int *)pData;
				pTemp = (char *)pTemp + sizeof(int);
				pData = (char *)pData + sizeof(int);
				size += sizeof(int);
				break;
			}
			case TypeReal:{
				*(float *)pTemp = *(float *)pData;
				pTemp = (char *)pTemp + sizeof(float);
				pData = (char *)pData + sizeof(float);
				size += sizeof(float);
				break;
			}

			}
			break;
		}
		else{
			switch(attrs[i].type){
			case TypeInt:
				pData = (char *)pData + sizeof(int);
				break;
			case TypeReal:
				pData = (char *)pData + sizeof(float);
				break;
			}
		}
	}

	memcpy(data,temp,size);
	//free(temp);
	//free(pData);
	//free(pTemp);
}

void Aggregate::projectSpecificAttrsForGroupAttr(void* data)
{
	vector<Attribute> attrs;
	input->getAttributes(attrs);
	void* pData = data;
	void* temp = malloc(200);
	void* pTemp = temp;
	int size = 0;
	for(unsigned int i = 0; i < attrs.size(); i++){
		if(strcmp(attrs[i].name.c_str(), gAttr.name.c_str()) == 0){
			switch(attrs[i].type){
			case TypeInt:{
				*(int *)pTemp = *(int *)pData;
				pTemp = (char *)pTemp + sizeof(int);
				pData = (char *)pData + sizeof(int);
				size += sizeof(int);
				break;
			}
			case TypeReal:{
				*(float *)pTemp = *(float *)pData;
				pTemp = (char *)pTemp + sizeof(float);
				pData = (char *)pData + sizeof(float);
				size += sizeof(float);
				break;
			}
			case TypeVarChar:{
				int len = *(int*)pData;
				pData = (char *)pData + sizeof(int);
				size += sizeof(int);
				*(int *)pTemp = len;
				memcpy((char *)pTemp + 4, (char*)pData, len);
				memset((char *)pTemp + 4 + len,'\0',1);
				pData = (char *)pData + len;
				size += len;
				break;
			}

			}
			break;
		}
		else{
			switch(attrs[i].type){
			case TypeInt:
				pData = (char *)pData + sizeof(int);
				break;
			case TypeReal:
				pData = (char *)pData + sizeof(float);
				break;
			case TypeVarChar:
				int len = *(int *)pData;
				pData = (char *)pData + sizeof(int);
				pData = (char *)pData + len;
				break;
			}
		}
	}

	memcpy(data,temp,size);
	//free(temp);
	//free(pData);
	//free(pTemp);
}

RC Aggregate::getMax(void *max){

	void* data = malloc(200);
	input->getNextTuple(data);
	projectSpecificAttrs(data);

	switch(aggAttr.type){
	case TypeInt:
		*(float *)max = (float)(*(int *)data);
		break;
	case TypeReal:
		*(float *)max = *(float *)data;
		break;
	}

	while(input->getNextTuple(data) != QE_EOF){
		projectSpecificAttrs(data);

		//check if max
		switch(aggAttr.type){
		case TypeInt:
			if(*(float *)max < (float)(*(int *)data))
				*(float *)max = (float)(*(int *)data);
			break;
		case TypeReal:
			if(*(float *)max < *(float *)data)
				*(float *)max = *(float *)data;
			break;
		}
	}

	flag = 1;
	return 0;
}

RC Aggregate::getMin(void *min){

	void* data = malloc(200);
	input->getNextTuple(data);
	projectSpecificAttrs(data);

	switch(aggAttr.type){
	case TypeInt:
		*(float *)min = (float)(*(int *)data);
		break;
	case TypeReal:
		*(float *)min = *(float *)data;
		break;
	}

	while(input->getNextTuple(data) != QE_EOF){
		projectSpecificAttrs(data);

		//check if min
		switch(aggAttr.type){
		case TypeInt:
			if(*(float *)min > (float)(*(int *)data))
				*(float *)min = (float)(*(int *)data);
			break;
		case TypeReal:
			if(*(float *)min > *(float *)data)
				*(float *)min = *(float *)data;
			break;
		}
	}

	flag = 1;
	return 0;
}

RC Aggregate::getSum(void *sum){

	void* data = malloc(200);
	input->getNextTuple(data);
	projectSpecificAttrs(data);

	switch(aggAttr.type){
	case TypeInt:
		*(float *)sum = (float)(*(int *)data);
		break;
	case TypeReal:
		*(float *)sum = *(float *)data;
		break;
	}

	while(input->getNextTuple(data) != QE_EOF){
		projectSpecificAttrs(data);

		//sum
		switch(aggAttr.type){
		case TypeInt:
			*(float *)sum += (float)(*(int *)data);
			break;
		case TypeReal:
			*(float *)sum += *(float *)data;
			break;
		}
	}

	flag = 1;
	return 0;
}

RC Aggregate::getAvg(void *avg){

	void* data = malloc(200);
	input->getNextTuple(data);
	projectSpecificAttrs(data);
	float count = 1;

	switch(aggAttr.type){
	case TypeInt:
		*(float *)avg = (float)(*(int *)data);
		break;
	case TypeReal:
		*(float *)avg = *(float *)data;
		break;
	}

	while(input->getNextTuple(data) != QE_EOF){
		projectSpecificAttrs(data);
		count++;
		//average
		switch(aggAttr.type){
		case TypeInt:
			*(float *)avg += (float)(*(int *)data);
			break;
		case TypeReal:
			*(float *)avg += *(float *)data;
			break;
		}
	}

	switch(aggAttr.type){
	case TypeInt:
		*(float *)avg = *(float *)avg/count;
		break;
	case TypeReal:
		*(float *)avg = *(float *)avg/count;
		break;
	}

	flag = 1;
	return 0;
}

RC Aggregate::getCount(void *count){

	void* data = malloc(200);
	input->getNextTuple(data);
	*(float *)count = 1;

	while(input->getNextTuple(data) != QE_EOF){
		(*(float *)count)++;
	}

	flag = 1;
	return 0;
}

int Aggregate::getNumOfUniqueKey(Attribute gAttr){
	void* data = malloc(200);
	int mark = 0;
	int count = 0;
	vector<int> uniqueKey;
	vector<float> uniqueKey_float;
	switch(gAttr.type){
	case TypeInt:
		input->getNextTuple(data);
		projectSpecificAttrsForGroupAttr(data);
		uniqueKey.push_back(*(int *)data);
		while(input->getNextTuple(data)!=0){
			projectSpecificAttrsForGroupAttr(data);
			for(unsigned int i = 0; i < uniqueKey.size();i++){
				if(*(int *)data == uniqueKey[i]){
					mark = 1;
					break;
				}
			}
			if(mark == 1)
				break;
			count++;
		}
		free(data);
		return count;
	case TypeReal:
		input->getNextTuple(data);
		projectSpecificAttrsForGroupAttr(data);
		uniqueKey_float.push_back(*(float *)data);
		while(input->getNextTuple(data)!=0){
			projectSpecificAttrsForGroupAttr(data);
			for(unsigned int i = 0; i < uniqueKey_float.size();i++){
				if(*(float *)data == uniqueKey_float[i]){
					mark = 1;
					break;
				}
			}
			if(mark == 1)
				break;
			count++;
		}
		free(data);
		return count;
	}
}





