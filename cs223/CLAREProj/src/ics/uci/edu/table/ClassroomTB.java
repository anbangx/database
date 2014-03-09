package ics.uci.edu.table;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Table CLASSROOM 
 * @author anbangx
 */
public class ClassroomTB extends S3TableImpl{

	private String tableName = "CLASSROOM"; 
	private String columnsName = "Name Location";

	public ClassroomTB() throws IOException{
		table = new HashMap<String, ArrayList<String>>();
		super.setTableName(tableName);
		super.setColumnsName(columnsName);
		readFromS3();
	}
}
