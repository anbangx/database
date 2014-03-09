package crawl.GPS;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;
import java.text.*;

import crawl.type.GPSStream;

public class GPS {
	
    static final double _eQuatorialEarthRadius = 6378.1370D;
    static final double _d2r = (Math.PI / 180D);

    // Calculate distance(Mile) between two coordinates(lat1,long1 to lat2,long2). lat = Latitude, long = Longitude 
    public static int HaversineInMile(double lat1, double long1, double lat2, double long2) {
        return (int) (1000D * HaversineInKM(lat1, long1, lat2, long2));
    }

    // Calculate distance(KiloMeter) between two coordinates(lat1,long1 to lat2,long2). lat = Latitude, long = Longitude 
    public static double HaversineInKM(double lat1, double long1, double lat2, double long2) {
        double dlong = (long2 - long1) * _d2r;
        double dlat = (lat2 - lat1) * _d2r;
        double a = Math.pow(Math.sin(dlat / 2D), 2D) + Math.cos(lat1 * _d2r) * Math.cos(lat2 * _d2r)
                * Math.pow(Math.sin(dlong / 2D), 2D);
        double c = 2D * Math.atan2(Math.sqrt(a), Math.sqrt(1D - a));
        double d = _eQuatorialEarthRadius * c;

        return d;
    }

    // Calculate distance(Meter) between two coordinates(lat1,long1 to lat2,long2). lat = Latitude, long = Longitude 
    public static double HaversineInM(double lat1, double long1, double lat2, double long2) {
        double dlong = (long2 - long1) * _d2r;
        double dlat = (lat2 - lat1) * _d2r;
        double a = Math.pow(Math.sin(dlat / 2D), 2D) + Math.cos(lat1 * _d2r) * Math.cos(lat2 * _d2r)
                * Math.pow(Math.sin(dlong / 2D), 2D);
        double c = 2D * Math.atan2(Math.sqrt(a), Math.sqrt(1D - a));
        double d = _eQuatorialEarthRadius * c;

        return d*1000;
    }

    
    public static GPSStream start(String date) {

    	// Declare Arraylist. Most important Array : clusterLatitude, clusterLongitude, recordDate and BasePoints
    	// clusterLatitude, clusterLongitude = Final Clustered GPSStream data after clustering.
    	// basePoints = BasePoints
    	// recordDate = Time of specific GPSStream Points
    	
    	// latitude, longitude = Original Raw Data
    	// distanceM = distance between two point
    	// distanceMfromBasePoint = distance between current base point to new point
    	// basePointsToRemove = Used for merging clusters.
    	// duration = Duration of a cluster(Base Point)
		ArrayList<Double> latitude = new ArrayList<Double>();
		ArrayList<Double> longitude = new ArrayList<Double>();
		ArrayList<Double> distanceM = new ArrayList<Double>();
		ArrayList<Double> distanceMfromBasePoint = new ArrayList<Double>();
		ArrayList<Double> clusterLatitude = new ArrayList<Double>();
		ArrayList<Double> clusterLongitude = new ArrayList<Double>();
	    ArrayList<Integer> basePoints = new ArrayList<Integer>();
	    ArrayList<Integer> basePointsToRemove = new ArrayList<Integer>();
	    ArrayList<Date> recordDate = new ArrayList<Date>();
	    ArrayList<Date> recordEndDate = new ArrayList<Date>();
	    ArrayList<Long> duration = new ArrayList<Long>();
	    
	    // recordCount = Record(Raw Data) Count on specific date
	    int fileLineCount = 0;
	    int recordCount = 0;
	    
	    // Data Format used in the Raw Data
	    SimpleDateFormat ft = new SimpleDateFormat ("yyyy-MM-ddHH:mm:ss");
	    SimpleDateFormat ft1 = new SimpleDateFormat ("yyyyMMdd");
	    
		// Specific Date to parse
	    String fileDate = date;//"20130603"
	    Date tmp1;
	    String fileNextDate = "";

	    // Calculate next date
		try {
		    	tmp1 = ft1.parse(fileDate);
		    	Calendar cal=Calendar.getInstance();
		    	cal.setTime(tmp1);
		    	cal.add(Calendar.DATE, +1);
		    	tmp1 = cal.getTime();
		    	fileNextDate = ft1.format(tmp1);
		} catch (ParseException e) {} 
	    
		File file = new File(fileDate + ".txt");
		File fileNDate = new File(fileNextDate + ".txt");	    	    
		
		System.out.println("1. Reading Raw Data from the file 1/2");
		// Use Scanner, read file and remove unnecessary text and save GPSStream coordinate and time 
		try {

			 fileLineCount = 0;
			 Scanner scanner = new Scanner(file);
	            while (scanner.hasNextLine()) {
	                String line = scanner.nextLine();
	                fileLineCount++;
	                if (fileLineCount > 1) {
	                	String lineGPS[] = line.split(",");
                        //	System.out.println("Lat:"+ lineGPS[1] + " Lon:" + lineGPS[2]);

	                	// Remove T, Z from Date String in the File data
	                	String test = lineGPS[0].replaceAll("T|Z","");

	                	Date t1, t2;
	                	String test1;
	                	
                		try {
	                	t1 = ft.parse(test);
	                	Calendar cal=Calendar.getInstance();
	                	cal.setTime(t1);
	                	cal.add(Calendar.HOUR, -7);
	                	t1 = cal.getTime();
	                	test1 = ft.format(t1);

	                	// Store the time of GPSStream Raw Data 
	                	test1 = test1.substring(0,10).replaceAll("-","");
	                	
	                	if(test1.equals(fileDate)) {
	                		recordCount++;
		                	// Store the time of GPSStream Raw Data 

	                		recordDate.add(t1);
		                	// Store the latitude, longitude of GPSStream Raw Data 
		                	latitude.add(Double.parseDouble(lineGPS[1]));
		                	longitude.add(Double.parseDouble(lineGPS[2]));
	                		}
                		
                		} catch (ParseException e) {} 

	                	// Check the date of raw data. If it's not equal to fileDate, do not parse it
	                	
	                }
	            }

            scanner.close();
	         
		} catch (FileNotFoundException e) {
            e.printStackTrace();
        }

		System.out.println("1-1. Reading Raw Data from the file 2/2");
		// Use Scanner, read file and remove unnecessary text and save GPSStream coordinate and time
		if(fileNDate.exists()){
			try {
				 fileLineCount = 0;
				 Scanner scanner = new Scanner(fileNDate);
		            while (scanner.hasNextLine()) {
		                String line = scanner.nextLine();
		                fileLineCount++;
		                if (fileLineCount > 1) {
		                	String lineGPS[] = line.split(",");
	                        //	System.out.println("Lat:"+ lineGPS[1] + " Lon:" + lineGPS[2]);
	
		                	// Remove T, Z from Date String in the File data
		                	String test = lineGPS[0].replaceAll("T|Z","");
	
		                	Date t1, t2;
		                	String test1;
		                	
	                		try {
		                	t1 = ft.parse(test);
		                	Calendar cal=Calendar.getInstance();
		                	cal.setTime(t1);
		                	cal.add(Calendar.HOUR, -7);
		                	t1 = cal.getTime();
		                	test1 = ft.format(t1);
	
		                	// Store the time of GPSStream Raw Data 
		                	test1 = test1.substring(0,10).replaceAll("-","");
		                	
		                	if(test1.equals(fileDate)) {
		                		recordCount++;
			                	// Store the time of GPSStream Raw Data 
	
		                		recordDate.add(t1);
			                	// Store the latitude, longitude of GPSStream Raw Data 
			                	latitude.add(Double.parseDouble(lineGPS[1]));
			                	longitude.add(Double.parseDouble(lineGPS[2]));
		                		}
	                		
	                		} catch (ParseException e) {} 
	
		                	// Check the date of raw data. If it's not equal to fileDate, do not parse it
		                	
		                }
		            }
	
	            scanner.close();
		         
			} catch (FileNotFoundException e) {
	            e.printStackTrace();
	        }
		}


		// Setting End Date to Start time of next Raw Data
		for (int i=0;i<recordCount-1;i++) {
			recordEndDate.add(recordDate.get(i+1)); 
		}

    	Calendar cal=Calendar.getInstance();
    	cal.setTime(recordDate.get(recordDate.size()-1));
    	cal.add(Calendar.MINUTE, +1);
		recordEndDate.add(cal.getTime());

		
		distanceM.add(0.0);
		distanceMfromBasePoint.add(0.0);

		clusterLatitude.add(latitude.get(0));
		clusterLongitude.add(longitude.get(0));

		// Threshold for one cluster(unit : meter)
		double cluster_threshold = 30.0;	
		
		// Threshold for between clusters(unit : meter)
		double interCluster_threshold = 160.0;	
		
		boolean cluster_found = false;

		// Initially, tentative base point=0
		int basePoint = 0;									
		
		basePoints.add(basePoint);
		
		// Check distance between two point, between current base point and new point
		// 
		System.out.println("\n2. Calculating clusters...");
		for (int i=1; i<recordCount; i++)
		{
			distanceM.add(HaversineInM(latitude.get(i-1), longitude.get(i-1), latitude.get(i), longitude.get(i)));
			distanceMfromBasePoint.add(HaversineInM(latitude.get(basePoint), longitude.get(basePoint), latitude.get(i), longitude.get(i)));

			// If distance between current base point and new point <= threshold, then add new point to cluster
			if (distanceMfromBasePoint.get(i) <= cluster_threshold)
			{
				cluster_found = true;
				clusterLatitude.add(clusterLatitude.get(i-1)); 
				clusterLongitude.add(clusterLongitude.get(i-1));
				System.out.println("Distance(Meter) from Base Point : " + distanceMfromBasePoint.get(i) + ". Base Point : " + basePoint + ". Adding " + i + " to this cluster." );
			}

			// If distance between current base point and new point > threshold, then start new base point
			else if (distanceMfromBasePoint.get(i) > cluster_threshold)
			{
				cluster_found = false;
				clusterLatitude.add(latitude.get(i)); 
				clusterLongitude.add(longitude.get(i));
				basePoint = i;
				basePoints.add(basePoint);
				System.out.println("Distance from Base Point : " + distanceMfromBasePoint.get(i) + ". Adding to current cluster failed. New tentative basePoint - " + basePoint );
			}
			
		}


		// Merging Clusters
		System.out.println("\n\n3. Merging Clusters...");
		for (int i=0;i<basePoints.size()-1;i++)
		{
			int currentBasePoint = basePoints.get(i);
			int nextBasePoint = basePoints.get(i+1);
			int nextOfNextBasePoint;
			if (i == basePoints.size()-2)
				nextOfNextBasePoint = recordCount;
			else
				nextOfNextBasePoint = basePoints.get(i+2);
				
			double distanceBetweenBasePoints = HaversineInM(latitude.get(currentBasePoint), longitude.get(currentBasePoint), latitude.get(nextBasePoint), longitude.get(nextBasePoint));
			System.out.println("Distance from Base Point " +  currentBasePoint + " to Base Point " + nextBasePoint + " is " + distanceBetweenBasePoints);
			
			if (distanceBetweenBasePoints <= interCluster_threshold) {
				basePointsToRemove.add(nextBasePoint);
				System.out.println("Merging Base Point " +  nextBasePoint + " to the Base Point " + currentBasePoint);
				for (int j=nextBasePoint; j<nextOfNextBasePoint; j++) {
					clusterLatitude.set(j, clusterLatitude.get(currentBasePoint));
					clusterLongitude.set(j, clusterLongitude.get(currentBasePoint));
				}
			}
		
		}

		// Remove old base point which were merged to previous base point
		for (int i=0; i<basePointsToRemove.size(); i++)
			basePoints.remove(basePointsToRemove.get(i));

		// Printing Base Points
		System.out.print("\n\n4. Printing Final Base Points, duration, time, GPSStream coordinate : \n");
		Date d;
		long diff;
		long diffMinutes;
		for (int i=0;i<basePoints.size();i++)
		{
			if(i < basePoints.size()-1) {
				diff = recordDate.get(basePoints.get(i+1)).getTime() - recordDate.get(basePoints.get(i)).getTime();
				diffMinutes = diff / (60 * 1000);
			}
			else {
				diff = recordDate.get(recordCount-1).getTime() - recordDate.get(basePoints.get(i)).getTime();
				diffMinutes = diff / (60 * 1000);
			}
				
			System.out.println(basePoints.get(i) + " - duration : " + diffMinutes + "min. " + clusterLatitude.get(basePoints.get(i)) + " " + clusterLongitude.get(basePoints.get(i)) + " " + recordDate.get(basePoints.get(i)));
			duration.add(diffMinutes);
		}
		System.out.println("");


		//Calculating End Time
		for (int i=0;i<basePoints.size();i++)
		{
	    	cal=Calendar.getInstance();
	    	cal.setTime(recordDate.get(basePoints.get(i)));
	    	cal.add(Calendar.MINUTE, Integer.valueOf(duration.get(i).toString()));
			recordEndDate.set(basePoints.get(i), cal.getTime());	
		}
		for (int i=0;i<basePoints.size()-1;i++)
		{
			int currentBasePoint = basePoints.get(i);
			int nextBasePoint = basePoints.get(i+1);
				
				for (int j=currentBasePoint; j<nextBasePoint; j++) {
					recordEndDate.set(j, recordEndDate.get(currentBasePoint));
				}
		}

		// Final Base Point
		System.out.println("5.Final Base Point. Printing only duration of BasePoint >= 5min");
		for (int i=0;i<basePoints.size();i++)
		{
			if (duration.get(i) >= 10)
				System.out.println(basePoints.get(i) + " - duration : " + duration.get(i) + "min. " + clusterLatitude.get(basePoints.get(i)) + " " + clusterLongitude.get(basePoints.get(i)) + "// From " + recordDate.get(basePoints.get(i))  + " // to " + recordEndDate.get(basePoints.get(i)));
			
		}

		
/*		for (int i=1; i<100; i++)
		{
			System.out.println( "Distance from point" + (i-1) + " to " + i + " : " + distanceM[i]);
		}
			
		for (int i=1; i<100; i++)
		{
			System.out.println( "Distance from point 0 to " + i + " : " + distanceMfromBasePoint[i]);
		}
*/			
		GPSStream gps = new GPSStream();
		gps.setBasePoints(basePoints);
		gps.setRecordDate(recordDate);
		gps.setRecordEndDate(recordEndDate);
		gps.setDuration(duration);
		gps.setClusterLatitude(clusterLatitude);
		gps.setClusterLongitude(clusterLongitude);
		gps.setLatitude(latitude);
		gps.setLongitude(longitude);
			
		return gps;
    }

}
