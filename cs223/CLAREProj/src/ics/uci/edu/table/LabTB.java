package ics.uci.edu.table;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Table CLASSROOM 
 * @author anbangx
 */
public class LabTB extends S3TableImpl{

	private String tableName = "LAB"; 
	private String columnsName = "Name Location";

	public LabTB() throws IOException{
		table = new HashMap<String, ArrayList<String>>();
		super.setTableName(tableName);
		super.setColumnsName(columnsName);
		readFromS3();
	}
}
