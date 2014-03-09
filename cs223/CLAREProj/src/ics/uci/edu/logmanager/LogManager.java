package ics.uci.edu.logmanager;

import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.FileNotFoundException;
import java.io.BufferedWriter;
import java.lang.Exception;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Collection;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.TreeSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.SortedSet;
import java.util.List;
import java.util.ArrayList;
import java.util.Hashtable;


public class LogManager {

	public static int LSN = 0;
	public static Hashtable<Integer, LogEntry> htLog = new Hashtable<Integer, LogEntry>();

    public LogManager() {
    	// Read LSN from LSN.log
		if (LSN == 0) {      
            File logFile = new File("./operation.log");
            
            try { 	
				if(!logFile.exists()) {
					try { BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter("./operation.log"));
						  bufferedWriter.append("");
						  bufferedWriter.close();
						  LSN = 0;
					} catch(IOException e){ 
						System.err.println("Can't write LSN.log file."); 
					}
				}
				else{
					FileInputStream in = new FileInputStream(logFile);
					BufferedReader br = new BufferedReader(new InputStreamReader(in));
					String oneRecord;
				
					while((oneRecord = br.readLine())!= null) { //     	2/1/0/1/3/4/Benson,professor/Benson,professor
																// 		lsn/Commit_YN/Checkpoint_YN/Xid/Type_of_operation/TableName
						String [] tokens = oneRecord.split("`");
						LogEntry le = new LogEntry();
						int tmpLSN = Integer.parseInt(tokens[0]);
						le.setLSN(tmpLSN);
						le.setCommit_YN(Integer.parseInt(tokens[1]));
						le.setCheckpoint_YN(Integer.parseInt(tokens[2]));
						le.setXid(Integer.parseInt(tokens[3]));
						le.setType_of_operation(Integer.parseInt(tokens[4]));
						le.setTableName(Integer.parseInt(tokens[5]));
						le.setBeforeImage(tokens[6]);
						le.setAfterImage(tokens[7]);
						
						htLog.put(tmpLSN, le);
					
						if(tmpLSN > LSN)
								LSN = tmpLSN;
					}
					
					br.close();
				}
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}	catch (IOException e) {
				e.printStackTrace();
			}		

		}

	}
    
    
    public synchronized boolean updateLog(int lsn, int commit_YN, int checkpoint_YN, int xid, int type_of_operation, int tableName, String beforeImage, String afterImage) {
    	LogEntry le = new LogEntry();
		if(!htLog.containsKey(lsn)){	
	    	le.setLSN(lsn);
			le.setCommit_YN(commit_YN);
			le.setCheckpoint_YN(checkpoint_YN);
			le.setXid(xid);
			le.setType_of_operation(type_of_operation);
			le.setTableName(tableName);
			le.setBeforeImage(beforeImage);
			le.setAfterImage(afterImage);

			writeLog(lsn, commit_YN, checkpoint_YN, xid, type_of_operation, tableName, beforeImage, afterImage);
			htLog.put(lsn, le);
			
			if(lsn > LSN)
				LSN = lsn;
		}
		
    	return true;
    }
    
    public synchronized boolean saveLog(int lsn, int commit_YN, int checkpoint_YN, int xid, int type_of_operation, int tableName, String beforeImage, String afterImage) {
    	LogEntry le = new LogEntry();
    	
    	le.setLSN(lsn);
		le.setCommit_YN(commit_YN);
		le.setCheckpoint_YN(checkpoint_YN);
		le.setXid(xid);
		le.setType_of_operation(type_of_operation);
		le.setTableName(tableName);
		le.setBeforeImage(beforeImage);
		le.setAfterImage(afterImage);
		
		htLog.put(lsn, le);
		
		if(lsn > LSN)
			LSN = lsn;

    	return true;
    }

	// Write an operation Log to the disk.
	// This method is synchronized, which means that not multiple class can access this Log file.

	public synchronized boolean writeLog(int lsn, int commit_YN, int checkpoint_YN, int xid, int type_of_operation, int tableName, String beforeImage, String afterImage) {

		String delimeter = "`";
		String logToWrite = Integer.toString(lsn) + delimeter + Integer.toString(commit_YN) + delimeter + Integer.toString(checkpoint_YN) + delimeter +
		                    Integer.toString(xid) + delimeter + Integer.toString(type_of_operation) + delimeter +
		                    tableName + delimeter + beforeImage + delimeter + afterImage;
	
		try { 
			BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter("operation.log", true));
			bufferedWriter.append(logToWrite+"\n");
			bufferedWriter.close();
		} catch(IOException e){ 
			System.err.println("Can't write operation.log file.");
			return false;
		}
	
		return true;

	}
	
	public synchronized void printLogTable(){
    	Enumeration<Integer> enumKey = htLog.keys();
    	while(enumKey.hasMoreElements()) {
    	    int key = enumKey.nextElement();
    	    System.out.println(htLog.get(key).serializableToString());
    	}
	}
	
	public synchronized ArrayList<LogEntry> getLogTableFromLogFile() throws NumberFormatException, IOException{
		File logFile = new File("./operation.log");
		FileInputStream in = new FileInputStream(logFile);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		String oneRecord;
		ArrayList<LogEntry> restoreLog = new ArrayList<LogEntry>();
		while((oneRecord = br.readLine())!= null) { 
			String [] tokens = oneRecord.split("`");
			LogEntry le = new LogEntry();
			int tmpLSN = Integer.parseInt(tokens[0]);
			le.setLSN(tmpLSN);
			le.setCommit_YN(Integer.parseInt(tokens[1]));
			le.setCheckpoint_YN(Integer.parseInt(tokens[2]));
			le.setXid(Integer.parseInt(tokens[3]));
			le.setType_of_operation(Integer.parseInt(tokens[4]));
			le.setTableName(Integer.parseInt(tokens[5]));
			le.setBeforeImage(tokens[6]);
			le.setAfterImage(tokens[7]);
			
			restoreLog.add(le);
		}
		
		br.close();
		return restoreLog;
	}
	
// end of CLASS
}
