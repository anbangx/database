package data;

public class Department {
	
	private int departmentId;
	private String departmentName;
	
	public Department(String[] columns){
		this.departmentId = Integer.parseInt(columns[0]);
		this.departmentName = columns[1];
	}

	public int getDepartmentId() {
		return departmentId;
	}

	public void setDepartmentId(int departmentId) {
		this.departmentId = departmentId;
	}

	public String getDepartmentName() {
		return departmentName;
	}

	public void setDepartmentName(String departmentName) {
		this.departmentName = departmentName;
	}
}
