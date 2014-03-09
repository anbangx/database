package ics.uci.edu.logmanager;

public class LogEntry {
	/**
	 * 	YES = 1
	 *  NO = 0
	 *  
	 *  type_of_operation: 0 = non-operation
	 *  				   1 = search
	 *  				   2 = insert
	 *  				   3 = update
	 *  				   4 = delete
	 *  tableName: 0 = non-table
	 *  		   1 = CLASSROOM
	 *  		   2 = LAB
	 *  		   3 = TOOL
	 *  		   4 = PEOPLE
	 *  		   5 = RESERVATION
	 */
	public final static String delimiter = "`";
	public final static String empty_delimiter = "/";
	public final static int YES = 1;
	public final static int NO = 0;
	public final static int NONOPERATION = 0;
	public final static int SEARCH = 1;
	public final static int INSERT = 2;
	public final static int UPDATE = 3;
	public final static int DELETE = 4;
	public final static int NONTABLE = 0;
	public final static int CLASSROOM = 1;
	public final static int LAB = 2;
	public final static int TOOL = 3;
	public final static int PEOPLE = 4;
	public final static int RESERVATION = 5;
	private int LSN;
	private int commit_YN;
	private int checkpoint_YN;
	private int xid;
	private int type_of_operation;
	private int tableName;
	private int num_before_record;
	private int num_after_record;
	private String beforeImage;
	private String afterImage;
	
	public LogEntry() {
		LSN = 0;
		commit_YN = 0;
		checkpoint_YN = 0;
		xid = 0;
		type_of_operation = NONOPERATION;
		tableName = NONTABLE;
		num_before_record = 0;
		num_after_record = 0;
		beforeImage = null;
		afterImage = null;
	}
	public String serializableToString(){
		String result = "";
		result += Integer.toString(LSN);
		result += delimiter;
		result += Integer.toString(commit_YN);
		result += delimiter;
		result += Integer.toString(checkpoint_YN);
		result += delimiter;
		result += Integer.toString(xid);
		result += delimiter;
		result += Integer.toString(type_of_operation);
		result += delimiter;
		result += Integer.toString(tableName);
		result += delimiter;
		result += Integer.toString(num_before_record);
		result += delimiter;
		result += Integer.toString(num_after_record);
		result += delimiter;
		result += beforeImage;
		result += delimiter;
		result += afterImage;
		result += delimiter;
		
		return result;
	}
	public void deserializableToLogEntry(String result){
		String[] fields = result.split(delimiter);
		this.setLSN(Integer.parseInt(fields[0]));
		this.setCommit_YN(Integer.parseInt(fields[1]));
		this.setCheckpoint_YN(Integer.parseInt(fields[2]));
		this.setXid(Integer.parseInt(fields[3]));
		this.setType_of_operation(Integer.parseInt(fields[4]));
		this.setTableName(Integer.parseInt(fields[5]));
		this.setNum_before_record(Integer.parseInt(fields[6]));
		this.setNum_after_record(Integer.parseInt(fields[7]));
		this.setBeforeImage(fields[8]);
		this.setAfterImage(fields[9]);
	}
	public int getLSN() {
		return LSN;
	}
	public void setLSN(int lSN) {
		LSN = lSN;
	}
	public int getCommit_YN() {
		return commit_YN;
	}
	public void setCommit_YN(int commit_YN) {
		this.commit_YN = commit_YN;
	}
	public int getCheckpoint_YN() {
		return checkpoint_YN;
	}
	public void setCheckpoint_YN(int checkpoint_YN) {
		this.checkpoint_YN = checkpoint_YN;
	}
	public int getXid() {
		return xid;
	}
	public void setXid(int xid) {
		this.xid = xid;
	}
	public int getType_of_operation() {
		return type_of_operation;
	}
	public void setType_of_operation(int type_of_operation) {
		this.type_of_operation = type_of_operation;
	}
	public int getTableName() {
		return tableName;
	}
	public void setTableName(int tableName) {
		this.tableName = tableName;
	}
	public int getNum_before_record() {
		return num_before_record;
	}
	public void setNum_before_record(int num_before_record) {
		this.num_before_record = num_before_record;
	}
	public int getNum_after_record() {
		return num_after_record;
	}
	public void setNum_after_record(int num_after_record) {
		this.num_after_record = num_after_record;
	}
	public String getBeforeImage() {
		return beforeImage;
	}
	public void setBeforeImage(String beforeImage) {
		this.beforeImage = beforeImage;
	}
	public String getAfterImage() {
		return afterImage;
	}
	public void setAfterImage(String afterImage) {
		this.afterImage = afterImage;
	}
	
	
}
