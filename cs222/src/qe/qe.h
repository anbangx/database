#ifndef _qe_h_
#define _qe_h_

#include <vector>
#include <sstream>

#include "../pf/pf.h"
#include "../rm/rm.h"
#include "../ix/ix.h"

# define QE_EOF (-1)  // end of the index scan

using namespace std;

typedef enum{ MIN = 0, MAX, SUM, AVG, COUNT } AggregateOp;


// The following functions use  the following 
// format for the passed data.
//    For int and real: use 4 bytes
//    For varchar: use 4 bytes for the length followed by
//                          the characters

struct Value {
    AttrType type;          // type of value               
    void     *data;         // value                       
};


struct Condition {
    string lhsAttr;         // left-hand side attribute                     
    CompOp  op;             // comparison operator                          
    bool    bRhsIsAttr;     // TRUE if right-hand side is an attribute and not a value; FALSE, otherwise.
    string rhsAttr;         // right-hand side attribute if bRhsIsAttr = TRUE
    Value   rhsValue;       // right-hand side value if bRhsIsAttr = FALSE
};


class Iterator {
    // All the relational operators and access methods are iterators.
    public:
        virtual RC getNextTuple(void *data) = 0;
        virtual void getAttributes(vector<Attribute> &attrs) const = 0;
        virtual ~Iterator() {};
};


class TableScan : public Iterator
{
    // A wrapper inheriting Iterator over RM_ScanIterator
    public:
        RM &rm;
        RM_ScanIterator *iter;
        string tablename;
        vector<Attribute> attrs;
        vector<string> attrNames;
        
        TableScan(RM &rm, const string tablename, const char *alias = NULL):rm(rm)
        {
            // Get Attributes from RM
            rm.getAttributes(tablename, attrs);

            // Get Attribute Names from RM
            unsigned i;
            for(i = 0; i < attrs.size(); ++i)
            {
                // convert to char *
                attrNames.push_back(attrs[i].name);
            }

            // Call rm scan to get iterator
            iter = new RM_ScanIterator();
            rm.scan(tablename, "", NO_OP, NULL, attrNames, *iter);

            // Store tablename
            this->tablename = tablename;
            if(alias) this->tablename = alias;
        };
       
        // Start a new iterator given the new compOp and value
        void setIterator()
        {
            iter->close();
            delete iter;
            iter = new RM_ScanIterator();
            rm.scan(tablename, "", NO_OP, NULL, attrNames, *iter);
        };
       
        RC getNextTuple(void *data)
        {
            RID rid;
            return iter->getNextTuple(rid, data);
        };
        
        void getAttributes(vector<Attribute> &attrs) const
        {
            attrs.clear();
            attrs = this->attrs;
            unsigned i;
            
            // For attribute in vector<Attribute>, name it as rel.attr
            for(i = 0; i < attrs.size(); ++i)
            {
                string tmp = tablename;
                tmp += ".";
                tmp += attrs[i].name;
                attrs[i].name = tmp;
            }
        };
        
        ~TableScan() 
        {
            iter->close();
        };
};


class IndexScan : public Iterator
{
    // A wrapper inheriting Iterator over IX_IndexScan
    public:
        RM &rm;
        IX_IndexScan *iter;
        IX_IndexHandle handle;
        string tablename;
        vector<Attribute> attrs;
        
        IndexScan(RM &rm, const IX_IndexHandle &indexHandle, const string tablename, const char *alias = NULL):rm(rm)
        {
            // Get Attributes from RM
            rm.getAttributes(tablename, attrs);
                     
            // Store tablename
            this->tablename = tablename;
            if(alias) this->tablename = string(alias);
            
            // Store Index Handle
            iter = NULL;
            this->handle = indexHandle;
        };
       
        // Start a new iterator given the new compOp and value
        void setIterator(CompOp compOp, void *value)
        {
            if(iter != NULL)
            {
                iter->CloseScan();
                delete iter;
            }
            iter = new IX_IndexScan();
            iter->OpenScan(handle, compOp, value);
        };
       
        RC getNextTuple(void *data)
        {
            RID rid;
            int rc = iter->GetNextEntry(rid);
            if(rc == 0)
            {
                rc = rm.readTuple(tablename.c_str(), rid, data);
            }
            return rc;
        };
        
        void getAttributes(vector<Attribute> &attrs) const
        {
            attrs.clear();
            attrs = this->attrs;
            unsigned i;

            // For attribute in vector<Attribute>, name it as rel.attr
            for(i = 0; i < attrs.size(); ++i)
            {
                string tmp = tablename;
                tmp += ".";
                tmp += attrs[i].name;
                attrs[i].name = tmp;
            }
        };
        
        ~IndexScan() 
        {
            iter->CloseScan();
        };
};


class Filter : public Iterator {
    // Filter operator
    public:
	Iterator *iter;
	Condition cond;

        Filter(Iterator *input,const Condition &condition); // Iterator of input R // Selection condition

        ~Filter();
        
        RC getNextTuple(void *data);
        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;

        RC compareAttriVal(void *data);
};


class Project : public Iterator {
    // Projection operator
    public:
	Iterator *iter;
	vector<string> attrNames;

	    // Iterator of input R // vector containing attribute names
        Project(Iterator *input, const vector<string> &attrNames);

        ~Project();
        
        RC getNextTuple(void *data);

        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;

        void projectSpecificAttrs(void* data);
};


class NLJoin : public Iterator {
    // Nested-Loop join operator
    public:
	Iterator *lIter;
	TableScan *rIter;
	Condition cond;
	int flag;
	void* lData;

	    // Iterator of input R // TableScan Iterator of input S // Join condition // Number of pages can be used to do join (decided by the optimizer)
        NLJoin(Iterator *leftIn, TableScan *rightIn, const Condition &condition, const unsigned numPages);

        ~NLJoin();
        
        RC getNextTuple(void *data);

        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;

        RC compareAttrFromLeftAndRight(void* lData, void* rData, int* lSize, int* rSize);
};


class INLJoin : public Iterator {
    // Index Nested-Loop join operator
    public:
	Iterator *lIter;
	IndexScan *rIter;
	Condition cond;
	int flag;
	void* lData;

	    // Iterator of input R // IndexScan Iterator of input S // Join condition // Number of pages can be used to do join (decided by the optimizer)
        INLJoin(Iterator *leftIn, IndexScan *rightIn, const Condition &condition, const unsigned numPages);

        
        ~INLJoin();

        RC getNextTuple(void *data);

        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;

        RC compareAttrFromLeftAndRight(void* lData, void* rData, int* lSize, int* rSize);
};

class ScanS;
class joinPage;
class HashJoin : public Iterator {
    // Hash join operator
    public:
	Iterator *lIter;
	Iterator *rIter;
	Condition cond;
	unsigned numPages;
	unsigned int I;
	ScanS *scanS;
	vector<joinPage> vJoinPage;
	int k;
	void *rData;
	int indexProbe;

        HashJoin(Iterator *leftIn,                                // Iterator of input R
                 Iterator *rightIn,                               // Iterator of input S
                 const Condition &condition,                      // Join condition
                 const unsigned numPages                          // Number of pages can be used to do join (decided by the optimizer)
        );
        
        ~HashJoin();

        RC getNextTuple(void *data);
        // For attribute in vector<Attribute>, name it as rel.attr
        void getAttributes(vector<Attribute> &attrs) const;

        int getLengthOfData(void* data, vector<Attribute> &attrs);

        Value projectSpecificAttrs(void* data, int leftOrRight);

        int hashFunction(Value value, int numOfPages);

        int hashFunctionForVarChar(Value value, int numOfPages);

        void partitioningPhase();

        void inMemoryHashTableForR();

    private:
     static HashJoin *_hashjoin;
};

class hashBlock;
class Aggregate : public Iterator {
    // Aggregation operator
    public:
	Iterator *input;
	Attribute aggAttr;
	Attribute gAttr;
	AggregateOp op;
	int flag;
	vector<hashBlock> vhashBlock;
	int countForHashBlock;

        Aggregate(Iterator *input,                              // Iterator of input R
                  Attribute aggAttr,                            // The attribute over which we are computing an aggregate
                  AggregateOp op                                // Aggregate operation
        );

        // Extra Credit
        Aggregate(Iterator *input,                              // Iterator of input R
                  Attribute aggAttr,                            // The attribute over which we are computing an aggregate
                  Attribute gAttr,                              // The attribute over which we are grouping the tuples
                  AggregateOp op                                // Aggregate operation
        );     

        ~Aggregate();
        
        RC getNextTuple(void *data);
        // Please name the output attribute as aggregateOp(aggAttr)
        // E.g. Relation=rel, attribute=attr, aggregateOp=MAX
        // output attrname = "MAX(rel.attr)"
        void getAttributes(vector<Attribute> &attrs) const;

        void projectSpecificAttrs(void* data);
        void projectSpecificAttrsForGroupAttr(void* data);
        RC getMax(void *max);
        RC getMin(void *min);
        RC getSum(void *sum);
        RC getAvg(void *avg);
        RC getCount(void *count);

        int getNumOfUniqueKey(Attribute gAttr);
};


#endif
