package ics.uci.edu.lockmanager;

import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.File;
import java.io.FileInputStream;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import org.apache.http.HttpEntityEnclosingRequest;
import java.util.Collection;
import java.util.Collections;
import java.util.TreeSet;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.SortedSet;
import java.util.List;
import java.util.ArrayList;


public class LockManager {

	// Create a Value(TreeSet) and make it to ThreadSafe
	public TreeSet<Integer> treeSet = new TreeSet<Integer>();

	// Assign LSN = 0
	public static int LSN = 0;

	//public static SortedSet synchronizedTreeSet = Collections.synchronizedSortedSet(treeSet);

    // Implement HashTable<Key, Value> to save a Lock. Structure = <String, Hashtable<Integer1, Integer2>>
	// String = key, Integer1 = xid, Integer2 = xid
	// Key = SLock + TableName or XLock + TableName, Value = Transaction ID
	public static Hashtable<String, Hashtable<Integer, Integer>> lockTable
    	    = new Hashtable<String, Hashtable<Integer, Integer>>();

	// Make a Lock object to make it Synchronized
	private static Object syncObj1 = new Object();

	// Define Read and Write mode of Lock. SLock=SharedLock, XLock=eXclusiveLock
	public static String SLock = "SLock";
	public static String XLock = "XLock";

	//Constructor
    public LockManager() {

		// Read LSN from LSN.log
		if (LSN == 0) {

			// Check whether LOG file exists or not. If not, create new LSN.log file and assign LSN to 1
            File LSNfile = new File("./LSN.log");
            if(!LSNfile.exists()) {
				try { BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter("./LSN.log"));
					  bufferedWriter.append("0" + "\n");
					  bufferedWriter.close();
					  LSN = 0;
					} catch(IOException e)
					  { System.err.println("Can't write LSN.log file."); }
		    }
			// If LSN.log exists, read LSN from the log file and assign LSN
			else {
				try{
						FileInputStream in = new FileInputStream("./LSN.log");
						BufferedReader br = new BufferedReader(new InputStreamReader(in));
						String oneLine = "";

						while((oneLine = br.readLine())!= null) {
							int temp = Integer.parseInt(oneLine);
							LSN = Math.max(temp, LSN);
						}
					} catch (FileNotFoundException e) {
						 e.printStackTrace();
					}	catch (IOException e) {
						 e.printStackTrace();
					}
			}


		}

	}


	// Lock method - check that requested Lock can be given, generates new LSN and returns new LSN
	// Return type : int, if LockManager can't give LSN to a transaction, it will return 0,
	// otherwise it will return new LSN
	public int lock(int xid, String tableName, int lockType) throws Exception {

	// lockMode = SLock or XLock
	String lockMode = "";
	String key = "";

	// 1 = Shared Lock,  2 = eXclusive Lock
	if (lockType == 1 )
		lockMode = SLock;
	else if (lockType == 2)
		lockMode = XLock;

	if (   !tableName.equals("1") && !tableName.equals("2")
		&& !tableName.equals("3")      && !tableName.equals("4") && !tableName.equals("5") ){
	 viewLockTable();
	 return 0;
	}
	else {
	// Combine tableName to lockMode to use as key. ex) SLockCLASSROOM, or XLockPEOPLE
	key = lockMode + tableName;
    }

	try {
	// Check Lock is already exists, if it is SLock, add transaction id into TreeSet
	// First Case : S Lock
	if (lockMode.equals(SLock) && !lockTable.containsKey(key) && !lockTable.containsKey(XLock + tableName))
	{
		// requestLockMode = SLock, Current Status of the Table : SLock (X), XLock (X)
		// So create new SLock entry and add transaction id into TreeSet

		// Allow only one thread accesses the lockTable
		synchronized (syncObj1) {
			lockTable.put(key,new Hashtable<Integer, Integer>());
			lockTable.get(key).put(new Integer(xid), new Integer(xid));

			// Increase LSN number and write to disk
			LSN++;
			try { BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter("./LSN.log"));
				  bufferedWriter.append(Integer.toString(LSN) + "\n");
				  bufferedWriter.close();
				} catch(IOException e)
				  { System.err.println("Can't write LSN.log file."); }
			viewLockTable();
			return LSN;
		}

	}
	else if (lockMode.equals(SLock) && lockTable.containsKey(key) && !lockTable.containsKey(XLock + tableName))
	{
		// requestLockMode = SLock, Current Status of the Table : SLock (O), XLock (X)
		// So just add one more shared Lock with xid to LockTable. After status : SLock (O), XLock (X)
		// Allow only one thread access the lockTable
		synchronized (syncObj1) {
			lockTable.get(key).put(new Integer(xid), new Integer(xid));

			// Increase LSN number and write to disk
			LSN++;
			try { BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter("./LSN.log"));
				  bufferedWriter.append(Integer.toString(LSN) + "\n");
				  bufferedWriter.close();
				} catch(IOException e)
				  { System.err.println("Can't write LSN.log file."); }
			viewLockTable();
			return LSN;

		}
	}
	else if ((lockMode.equals(SLock) && !lockTable.containsKey(key) && lockTable.containsKey(XLock + tableName)) ||
       	     (lockMode.equals(SLock) && lockTable.containsKey(key) && lockTable.containsKey(XLock + tableName))
	        )
	{
		// requestLockMode = SLock, Current Status of the Table : SLock (X), XLock (O)
		// So can't acquire S Lock. Return false

		// requestLockMode = SLock, Current Status of the Table : SLock (O), XLock (O)
		// So can't acquire S Lock. Return false
		// In practice, a Lock Table can't have SLock and XLock on a table at the same time.
		// However, this case is provided for the extreme case.
		viewLockTable();
		return 0;
	}

	// Second Case : X Lock
	else if (lockMode.equals(XLock) && !lockTable.containsKey(key) && !lockTable.containsKey(SLock + tableName))
	{
		// requestLockMode = XLock, Current Status of the Table : SLock (X), XLock (X)
		// So create new XLock entry and add transaction id into TreeSet
		// Allow only one thread access the lockTable
		synchronized (syncObj1) {
			lockTable.put(key, new Hashtable<Integer, Integer>());
			lockTable.get(key).put(new Integer(xid), new Integer(xid));

			// Increase LSN number and write to disk
			LSN++;
			try { BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter("./LSN.log"));
				  bufferedWriter.append(Integer.toString(LSN) + "\n");
				  bufferedWriter.close();
				} catch(IOException e)
				  { System.err.println("Can't write LSN.log file."); }
			viewLockTable();
			return LSN;

		}

	}
	else if ((lockMode.equals(XLock) && lockTable.containsKey(key) && !lockTable.containsKey(SLock + tableName)) ||
		     (lockMode.equals(XLock) && lockTable.containsKey(key) && lockTable.containsKey(SLock + tableName))
	        )
	{
        // requestLockMode = XLock, Current Status of the Table : SLock(X), XLock(O)
		// So can't create XLock.

        // requestLockMode = XLock, Current Status of the Table : SLock(O), XLock(O)
		// So can't create XLock.
		// In practice, a Lock Table can't have SLock and XLock on a table at the same time.
		// However, this case is provided for the extreme case.
		viewLockTable();
		return 0;
	}


     else if (lockMode.equals(XLock) && !lockTable.containsKey(key) && lockTable.containsKey(SLock + tableName)) {

	    // requestLockMode = XLock, Current Status of the Table : SLock(O), XLock(X)
		// So, check if only this transaction holds SLock on the table, then Lock Conversion(SLock -> XLock)
		// can happen. Else if another transactions also have SLock on the table, then can't convert SLock to XLock.

		// Determine whether only this transaction has SLock on the table
		if (lockTable.get(SLock + tableName).contains(xid) && lockTable.get(SLock + tableName).size() == 1)
		{
			// Allow only one thread access the lockTable
			synchronized (syncObj1) {
				// If only requesting transaction has SLock on table, convert SLock to XLock
				lockTable.put(key, new Hashtable<Integer, Integer>());
				lockTable.get(key).put(new Integer(xid), new Integer(xid));

				lockTable.get(SLock + tableName).clear();
				lockTable.remove(SLock + tableName);

				// Increase LSN number and write to disk
				LSN++;
				try { BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter("./LSN.log"));
					  bufferedWriter.append(Integer.toString(LSN) + "\n");
					  bufferedWriter.close();
					} catch(IOException e)
					  { System.err.println("Can't write LSN.log file."); }
				viewLockTable();
				return LSN;
			}
		}
		// If another transaction has SLock on the same table, can't convert SLock to XLock.
		else {	viewLockTable();
			return 0; }

	}


	} catch (Exception e) {
       System.out.println("An Lock Exception happened : " + e.getMessage());
	}
	viewLockTable();
	return 0;

	}





	// UNLOCK request
	public int unlock(int xid) throws Exception {

	try {

	List<String> keyList = new ArrayList<String>();

	// Allow only one process to access lockTable at a time
	synchronized (syncObj1) {
		for (Map.Entry<String,Hashtable<Integer, Integer>> entry : lockTable.entrySet()) {
			// If is there entry on the lockTable which has this transaction id?
			if (entry.getValue().containsKey(new Integer(xid))) {

				// Xid Hashtable size=1? So that this Lock entry can be removed? Add Lock to the deletion list
				if (entry.getValue().size() == 1) {
					entry.getValue().clear();
					keyList.add(entry.getKey());
				}
				// If not, just remove this transaction id from list
				else
					entry.getValue().remove(new Integer(xid));

			}
		}

		// Remove Lock from Lock Table
		for (String key : keyList) {
			lockTable.remove(key);
		}
		
		viewLockTable();
		
	}

	/*
	Set set = lockTable.entrySet();
    Iterator it = set.iterator();
    while (it.hasNext()) {
      Map.Entry entry = (Map.Entry) it.next();

	  // If is there entry on the lockTable which has this transaction id?
	  if (entry.getValue().containsKey(new Integer(xid))) {

		// TreeSet size=1? So that this Lock entry can be removed?
		if (entry.getValue().size() == 1) {

			entry.getValue().clear();
			lockTable.remove(entry.getKey());
		}
		// If not, just remove this transaction id from list
		else
			entry.getValue().remove(new Integer(xid));

	  }

	}
	*/

	return 1;

	} catch (Exception e) {
       System.out.println("An Lock Exception happened : " + e.getMessage());
	}

    return 0;

	}

	
	public void viewLockTable() throws Exception {
	
		synchronized (syncObj1) {
		
			System.out.println("\n\n========================================");
			System.out.println("Current Lock Table Status               ");				
			System.out.println("========================================");
			System.out.println("Lock Mode, Table, Transaction ID");	
			
			for (Map.Entry<String,Hashtable<Integer, Integer>> entry : lockTable.entrySet()) {
				// If is there entry on the lockTable which has this transaction id?
				
					String tmp = entry.getKey().replaceAll("1",", CLASSROOM table").replaceAll("2",", LAB table").replaceAll("3",", TOOL table").replaceAll("4",", PEOPLE table").replaceAll("5",", RESERVATION table");
					
					System.out.print(tmp);
					
				for (Map.Entry<Integer, Integer> entry1 : entry.getValue().entrySet()) {

					if (entry.getValue().size() > 0) 
						System.out.print(", " + entry1.getKey());

				}
				
				System.out.print("\n");
			}
			
				System.out.print("\n");
		}

	}

}
