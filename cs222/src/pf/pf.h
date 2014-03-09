#ifndef _pf_h_
#define _pf_h_

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>

using namespace std;

typedef int RC;
typedef unsigned PageNum;

#define PF_PAGE_SIZE 4096

class PF_FileHandle;


class PF_Manager
{
public:
    static PF_Manager* Instance();                                      // Access to the _pf_manager instance
        
    RC CreateFile    (const char *fileName);                            // Create a new file
    RC DestroyFile   (const char *fileName);                            // Destroy a file
    RC OpenFile      (const char *fileName, PF_FileHandle &fileHandle); // Open a file
    RC CloseFile     (PF_FileHandle &fileHandle);                       // Close a file 

protected:    
    PF_Manager();                                                       // Constructor
    ~PF_Manager   ();
     // Destructor

private:
    static PF_Manager *_pf_manager;
};


class PF_FileHandle
{
public:
    PF_FileHandle();                                                    // Default constructor
    ~PF_FileHandle();                                                   // Destructor

    RC ReadPage(PageNum pageNum, void *data);                           // Get a specific page
    RC WritePage(PageNum pageNum, const void *data);                    // Write a specific page
    RC AppendPage(const void *data);                                    // Append a specific page
    void setFile ( FILE * fileName );
    FILE * readFile();

    unsigned GetNumberOfPages();
    // Get the number of pages in the file
    FILE * file;
 };
 
 #endif
