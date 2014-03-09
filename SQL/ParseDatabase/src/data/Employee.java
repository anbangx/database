package data;

public class Employee {

	private String employeeType;
	private String employeeId;
	private String firstName;
	private String lastName;
	private char gender;
	private String hireDate;
	private String terminationData;
	private String departmentNumbers;

	public Employee(String[] columns) {
		this.employeeType = columns[0];
		this.employeeId = columns[1];
		this.firstName = columns[2];
		this.lastName = columns[3];
		this.gender = columns[4].equals("MALE") ? 'M' : 'F';
		this.hireDate = columns[5];
		this.terminationData = columns[6];
		this.departmentNumbers = columns[7];
	}

	public String getEmployeeType() {
		return employeeType;
	}

	public void setEmployeeType(String employeeType) {
		this.employeeType = employeeType;
	}
	
	public String getEmployeeId() {
		return employeeId;
	}

	public void setEmployeeId(String employeeId) {
		this.employeeId = employeeId;
	}

	public String getFirstName() {
		return firstName;
	}

	public void setFirstName(String firstName) {
		this.firstName = firstName;
	}

	public String getLastName() {
		return lastName;
	}

	public void setLastName(String lastName) {
		this.lastName = lastName;
	}

	public char getGender() {
		return gender;
	}

	public void setGender(char gender) {
		this.gender = gender;
	}

	public String getHireDate() {
		return hireDate;
	}

	public void setHireDate(String hireDate) {
		this.hireDate = hireDate;
	}

	public String getTerminationData() {
		return terminationData;
	}

	public void setTerminationData(String terminationData) {
		this.terminationData = terminationData;
	}

	public String getDepartmentNumbers() {
		return departmentNumbers;
	}

	public void setDepartmentNumbers(String departmentNumbers) {
		this.departmentNumbers = departmentNumbers;
	}

}
