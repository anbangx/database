package ics.uci.edu.table;

import java.io.IOException;
import java.util.ArrayList;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;

public interface S3TableInterface {
	
	public boolean addRow(String primaryKey , ArrayList<String> columns) throws AmazonServiceException, AmazonClientException, IOException;
	
	public boolean deleteRow(String primaryKey) throws AmazonServiceException, AmazonClientException, IOException;
	
	public boolean updateRow(String primaryKey, ArrayList<String> columns) throws AmazonServiceException, AmazonClientException, IOException;
	
	public void reset();
	
	public boolean writeToS3() throws AmazonServiceException, AmazonClientException, IOException;
	
	public boolean readFromS3() throws IOException;
	
    public void clearTable() throws AmazonServiceException, AmazonClientException, IOException;
}
