#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <fstream>
#include <iostream>
#include <cassert>

#include "rm.h"

using namespace std;

const int success = 0;

RM *rm = RM::Instance();


// Function to prepare the data in the correct form to be inserted/read/updated.
void prepareTuple(const int name_length, const string name, const int age, const float height, const int salary, void *buffer, int *tuple_size)
{
    int offset = 0;
 
    memcpy((char *)buffer + offset, &(name_length), sizeof(int));
    offset += sizeof(int);    
    memcpy((char *)buffer + offset, name.c_str(), name_length);
    offset += name_length;
    
    memcpy((char *)buffer + offset, &age, sizeof(int));
    offset += sizeof(int);
    
    memcpy((char *)buffer + offset, &height, sizeof(float));
    offset += sizeof(float);
    
    memcpy((char *)buffer + offset, &salary, sizeof(int));
    offset += sizeof(int);
    
    *tuple_size = offset;
}


// Function to get the data in the correct form to be inserted/read after adding
// the attribute ssn
void prepareTupleAfterAdd(const int name_length, const string name, const int age, const float height, const int salary, const int ssn, void *buffer, int *tuple_size)
{
    int offset=0;
    
    memcpy((char*)buffer + offset, &(name_length), sizeof(int));
    offset += sizeof(int);    
    memcpy((char*)buffer + offset, name.c_str(), name_length);
    offset += name_length;
    
    memcpy((char*)buffer + offset, &age, sizeof(int));
    offset += sizeof(int);
        
    memcpy((char*)buffer + offset, &height, sizeof(float));
    offset += sizeof(float);
        
    memcpy((char*)buffer + offset, &salary, sizeof(int));
    offset += sizeof(int);
    
    memcpy((char*)buffer + offset, &ssn, sizeof(int));
    offset += sizeof(int);

    *tuple_size = offset;
}


// Function to parse the data in buffer
void printTuple(const void *buffer, const int tuple_size)
{
    int offset = 0;
    cout << "****Printing Buffer: Start****" << endl;
   
    int name_length = 0;     
    memcpy(&name_length, (char *)buffer+offset, sizeof(int));
    offset += sizeof(int);
    cout << "name_length: " << name_length << endl;
   
    char *name = (char *)malloc(100);
    memcpy(name, (char *)buffer+offset, name_length);
    name[name_length] = '\0';
    offset += name_length;
    cout << "name: " << name << endl;
    
    int age = 0; 
    memcpy(&age, (char *)buffer+offset, sizeof(int));
    offset += sizeof(int);
    cout << "age: " << age << endl;
   
    float height = 0.0; 
    memcpy(&height, (char *)buffer+offset, sizeof(float));
    offset += sizeof(int);
    cout << "height: " << height << endl;
       
    int salary = 0; 
    memcpy(&salary, (char *)buffer+offset, sizeof(int));
    offset += sizeof(int);
    cout << "salary: " << salary << endl;

    cout << "****Printing Buffer: End****" << endl << endl;    
}


void printTupleAfterDrop( const void *buffer, const int tuple_size)
{
    int offset = 0;
    cout << "****Printing Buffer: Start****" << endl;
   
    int name_length = 0;     
    memcpy(&name_length, (char *)buffer+offset, sizeof(int));
    offset += sizeof(int);
    cout << "name_length: " << name_length << endl;
   
    char *name = (char *)malloc(100);
    memcpy(name, (char *)buffer+offset, name_length);
    name[name_length] = '\0';
    offset += name_length;
    cout << "name: " << name << endl;
    
    int age = 0; 
    memcpy(&age, (char *)buffer+offset, sizeof(int));
    offset += sizeof(int);
    cout << "age: " << age << endl;
   
    float height = 0.0; 
    memcpy(&height, (char *)buffer+offset, sizeof(float));
    offset += sizeof(float);
    cout << "height: " << height << endl;
       
    cout << "****Printing Buffer: End****" << endl << endl;    
}   


void printTupleAfterAdd(const void *buffer, const int tuple_size)
{
    int offset = 0;
    cout << "****Printing Buffer: Start****" << endl;
   
    int name_length = 0;     
    memcpy(&name_length, (char *)buffer+offset, sizeof(int));
    offset += sizeof(int);
    cout << "name_length: " << name_length << endl;
   
    char *name = (char *)malloc(100);
    memcpy(name, (char *)buffer+offset, name_length);
    name[name_length] = '\0';
    offset += name_length;
    cout << "name: " << name << endl;
    
    int age = 0; 
    memcpy(&age, (char *)buffer+offset, sizeof(int));
    offset += sizeof(int);
    cout << "age: " << age << endl;
   
    float height = 0; 
    memcpy(&height, (char *)buffer+offset, sizeof(float));
    offset += sizeof(float);
    cout << "height: " << height << endl;
    
    int ssn = 0;   
    memcpy(&ssn, (char *)buffer+offset, sizeof(int));
    offset += sizeof(int);
    cout << "SSN: " << ssn << endl;

    cout << "****Printing Buffer: End****" << endl << endl;    
}   


// Create an employee table
void createTable(const string tablename)
{
    cout << "****Create Table " << tablename << " ****" << endl;
    
    // 1. Create Table ** -- made separate now.
    vector<Attribute> attrs;

    Attribute attr;
    attr.name = "EmpName";
    attr.type = TypeVarChar;
    attr.length = (AttrLength)30;
    attrs.push_back(attr);

    attr.name = "Age";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    attrs.push_back(attr);

    attr.name = "Height";
    attr.type = TypeReal;
    attr.length = (AttrLength)4;
    attrs.push_back(attr);

    attr.name = "Salary";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    attrs.push_back(attr);

    int rc = rm->createTable(tablename, attrs);
    assert(rc == success);
    cout << "****Table Created: " << tablename << " ****" << endl << endl;
}


// Advanced Features:
void secB_1(const string tablename, const int name_length, const string name, const int age, const int height, const int salary)
{
    // Functions Tested
    // 1. Insert tuple
    // 2. Read Attributes
    // 3. Drop Attributes **
    cout << "****In Extra Credit Test Case 1****" << endl;

    RID rid;    
    int tuple_size = 0;
    void *tuple = malloc(100);
    void *data_returned = malloc(100);
   
    // Insert Tuple 
    prepareTuple(name_length, name, age, height, salary, tuple, &tuple_size);
    int rc = rm->insertTuple(tablename, tuple, rid);
    assert(rc == success);

    // Read Attribute
    rc = rm->readAttribute(tablename, rid, "Salary", data_returned);
    assert(rc == success);
    cout << "Salary: " << *(int *)data_returned << endl;
 
    if(memcmp((char *)data_returned, (char *)tuple+18, 4) != 0)
    {
        cout << "Read attribute failed!" << endl; 
    }
    else
    {
        cout << "Read attribute passed!" << endl; 

        // Drop the attribute
        rc = rm->dropAttribute(tablename, "Salary");
        assert(rc == success);

        // Read Tuple and print the tuple
        rc = rm->readTuple(tablename, rid, data_returned);
        assert(rc == success);
        printTupleAfterDrop(data_returned, tuple_size);
    }
    
    free(tuple);
    free(data_returned);
    return;
}


void secB_2(const string tablename, const int name_length, const string name, const int age, const int height, const int salary, const int ssn)
{
    // Functions Tested
    // 1. Add Attribute **
    // 2. Insert Tuple
    cout << "****In Extra Credit Test Case 2****" << endl;
   
    RID rid; 
    int tuple_size=0;
    void *tuple = malloc(100);
    void *data_returned = malloc(100);
   
    // Test Add Attribute
    Attribute attr;
    attr.name = "SSN";
    attr.type = TypeInt;
    attr.length = 4;
    int rc = rm->addAttribute(tablename, attr);
    assert(rc == success);

    // Test Insert Tuple
    prepareTupleAfterAdd(name_length, name, age, height, salary, ssn, tuple, &tuple_size);
    rc = rm->insertTuple(tablename, tuple, rid);
    assert(rc == success);

    // Test Read Tuple
    rc = rm->readTuple(tablename, rid, data_returned);
    assert(rc == success);

    cout << "Insert Data:" << endl;
    printTupleAfterAdd(tuple, tuple_size);

    cout << "Returned Data:" << endl;
    printTupleAfterAdd(data_returned, tuple_size);

    if (memcmp(data_returned, tuple, tuple_size) != 0)
    {
        cout << "****Extra Credit Test Case 2 failed****" << endl << endl; 
    }
    else
    {
        cout << "****Extra Credit Test Case 2 passed****" << endl << endl; 
    }

    free(tuple);
    free(data_returned);
    return;   
}


void secB_3(const string tablename)
{
    // Functions Tested
    // 1. Reorganize Table **
    cout << "****In Extra Credit Test Case 3****" << endl;
    
    int tuple_size = 0;
    void *tuple = malloc(100);
    void *data_returned = malloc(100);
   
    RID rid; 
    int num_records = 200;
    RID rids[num_records];
   
    int rc = 0; 
    for(int i=0; i < num_records; i++)
    {
        // Insert Tuple
        prepareTuple(6, "Tester", 100+i, i, 123, tuple, &tuple_size);
        rc = rm->insertTuple(tablename, tuple, rid);
        assert(rc == success);

        rids[i] = rid;
    }
	
	// Delete the first 100 records
    for(int i = 0; i < 100; i++)
    {
        rc = rm->deleteTuple(tablename, rids[i]);
        assert(rc == success);

        rc = rm->readTuple(tablename, rids[i], data_returned);
        assert(rc != success);
    }
    cout << "After deletion!" << endl;

    // Reorganize Table
    rc = rm->reorganizeTable(tablename);
    assert(rc == success);

    for(int i = 0; i < 100; i++)
    {
        // Read Tuple
        rc = rm->readTuple(tablename, rids[i], data_returned);
        assert(rc == success);

        // Print the tuple
        printTuple(data_returned, tuple_size);
    }

    // Delete the table
    rc = rm->deleteTable(tablename);
    assert(rc == success);

    free(tuple);
    free(data_returned);
    return;
}    
    
int main()
{
    string name1 = "Peters";
    string name2 = "Victors";
    
    // Extra Credits
    cout << "Test Extra Credits...." << endl;

    // Drop Attribute
    createTable("tbl_employee");
    secB_1("tbl_employee", 6, name1, 24, 170, 5000);

    // Add Attributes
    createTable("tbl_employee2");
    secB_2("tbl_employee2", 6, name2, 22, 180, 6000, 999);

    // Reorganize Table
    createTable("tbl_employee3");  
    secB_3("tbl_employee3");

    return 0;
}

