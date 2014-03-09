package ics.uci.edu.table;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.DeleteObjectRequest;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.ObjectListing;
import com.amazonaws.services.s3.model.PutObjectRequest;
import com.amazonaws.services.s3.model.S3Object;
import com.amazonaws.services.s3.model.S3ObjectSummary;

public class S3TableImpl implements S3TableInterface {
	/**
	 *  This credentials provider implementation loads from two String -- accessKey and secretKey
	 */
	private AmazonS3 s3 = new AmazonS3Client(new BasicAWSCredentials("AKIAJTW5BOY6EXOGV2YQ","PDcnFYIf9Hdo9GsKTEjLXretZ3yEg4mRCDQKjxu6"));
	private String bucketName = "CLAREF";//""; RestoreDB|CLAREDatabase
	private String tableName;
	private String columnsName;
	
	protected Map<String, ArrayList<String>> table= new HashMap<String, ArrayList<String>>();
	
	@Override
	public boolean addRow(String primaryKey, ArrayList<String> columns) throws AmazonServiceException, AmazonClientException, IOException {
		if(table.containsKey(primaryKey)){
			System.out.println("Add failed!" + tableName + "." + primaryKey + " has already existed!");
			return false;
		}
		String path = bucketName + "/" + tableName;
		table.put(primaryKey, columns);
		
        String key = primaryKey;       	
    	for(int i = 0; i < columns.size(); i++)
    		key += "," + columns.get(i);

    	/**
         * Upload an object to bucket
         */
    	s3.putObject(new PutObjectRequest(path, key, createSampleFile()));
		return true;
	}
	
	@Override
	public boolean deleteRow(String primaryKey) throws AmazonServiceException, AmazonClientException, IOException{
		if(!table.containsKey(primaryKey)){
			System.out.println("Delete failed! " + tableName + "." + primaryKey + " doesn't exist!");
			return false;
		}
		String path = bucketName + "/" + tableName;
		/**
         * Delete an object to bucket
         */
		String key = primaryKey;
		ArrayList<String> tmp = table.get(primaryKey);
		for(int i = 0; i < tmp.size(); i++)
			key += "," + tmp.get(i);
		s3.deleteObject(new DeleteObjectRequest(path, key)); 
		
		table.remove(primaryKey);
		return true;
	}

	@Override
	public boolean updateRow(String primaryKey, ArrayList<String> columns) throws AmazonServiceException, AmazonClientException, IOException {
		if(!table.containsKey(primaryKey)){
			System.out.println("Update failed! " + tableName + "." + primaryKey + " doesn't exist!");
			return false;
		}
		System.out.println("Table Management: Update Now");
		deleteRow(primaryKey);
		addRow(primaryKey,columns);
		table.remove(primaryKey);
		table.put(primaryKey, columns);
		return true;
	}

	@Override
	public void reset() {
		// TODO Auto-generated method stub
		this.table = new HashMap<String, ArrayList<String>>();
	}

	@Override
	public boolean writeToS3() throws AmazonServiceException,
			AmazonClientException, IOException {
		clearTable();
		String path = "/" + tableName;
        String key;
        
        for(Map.Entry<String, ArrayList<String>> entry: table.entrySet()){
        	key = "";
        	key = entry.getKey();       	
        	if(key.compareTo(tableName + "/") == 0)
        		continue;
        	for(int i = 0; i < entry.getValue().size(); i++)
        		key += "," + entry.getValue().get(i);

        	/**
	         * Upload an object to bucket
	         */
        	s3.putObject(new PutObjectRequest(bucketName + path, key, createSampleFile()));
        }
        
		return true;
	}

	@Override
	public boolean readFromS3() throws IOException {
		reset();
		/**
		 * List objects in bucket
		 */
		ObjectListing objectListing = s3.listObjects(bucketName,tableName);
        if(objectListing.getObjectSummaries().size() == 1)
        	return true;
		String [] fields;
		int l = tableName.length() + 1;
		ArrayList<String> columns = null;
		for (S3ObjectSummary objectSummary : objectListing.getObjectSummaries()) {
			columns = new ArrayList<String>();
        	if(objectSummary.getKey().compareTo(tableName + "/") == 0)
        		continue;
			fields = objectSummary.getKey().split(",");
			for(int i = 1; i < fields.length; i++)
				columns.add(fields[i]);
			table.put(fields[0].substring(l), columns);
		}
		return false;
	}

	@Override
	public void clearTable() throws AmazonServiceException, AmazonClientException, IOException {
		// TODO Auto-generated method stub
		this.table = new HashMap<String, ArrayList<String>>();
		ObjectListing objectListing = s3.listObjects(bucketName,tableName);

		String [] fields;
		int l = tableName.length() + 1;
		String path = bucketName;
		//ArrayList<String> columns = null;
		for (S3ObjectSummary objectSummary : objectListing.getObjectSummaries()) {
			//columns = new ArrayList<String>();
        	if(objectSummary.getKey().compareTo(tableName + "/") == 0)
        		continue;
        	s3.deleteObject(new DeleteObjectRequest(path, objectSummary.getKey())); 
		}
	}
	
    public void printTable(){
    	System.out.println(tableName + ": \n");
    	if(table.size() == 0){
    		System.out.println("Empty!");
    		return;
    	}
    	String[] fields = columnsName.split(",");
    	for(int i = 0; i < fields.length; i++)
    		System.out.print(fields[i] + "\t");
    	System.out.println();
        for(Map.Entry<String, ArrayList<String>> entry: table.entrySet()){
        	for(int i = 0; i < entry.getValue().size(); i++)
        		System.out.print(entry.getKey() + "\t" + entry.getValue().get(i));
        	System.out.print("\n");
        }
    }
    
    public String serializeTable(){
    	String serializableTable = "";
    	serializableTable += tableName + ": \n";
    	if(table.size() == 0){
    		serializableTable += "Empty!";
    		return serializableTable;
    	}
    	String[] fields = columnsName.split(",");
    	for(int i = 0; i < fields.length; i++)
    		serializableTable += fields[i] + "\t";
    	serializableTable += "\n";
        for(Map.Entry<String, ArrayList<String>> entry: table.entrySet()){
        	serializableTable += entry.getKey();
        	for(int i = 0; i < entry.getValue().size(); i++)
        		serializableTable += "\t" + entry.getValue().get(i);
        	serializableTable += "\n";
        }
        return serializableTable;
    }
    
    public File createSampleFile() throws IOException {
    	File file = new File("tmpfile");
    	return file;
    }
    
    public String getRow(String primaryKey){
    	String row = primaryKey;
    	ArrayList<String> columns = table.get(primaryKey);
    	for(int i = 0; i < columns.size(); i++)
    		row += "," + columns.get(i);
    	return row;
    }
    
    //modify
    public boolean isExisted(String primaryKey){
		if(!table.containsKey(primaryKey)){
			System.out.println(tableName + "." + primaryKey + " doesn't exist!");
			return false;
		}
		
		return true;
    }
    
	public String getTableName() {
		return tableName;
	}

	public void setTableName(String tableName) {
		this.tableName = tableName;
	}

	public String getColumnsName() {
		return columnsName;
	}

	public void setColumnsName(String columnsName) {
		this.columnsName = columnsName;
	}

	public Map<String, ArrayList<String>> getTable() {
		return table;
	}

	public void setTable(Map<String, ArrayList<String>> table) {
		this.table = table;
	}    
}
