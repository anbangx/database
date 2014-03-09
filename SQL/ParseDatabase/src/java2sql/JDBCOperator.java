package java2sql;

//STEP 1. Import required packages
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;

import data.Department;
import data.Employee;
import data.Manager;
import data.Salary;

public class JDBCOperator {

	// JDBC driver name and database URL
	static final String JDBC_DRIVER = "com.mysql.jdbc.Driver";
	static String DB_URL = "jdbc:mysql://localhost/CODAZEN";

	// Database credentials
	private String USER;
	private String PASS;

	static Connection conn = null;

	public JDBCOperator(String user, String password) {
		this.USER = user;
		this.PASS = password;
	}

	public void start() {
		try {
			// STEP 2: Register JDBC driver
			Class.forName("com.mysql.jdbc.Driver");

			// STEP 3: Open a connection
			System.out.println("Connecting to a selected database...");
			conn = DriverManager.getConnection(DB_URL, USER, PASS);
			System.out.println("Connected database successfully...");

		} catch (SQLException se) {
			// Handle errors for JDBC
			se.printStackTrace();
		} catch (Exception e) {
			// Handle errors for Class.forName
			e.printStackTrace();
		} finally {
		}// end try
	}

	public void close() {
		// finally block used to close resources
		try {
			conn.close();
		} catch (SQLException se) {
		}// do nothing
		try {
			if (conn != null)
				conn.close();
		} catch (SQLException se) {
			se.printStackTrace();
		}// end finally try
		System.out.println("Goodbye!");
	}

	public void insertRecordsToDepartment(ArrayList<Department> departments) {
		Statement stmt = null;
		try {
			// STEP 4: Execute a query
			System.out.println("Inserting records into the table Department...");
			stmt = conn.createStatement();

			String sql;
			for (Department department : departments) {
				sql = "INSERT INTO Department " + "VALUES ('"
						+ department.getDepartmentId() + "', '"
						+ department.getDepartmentName() + "');";
				stmt.executeUpdate(sql);
			}

			System.out.println("Inserted records into the table Department...");
		} catch (SQLException se) {
			// Handle errors for JDBC
			se.printStackTrace();
		}
	}

	public void insertRecordsToEmplyee(ArrayList<Employee> employees) {
		Statement stmt = null;
		try {
			// STEP 4: Execute a query
			System.out.println("Inserting records into the table Employee...");
			stmt = conn.createStatement();

			String sql;
			for (Employee employee : employees) {
				sql = "INSERT INTO Employee " + "VALUES ('"
						+ employee.getEmployeeType() + "', '"
						+ employee.getEmployeeId() + "', '"
						+ employee.getFirstName() + "', '"
						+ employee.getLastName() + "', '"
						+ employee.getGender() + "', '"
						+ employee.getHireDate() + "', '"
						+ employee.getTerminationData() + "', '"
						+ employee.getDepartmentNumbers() + "');";
				stmt.executeUpdate(sql);
			}

			System.out.println("Inserted records into the table Employee...");
		} catch (SQLException se) {
			// Handle errors for JDBC
			se.printStackTrace();
		}
	}

	public void insertRecordsToManager(ArrayList<Manager> managers) {
		Statement stmt = null;
		try {
			// STEP 4: Execute a query
			System.out.println("Inserting records into the table Manager...");
			stmt = conn.createStatement();

			String sql;
			for (Manager manager : managers) {
				sql = "INSERT INTO Manager " + "VALUES ('"
						+ manager.getEmployeeType() + "', '"
						+ manager.getEmployeeId() + "', '"
						+ manager.getFirstName() + "', '"
						+ manager.getLastName() + "', '" + manager.getGender()
						+ "', '" + manager.getHireDate() + "', '"
						+ manager.getTerminationData() + "', '"
						+ manager.getDepartmentNumbers() + "', "
						+ manager.getAnnualBonus() + ", '"
						+ manager.getManagerStartDate() + "');";
				stmt.executeUpdate(sql);
			}

			System.out.println("Inserted records into the table Manager...");
		} catch (SQLException se) {
			// Handle errors for JDBC
			se.printStackTrace();
		}
	}

	public void insertRecordsToSalary(ArrayList<Salary> salarys) {
		Statement stmt = null;
		try {
			// STEP 4: Execute a query
			System.out.println("Inserting records into the table Salary...");
			stmt = conn.createStatement();

			String sql;
			for (Salary salary : salarys) {
				sql = "INSERT INTO Salary " + "VALUES ('"
						+ salary.getEmployeeId() + "', '"
						+ salary.getStartDate() + "', '"
						+ salary.getEmployeeId() + "', " + salary.getSalary()
						+ ");";
				stmt.executeUpdate(sql);
			}

			System.out.println("Inserted records into the table Salary...");
		} catch (SQLException se) {
			// Handle errors for JDBC
			se.printStackTrace();
		}
	}
}
