/*
 * LogMgr.cpp
 *
 *  Created on: Nov 7, 2016
 *      Author: Yiming Duan
 */

#include <cstring>
#include <sstream>
#include <string>
#include "LogMgr.h"
#include <map>
#include <set>
#include <vector>
vector<LogRecord*> LogMgr::stringToLRVector(string logstring){
	  vector<LogRecord*> result;
	  istringstream stream(logstring);
	  string line;
	  while(getline(stream, line)) {
	  LogRecord* lr = LogRecord::stringToRecordPtr(line);
	  result.push_back(lr);
	  }
	  return result;
  }

/*
   * Find the LSN of the most recent log record for this TX.
   * If there is no previous log record for this TX, return
   * the null LSN.
   */
int LogMgr::getLastLSN(int txnum){
	//use iterator to traverse the vector
	/*
	for (vector<LogRecord*>::reverse_iterator it = logtail.rbegin(); it != logtail.rend(); ++it){
		LogRecord * lr = *it;
		if(txnum==(lr->getTxID())){
			return lr->getLSN();
		}
	}
	vector<LogRecord*> result = stringToLRVector(se->getLog());
	for (vector<LogRecord*>::reverse_iterator it2 = result.rbegin(); it2 != result.rend(); ++it2){
		LogRecord * lr = * it2;
		if(txnum==lr->getTxID()){
			return lr->getLSN();
		}
	}
	*/
	for(map<int,txTableEntry>::iterator tx = tx_table.begin(); tx!=tx_table.end(); ++tx){
		if (tx->first==txnum){
			return tx->second.lastLSN;
		}
	}
	return NULL_LSN;

}

  /*
   * Update the TX table to reflect the LSN of the most recent
   * log entry for this transaction.
   */
  void LogMgr::setLastLSN(int txnum, int lsn){
	  TxType type;
	  vector<LogRecord*> log = stringToLRVector(se->getLog());
	  for(vector<LogRecord*>::iterator it = logtail.begin(); it != logtail.end(); ++it){
		  LogRecord * lr = * it;
		  log.push_back(lr);
	  }

	  for(vector<LogRecord*>::reverse_iterator it = log.rbegin(); it != log.rend(); ++it){
		  LogRecord * lr = * it;
		  if (lr->getLSN()==lsn){
			  type = lr->getType();
			  break;
		  }
	  }
	  map<int, txTableEntry>::iterator iter;
	  iter = tx_table.find(txnum);
	  //use map.find to check whether txnum is in the table.
	  //If not in the table, iter will point at the end of table
	  if(type != BEGIN_CKPT && type != END_CKPT){
		  if(iter != tx_table.end()){
			 iter->second.lastLSN=lsn;
			 if(type==COMMIT){
				 iter->second.status=C;
			 }
			 else if(type==END){
				 iter = tx_table.erase(iter);
			 }
			 else if(type==UPDATE || type==ABORT || type==CLR)
			 {
				 iter->second.status=U;
			 }
		  }
		  else
		  {
			  tx_table.insert(pair<int, txTableEntry>(txnum, txTableEntry(lsn, U)));
		  }
	  }
  }
		  


  /*
   * Force log records up to and including the one with the
   * maxLSN to disk. Don't forget to remove them from the
   * logtail once they're written!
   */
 void LogMgr::flushLogTail(int maxLSN){
	  for (vector<LogRecord*>::iterator it = logtail.begin(); it != logtail.end();){
		  LogRecord * lr = *it;
		  if(lr->getLSN()>maxLSN){
			  break;
		  }
		  else{
			  se->updateLog(lr->toString());
			  it = logtail.erase(it);
			  }

	  }
  }

  /*
   * Run the analysis phase of ARIES.
   */

  void LogMgr::analyze(vector <LogRecord*> log){
	  int start_lsn = se->get_master();
	  ChkptLogRecord * end_chk_ptr;
	  for (vector<LogRecord*>::reverse_iterator it = log.rbegin(); it != log.rend(); ++it){
		  LogRecord * lr = * it;
		  if(lr->getType()==END_CKPT){
			  end_chk_ptr = dynamic_cast<ChkptLogRecord *>(lr);
			  break;
		  }
	  }
	  tx_table = end_chk_ptr->getTxTable();
	  dirty_page_table = end_chk_ptr->getDirtyPageTable();

	  for (vector<LogRecord*>::iterator it = log.begin(); it != log.end(); ++it){
		  LogRecord * lr = * it; int lsn = lr->getLSN();
		  if(lr->getLSN()>=start_lsn){
			  TxType type = lr->getType();
			  int txnum = lr->getTxID();
			  //change txtable
			  map<int, txTableEntry>::iterator iter;
			  	  iter = tx_table.find(txnum);
			  	  if(type != BEGIN_CKPT && type != END_CKPT){
			  		  if(iter != tx_table.end()){
			  			 iter->second.lastLSN=lsn;
			  			 if(type==COMMIT){
			  				 iter->second.status=C;
			  			 }
			  			 else if(type==END){
			  				 iter = tx_table.erase(iter);
			  			}
			  			 else if(type==UPDATE || type==ABORT || type==CLR)
			  			 {
			  				 iter->second.status=U;
			  			 }
			  		  }
			  		  else
			  		  {
			  			  tx_table.insert(pair<int, txTableEntry>(txnum, txTableEntry(lsn, U)));
			  		  }
			  	  }

			  //setLastLSN(lr->getTxID(),lr->getLSN());
		  }
	  }

	  //map <int, int>::iterator dir = dirty_page_table.begin();
	  for (vector<LogRecord*>::iterator it = log.begin(); it != log.end(); ++it){
	  		  LogRecord * lr = * it;
	  		  int lsn = lr->getLSN();
	  		  if(lsn>=start_lsn){
	  			  if(lr->getType()==UPDATE){
	  				UpdateLogRecord* ulr = dynamic_cast<UpdateLogRecord *>(lr);
	  				map<int, int>::iterator iter;
	  				iter = dirty_page_table.find(ulr->getPageID());
	  				if(iter == dirty_page_table.end()){
	  					dirty_page_table.insert(pair<int, int>(ulr->getPageID(), lr->getLSN()));
	  				}
	  			  }


	  			  if(lr->getType()==CLR){
	  				  CompensationLogRecord* clr = dynamic_cast<CompensationLogRecord *>(lr);
	  			  	  map<int, int>::iterator iter2;
	  				  iter2 = dirty_page_table.find(clr->getPageID());
	  				  if(iter2 == dirty_page_table.end()){
	  					  dirty_page_table.insert(pair<int, int>(clr->getPageID(), lr->getLSN()));
	  				  }
	  				  }
	  			  
	  		  }
	  }
  }


  /*
   * Run the redo phase of ARIES.
   * If the StorageEngine stops responding, return false.
   * Else when redo phase is complete, return true.
   */

  bool LogMgr::redo(vector <LogRecord*> log){
	  if(dirty_page_table.empty()) return true;
	  
	  map<int,int>::iterator dir = dirty_page_table.begin();
	  int begin_lsn = dir->second;
	  ++dir;
	  for(;dir != dirty_page_table.end();++dir){
		  if(dir->second<begin_lsn){
			  begin_lsn = dir->second;
		  }
	  }
	  
	  
	  vector<LogRecord*>::iterator it = log.begin();
	  LogRecord * lr = * it;
	  while(lr->getLSN()<begin_lsn){
		  ++it;
		  lr = *it;
	  }

	  for (; it != log.end(); ++it){
		  lr = *it;
		  int small_lsn = lr->getLSN();
		  int pageid;int off;string aft_img;
		  TxType type = lr->getType();
		  if (type==UPDATE || type==CLR){
		  if (type==UPDATE){

			  UpdateLogRecord* ulr = dynamic_cast<UpdateLogRecord *>(lr);
			  pageid = ulr->getPageID();
			  //trxid = ulr->getTxID();
			  off = ulr->getOffset();
			  //bef_img = ulr->getBeforeImage();
			  aft_img = ulr->getAfterImage();

		  }
		  if (type==CLR){
			  CompensationLogRecord* clr = dynamic_cast<CompensationLogRecord *>(lr);
			  pageid = clr->getPageID();
			  //trxid = clr->getTxID();
			  off = clr->getOffset();
			  //todolsn = clr->getUndoNextLSN();
			  aft_img = clr->getAfterImage();

		  }
			  map<int, int>::iterator dir = dirty_page_table.begin();
			  dir = dirty_page_table.find(pageid);
			  
			  if(dir != dirty_page_table.end()){
				  if (dir->second<=small_lsn){
					  if (se->getLSN(pageid)<small_lsn){
						  bool abc = se->pageWrite(pageid, off, aft_img, small_lsn);
						  if (abc==false){
							  return false;
						  }
						
					  }
				  }
			  }





			  }

		  }
	  for (map<int, txTableEntry>::iterator tx = tx_table.begin(); tx != tx_table.end();){
		  if(tx->second.status==C){
			  logtail.push_back(new LogRecord(se->nextLSN(),tx->second.lastLSN,tx->first,END));
			  tx = tx_table.erase(tx);
		  }
		  else ++tx;
	  }
	  return true;
	  }


  /*
   * If no txnum is specified, run the undo phase of ARIES.
   * If a txnum is provided, abort that transaction.
   * Hint: the logic is very similar for these two tasks!
   */
  void LogMgr::undo(vector <LogRecord*> log, int txnum){
	set<int> undolist;
	int pre_lsn;int pageid;int off;string aft_img;int lsn2;int txid;string bef_img;
	if (txnum!=NULL_TX){
		undolist.insert(tx_table[txnum].lastLSN);
	}
	else {
		for(map<int,txTableEntry>::iterator txt = tx_table.begin(); txt != tx_table.end(); ++txt){
			undolist.insert(txt->second.lastLSN);
		}
	}

	set<int>::reverse_iterator undoiter = undolist.rbegin();
	while (!undolist.empty()){
		int maxlsn = *undoiter;
		undolist.erase(maxlsn);
		LogRecord * lr;
		for(vector<LogRecord*>::iterator it = log.begin(); it != log.end(); ++it){
			lr = * it;
			if (lr->getLSN()==maxlsn){
				break;
			}
		}
		if (lr->getType()==UPDATE){
			UpdateLogRecord* ulr = dynamic_cast<UpdateLogRecord *>(lr);
			pageid = ulr->getPageID();
			txid = ulr->getTxID();
			off = ulr->getOffset();
			aft_img = ulr->getAfterImage();
			bef_img = ulr->getBeforeImage();
			
			pre_lsn = ulr->getprevLSN();
			se->getLSN(pageid);
			lsn2 = se->nextLSN();
			
			logtail.push_back(new CompensationLogRecord(lsn2,getLastLSN(txid),txid,pageid,off,bef_img,pre_lsn));
			setLastLSN(txid,lsn2);
			
			bool abc = se->pageWrite(pageid, off, bef_img, lsn2);
			if (abc==false){
				return;
			}

			if (pre_lsn != NULL_LSN){
				undolist.insert(pre_lsn);
			}
			else{
				logtail.push_back(new LogRecord(se->nextLSN(),lsn2,txid,END));
				tx_table.erase(txid);
			}
		}
		if (lr->getType()==ABORT){
			if (lr->getprevLSN()==NULL_LSN){
				logtail.push_back(new LogRecord(se->nextLSN(),maxlsn,lr->getTxID(),END));
				tx_table.erase(lr->getTxID());
			}
			else{
				undolist.insert(lr->getprevLSN());
			}
		}
		if (lr->getType()==CLR){
			CompensationLogRecord* clr = dynamic_cast<CompensationLogRecord *>(lr);
			pageid = clr->getPageID();
			txid = clr->getTxID();
			off = clr->getOffset();
			aft_img = clr->getAfterImage();
			
			pre_lsn = clr->getprevLSN();
			int nextlsn = clr->getUndoNextLSN();
			if (nextlsn==NULL_LSN){
				lsn2 = se->nextLSN();
				logtail.push_back(new LogRecord(lsn2,getLastLSN(txid),txid,END));
				tx_table.erase(txid);
			}
			else{
				undolist.insert(nextlsn);
			}
		}
	}
  }








	
 /*
   * Abort the specified transaction.
   * Hint: you can use your undo function
   */
  void LogMgr::abort(int txid){
	  int lsn = se->nextLSN();
	  logtail.push_back(new LogRecord(lsn,getLastLSN(txid),txid,ABORT));
	  vector<LogRecord*> log = stringToLRVector(se->getLog());
	  
	  for(vector<LogRecord*>::iterator it = logtail.begin(); it != logtail.end(); ++it){
		  log.push_back(*it);
	  }
	  
	  setLastLSN(txid,lsn);
	  undo(log,txid);  
  }

  /*
   * Write the begin checkpoint and end checkpoint
   */
  void LogMgr::checkpoint(){
	  int lsn1 = se->nextLSN();
	  int lsn2 = se->nextLSN();
	  LogRecord * begin_cpt = new LogRecord(lsn1, NULL_LSN, NULL_TX, BEGIN_CKPT);
	  logtail.push_back(begin_cpt);
	  se->store_master(lsn1);
	  ChkptLogRecord * end_cpt = new ChkptLogRecord(lsn2, lsn1, NULL_TX, tx_table, dirty_page_table);
	  logtail.push_back(end_cpt);
	  //======================================================================
	  flushLogTail(lsn2);
	  
	  //======================================================================

  }

  /*
   * Commit the specified transaction.
   */
  void LogMgr::commit(int txid){
	  int newlsn = se->nextLSN();
	  LogRecord *log = new LogRecord(newlsn,getLastLSN(txid),txid,COMMIT);
	  logtail.push_back(log);
	  tx_table[txid].lastLSN = newlsn;
	  tx_table[txid].status = C;
	  flushLogTail(newlsn);
	  
	  //=======================================================================
	  /*
	  for (vector<LogRecord*>::iterator it = logtail.begin(); it != logtail.end();){
	 		  LogRecord * lr = * it;
	 		  if (txid==lr->getTxID()){
	 			  it = logtail.erase(it);
	 		  }
	 		  else ++it;
	 	  }
		   */
		   logtail.push_back(new LogRecord(se->nextLSN(),newlsn,txid,END));
		   tx_table.erase(txid);
  }

  /*
   * A function that StorageEngine will call when it's about to
   * write a page to disk.
   * Remember, you need to implement write-ahead logging
   */
  void LogMgr::pageFlushed(int page_id){
	  /*vector<LogRecord*>::reverse_iterator it = logtail.rbegin();
	  LogRecord * lr = * it;
	  int last_lsn = lr->getLSN();*/
	  int last_lsn = se->getLSN(page_id);
	  flushLogTail(last_lsn);
	  map<int, int>::iterator iter = dirty_page_table.begin();
	  iter = dirty_page_table.find(page_id);
	  if(iter != dirty_page_table.end()){
		  iter = dirty_page_table.erase(iter);
	  }


  }

  /*
   * Recover from a crash, given the log from the disk.
   */
  void LogMgr::recover(string log){
	  //log = se->getLog();
	  vector<LogRecord*> log2= stringToLRVector(log);
	  analyze(log2);
	  if(redo(log2)==false){
		  return;
	  }
	  undo(log2);
  }

  /*
   * Logs an update to the database and updates tables if needed.
   */
  int LogMgr::write(int txid, int page_id, int offset, string input, string oldtext){
	  /*
	  int lsn_pre;
	  for (vector <LogRecord*>::reverse_iterator it = logtail.rbegin(); it != logtail.rend(); ++it){
		  LogRecord * lr = * it;
		  if (lr->getTxID()==txid){
			  lsn_pre = lr->getLSN();
		  }
	  }
	  */
	  int lsn_new = se->nextLSN();
	  UpdateLogRecord * newlog = new UpdateLogRecord(lsn_new, getLastLSN(txid), txid, page_id, offset, oldtext, input);
	  logtail.push_back(newlog);

	  map<int, int>::iterator dir = dirty_page_table.begin();
	  dir = dirty_page_table.find(page_id);

	  if(dir == dirty_page_table.end()){
		  dirty_page_table.insert(pair<int, int>(page_id, lsn_new));
	  }

	  setLastLSN(txid, lsn_new);
	  return lsn_new;
  }

  /*
   * Sets this.se to engine.
   */
  void LogMgr::setStorageEngine(StorageEngine* engine){
	  this->se = engine;
  }


