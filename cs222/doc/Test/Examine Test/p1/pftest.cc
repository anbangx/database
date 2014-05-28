#include <iostream>
#include <string>
#include <cassert>
#include <sys/stat.h>
#include <stdlib.h> 
#include <string.h>
#include <stdexcept>
#include <stdio.h> 

#include "pf.h"

using namespace std;

const int success = 0;


// Check if a file exists
bool FileExists(string fileName)
{
    struct stat stFileInfo;

    if(stat(fileName.c_str(), &stFileInfo) == 0) return true;
    else return false;
}


int PFTest_1(PF_Manager *pf)
{
    // Functions Tested:
    // 1. CreateFile
    cout << "****In PF Test Case 1****" << endl;

    RC rc;
    string fileName = "test";

    // Create a file named "test"
    rc = pf->CreateFile(fileName.c_str());
    assert(rc == success);

    if(FileExists(fileName.c_str()))
    {
        cout << "File " << fileName << " has been created." << endl << endl;
        return 0;
    }
    else
    {
        cout << "Failed to create file!" << endl;
        return -1;
    }

    // Create "test" again, should fail
    rc = pf->CreateFile(fileName.c_str());
    assert(rc != success);

    return 0;
}


int PFTest_2(PF_Manager *pf)
{
    // Functions Tested:
    // 1. DestroyFile
    cout << "****In PF Test Case 2****" << endl;

    RC rc;
    string fileName = "test";

    rc = pf->DestroyFile(fileName.c_str());
    assert(rc == success);

    if(!FileExists(fileName.c_str()))
    {
        cout << "File " << fileName << " has been destroyed." << endl << endl;
        return 0;
    }
    else
    {
        cout << "Failed to destroy file!" << endl;
        return -1;
    }
}


int PFTest_3(PF_Manager *pf)
{
    // Functions Tested:
    // 1. CreateFile
    // 2. OpenFile
    // 3. GetNumberOfPages
    // 4. CloseFile
    cout << "****In PF Test Case 3****" << endl;

    RC rc;
    string fileName = "test_1";

    // Create a file named "test_1"
    rc = pf->CreateFile(fileName.c_str());
    assert(rc == success);

    if(FileExists(fileName.c_str()))
    {
        cout << "File " << fileName << " has been created." << endl;
    }
    else
    {
        cout << "Failed to create file!" << endl;
    }

    // Open the file "test_1"
    PF_FileHandle fileHandle;
    rc = pf->OpenFile(fileName.c_str(), fileHandle);
    assert(rc == success);

    // Get the number of pages in the test file
    unsigned count = fileHandle.GetNumberOfPages();
    assert(count == (unsigned)0);

    // Close the file "test_1"
    rc = pf->CloseFile(fileHandle);
    assert(rc == success);

    cout << "Test Case 3 Passed!" << endl << endl;

    return 0;
}



int PFTest_4(PF_Manager *pf)
{
    // Functions Tested:
    // 1. OpenFile
    // 2. AppendPage
    // 3. GetNumberOfPages
    // 3. CloseFile
    cout << "****In PF Test Case 4****" << endl;

    RC rc;
    string fileName = "test_1";

    // Open the file "test_1"
    PF_FileHandle fileHandle;
    rc = pf->OpenFile(fileName.c_str(), fileHandle);
    assert(rc == success);

    // Append the first page
    void *data = malloc(PF_PAGE_SIZE);
    for(unsigned i = 0; i < PF_PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 94 + 32;
    }
    rc = fileHandle.AppendPage(data);
    assert(rc == success);
   
    // Get the number of pages
    unsigned count = fileHandle.GetNumberOfPages();
    assert(count == (unsigned)1);

    // Close the file "test_1"
    rc = pf->CloseFile(fileHandle);
    assert(rc == success);

    free(data);

    cout << "Test Case 4 Passed!" << endl << endl;

    return 0;
}


int PFTest_5(PF_Manager *pf)
{
    // Functions Tested:
    // 1. OpenFile
    // 2. ReadPage
    // 3. CloseFile
    cout << "****In PF Test Case 5****" << endl;

    RC rc;
    string fileName = "test_1";

    // Open the file "test_1"
    PF_FileHandle fileHandle;
    rc = pf->OpenFile(fileName.c_str(), fileHandle);
    assert(rc == success);

    // Read the first page
    void *buffer = malloc(PF_PAGE_SIZE);
    rc = fileHandle.ReadPage(0, buffer);
    assert(rc == success);
  
    // Check the integrity of the page    
    void *data = malloc(PF_PAGE_SIZE);
    for(unsigned i = 0; i < PF_PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 94 + 32;
    }
    rc = memcmp(data, buffer, PF_PAGE_SIZE);
    assert(rc == success);
 
    // Close the file "test_1"
    rc = pf->CloseFile(fileHandle);
    assert(rc == success);

    free(data);
    free(buffer);

    cout << "Test Case 5 Passed!" << endl << endl;

    return 0;
}


int PFTest_6(PF_Manager *pf)
{
    // Functions Tested:
    // 1. OpenFile
    // 2. WritePage
    // 3. ReadPage
    // 3. CloseFile
    cout << "****In PF Test Case 6****" << endl;

    RC rc;
    string fileName = "test_1";

    // Open the file "test_1"
    PF_FileHandle fileHandle;
    rc = pf->OpenFile(fileName.c_str(), fileHandle);
    assert(rc == success);

    // Update the first page
    void *data = malloc(PF_PAGE_SIZE);
    for(unsigned i = 0; i < PF_PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 10 + 32;
    }
    rc = fileHandle.WritePage(0, data);
    assert(rc == success);

    // Read the page
    void *buffer = malloc(PF_PAGE_SIZE);
    rc = fileHandle.ReadPage(0, buffer);
    assert(rc == success);

    // Check the integrity
    rc = memcmp(data, buffer, PF_PAGE_SIZE);
    assert(rc == success);
 
    // Close the file "test_1"
    rc = pf->CloseFile(fileHandle);
    assert(rc == success);

    free(data);
    free(buffer);

    // DestroyFile
    rc = pf->DestroyFile(fileName.c_str());
    assert(rc == success);
    
    if(!FileExists(fileName.c_str()))
    {
        cout << "File " << fileName << " has been destroyed." << endl;
        cout << "Test Case 6 Passed!" << endl << endl;
        return 0;
    }
    else
    {
        cout << "Failed to destroy file!" << endl;
        return -1;
    }
}


int PFTest_7(PF_Manager *pf)
{
    // Functions Tested:
    // 1. CreateFile
    // 2. OpenFile
    // 3. AppendPage
    // 4. GetNumberOfPages
    // 5. ReadPage
    // 6. WritePage
    // 7. CloseFile
    // 8. DestroyFile
    cout << "****In PF Test Case 7****" << endl;

    RC rc;
    string fileName = "test_2";

    // Create the file named "test_2"
    rc = pf->CreateFile(fileName.c_str());
    assert(rc == success);

    if(FileExists(fileName.c_str()))
    {
        cout << "File " << fileName << " has been created." << endl;
    }
    else
    {
        cout << "Failed to create file!" << endl;
    }

    // Open the file "test_2"
    PF_FileHandle fileHandle;
    rc = pf->OpenFile(fileName.c_str(), fileHandle);
    assert(rc == success);

    // Append 50 pages
    void *data = malloc(PF_PAGE_SIZE);
    for(unsigned j = 0; j < 50; j++)
    {
        for(unsigned i = 0; i < PF_PAGE_SIZE; i++)
        {
            *((char *)data+i) = i % (j+1) + 32;
        }
        rc = fileHandle.AppendPage(data);
        assert(rc == success);
    }
    cout << "50 Pages have been successfully appended!" << endl;
   
    // Get the number of pages
    unsigned count = fileHandle.GetNumberOfPages();
    assert(count == (unsigned)50);

    // Read the 25th page and check integrity
    void *buffer = malloc(PF_PAGE_SIZE);
    rc = fileHandle.ReadPage(24, buffer);
    assert(rc == success);

    for(unsigned i = 0; i < PF_PAGE_SIZE; i++)
    {
        *((char *)data + i) = i % 25 + 32;
    }
    rc = memcmp(buffer, data, PF_PAGE_SIZE);
    assert(rc == success);
    cout << "The data in 25th page is correct!" << endl;

    // Update the 25th page
    for(unsigned i = 0; i < PF_PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 60 + 32;
    }
    rc = fileHandle.WritePage(24, data);
    assert(rc == success);

    // Read the 25th page and check integrity
    rc = fileHandle.ReadPage(24, buffer);
    assert(rc == success);
    
    rc = memcmp(buffer, data, PF_PAGE_SIZE);
    assert(rc == success);

    // Close the file "test_2"
    rc = pf->CloseFile(fileHandle);
    assert(rc == success);

    // DestroyFile
    rc = pf->DestroyFile(fileName.c_str());
    assert(rc == success);

    free(data);
    free(buffer);

    if(!FileExists(fileName.c_str()))
    {
        cout << "File " << fileName << " has been destroyed." << endl;
        cout << "Test Case 7 Passed!" << endl << endl;
        return 0;
    }
    else
    {
        cout << "Failed to destroy file!" << endl;
        return -1;
    }
}


int main()
{
    PF_Manager *pf = PF_Manager::Instance();
    remove("test");
    remove("test_1");
    remove("test_2");
    
    PFTest_1(pf);
    PFTest_2(pf); 
    PFTest_3(pf);
    PFTest_4(pf);
    PFTest_5(pf); 
    PFTest_6(pf);
    PFTest_7(pf);
    
    return 0;
}
