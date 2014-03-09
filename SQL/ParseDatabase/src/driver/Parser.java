package driver;

import java.io.BufferedReader;
import java.io.FileReader;

import data.Department;
import data.Employee;
import data.Manager;
import data.Records;
import data.Salary;

public class Parser {
	
	public Records parseCSV(String fileName) throws Exception {
		Records records = new Records();

		String line = "";
		String seperator = ",";
		BufferedReader br = new BufferedReader(new FileReader(fileName));
		while ((line = br.readLine()) != null) {
			String[] columns = line.split(seperator);
			switch (columns.length) {
			case 2: // department
				records.addDepartment(new Department(columns));
				break;
			case 8: // Emplyee
				records.addEmployee(new Employee(columns));
				break;
			case 10: // Manager
				records.addManager(new Manager(columns));
				break;
			case 4: // Salary
				records.addSalary(new Salary(columns));
				break;
			default: {
				throw new Exception(
						"Input .csv file has incorrect format, the columns length is not 2,8,10,4!");
				}
			}
		}
		br.close();
		
		return records;
	}
	
}	
