package ics.uci.edu.rmi;

import ics.uci.edu.logmanager.LogManager;

import java.io.IOException;
import java.rmi.Remote;
import java.rmi.RemoteException;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;

public interface DC extends Remote {
	/**
	 * Add a room to s3
	 * @return
	 * @throws RemoteException
	 */

    String listTable(String input, int xid, int LSN) throws RemoteException;
    String insertTable(String input, String insertRow, int xid, int LSN) throws Exception;
    String updateTable(String input, String updateRow, int xid, int LSN) throws RemoteException, Exception;
    String deleteTable(String input, String deleteRow, int xid, int LSN) throws RemoteException, Exception;
    String clearTable() throws RemoteException, Exception;
    String receiveLsn(int LSN) throws RemoteException, Exception;
    String restoreDB(String input) throws RemoteException, Exception;
    String insertFail(String input, String insertRow, int xid, int LSN) throws Exception;
}

