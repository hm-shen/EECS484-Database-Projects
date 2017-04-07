#include <map>
#include <vector>
#include <queue>
#include "LogMgr.h"
#include <assert.h>
#include <cstring>
#include <sstream>
#include <string>
#include <set>
#include <algorithm>
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
void LogMgr::analyze(vector <LogRecord*> log)
{
    // retrieve snapshots using master record
    int masterRecord = se->get_master();
    LogRecord *_begin = nullptr;
    ChkptLogRecord *begin = nullptr;
    ChkptLogRecord *end = nullptr;

    for (vector<LogRecord*>::reverse_iterator it = log.rbegin(); it != log.rend(); ++it)
    {
        if ((*it)->getType() == BEGIN_CKPT) 
        { 
            _begin = *it;
            begin = dynamic_cast<ChkptLogRecord*>(*it);
        }
        if ((*it)->getType() == END_CKPT) { end = dynamic_cast<ChkptLogRecord *>(*it); }
        if (begin != nullptr && end != nullptr) { break; }
    }
    assert(begin->getLSN() == masterRecord);
    vector<LogRecord*>::iterator iter = find(log.begin(),log.end(),_begin);
    this->tx_table = end->getTxTable();
    this->dirty_page_table = end->getDirtyPageTable();

    // update TT and DPT from begin
    for (vector<LogRecord*>::iterator it = iter; it != log.end(); ++it)
    {
        LogRecord *curLog = *it;   
        TxType operType = curLog->getType();
        int txId = curLog->getTxID();
        int curLSN = curLog->getLSN();
        if (operType == ABORT)
        {
            this->tx_table[txId] = txTableEntry(curLSN, U);
        }
        else if (operType == UPDATE)
        {
            this->tx_table[txId] = txTableEntry(curLSN, U);
            UpdateLogRecord *upLog = dynamic_cast<UpdateLogRecord*>(curLog);
            if (dirty_page_table.find(upLog->getPageID()) == dirty_page_table.end())
            {   dirty_page_table[upLog->getPageID()] = curLog->getLSN(); }
        }
        else if (operType == CLR)
        {
            this->tx_table[txId] = txTableEntry(curLSN, U);
            CompensationLogRecord *clrLog = dynamic_cast<CompensationLogRecord*>(curLog);
            if (dirty_page_table.find(clrLog->getPageID()) == dirty_page_table.end())
            {   dirty_page_table[clrLog->getPageID()] = clrLog->getLSN(); }
        }
        else if (operType == COMMIT)
        {
            this->tx_table[txId] = txTableEntry(curLSN, C);
        }
        else if (operType == END)
        {
            this->tx_table.erase(txId);
        }
    }
}

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
    vector<LogRecord*>::iterator log_iter = logtail.begin();
    while( (*log_iter)->getLSN() < sm_lsn){ ++log_iter; }

    for(;log_iter!=logtail.end();++log_iter)
    {
        TxType curtype = (*log_iter)->getType();
        // if is record is update or clr type, then judge whether need to do something
        if(curtype == UPDATE || curtype == CLR)
        {
            int cur_pageid = 0;
            int curlog_LSN = 0;
            int off = 0;
            string after_text;
            if(curtype == UPDATE)
            {
                UpdateLogRecord* updata_record = dynamic_cast<UpdateLogRecord*>(*log_iter);
                cur_pageid = updata_record->getPageID();
                curlog_LSN = updata_record->getLSN();
                off = updata_record->getOffset();
                after_text = updata_record->getAfterImage();
            }
            if(curtype == CLR)
            {
                CompensationLogRecord* clr_record = dynamic_cast<CompensationLogRecord*>(*log_iter);
                cur_pageid = clr_record->getPageID();
                curlog_LSN = clr_record->getLSN();
                off = clr_record->getOffset();
                after_text = clr_record->getAfterImage();
            }
            assert(cur_pageid != 0);
            assert(curlog_LSN != 0);
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
        else{++tx_iter;}
    }
    return true;
}

/*
 * If no txnum is specified, run the undo phase of ARIES.
 * If a txnum is provided, abort that transaction.
 * Hint: the logic is very similar for these two tasks!
 */
void LogMgr::undo(vector <LogRecord*> log, int txnum /*=NULL_TX*/)
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
void LogMgr::checkpoint()
{
    // ******************how to write the preLSN & tx_id for the checkpoint?*******
    int beginChkpt = se->nextLSN();
    se->store_master(beginChkpt);
    LogRecord *check_begin = new LogRecord(beginChkpt,NULL_LSN,NULL_TX, BEGIN_CKPT);
    logtail.push_back(check_begin);
    
    int endChkpt = se->nextLSN();
    ChkptLogRecord *check_end = new ChkptLogRecord(endChkpt,beginChkpt,NULL_TX,this->tx_table,this->dirty_page_table);
    logtail.push_back(check_end);
    this->flushLogTail(endChkpt);
}
/*
 * Commit the specified transaction.
 */
void LogMgr::commit(int txid)
{
    // write a COMMIT log
    int com_id = se->nextLSN();
    LogRecord *commit_log = new LogRecord(com_id,this->getLastLSN(txid),txid,COMMIT);
    logtail.push_back(commit_log);

    // flush -> change status    or  change status->flush???
    // update TT Status
    tx_table[txid].status = C;
    tx_table[txid].lastLSN = com_id;
    this->flushLogTail(com_id); 

    // write END & remove this trx from TT
    int newLSN = se->nextLSN();
    commit_log = new LogRecord(newLSN,this->getLastLSN(txid),txid,END);
    logtail.push_back(commit_log);
    tx_table.erase(txid);


    //it is necessary to write TT status to "C" ??
    //if :  COMMIT ->  begin_checkpoint ->  END  
    // will the state of TT at begin_checkpoint is wrong ??

}
/*
 * A function that StorageEngine will call when it's about to 
 * write a page to disk. 
 * Remember, you need to implement write-ahead logging
 */
void LogMgr::pageFlushed(int page_id)
{
    // flush log file to disk 
    int page_lsn = se->getLSN(page_id);
    this->flushLogTail(page_lsn);
    // remove the page from DPT
    map <int, int>::iterator dp_i =  dirty_page_table.find(page_id);
    assert(dp_i != dirty_page_table.end());
    dirty_page_table.erase(dp_i);
}

/*
 * Recover from a crash, given the log from the disk.
 */
void LogMgr::recover(string log)
{
    vector <LogRecord*> recover_lg = stringToLRVector(log);
    this->analyze(recover_lg);
    bool flag = this->redo(recover_lg);
    assert(flag == true);
    if(!flag){return;}
    this->undo(recover_lg);
}

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

vector<LogRecord*> LogMgr::stringToLRVector(string logstring)
{
    istringstream ss_stream(logstring);
    vector<LogRecord*> stv_vector;
    string temp;

    while(!ss_stream.eof())
    {
        getline(ss_stream,temp);
        LogRecord* nrecord = LogRecord::stringToRecordPtr(temp);
        stv_vector.push_back(nrecord);
    }
    return stv_vector;
}
/*
 * Sets this.se to engine. 
 */
void LogMgr::setStorageEngine(StorageEngine* engine)
{
    this->se = engine;
}
