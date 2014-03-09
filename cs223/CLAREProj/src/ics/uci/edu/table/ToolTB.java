package ics.uci.edu.table;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Table CLASSROOM 
 * @author anbangx
 */
public class ToolTB extends S3TableImpl{

	private String tableName = "TOOL"; 
	private String columnsName = "Name Location";

	public ToolTB() throws IOException{
		table = new HashMap<String, ArrayList<String>>();
		super.setTableName(tableName);
		super.setColumnsName(columnsName);
		readFromS3();
	}
}