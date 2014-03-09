package ics.uci.edu.rmi;
	
import ics.uci.edu.logmanager.LogManager;
import ics.uci.edu.logmanager.LogEntry;
import ics.uci.edu.lockmanager.LockManager;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Map;
import java.util.TimerTask;
import java.util.Timer;

import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.UnicastRemoteObject;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.auth.ClasspathPropertiesFileCredentialsProvider;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.Bucket;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.ListObjectsRequest;
import com.amazonaws.services.s3.model.ObjectListing;
import com.amazonaws.services.s3.model.PutObjectRequest;
import com.amazonaws.services.s3.model.S3Object;
import com.amazonaws.services.s3.model.S3ObjectSummary;

public class TCServer implements TC {
	private int xid;
//	private int lsn;
	private static LogManager logMgr;
	private static LockManager lockMgr;
	public static String TCIP = "169.234.134.220"; 
	public static String DC1IP = "169.234.134.226";
	public static String DC2IP = "127.0.0.1";
	
    public TCServer() {
		logMgr = new LogManager();
		lockMgr = new LockManager();
    	this.xid = 0;
    }
    
    public String callDC_List(String input) { // input 1 = CLASSROOM, 2 = LAB, 3 = TOOL, 4 = PEOPLE, 5 = RESERVATION
    	String result ="";
		int xid = getXid();
		int lsn = 0;
		int ulresult = 0;
		LogEntry le = new LogEntry();
		
		if(Integer.parseInt(input) < 6){
			try {
				if((lsn = lockMgr.lock(xid, input, 1)) > 0){
				    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
				    DC stub = (DC) registry.lookup("DC");
				    
				    result = stub.listTable(input, xid, lsn);	// result: Serialized Table Entry
				    				    
				    ulresult = lockMgr.unlock(xid);
				    if(ulresult == 0) // try just one more time
				    	ulresult = lockMgr.unlock(le.getXid());	
				    
				}
				else{
					return "Aborted by Lock Manager, operation fails.";
				}
			} catch (Exception e) {
				System.err.println("TC's attempt to DC is failed[callDC_List]: " + e.toString());
			    e.printStackTrace();
			}
		}
		else{
			try {
				if((lsn = lockMgr.lock(xid, "1", 1)) > 0){ // This transaction is for getting S Lock on Classroom Table
				    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
				    DC stub = (DC) registry.lookup("DC");
				    
				    result = stub.listTable("1", xid, lsn);	// result: Serialized Table Entry

				    System.out.println("[TC Notice!!!] Xid that should be unlock: " + xid);
				}
				else{
					return "Aborted by Lock Manager, operation fails.";
				}
			} catch (Exception e) {
				System.err.println("TC's attempt to DC is failed[callDC_List]: " + e.toString());
			    e.printStackTrace();
			}			
		}
		return result; //"Successfully returned from callDC_List()";
    }
    
    public String callDC_Insert(String input, String insertRow) { // input 1-5: normal case, 6-7: for demo
    	String result ="";
		int xid = getXid();
		int lsn = 0;
		int ulresult = 0;
		LogEntry le = new LogEntry();
		
		if(Integer.parseInt(input) < 6){
			try {
				if((lsn = lockMgr.lock(xid, input, 2)) > 0){
				    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
				    DC stub = (DC) registry.lookup("DC");
				    
				    result = stub.insertTable(input, insertRow, xid, lsn);
				    
				    if(result.equals("existed")){
				    	ulresult = lockMgr.unlock(xid);
				    	if(ulresult == 0) // try just one more time
				    		ulresult = lockMgr.unlock(xid);	
				    	
				    	return "Insert " + insertRow + " fails because this row has already existed."; //modify
				    }
					else if(!result.equals("fail")){
					    le.deserializableToLogEntry(result);
					    //System.out.println("Test fail to write insert log to log file!");
					    logMgr.saveLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
					    logMgr.writeLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
					    System.out.println("Succeed to write to log file!");
					    
					    ulresult = lockMgr.unlock(le.getXid());
					    if(ulresult == 0) // try just one more time
					    	ulresult = lockMgr.unlock(le.getXid());			
					    
					    return "Insert " + insertRow + " successfully."; //modify
				    }
				}
				else{
					return "Aborted by Lock Manager, operation fails. xid:" + xid;
				}
			} catch (Exception e) {
				System.err.println("TC's attempt to DC is failed: " + e.toString());
			    e.printStackTrace();
			}	
		}
		else if(Integer.parseInt(input) == 6){ // Test Case for Failing to Send Msg From DC to TC on Classroom Table
			try {
				if((lsn = lockMgr.lock(xid, "1", 2)) > 0){
				    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
				    DC stub = (DC) registry.lookup("DC");
				    
				    result = stub.insertFail("1", insertRow, xid, lsn);
				    if(!result.equals("fail")){
					    le.deserializableToLogEntry(result);
					    //System.out.println("Test fail to write insert log to log file!");
					    logMgr.saveLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
					    logMgr.writeLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
					    System.out.println("Succeed to write to log file!");
				    }
				    
				    ulresult = lockMgr.unlock(xid);
				    if(ulresult == 0) // try just one more time
				    	ulresult = lockMgr.unlock(xid);				    
				}
				else{
					return "Aborted by Lock Manager, operation fails., xid:" + xid;
				}
			} catch (Exception e) {
				System.err.println("TC's attempt to DC is failed: " + e.toString());
			    e.printStackTrace();
			}
			
			if(result.equals("fail"))
				return "Failure occurred!";
			else
				return "Successfully returned!";
		}
		else if (Integer.parseInt(input) == 7){ // Test Case for Failing to Write Log in TC on Classroom Table
			try {
				if((lsn = lockMgr.lock(xid, "1", 2)) > 0){
				    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
				    DC stub = (DC) registry.lookup("DC");
				    
				    result = stub.insertTable("1", insertRow, xid, lsn);
				    if(!result.equals("fail")){
				    	le.deserializableToLogEntry(result);
//				    	logMgr.saveLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
//				   		logMgr.writeLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
				    	System.out.println("Fail to write to log file on Purpose!");
				    }
				    
				    ulresult = lockMgr.unlock(le.getXid());
				    if(ulresult == 0) // try just one more time
				    	ulresult = lockMgr.unlock(le.getXid());
				}
				else{
					return "Aborted by Lock Manager, operation fails.";
				}
			} catch (Exception e) {
				System.err.println("TC's attempt to DC is failed: " + e.toString());
			    e.printStackTrace();
			}
			
			return "Failure occurred on Writing Log on Purpose!";
		}
		else if (Integer.parseInt(input) == 8){ // not lock
			try {
				if((lsn = lockMgr.lock(xid, "1", 2)) > 0){
				    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
				    DC stub = (DC) registry.lookup("DC");
				    
				    result = stub.insertTable("1", insertRow, xid, lsn);
				    if(!result.equals("fail")){
				    	le.deserializableToLogEntry(result);
				    	logMgr.saveLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
				   		logMgr.writeLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
				    	System.out.println("Fail to write to log file on Purpose!");
				    }
				    
				    System.out.println("[TC Notice!!!] Xid that should be unlock: " + le.getXid());
				    System.out.println();
				    System.out.println();
				}
				else{
					return "Aborted by Lock Manager";
				}
			} catch (Exception e) {
				System.err.println("TC's attempt to DC is failed " + e.toString());
			    e.printStackTrace();
			}
			
			return "Failure occurred on Writing Log on Purpose!";
		}
		
		return "Should not be returned by THIS!!!!!";
    }
    
    @Override
	public String callDC_Update(String input, String updateRow) {
    	String result ="";
		int xid = getXid();
		int ulresult = 0;
		
		LogEntry le = new LogEntry();
		
		try {
			int lsn = 0;
//			System.out.println("##### xid: " +xid + ", input: " + input);
			if( (lsn = lockMgr.lock(xid, input, 2)) > 0){
//System.out.println("######### LSN in callDC_Update $$$$$$$$$$$$" + lsn);
			    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
			    DC stub = (DC) registry.lookup("DC");
			    
			    result = stub.updateTable(input, updateRow, xid, lsn);
			    if(result.equals("unexisted")){
					
					ulresult = lockMgr.unlock(xid);
				    if(ulresult == 0) // try just one more time
				    	ulresult = lockMgr.unlock(xid);		
				    
				    return "Update " + updateRow + " fails because this row hasn't already existed.";
			    }
			    else{
				    le.deserializableToLogEntry(result);
				    logMgr.saveLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
				    logMgr.writeLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
				    
				    ulresult = lockMgr.unlock(le.getXid());
				    if(ulresult == 0) // try just one more time
				    	ulresult = lockMgr.unlock(le.getXid());	
				    
				    return "Update " + updateRow + " successfully."; //modify
			    }
			}
			else{
				return "Aborted by Lock Manager in callDC_Update()";
			}
		} catch (Exception e) {
			System.err.println("TC's attempt to DC is failed[callDC_Update]: " + e.toString());
		    e.printStackTrace();
		}
		return "";
	}
    
    public String callDC_Delete(String input, String deleteRow) {
    	String result ="";
		int xid = getXid();
		int lsn = 0;
		int ulresult = 0;
		LogEntry le = new LogEntry();
		
		try {
			if((lsn = lockMgr.lock(xid, input, 2)) > 0){
			    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
			    DC stub = (DC) registry.lookup("DC");
			    
			    result = stub.deleteTable(input, deleteRow, xid, lsn);
			    if(result.equals("unexisted")){
			    	ulresult = lockMgr.unlock(xid);
				    if(ulresult == 0) // try just one more time
				    	ulresult = lockMgr.unlock(xid);
				    
				    return "Delete fails because this row hasn't already existed.";
			    }
			    else{	
				    le.deserializableToLogEntry(result);
					logMgr.saveLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
				    logMgr.writeLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
				    
				    ulresult = lockMgr.unlock(le.getXid());
				    if(ulresult == 0) // try just one more time
				    	ulresult = lockMgr.unlock(le.getXid());
				    
				    return "Delete " + deleteRow + " successfully.";	
			    }
			}
			else{
				return "Aborted by Lock Manager, operation fails.";
			}
		} catch (Exception e) {
			System.err.println("TC's attempt to DC is failed[callDC_Delete]: " + e.toString());
		    e.printStackTrace();
		}		
		return "";
    }
    
    public boolean unlockTX(int xid){
    	int ulresult = 0;
    	
    	try {
	    	ulresult = lockMgr.unlock(xid);
		    if(ulresult == 0) // try just one more time
		    	ulresult = lockMgr.unlock(xid);
	    	
		    if(ulresult != 0)
		    	return true;
		    else
		    	return false;
    	} catch (Exception e) {
			System.err.println("TC's attempt to DC is failed[callDC_Delete]: " + e.toString());
		    e.printStackTrace();
		}
    	return false;
    }
    
    public String callDC_RestoreDB() throws Exception, IOException{
    	System.out.println("Success to enter TC restoreDB");
    	ArrayList<LogEntry> restoreLog = logMgr.getLogTableFromLogFile();
    	String input = "";
    	for (int i = 0; i < restoreLog.size(); i++)
    		input += restoreLog.get(i).serializableToString() + " ";
    	try {
	    	Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
		    DC stub = (DC) registry.lookup("DC");
		    
		    String result = stub.restoreDB(input);
		} catch (Exception e) {
			System.err.println("TC's attempt to DC is failed[callDC_Restore]: " + e.toString());
		    e.printStackTrace();
		}
		return "Successfully returned from DC";
    }
    
    public static void main(String args[]) {
	
		try {
		    TCServer obj = new TCServer();
		    TC stub = (TC) UnicastRemoteObject.exportObject(obj, 0);
	
		    // Bind the remote object's stub in the registry
		    Registry registry = LocateRegistry.getRegistry(null, 1099);
		    registry.bind("TC", stub);
		    		    
		    
		    Timer time = new Timer(); // Instantiate Timer Object
			ScheduledTask st = new ScheduledTask(); // Instantiate SheduledTask class
			time.schedule(st, 0, 10000); // Create Repetitively task for every 1 secs		    
		    
		    
		    System.err.println("TCServer is ready.");
		} catch (Exception e) {
		    System.err.println("TC Server Exception: " + e.toString());
		    e.printStackTrace();
		}
    }

    public synchronized int getXid(){
    	this.xid++;
    	return this.xid;
    }
    
    static class ScheduledTask extends TimerTask {

    	// Add your task here
    	public void run() {
    		try {
    			String result = "";
    			String [] results;
    		    Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
    		    DC stub = (DC) registry.lookup("DC");
    		    result = stub.receiveLsn(LogManager.LSN);
//    		    System.out.println("[Periodic Call] receiveLsn() is properly returned!!!!");
    		    
    		    if(result.equals(""))
    		    	System.out.println("[Periodic Call] Sent LSN: " + LogManager.LSN + " and LogEntry in DC is empty");
    		    else
    		    	System.out.println("[Periodic Call] Sent LSN: " + LogManager.LSN);
    		    
    		    if(!result.equals("")){
	    		    results = result.split("\\^");
	    		    //System.out.println("[Periodic Call] results length: " + results.length);
	    		    for(int i = 0; i < results.length; i++){
	    		    	LogEntry le = new LogEntry();
	    		    	le.deserializableToLogEntry(results[i]);
	    		    	//logMgr.printLogTable();
	    		    	logMgr.updateLog(le.getLSN(), le.getCommit_YN(), le.getCheckpoint_YN(), le.getXid(), le.getType_of_operation(), le.getTableName(), le.getBeforeImage(), le.getAfterImage());
	    		    	//logMgr.printLogTable();
	    		    }
    		    }
    		} catch (Exception e) {
    			System.err.println("TC's attempt to DC is failed[ScheduleTask]: " + e.toString());
    		    e.printStackTrace();
    		}
    	}
    }

	@Override
	public String callDC_ClearTable() throws RemoteException, Exception {
		Registry registry = LocateRegistry.getRegistry(DC1IP, 1100);
	    DC stub = (DC) registry.lookup("DC");
	    String result = stub.clearTable();
	    return result;
	}
    
}