package data;

import java.util.ArrayList;

public class Records {
	
	public ArrayList<Department> departments;
	public ArrayList<Employee> employees;
	public ArrayList<Manager> managers;
	public ArrayList<Salary> salarys;
	
	public Records(){
		departments = new ArrayList<Department>();
		employees = new ArrayList<Employee>();
		managers = new ArrayList<Manager>();
		salarys = new ArrayList<Salary>();
	}
	
	public void addDepartment(Department department){
		departments.add(department);
	}
	
	public void addEmployee(Employee employee){
		employees.add(employee);
	}
	
	public void addManager(Manager manager){
		managers.add(manager);
	}
	
	public void addSalary(Salary salary){
		salarys.add(salary);
	}
}
