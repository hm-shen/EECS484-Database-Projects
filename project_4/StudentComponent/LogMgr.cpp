#include <map>
#include <vector>
#include "LogMgr.h"
#include <assert.h>

using namespace std;

/*
 * Find the LSN of the most recent log record for this TX.
 * If there is no previous log record for this TX, return 
 * the null LSN.
 */
int LogMgr::getLastLSN(int txnum) 
{ 
		map<int, txTableEntry>::iterator iter = this->tx_table.find(txnum);
		if ( iter != this->tx_table.end() ) 
		{ 
				return iter->second.lastLSN; 
		}
		else 
		{
				return NULL_LSN;
		}
}

/*
 * Update the TX table to reflect the LSN of the most recent
 * log entry for this transaction.
 */

void LogMgr::setLastLSN(int txnum, int lsn)
{
    // identify the type of the operation by inspecting logs (both on disk and mem)
    TxType operType;
    vector<LogRecord*> tempLog = this->stringToLRVector(se->getLog());
    tempLog.insert(end(tempLog), begin(this->logtail), end(this->logtail));
    for(vector<LogRecord*>::iterator it = tempLog.begin(); it != tempLog.end(); ++it)
    {
        if ((*it)->getLSN() == lsn)
        {
            operType = (*it)->getType();
        }
    }
    // find txnum
    map <int, txTableEntry> ::iterator iter = this->tx_table.find(txnum);
    if ( iter != this->tx_table.end() )
    {
        // already exists; update is needed
        iter->second.lastLSN = lsn;
        if (operType == UPDATE || operType == CLR || operType == ABORT) 
        { iter->second.status = U; return; }
        if (operType == COMMIT) { iter->second.status = C; return; }
        if (operType == END) { this->tx_table.erase(iter); return; }
    }
    else
    {
        // new entry is needed
        tx_table[txnum] = txTableEntry(lsn, U);
        return;
    }
}


/*
 * Force log records up to and including the one with the
 * maxLSN to disk. Don't forget to remove them from the
 * logtail once they're written!
 */
void LogMgr::flushLogTail(int maxLSN)
{
    // append LogTail to disk & remove them from LogTail
    while(this->logtail.size() != 0)
    {
        if (logtail.front()->getLSN() > maxLSN) { return; }
        se->updateLog(logtail.front()->toString());
        logtail.erase(logtail.begin());
    }
}


/* 
 * Run the analysis phase of ARIES.
 */
void LogMgr::analyze(vector <LogRecord*> log){}

/*
 * Run the redo phase of ARIES.
 * If the StorageEngine stops responding, return false.
 * Else when redo phase is complete, return true. 
 */
bool LogMgr::redo(vector <LogRecord*> log){}

/*
 * If no txnum is specified, run the undo phase of ARIES.
 * If a txnum is provided, abort that transaction.
 * Hint: the logic is very similar for these two tasks!
 */
void LogMgr::undo(vector <LogRecord*> log, int txnum = NULL_TX){}


/*
 * Abort the specified transaction.
 * Hint: you can use your undo function
 */
void abort(int txid){}

/*
 * Write the begin checkpoint and end checkpoint
 */
void checkpoint(){}

/*
 * Commit the specified transaction.
 */
void commit(int txid){}

/*
 * A function that StorageEngine will call when it's about to 
 * write a page to disk. 
 * Remember, you need to implement write-ahead logging
 */
void pageFlushed(int page_id){}

/*
 * Recover from a crash, given the log from the disk.
 */
void recover(string log){}

/*
 * Called by StorageEngine whenever an update is called
 * LogMgr should update tables if required and return the LSN of the action performed
 */
int LogMgr::write(int txid, int page_id, int offset, string input, string oldtext)
{
    assert(txid >= 0);
    // update logtail
    int newLSN = se->nextLSN();   
    UpdateLogRecord *newLog = new UpdateLogRecord(newLSN, this->getLastLSN(txid),
                                                  txid, page_id, offset, oldtext, input);
    logtail.push_back(newLog);

    // update DPT
		map<int, int>::iterator iter = this->dirty_page_table.find(page_id);
    if (iter == this->dirty_page_table.end()) 
    {
        // new dirty page
        this->dirty_page_table[page_id] = newLSN;
    }
    else  {assert(iter->second < newLSN);};
    
    // update TT
    this->setLastLSN(txid, newLSN);

    // ? update pageLSN / page Status to dirty? 
    return newLSN;
    
}

vector<LogRecord*> LogMgr::stringToLRVector(string logstring){}

/*
 * Sets this.se to engine. 
 */
void setStorageEngine(StorageEngine* engine){}
