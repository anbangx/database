package ics.uci.edu.rmi;

import java.io.IOException;
import java.rmi.Remote;
import java.rmi.RemoteException;


public interface TC extends Remote {
	/**
	 * Add a room to s3
	 * @return
	 * @throws RemoteException
	 */
	
    public String callDC_List(String input) throws RemoteException;
    public String callDC_Insert(String input, String insertRow) throws RemoteException;
    public String callDC_Update(String input, String updateRow) throws RemoteException;
    public String callDC_Delete(String input, String deleteRow) throws RemoteException;
    public String callDC_ClearTable() throws RemoteException, Exception;
    public boolean unlockTX(int xid) throws RemoteException;
    public String callDC_RestoreDB() throws RemoteException, Exception, IOException;
}