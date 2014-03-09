package driver;

import java2sql.JDBCOperator;

import data.Records;

public class Driver {

	public static void main(String[] args) throws Exception {
		Parser ps = new Parser();
		String csvPath = "data/backend-developer-assessment-1390349080534.csv";
		Records records = ps.parseCSV(csvPath);

		String user = args[0];
		String password = args[1];
		JDBCOperator jdbc = new JDBCOperator(user, password);
		jdbc.start();
		jdbc.insertRecordsToDepartment(records.departments);
		jdbc.insertRecordsToEmplyee(records.employees);
		jdbc.insertRecordsToManager(records.managers);
		jdbc.insertRecordsToSalary(records.salarys);
		jdbc.close();
	}

}
