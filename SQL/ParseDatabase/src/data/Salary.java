package data;

public class Salary {

	private String employeeId;
	private String startDate;
	private String endDate;
	private int salary;

	public Salary(String[] columns){
		this.employeeId = columns[0];
		this.startDate = columns[1];
		this.endDate = columns[2];
		this.salary = Integer.parseInt(columns[3]);
	}

	public String getEmployeeId() {
		return employeeId;
	}

	public void setEmployeeId(String employeeId) {
		this.employeeId = employeeId;
	}

	public String getStartDate() {
		return startDate;
	}

	public void setStartDate(String startDate) {
		this.startDate = startDate;
	}

	public String getEndDate() {
		return endDate;
	}

	public void setEndDate(String endDate) {
		this.endDate = endDate;
	}

	public int getSalary() {
		return salary;
	}

	public void setSalary(int salary) {
		this.salary = salary;
	}

}
