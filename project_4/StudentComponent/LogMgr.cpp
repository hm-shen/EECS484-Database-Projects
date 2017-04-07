#include <map>
#include <vector>
#include <queue>
#include "LogMgr.h"
#include <assert.h>
#include <cstring>
#include <sstream>
#include <string>
#include <set>
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
bool LogMgr::redo(vector <LogRecord*> log)
{
    if (dirty_page_table.empty()) return true;
    map<int,int>::iterator dp_iter = dirty_page_table.begin();
    // find the smallest recLsn of any page in dirty page table
    int sm_lsn = dp_iter->second;
    for(;dp_iter!=dirty_page_table.end();++dp_iter)
    {
        if(dp_iter->second < sm_lsn)
        {
            sm_lsn = dp_iter->second;
        }
    }

    //start at recLSN
    vector<LogRecord*>::iterator log_iter = logtail.begin()
    while( (*log_iter)->getLSN() < sm_lsn){ ++log_iter; }

    for(;log_iter!=logtail.end();++log_iter)
    {
        TxType curtype = *log_iter->getType();
        // if is record is update or clr type, then judge whether need to do something
        if(curtype == UPDATE || curtype == CLR)
        {
            if(curtype == UPDATE)
            {
                UpdateLogRecord* updata_record = dynamic_cast<UpdateLogRecord *>(log_iter);
                cur_pageid = updata_record->getPageID();
                curlog_LSN = updata_record->getLSN();
                off = updata_record->getOffset();
                after_text = updata_record->getAfterImage();
            }
            if(curtype == CLR)
            {
                UpdateLogRecord* clr_record = dynamic_cast<UpdateLogRecord *>(log_iter);
                cur_pageid = clr_record->getPageID();
                curlog_LSN = clr_record->getLSN();
                off = clr_record->getOffset();
                after_text = clr_record->getAfterImage();
            }

            //is this page in the dirty table?
            map<int,int>::iterator it = dirty_page_table.find(cur_pageid);
            if( it!= dirty_page_table.end() )
            {
                // is the dirty page entry's recLSN <= current log LSN?
                if( it->second <= curlog_LSN)
                {
                    // is the LSN recorded on page is smaller than current log LSN
                    if( se->getLSN(cur_pageid)< curlog_LSN)
                    {
                        // apply the update/CLR log 
                        // set its PageLSN to the current log's LSN
                        bool w = se->pageWrite(cur_pageid, off, after_text, curlog_LSN);
                        if (w){ return false;}
                    }
                }
            }
            // end of dealing with UPDATE/CLR
        }  
    }

    //at end of the redo phase,
    //"END" the records for all transactions with status"C"
    map<int,txTableEntry>::iterator tx_iter = tx_table.begin();
    while(tx_iter!=tx_table.end())
    {
        if((tx_iter->second).status == C)
        {
            LogRecord *add = new LogRecord(se->nextLSN(),(tx_iter->second).lastLSN,tx_iter->first,END);
            logtail.push_back(add);
            tx_table.erase(tx_iter);
        }
        else{++tx_iter}
    }
    return true;
}

/*
 * If no txnum is specified, run the undo phase of ARIES.
 * If a txnum is provided, abort that transaction.
 * Hint: the logic is very similar for these two tasks!
 */
void LogMgr::undo(vector <LogRecord*> log, int txnum = NULL_TX)
{
    // identify live txs when crash
    priority_queue<int> undoList;  
    if (txnum != NULL_TX)
    {
        undoList.push(this->tx_table[txnum].lastLSN);
    }
    else
    {
        for (map<int, txTableEntry>::iterator it = tx_table.begin(); it != tx_table.end(); ++it)
        {
            undoList.push(it->second.lastLSN);
        }
    }
    
    // try to undo each log
    while (!undoList.empty())
    {
        int    curLSN = undoList.top(); //max LSN in undoList
        int    prevLSN = 0; // store prev LSN
        int    txId = 0; // store trx ID corresponding to curLSN
        TxType operType; // store current log type
        LogRecord *curLog = nullptr; // points to current log
        undoList.pop();

        // get corresponding log from logTail 
        for(vector<LogRecord*>::iterator it = log.begin();it != log.end(); ++it)
        {
            if ((*it)->getLSN() == curLSN)
            {
                operType = (*it)->getType();
                prevLSN = (*it)->getprevLSN();
                txId = (*it)->getTxID();
                curLog = *it;
            }
        }
        // undo operations
        if (operType == UPDATE)
        {
            // undo this log
            UpdateLogRecord *upLog = dynamic_cast<UpdateLogRecord*>(curLog);
            assert(upLog != nullptr);
            int pageId = upLog->getPageID();
            int offSet = upLog->getOffset();
            int newLSN = se->nextLSN();
            string befImg = upLog->getBeforeImage();
            string aftImg = upLog->getAfterImage();
            
            // write a CLR log
            this->logtail.push_back(new CompensationLogRecord(newLSN, getLastLSN(txId), txId, pageId, offSet, befImg, prevLSN));
            this->setLastLSN(txId, newLSN);

            // write to page in mem
            bool flag = se->pageWrite(pageId, offSet, befImg, newLSN);
            if (flag == false) { assert(flag != false); return; }
            if (prevLSN != NULL_LSN) { undoList.push(prevLSN); }
        }
        else if (operType == CLR)
        {
            // go to the next LSN
            CompensationLogRecord *clrLog = dynamic_cast<CompensationLogRecord*>(curLog);
            assert(clrLog != nullptr);
//            int pageId = clrLog->getPageID();
//            int offSet = clrLog->getOffset();
            int undoNextLSN = clrLog->getUndoNextLSN();
            int newLSN = se->nextLSN();
            string aftImg = clrLog->getAfterImage();
            if (undoNextLSN != NULL_LSN)
            {
                undoList.push(undoNextLSN);
            }
            else 
            {
                // current trx has been undone
                this->logtail.push_back(new LogRecord(newLSN, getLastLSN(txId), txId, END));
                this->tx_table.erase(txId);
            }
        }
        else if (operType == ABORT)
        {
            int newLSN = se->nextLSN();
            if (prevLSN != NULL_LSN) 
            { 
                undoList.push(prevLSN); 
            }
            else
            {
                this->logtail.push_back(new LogRecord(newLSN, getLastLSN(txId), txId, END));
                this->tx_table.erase(txId);
            }
        }
    }
    return;
}


/*
 * Abort the specified transaction.
 * Hint: you can use your undo function
 */
void LogMgr::abort(int txid)
{
    // write log
    int prevLSN = this->getLastLSN(txid);
    int newLSN = se->nextLSN();
    LogRecord *abortLog = new LogRecord(newLSN, prevLSN, txid, ABORT);
    this->logtail.push_back(abortLog);
    this->setLastLSN(txid, newLSN);

    // get all logs from disk and mem
    vector<LogRecord*> tempLog = this->stringToLRVector(se->getLog());
    tempLog.insert(end(tempLog), begin(this->logtail), end(this->logtail));
    this->undo( tempLog, txid );
}

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
