package ics.uci.edu.rmi;

import ics.uci.edu.logmanager.LogEntry;
import ics.uci.edu.table.ClassroomTB;
import ics.uci.edu.table.LabTB;
import ics.uci.edu.table.ToolTB;
import ics.uci.edu.table.PeopleTB;
import ics.uci.edu.table.ReservationTB;

import java.io.IOException;
import java.rmi.RemoteException;
import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Iterator;


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

public class DCServer implements DC {
	private int previousLSN;
	private int currentLSN;
	
	private ClassroomTB classroom;
	private LabTB lab;
	private ToolTB tool;
	private PeopleTB people;
	private ReservationTB reservation;
	private Hashtable<Integer,LogEntry> htLog;
	
	public static String TCIP = "169.234.134.220"; 
	public static String DC1IP = "169.234.134.226";
	public static String DC2IP = "127.0.0.1";
	
    public DCServer() throws IOException {
    	previousLSN = -1;
    	currentLSN = -1;
    	classroom = new ClassroomTB();
    	lab = new LabTB();
    	tool = new ToolTB();
    	people = new PeopleTB();
    	reservation = new ReservationTB();
    	htLog = new Hashtable<Integer,LogEntry>();
    }
    
    public String listTable(String input, int xid, int LSN){
    	int in = Integer.parseInt(input);
    	switch(in){
    	case 1:
    		System.out.println("Listing CLASSROOM succeeds.");
    		return classroom.serializeTable();
    	case 2:
    		System.out.println("Listing LAB succeeds.");
    		return lab.serializeTable();
    	case 3:
    		System.out.println("Listing TOOL succeeds.");
    		return tool.serializeTable();
    	case 4:
    		System.out.println("Listing PEOPEL succeeds.");
    		return people.serializeTable();
    	case 5:
    		System.out.println("Listing RESERVATION succeeds.");
    		return reservation.serializeTable();
    		default:
    			System.out.println("Switch error in listTable method of DC!");
    			return null;
    	}
    }
    /**
     *  flag = true means TC succeeds to receive log entry from DC
     *       = false means TC doesn't succeed to receive log entry from DC
     */
    public String insertTable(String input, String insertRow, int xid, int LSN) throws Exception {
    	boolean flag = false;
    	boolean check = false; //modify
    	int in = Integer.parseInt(input);
    	String[] fields = insertRow.split(",");
    	ArrayList<String> columns = new ArrayList<String>();
    	for(int i = 1; i < fields.length; i++)
    		columns.add(fields[i]);
    	try{
	    	writeLogToMemory(input,insertRow,xid,LSN,LogEntry.INSERT);

	    	switch(in){
	    	case 1:
	    		System.out.println("Inserting CLASSROOM succeeds.");
	    		check = classroom.addRow(fields[0], columns); //modify
	    		break;
	    	case 2:
	    		System.out.println("Inserting LAB succeeds."); 
	    		check = lab.addRow(fields[0], columns);
	    		break;
	    	case 3:
	    		System.out.println("Inserting TOOL succeeds.");
	    		check = tool.addRow(fields[0], columns);
	    		break;
	    	case 4:
	    		System.out.println("Inserting PEOPEL succeeds.");
	    		check = people.addRow(fields[0], columns);
	    		break;
	    	case 5:
	    		System.out.println("Inserting RESERVATION succeeds.");
	    		check = reservation.addRow(fields[0], columns);
	    		break;
	    		default:
	    			System.out.println("Switch error in insertTable method of DC!");
	    			flag = false;
	    			break;
	    	}
	    	if(check == false)
	    		return "existed"; //modify
	    	flag = true;
    	}
    	catch(RemoteException e){
    		flag = false;
    		System.out.println("Caught RemoteException between TC and DC: " + e.getMessage());
    	}    	
    	LogEntry logEntry = htLog.get(LSN);
		if(flag == true){
			logEntry.setCommit_YN(LogEntry.YES);
			return logEntry.serializableToString();
		}
		else{
			rollback(input,logEntry,LogEntry.INSERT);
			return "fail";
		}
    }
    
    public String insertFail(String input, String insertRow, int xid, int LSN) throws Exception {
    	boolean flag = false;
    	int in = Integer.parseInt(input);
    	String[] fields = insertRow.split(",");
    	ArrayList<String> columns = new ArrayList<String>();
    	for(int i = 1; i < fields.length; i++)
    		columns.add(fields[i]);
    	try{
	    	writeLogToMemory(input,insertRow,xid,LSN,LogEntry.INSERT);

	    	switch(in){
	    	case 1:
	    		System.out.println("Inserting CLASSROOM succeeds.");
	    		classroom.addRow(fields[0], columns);
	    		break;
	    	case 2:
	    		System.out.println("Inserting LAB succeeds.");
	    		lab.addRow(fields[0], columns);
	    		break;
	    	case 3:
	    		System.out.println("Inserting TOOL succeeds.");
	    		tool.addRow(fields[0], columns);
	    		break;
	    	case 4:
	    		System.out.println("Inserting PEOPEL succeeds.");
	    		people.addRow(fields[0], columns);
	    		break;
	    	case 5:
	    		System.out.println("Inserting RESERVATION succeeds.");
	    		reservation.addRow(fields[0], columns);
	    		break;
	    		default:
	    			System.out.println("Switch error in insertTable method of DC!");
	    			flag = false;
	    			break;
	    	}
	    	//flag = true;
    	}
    	catch(RemoteException e){
    		flag = false;
    		System.out.println("Caught RemoteException between TC and DC: " + e.getMessage());
    	}    	
    	LogEntry logEntry = htLog.get(LSN);
		if(flag == true){
			logEntry.setCommit_YN(LogEntry.YES);
			return logEntry.serializableToString();
		}
		else{
			System.out.println("[DC] Start Sleep for 20 seconds");
			Thread.sleep(20000);
			System.out.println("[DC] End Sleep");
			rollback(input,logEntry,LogEntry.INSERT);
			return "fail";
		}
    }
    
    @Override
	public String updateTable(String input, String updateRow, int xid, int LSN)
			throws RemoteException, Exception {
    	boolean flag = false;
		int in = Integer.parseInt(input);
		String[] fields = updateRow.split(",");
		ArrayList<String> columns = new ArrayList<String>();
		for(int i = 1; i < fields.length; i++)
			columns.add(fields[i]);
    	
    	try{
			switch(in){
			case 1:
				if(!classroom.isExisted(fields[0]))
					return "unexisted"; //modify
				writeLogToMemory(input,updateRow,xid,LSN,LogEntry.UPDATE);
				System.out.println("Updating CLASSROOM succeeds.");
				classroom.updateRow(fields[0], columns);
				break;
			case 2:
				if(!lab.isExisted(fields[0]))
					return "unexisted"; //modify
				writeLogToMemory(input,updateRow,xid,LSN,LogEntry.UPDATE);
				System.out.println("Updating LAB succeeds.");
				lab.updateRow(fields[0], columns);
				break;
			case 3:
				if(!tool.isExisted(fields[0]))
					return "unexisted"; //modify
				writeLogToMemory(input,updateRow,xid,LSN,LogEntry.UPDATE);
				System.out.println("Updating TOOL succeeds.");
				tool.updateRow(fields[0], columns);
				break;
			case 4:
				if(!people.isExisted(fields[0]))
					return "unexisted"; //modify
				writeLogToMemory(input,updateRow,xid,LSN,LogEntry.UPDATE);
				System.out.println("Updating PEOPEL succeeds.");
				people.updateRow(fields[0], columns);
				break;
			case 5:
				if(!reservation.isExisted(fields[0]))
					return "unexisted"; //modify
				writeLogToMemory(input,updateRow,xid,LSN,LogEntry.UPDATE);
				System.out.println("Updating RESERVATION succeeds.");
				reservation.updateRow(fields[0], columns);
				break;
				default:
					System.out.println("Switch error in updateTable method of DC!");
					break;
			}
	    	flag = true;
    	}
    	catch(RemoteException e){
    		flag = false;
    		System.out.println("Caught RemoteException between TC and DC: " + e.getMessage());
    	}
    	LogEntry logEntry = htLog.get(LSN);
		if(flag == true){
			
			logEntry.setCommit_YN(LogEntry.YES);
			return logEntry.serializableToString();
		}
		else{
			rollback(input,logEntry,LogEntry.UPDATE);
			return "fail";
		}
	}
    
    public String deleteTable(String input, String deleteRow, int xid, int LSN) throws Exception {
    	boolean flag = false;
    	int in = Integer.parseInt(input);
    	try{
    		switch(in){
	    	case 1:
				if(!classroom.isExisted(deleteRow))
					return "unexisted"; //modify
				writeLogToMemory(input,deleteRow,xid,LSN,LogEntry.DELETE);
	    		System.out.println("Deleting Record from CLASSROOM succeeds.");
	    		classroom.deleteRow(deleteRow); //modify
	    		break;
	    	case 2:
				if(!lab.isExisted(deleteRow))
					return "unexisted"; //modify
				writeLogToMemory(input,deleteRow,xid,LSN,LogEntry.DELETE);
	    		System.out.println("Deleting Record from LAB succeeds.");
	    		lab.deleteRow(deleteRow);
	    		break;
	    	case 3:
				if(!tool.isExisted(deleteRow))
					return "unexisted"; //modify
				writeLogToMemory(input,deleteRow,xid,LSN,LogEntry.DELETE);
	    		System.out.println("Deleting Record from TOOL succeeds.");
	    		tool.deleteRow(deleteRow);
	    		break;
	    	case 4:
				if(!people.isExisted(deleteRow))
					return "unexisted"; //modify
				writeLogToMemory(input,deleteRow,xid,LSN,LogEntry.DELETE);
	    		System.out.println("Deleting Record from PEOPEL succeeds.");
	    		people.deleteRow(deleteRow);
	    		break;
	    	case 5:
				if(!reservation.isExisted(deleteRow))
					return "unexisted"; //modify
				writeLogToMemory(input,deleteRow,xid,LSN,LogEntry.DELETE);
	    		System.out.println("Deleting Record from RESERVATION succeeds.");
	    		reservation.deleteRow(deleteRow);
	    		break;
	    		default:
	    			System.out.println("Switch error in deleteTable method of DC!");
	    			break;
	    	}
    		flag = true;
    	}
    	catch(RemoteException e){
    		flag = false;
    		System.out.println("Caught RemoteException between TC and DC: " + e.getMessage());
    	}
       	LogEntry logEntry = htLog.get(LSN);
		if(flag == true){
			
			logEntry.setCommit_YN(LogEntry.YES);
			return logEntry.serializableToString();
		}
		else{
			rollback(input,logEntry,LogEntry.DELETE);
			return "fail";
		}
    }
    
    public String restoreDB(String input) throws Exception{
    	ArrayList<LogEntry> restoreLog = new ArrayList<LogEntry>();
    	String[] logEntry = input.split(" ");
    	for(int i = 0; i < logEntry.length; i++){
    		LogEntry le = new LogEntry();
    		le.deserializableToLogEntry(logEntry[i]);
    		restoreLog.add(le);
    	}
    	//sort restoreLog based on LSN
    	Collections.sort(restoreLog,new LogEntryComparator());
    	//System.out.println("Sorted");
    	for(int i = 0; i < restoreLog.size(); i++)
    		redo(restoreLog.get(i));
    	return "";
    }
    
    public static class LogEntryComparator implements Comparator<LogEntry> {
        @Override
        public int compare(LogEntry o1, LogEntry o2) {
            if (o1.getLSN() > o2.getLSN())
				return 1;
			else
				return -1;
        }
    }
    
    public void redo(LogEntry logEntry) throws AmazonServiceException, AmazonClientException, IOException{
    	String[] fields = logEntry.getAfterImage().split(",");
		String primaryKey = fields[0];
		ArrayList<String> columns = new ArrayList<String>();
		for(int i = 1; i < fields.length; i++)
			columns.add(fields[i]);
		switch(logEntry.getType_of_operation()){
		case LogEntry.INSERT:
			//REDO s3	restore s3 = add afterImage
			switch(logEntry.getTableName()){
	    	case 1:
	    		classroom.addRow(primaryKey,columns);
	    		break;
	    	case 2:
	    		lab.addRow(primaryKey,columns);
	    		break;
	    	case 3:
	    		tool.addRow(primaryKey,columns);
	    		break;
	    	case 4:
	    		people.addRow(primaryKey,columns);
	    		break;
	    	case 5:
	    		reservation.addRow(primaryKey,columns);
	    		break;
	    		default:
	    			System.out.println("");
	    			break;  
	    	}
			break;
		case LogEntry.UPDATE:
			//REDO s3	restore s3 = update the new row to original row
			switch(logEntry.getTableName()){
	    	case 1:
	    		classroom.updateRow(fields[0], columns);
				break;
	    	case 2:
	    		lab.updateRow(fields[0], columns);
	    		break;
	    	case 3:
	    		tool.updateRow(fields[0], columns);
	    		break;
	    	case 4:
	    		people.updateRow(fields[0], columns);
	    		break;
	    	case 5:
	    		reservation.updateRow(fields[0], columns);
	    		break;
	    		default:
	    			System.out.println("");
	    			break;  
	    	}
			break;
		case LogEntry.DELETE:
			//REDO s3	restore s3 = delete beforeImage
			fields = logEntry.getBeforeImage().split(",");
			primaryKey = fields[0];
			columns = new ArrayList<String>();
			for(int i = 1; i < fields.length; i++)
				columns.add(fields[i]);
			switch(logEntry.getTableName()){
	    	case 1:
	    		classroom.deleteRow(fields[0]);
				break;
	    	case 2:
	    		lab.deleteRow(fields[0]);
	    		break;
	    	case 3:
	    		tool.deleteRow(fields[0]);
	    		break;
	    	case 4:
	    		people.deleteRow(fields[0]);
	    		break;
	    	case 5:
	    		reservation.deleteRow(fields[0]);
	    		break;
	    		default:
	    			System.out.println("");
	    			break;  
	    	}
			break;
			default:
				break;
		}
    }
	@Override
	public String receiveLsn(int LSN) throws RemoteException, Exception {
		System.out.println("Previous LSN: " + previousLSN);
		System.out.println("Current LSN: " + currentLSN);
		System.out.println("receive LSN: " + LSN);
		System.out.println();
		String result = "";// serializblehtLog();
		String log = "";
		
		List<Integer> keyList = new ArrayList<Integer>();

		previousLSN = currentLSN;
		currentLSN = LSN;
		
		for (Map.Entry<Integer, LogEntry> entry : htLog.entrySet()) {
			// If is there entry on the lockTable which has this transaction id?
			if( entry.getKey() <= currentLSN && (entry.getValue().getCommit_YN() == LogEntry.YES) ){
				result += entry.getValue().serializableToString() + "^";
				System.out.println("[DC] return LSN: " +entry.getKey());
			}
		}
		
		for (Map.Entry<Integer, LogEntry> entry : htLog.entrySet()) {
			// If is there entry on the lockTable which has this transaction id?
			if(entry.getKey() <= previousLSN){
				keyList.add(entry.getKey());
				System.out.println("[DC] remove LSN: " +entry.getKey());
			}
		}
		
		for (Integer key : keyList) {
			htLog.remove(key);
		}

		return result;
	}
	
	public String serializblehtLog(){
		String delimiter = "^";
		String result = "";
		if(htLog.size()!=0){
            Set<Integer> s = htLog.keySet();
            Iterator<Integer> it = s.iterator();
            while (it.hasNext())
                result += htLog.get(it.next()).serializableToString() + delimiter;
        }
		
		//System.out.println("Logentry returned: " + result);
		return result;
	}
    
    public void writeLogToMemory(String input, String newRow, int xid, int LSN, int type_of_operation){
    	LogEntry logEntry = new LogEntry();
    	//set LSN
    	logEntry.setLSN(LSN);
    	//set commit_YN
    	logEntry.setCommit_YN(LogEntry.NO);
    	//set xid
    	logEntry.setXid(xid);
    	//set type_of_operation
    	logEntry.setType_of_operation(type_of_operation);
    	//set tableName
    	String[] fields = newRow.split(",");
    	String primaryKey = fields[0];
    	String originalRow = "";
    	int in = Integer.parseInt(input);
    	switch(in){
    	case 1:
    		logEntry.setTableName(LogEntry.CLASSROOM);
    		if(type_of_operation != LogEntry.INSERT)
    			originalRow = classroom.getRow(primaryKey);
    		break;
    	case 2:
    		logEntry.setTableName(LogEntry.LAB);
    		if(type_of_operation != LogEntry.INSERT)
    			originalRow = lab.getRow(primaryKey);
    		break;
    	case 3:
    		logEntry.setTableName(LogEntry.TOOL);
    		if(type_of_operation != LogEntry.INSERT)
    			originalRow = tool.getRow(primaryKey);
    		break;
    	case 4:
    		logEntry.setTableName(LogEntry.PEOPLE);
    		if(type_of_operation != LogEntry.INSERT)
    			originalRow = people.getRow(primaryKey);
    		break;
    	case 5:
    		logEntry.setTableName(LogEntry.RESERVATION);
    		if(type_of_operation != LogEntry.INSERT)
    			originalRow = reservation.getRow(primaryKey);
    		break;
    		default:
    			System.out.println("");
    			break;
    	}
    	/**
    	 * set num_before_record
    	 * set num_after_record
    	 * set beforeImage
    	 * set afterImage
    	 */
    	switch(type_of_operation){
    	case LogEntry.INSERT:
    		logEntry.setNum_before_record(0);
    		logEntry.setNum_after_record(1);
    		logEntry.setBeforeImage(logEntry.empty_delimiter);
    		logEntry.setAfterImage(newRow);
    		break;
    	case LogEntry.UPDATE:
    		logEntry.setNum_before_record(1);
    		logEntry.setNum_after_record(1);
    		logEntry.setBeforeImage(originalRow);
    		logEntry.setAfterImage(newRow);
    		break;	
    	case LogEntry.DELETE:
    		logEntry.setNum_before_record(1);
    		logEntry.setNum_after_record(0);
    		logEntry.setBeforeImage(originalRow);
    		logEntry.setAfterImage(logEntry.empty_delimiter);
    		break;		
    		default:{
    			System.out.println();
    			break;
    		}
    	}
    	htLog.put(LSN, logEntry);
    }

    public void rollback(String input, LogEntry logEntry, int type_of_operation) throws AmazonServiceException, AmazonClientException, IOException{
    	System.out.println("Before rollback:");
    	listAllTables();
		String[] fields = logEntry.getAfterImage().split(",");
		String primaryKey = fields[0];
		int in = Integer.parseInt(input);
		ArrayList<String> columns = new ArrayList<String>();
		for(int i = 1; i < fields.length; i++)
			columns.add(fields[i]);
		switch(type_of_operation){
		case LogEntry.INSERT:
			//UNDO s3	restore s3 = delete afterImage
			switch(in){
	    	case 1:
	    		classroom.deleteRow(primaryKey);
	    		break;
	    	case 2:
	    		lab.deleteRow(primaryKey);
	    		break;
	    	case 3:
	    		tool.deleteRow(primaryKey);
	    		break;
	    	case 4:
	    		people.deleteRow(primaryKey);
	    		break;
	    	case 5:
	    		reservation.deleteRow(primaryKey);
	    		break;
	    		default:
	    			System.out.println("");
	    			break;  
	    	}
			break;
		case LogEntry.UPDATE:
			//UNDO s3	restore s3 = update the new row to original row
			switch(in){
	    	case 1:
	    		classroom.updateRow(fields[0], columns);
				break;
	    	case 2:
	    		lab.updateRow(fields[0], columns);
	    		break;
	    	case 3:
	    		tool.updateRow(fields[0], columns);
	    		break;
	    	case 4:
	    		people.updateRow(fields[0], columns);
	    		break;
	    	case 5:
	    		reservation.updateRow(fields[0], columns);
	    		break;
	    		default:
	    			System.out.println("");
	    			break;  
	    	}
			break;
		case LogEntry.DELETE:
			//UNDO s3	restore s3 = insert beforeImage
			switch(in){
	    	case 1:
	    		classroom.addRow(fields[0], columns);
				break;
	    	case 2:
	    		lab.addRow(fields[0], columns);
	    		break;
	    	case 3:
	    		tool.addRow(fields[0], columns);
	    		break;
	    	case 4:
	    		people.addRow(fields[0], columns);
	    		break;
	    	case 5:
	    		reservation.addRow(fields[0], columns);
	    		break;
	    		default:
	    			System.out.println("");
	    			break;  
	    	}
			break;
			default:
				break;
		}
		System.out.println("After rollback: ");
		listAllTables();
    }
    
	@Override
	public String clearTable() throws RemoteException, Exception {
		classroom.clearTable();
		lab.clearTable();
		tool.clearTable();
		people.clearTable();
		reservation.clearTable();
		return "Succeed to delete all the tables!";
	}
	
    public void listAllTables(){
    	System.out.println("The state of database is \n");
    	System.out.println("CLASSROOM: ");
    	classroom.printTable();
    	System.out.println();
    	System.out.println("LAB: ");
    	lab.printTable();
    	System.out.println();
    	System.out.println("TOOL: ");
    	tool.printTable();
    	System.out.println();
    	System.out.println("PEOPLE: ");
    	people.printTable();
    	System.out.println();
    	System.out.println("Reservation: ");
    	reservation.printTable();
    	System.out.println("\n");
    }
    
    public static void main(String args[]) {

	try {
	    DCServer obj = new DCServer();
	    DC stub = (DC) UnicastRemoteObject.exportObject(obj, 0);

	    // Bind the remote object's stub in the registry
	    Registry registry = LocateRegistry.getRegistry(1100);
	    registry.bind("DC", stub);

	    System.err.println("DCServer is ready");
	} catch (Exception e) {
	    System.err.println("Server exception: " + e.toString());
	    e.printStackTrace();
	}
    }


}
