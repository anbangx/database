#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include "pf.h"

using namespace std;


PF_Manager* PF_Manager::_pf_manager = 0;


PF_Manager* PF_Manager::Instance()
{
    if(!_pf_manager)
        _pf_manager = new PF_Manager();
    
    return _pf_manager;    
}


PF_Manager::PF_Manager()
{
}


PF_Manager::~PF_Manager()
{
}

// Check if a file exists
bool FileExist(string fileName)
{
	struct stat stFileInfo;
	if(stat(fileName.c_str(), &stFileInfo) == 0) return true;
	else return false;
}

RC PF_Manager::CreateFile(const char *fileName)
{

	if(FileExist(fileName))
	{
//		cout << "Failed to create " << fileName << " file! Because this file already exists." <<endl <<endl;
		return -1;
	}
	else
	{
		FILE * pfile = fopen(fileName,"wb");
		fclose(pfile);
//		cout << "Succeeded to create " << fileName << " file! Users can also check in directory!" <<endl <<endl;
		return 0;
	}
}


RC PF_Manager::DestroyFile(const char *fileName)
{
	if( remove( fileName ) != 0 )
	{
//		cout << "Failed to destroy " << fileName << " file! Because this file doesn't exist." <<endl <<endl;
		return -1;
	}
	else
	{
//		cout << "Succeeded to destroy " << fileName << " file! Users can also check in directory!" <<endl <<endl;
		return 0;
	}
}


RC PF_Manager::OpenFile(const char *fileName, PF_FileHandle &fileHandle)
{
	if(fileHandle.readFile()!=0){
//		cout << "Failed to open " << fileName <<" file using the same fileHandle!" <<endl <<endl;
		return -1;
	}
	FILE * pfile = fopen(fileName,"rb+");
	if ( pfile!=NULL )
	{
		fileHandle.setFile(pfile);
//		cout << "Succeeded to open " << fileName <<" file! " <<endl <<endl;
		return 0;
	}
	else
	{
//		cout << "Failed to open " << fileName <<" file! Because this file doesn't exist!" <<endl <<endl;
		return -1;
	}

}

PF_FileHandle::PF_FileHandle()
{
	file = 0;
}
 

PF_FileHandle::~PF_FileHandle()
{
}

RC PF_Manager::CloseFile(PF_FileHandle &fileHandle)
{
	if(fclose(fileHandle.readFile()) == 0){
		fileHandle.setFile(0);
//		cout << "Succeeded to close this file!" <<endl <<endl;
		return 0;
	}
	else{
//		cout << "Failed to close the file! Because no file is open!" << endl <<endl;
		return -1;
	}
}

RC PF_FileHandle::ReadPage(PageNum pageNum, void *data)
{
	if(ftell(file)==-1L){
//		cout << "Failed to read a page from the file! Because you don't open any file."<<endl <<endl;
		return -1;
	}
	else{
		unsigned pageSize = GetNumberOfPages();
		if(pageNum < pageSize){
			fseek(file,(pageNum)*PF_PAGE_SIZE,0);
			fread(data,1,PF_PAGE_SIZE,file);
			rewind(file);
//			cout << "Succeeded to read the " << pageNum << " page from the file!"<<endl<<endl ;
			//cout << "The context of reading is ";
			//printf("%s",data);  //Data Type char, int, long
			return 0;
		}
		else{
//			cout << "Failed to read this page from file! Because this pageNum is bigger than file size."<<endl <<endl;
			return -1;
		}
	}
}


RC PF_FileHandle::WritePage(PageNum pageNum, const void *data)
{
	if(ftell(file)==-1L){
//		cout << "Failed to write a page to the file! Because you don't open any file."<<endl <<endl;
		return -1;
	}
	else{
		unsigned pageSize = GetNumberOfPages();
		if(pageNum < pageSize){
			fseek(file,(pageNum)*PF_PAGE_SIZE,0);
			fwrite(data,PF_PAGE_SIZE,1,file);
			rewind(file);
			fflush(file);
//			cout << "Succeeded to write a page to the file!" <<endl<<endl;
			//cout << "The context of writting is ";
			//printf("%s",data);
			return 0;
		}
		else{
//			cout << "Failed to write this page to file! Because this pageNum is bigger than file size."<<endl <<endl;
			return -1;
		}

	}
}


RC PF_FileHandle::AppendPage(const void *data)
{
	if(ftell(file)==-1L){
//		cout << "Failed to append a page to the file! Because you don't open any file."<<endl <<endl;
		return -1;
	}

	else{
		fseek(file,GetNumberOfPages()*PF_PAGE_SIZE,0);
		fwrite(data,PF_PAGE_SIZE,1,file);
		rewind(file);
		fflush(file);
//		cout << "Succeeded to append a page to the file!"<<endl<<endl;
		//cout << "This page is ";
		//printf("%s",data);
		return 0;
	}
}

FILE * PF_FileHandle::readFile()
{
	return file;
}

void PF_FileHandle::setFile(FILE *fileName)
{
	file = fileName;
}

unsigned PF_FileHandle::GetNumberOfPages()
{
	if(ftell(file)==-1L){
//		cout << "Failed to append a page to the file! Because you don't open any file."<<endl <<endl;
		return -1;
	}

	else{
		fseek(file,0,SEEK_END);
		long int size=ftell(file);
		rewind(file);
		return size/PF_PAGE_SIZE;
	}
}


