package ics.uci.edu.table;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Table RESERVATION
 * @author anbangx
 */
public class ReservationTB extends S3TableImpl {
	
	private String tableName = "RESERVATION";
	private String columnsName = "ReserveKey PeopleName ReserveType StartTime EndTime";
	
	public ReservationTB() throws IOException{
		table = new HashMap<String, ArrayList<String>>();
		super.setTableName(tableName);
		super.setColumnsName(columnsName);
		readFromS3();
	}
}
