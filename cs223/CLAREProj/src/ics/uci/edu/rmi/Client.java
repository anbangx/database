package ics.uci.edu.rmi;

import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.rmi.NotBoundException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class Client {

	public static String TCIP = "169.234.134.220"; 
	public static String DC1IP = "169.234.134.226";
	public static String DC2IP = "127.0.0.1";
    private Client() {
    	
    }

    public static void main(String[] args) {

		String host = (args.length < 1) ? null : args[0];
		boolean showLogo = true;
		String input = "";
		
		while(true){
			try {
			
				showMenu(showLogo);
				showLogo = false;
				input = receiveOperation_mainMenu();
				
				if (input.equals("1")) listTable();
				else if (input.equals("2")) insertTable();
				else if (input.equals("3")) updateTable();
				else if (input.equals("4")) deleteTable();
				else if (input.equals("5")) unlockTx();
				else if (input.equals("6")) {
					System.out.println("\033[2J\n"); //modify
				    Registry registry = LocateRegistry.getRegistry(TCIP, 1099);
				    TC stub = (TC) registry.lookup("TC");
				    String response = stub.callDC_RestoreDB();
				    System.out.println("Restore DB " + response);
				}
				else if (input.equals("7")) {
					System.out.println("\033[2J\n"); //modify
				    Registry registry = LocateRegistry.getRegistry(TCIP, 1099);
				    TC stub = (TC) registry.lookup("TC");
				    String response = stub.callDC_ClearTable();
				    System.out.println(response);
				}
				else if (input.equals("8"))  batchInsert();
				else if (input.equals("9")) return;
				
			} catch (Exception e) {
			    System.err.println("Client exception: " + e.toString());
			    e.printStackTrace();
			}
		}
    }
	
	public static void showMenu(boolean showLogo) {
	
		if (showLogo) {
			System.out.println("\033[2J\n"); //modify
			System.out.println("Welcome to CLARE database. \n--------------------------------\n\n");
		}
		System.out.println("");
		System.out.println("Available Data(Table) : CLASSROOM, LAB, TOOL, PEOPLE, or RESERVATION\n");
		System.out.println("1. Select");
		System.out.println("2. Insert ");
		System.out.println("3. Update ");
		System.out.println("4. Delete ");
		System.out.println("5. Unlock ");
		System.out.println("6. Restore DB");
		System.out.println("7. Clear Table");
		System.out.println("8. Batch Insert Test\n");
		System.out.println("9. Quit\n");
		
		System.out.println("Please Select an Operation (1-9) : ");
	}

	public static void listTable() throws IOException, NotBoundException {

		System.out.println("\033[2J\n"); //modify
		System.out.println("Select Table: CLASSROOM, LAB, TOOL, PEOPLE, or RESERVATION\n");
		System.out.println("1. Classroom");
		System.out.println("2. Lab");
		System.out.println("3. Tool");
		System.out.println("4. People");
		System.out.println("5. Reservation");
		System.out.println("6. [Demo] Fail to Unlock the S Lock on Classroom");
		System.out.println(); //modify
		
		System.out.println("Please Select an Operation (1-6) : ");
		
		String input = receiveOperation_list();
	    Registry registry = LocateRegistry.getRegistry(TCIP, 1099);
	    TC stub = (TC) registry.lookup("TC");
	    String response = stub.callDC_List(input);
	    System.out.println("\033[2J\n"); //modify
	    System.out.println(response);
	}
	
	public static void insertTable() throws IOException, NotBoundException {

		System.out.println("\033[2J\n"); //modify
		System.out.println("Select Table: CLASSROOM, LAB, TOOL, PEOPLE, or RESERVATION\n");
		System.out.println("1. Classroom");
		System.out.println("2. Lab");
		System.out.println("3. Tool");
		System.out.println("4. People");
		System.out.println("5. Reservation");
		System.out.println("6. [Demo] Fail to Send Msg From DC to TC on Classroom Table");
		System.out.println("7. [Demo] Fail to Write Log in TC on Classroom Table");
		System.out.println("8. [Demo] Fail to Unlock the X Lock on Classroom");
		System.out.println(); //modify
		System.out.println("Please Select an Operation (1-8) : ");
		
		String input = receiveOperation_insert();
		int in = Integer.parseInt(input);
		switch(in){
		case 1:
		case 6:
		case 7:
		case 8:
			System.out.println("\033[2J\n"); //modify
			System.out.println("CLASSROOM\n"); //modify
			System.out.println("Input what you want to insert based on this format -- classroomName,location");
			break;
		case 2:
			System.out.println("\033[2J\n"); //modify
			System.out.println("LAB\n"); //modify
			System.out.println("Input what you want to insert based on this format -- labName,location");
			break;
		case 3:
			System.out.println("\033[2J\n"); //modify
			System.out.println("TOOL\n"); //modify
			System.out.println("Input what you want to insert based on this format -- toolName,location");
			break;
		case 4:
			System.out.println("\033[2J\n"); //modify
			System.out.println("PEOPLE\n"); //modify
			System.out.println("Input what you want to insert based on this format -- name,title");
			break;
		case 5:
			System.out.println("\033[2J\n"); //modify
			System.out.println("RESERVATION\n"); //modify
			System.out.println("Input what you want to insert based on this format -- name,reserveType,reserveKey,startTime,endTime");
			break;
			default:
				break;
		}

		String insertRow = receiveInsertOperation(in);
		
	    Registry registry = LocateRegistry.getRegistry(TCIP, 1099);
	    TC stub = (TC) registry.lookup("TC");
	    String response = stub.callDC_Insert(input, insertRow);
	    System.out.println("\033[2J\n"); //modify
	    System.out.println(response); //modify
	}
	
	public static void updateTable() throws IOException, NotBoundException {

		System.out.println("\033[2J\n"); //modify
		System.out.println("Select Table: CLASSROOM, LAB, TOOL, PEOPLE, or RESERVATION\n");
		System.out.println("1. Classroom");
		System.out.println("2. Lab");
		System.out.println("3. Tool");
		System.out.println("4. People");
		System.out.println("5. Reservation");
		System.out.println(); //modify
		System.out.println("Please Select an Operation (1-5) : ");
		
		String input = receiveOperation();
		int in = Integer.parseInt(input);
		switch(in){
		case 1:
			System.out.println("\033[2J\n"); //modify
			System.out.println("CLASSROOM\n"); //modify
			System.out.println("Input what you want to update based on this format -- classroomName,location");
			break;
		case 2:
			System.out.println("\033[2J\n"); //modify
			System.out.println("LAB\n"); //modify
			System.out.println("Input what you want to update based on this format -- labName,location");
			break;
		case 3:
			System.out.println("\033[2J\n"); //modify
			System.out.println("TOOL\n"); //modify
			System.out.println("Input what you want to update based on this format -- toolName,location");
			break;
		case 4:
			System.out.println("\033[2J\n"); //modify
			System.out.println("PEOPLE\n"); //modify
			System.out.println("Input what you want to update based on this format -- name,title");
			break;
		case 5:
			System.out.println("\033[2J\n"); //modify
			System.out.println("RESERVATION\n"); //modify
			System.out.println("Input what you want to update based on this format -- name,reserveType,reserveKey,startTime,endTime");
			break;
			default:
				break;
		}

		String updateRow = receiveInsertOperation(in);
		
	    Registry registry = LocateRegistry.getRegistry(TCIP, 1099);
	    TC stub = (TC) registry.lookup("TC");
	    String response = stub.callDC_Update(input, updateRow);
	    System.out.println("\033[2J\n"); //modify
	    System.out.println(response); //modify
	}
	
	public static void deleteTable() throws IOException, NotBoundException {

		System.out.println("\033[2J\n"); //modify
		System.out.println("Select Table to delete Record from: CLASSROOM, LAB, TOOL, PEOPLE, or RESERVATION\n");
		System.out.println("1. Classroom");
		System.out.println("2. Lab");
		System.out.println("3. Tool");
		System.out.println("4. People");
		System.out.println("5. Reservation");
		System.out.println(); //modify
		System.out.println("Please Select an Operation (1-5) : ");
		
		String input = receiveOperation();
		int in = Integer.parseInt(input);
		switch(in){
		case 1:
			System.out.println("\033[2J\n"); //modify
			System.out.println("CLASSROOM\n"); //modify
			System.out.println("Input primary key of what you want to delete");
			break;
		case 2:
			System.out.println("\033[2J\n"); //modify
			System.out.println("LAB\n"); //modify
			System.out.println("Input primary key of what you want to delete");
			break;
		case 3:
			System.out.println("\033[2J\n"); //modify
			System.out.println("TOOL\n"); //modify
			System.out.println("Input primary key of what you want to delete");
			break;
		case 4:
			System.out.println("\033[2J\n"); //modify
			System.out.println("PEOPLE\n"); //modify
			System.out.println("Input primary key of what you want to delete");
			break;
		case 5:
			System.out.println("\033[2J\n"); //modify
			System.out.println("RESERVATION\n"); //modify
			System.out.println("Input primary key of what you want to delete");
			break;
			default:
				break;
		}

		String deleteRow = receiveDeleteOperation();
		
	    Registry registry = LocateRegistry.getRegistry(TCIP, 1099);
	    TC stub = (TC) registry.lookup("TC");
	    String response = stub.callDC_Delete(input, deleteRow);
	    System.out.println("\033[2J\n"); //modify
	    System.out.println(response); //modify
	}
	
	public static void batchInsert() throws IOException, Exception{
		System.out.println("\033[2J\n"); //modify
		System.out.println("Select Table: CLASSROOM, LAB, TOOL, PEOPLE, or RESERVATION\n");
		System.out.println("1. Classroom");
		System.out.println("2. Lab");
		System.out.println("3. Tool");
		System.out.println("4. People");
		System.out.println("5. Reservation");
		System.out.println(); //modify
		
		System.out.println("Please Select an Operation (1-5) : ");
		
		String input = receiveOperation();
		int in = Integer.parseInt(input);
		switch(in){
		case 1:
			System.out.println("\033[2J\n"); //modify
			System.out.println("CLASSROOM\n"); //modify
			System.out.println("How many records for insertion: ");
			break;
		case 2:
			System.out.println("\033[2J\n"); //modify
			System.out.println("LAB\n"); //modify
			System.out.println("How many records for insertion: ");
			break;
		case 3:
			System.out.println("\033[2J\n"); //modify
			System.out.println("TOOL\n"); //modify
			System.out.println("How many records for insertion: ");
			break;
		case 4:
			System.out.println("\033[2J\n"); //modify
			System.out.println("PEOPLE\n"); //modify
			System.out.println("How many records for insertion: ");
			break;
		case 5:
			System.out.println("\033[2J\n"); //modify
			System.out.println("RESERVATION\n"); //modify
			System.out.println("How many records for insertion: ");
			break;
			default:
				break;
		}
		String number = receiveDeleteOperation();
		
		System.out.println("\033[2J\n"); //modify
		System.out.println("Input the begin index: ");
		String beginIndex = receiveDeleteOperation();
	    Registry registry = LocateRegistry.getRegistry(TCIP, 1099);
	    TC stub = (TC) registry.lookup("TC");
		
		int num = Integer.parseInt(number);
		int begin = Integer.parseInt(beginIndex);
		String insertRow = "";
		for(int i = 0; i < num; i++){
			insertRow = "";
			if(in == 5)
				insertRow = Integer.toString(begin + i) + "," + Integer.toString(begin + i) + "," + 
				Integer.toString(begin + i) + "," + Integer.toString(begin + i) + "," + 
				Integer.toString(begin + i);
			else
				insertRow = Integer.toString(begin + i) + "," + Integer.toString(begin + i);
			String response = stub.callDC_Insert(input, insertRow);
			if(!response.equals("Insert " + insertRow + " successfully."))
				Thread.sleep(100);
			else
				Thread.sleep(50);
			System.out.println(response); //modify
		}
	}
	
	public static void unlockTx() throws IOException, NotBoundException {

		System.out.println("\033[2J\n"); //modify
		System.out.println("Input xid you want to unlock");

		String input = receiveOp();
		
	    Registry registry = LocateRegistry.getRegistry(TCIP, 1099);
	    TC stub = (TC) registry.lookup("TC");
	    boolean response = stub.unlockTX(Integer.parseInt(input));
	    System.out.println("Unlock: " + response);
	}
	
	public static String receiveOp() throws IOException {
		
		InputStreamReader keyin = new InputStreamReader(System.in);
	    BufferedReader in = new BufferedReader(keyin);
		String temp = "";

		temp = in.readLine();
		
		return temp;
		
	}
	
	public static String receiveOperation() throws IOException {
	
		InputStreamReader keyin = new InputStreamReader(System.in);
	    BufferedReader in = new BufferedReader(keyin);
		String temp = "";
		
		while (!(temp.equals("1")) && !(temp.equals("2")) && !(temp.equals("3")) && 
				!(temp.equals("4")) && !(temp.equals("5"))){
	              temp = in.readLine();
	              
	              if (!(temp.equals("1")) && !(temp.equals("2")) && !(temp.equals("3")) 
	            		  && !(temp.equals("4")) && !(temp.equals("5"))){
	                  System.out.println("You typed an incorrect operation. Please enter between 1-5 : ");
	              }
	          }
		
		return temp;
		
	}
	
	public static String receiveOperation_mainMenu() throws IOException {
		
		InputStreamReader keyin = new InputStreamReader(System.in);
	    BufferedReader in = new BufferedReader(keyin);
		String temp = "";
		
		while (!(temp.equals("1")) && !(temp.equals("2")) && !(temp.equals("3")) && 
				!(temp.equals("4")) && !(temp.equals("5")) && !(temp.equals("6")) &&
				!(temp.equals("7")) && !(temp.equals("8")) && !(temp.equals("9"))){
	              temp = in.readLine();
	              
	              if (!(temp.equals("1")) && !(temp.equals("2")) && !(temp.equals("3")) 
	            		  && !(temp.equals("4")) && !(temp.equals("5")) && !(temp.equals("6")) 
	            		  && !(temp.equals("7")) && !(temp.equals("8")) && !(temp.equals("9"))){
	                  System.out.println("You typed an incorrect operation. Please enter between 1-9 : ");
	              }
	          }
		
		return temp;
		
	}
	
	public static String receiveOperation_list() throws IOException {
		
		InputStreamReader keyin = new InputStreamReader(System.in);
	    BufferedReader in = new BufferedReader(keyin);
		String temp = "";
		
		while (!(temp.equals("1")) && !(temp.equals("2")) && !(temp.equals("3")) && 
				!(temp.equals("4")) && !(temp.equals("5")) && !(temp.equals("6"))){
	              temp = in.readLine();
	              
	              if (!(temp.equals("1")) && !(temp.equals("2")) && !(temp.equals("3")) 
	            		  && !(temp.equals("4")) && !(temp.equals("5")) && !(temp.equals("6"))){
	                  System.out.println("You typed an incorrect operation. Please enter between 1-6 : ");
	              }
	          }
		
		return temp;
		
	}
	
	public static String receiveOperation_insert() throws IOException {
		
		InputStreamReader keyin = new InputStreamReader(System.in);
	    BufferedReader in = new BufferedReader(keyin);
		String temp = "";
		
		while (!(temp.equals("1")) && !(temp.equals("2")) && !(temp.equals("3")) && 
				!(temp.equals("4")) && !(temp.equals("5")) && !(temp.equals("6")) &&
				!(temp.equals("7")) && !(temp.equals("8"))){
	              temp = in.readLine();
	              
	              if (!(temp.equals("1")) && !(temp.equals("2")) && !(temp.equals("3")) 
	            		  && !(temp.equals("4")) && !(temp.equals("5")) && !(temp.equals("6")) 
	            		  && !(temp.equals("7")) && !(temp.equals("8"))){
	                  System.out.println("You typed an incorrect operation. Please enter between 1-8 : ");
	              }
	          }
		
		return temp;
		
	}
	
	public static String receiveInsertOperation(int in) throws IOException {
		InputStreamReader keyin = new InputStreamReader(System.in);
	    BufferedReader reader = new BufferedReader(keyin);
		String insertRow = reader.readLine();
		
		if(in < 5 || in > 5){
			String[] inp = insertRow.split(",");
			if(inp.length != 2){
				System.out.println("Wrong input!! input: " +in);
				return "";
			}
		}
		else{
			String[] inp = insertRow.split(",");
			if(inp.length != 5 ){
				System.out.println("Wrong input in Reservation!!\n");
				return "";
			}
		}
		return insertRow;
	}
	
	public static String receiveDeleteOperation() throws IOException {
		InputStreamReader keyin = new InputStreamReader(System.in);
	    BufferedReader reader = new BufferedReader(keyin);
		String insertRow = reader.readLine();
		
		return insertRow;
	}
}
