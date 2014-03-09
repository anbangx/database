package ics.uci.edu.rmi;

import ics.uci.edu.table.ClassroomTB;

import java.io.IOException;
import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.Date;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.auth.ClasspathPropertiesFileCredentialsProvider;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.Bucket;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.ListObjectsRequest;
import com.amazonaws.services.s3.model.ObjectListing;
import com.amazonaws.services.s3.model.PutObjectRequest;
import com.amazonaws.services.s3.model.S3Object;
import com.amazonaws.services.s3.model.S3ObjectSummary;

public class DCServer implements DC {

    public DCServer() {}

    public String sayHello() {
	return "Hello, world! FROM DC";
    }

    public String operationS3() {
	return "This test message is generated from DC";
    }

    public String addClassroomDC(String info) throws IOException{

ClassroomTB classroom = new ClassroomTB();
		
		/**
		 * Test1: add a row
		 */
		long begin,end,time;
        Date beginDate = new Date();
        begin = beginDate.getTime();
		int n = 100;
		System.out.println("Test1 start -- add three rows:");
		System.out.println();

		for(int i = 0; i < n; i++){
			String className = "testClass" + i;
			ArrayList<String> classLocation = new ArrayList<String>();
			classLocation.add("testLocation" + i);
			classroom.addRow(className, classLocation);
		}
		System.out.println("Before write");
		System.out.println("After write");
        Date endDate = new Date();
        end = endDate.getTime();
        time = end - begin;
        System.out.println("Time: " + time);
		System.out.println("The table read from S3 is:");
		System.out.println("Before read");

		System.out.println("After read");
        endDate = new Date();
        end = endDate.getTime();
        time = end - begin;
        System.out.println("Time: " + time);
		System.out.println();
		classroom.printTable();
		System.out.println();
		
        endDate = new Date();
        end = endDate.getTime();
        time = end - begin;
        System.out.println("Time: " + time);       

		return "addClassroom() has been successful! This message is generated from DC";
    }

    public static void main(String args[]) {

	try {
	    DCServer obj = new DCServer();
	    DC stub = (DC) UnicastRemoteObject.exportObject(obj, 0);

	    // Bind the remote object's stub in the registry
	    Registry registry = LocateRegistry.getRegistry(1100);
	    registry.bind("DC", stub);

	    System.err.println("DCServer is ready");
	} catch (Exception e) {
	    System.err.println("Server exception: " + e.toString());
	    e.printStackTrace();
	}
    }
}
