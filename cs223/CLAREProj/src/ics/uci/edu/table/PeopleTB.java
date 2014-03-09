package ics.uci.edu.table;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Table CLASSROOM 
 * @author anbangx
 */
public class PeopleTB extends S3TableImpl{

	private String tableName = "PEOPLE"; 
	private String columnsName = "Name Title";

	public PeopleTB() throws IOException{
		table = new HashMap<String, ArrayList<String>>();
		super.setTableName(tableName);
		super.setColumnsName(columnsName);
		readFromS3();
	}
}