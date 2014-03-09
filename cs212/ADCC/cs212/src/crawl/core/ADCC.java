package crawl.core;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.InputStreamReader;

import crawl.GPS.GPS;
import crawl.calendar.CalendarEventFeedCrawl;
import crawl.facebook.FacebookCrawl;
import crawl.phone.CallCrawl;
import crawl.phone.PhotoCrawl;
import crawl.phone.SMSCrawl;
import crawl.type.Call;
import crawl.type.GPSStream;
import crawl.type.GoogleCalendar;
import crawl.type.FB;
import crawl.type.Photo;
import crawl.type.SMS;


import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

/*
import com.google.gdata.util.*;

import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
*/


import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;


public class ADCC {
	
	public static void main(String[] args){
		String queryDate = args[0];
		System.out.println("date: " + args[0]);
        String accessToken = "CAACEdEose0cBAPR6PMaRJjAWHUOmnaBHQTcuZBXJy31Q6wlGWhF2P3ZB8rJwW4YKgatl3vACY8EBs1HqAOWoInppPivc7lcKAQxEDWAx0iIbzhnAGtZBvsAJzKbMJ9pq3Vkx8HGSnZAEy54wnqHsMvcLPV98sxAZD";
		
	    GPSStream gps = GPS.start(queryDate);
	    
	    try{
	    	
		    BufferedWriter bw = new BufferedWriter(new FileWriter("./"+queryDate+".kml", false));
		    
		    String titleStr = "===== Anbang's Dairy on " + queryDate;
		    String preStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n<Document>\n<name>" + titleStr + "</name>\n";
		    String styleStr = "<Style id=\"iconstyle0\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/A.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle1\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/B.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle2\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/C.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle3\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/D.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle4\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/E.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle5\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/F.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle6\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/G.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle7\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/H.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle8\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/I.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle9\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/J.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle10\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/K.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle11\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/L.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle12\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/M.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle13\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/N.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle14\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/O.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle15\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/P.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle16\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/Q.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle17\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/R.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle18\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/S.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle19\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/T.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle20\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/U.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle21\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/V.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle22\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/W.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle23\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/X.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle24\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/Y.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"iconstyle25\">\n<IconStyle>\n<Icon>\n<href>http://www.google.com/mapfiles/kml/paddle/Z.png</href>\n</Icon>\n</IconStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle0\">\n<LineStyle>\n<width>5</width>\n<color>ff0033cc</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle1\">\n<LineStyle>\n<width>5</width>\n<color>ffff9933</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle2\">\n<LineStyle>\n<width>5</width>\n<color>ff333399</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle3\">\n<LineStyle>\n<width>5</width>\n<color>ffcccc66</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle4\">\n<LineStyle>\n<width>5</width>\n<color>ff9999ff</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle5\">\n<LineStyle>\n<width>5</width>\n<color>ff666600</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle6\">\n<LineStyle>\n<width>5</width>\n<color>ffcc99cc</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +

		    		"<Style id=\"linestyle7\">\n<LineStyle>\n<width>5</width>\n<color>ff99cc99</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle8\">\n<LineStyle>\n<width>5</width>\n<color>ff000066</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle9\">\n<LineStyle>\n<width>5</width>\n<color>ffffff99</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle10\">\n<LineStyle>\n<width>5</width>\n<color>ff6666cc</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle11\">\n<LineStyle>\n<width>5</width>\n<color>ff999933</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle12\">\n<LineStyle>\n<width>5</width>\n<color>ffcc99ff</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +		    		
		    		"<Style id=\"linestyle13\">\n<LineStyle>\n<width>5</width>\n<color>ffcc9900</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		
		    		"<Style id=\"linestyle14\">\n<LineStyle>\n<width>5</width>\n<color>ffccff99</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle15\">\n<LineStyle>\n<width>5</width>\n<color>ffffcc99</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle16\">\n<LineStyle>\n<width>5</width>\n<color>ffffcccc</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle17\">\n<LineStyle>\n<width>5</width>\n<color>ffff99cc</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle18\">\n<LineStyle>\n<width>5</width>\n<color>ffcc6699</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle19\">\n<LineStyle>\n<width>5</width>\n<color>ff993366</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle20\">\n<LineStyle>\n<width>5</width>\n<color>ff660033</color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n" +
		    		"<Style id=\"linestyle21\">\n<LineStyle>\n<width>5</width>\n<color></color>\n<colorMode>random</colorMode>\n</LineStyle>\n</Style>\n";

		    String postStr = "</Document>\n</kml>";
		    String spotPreStr1 = "<Placemark>\n<name>"; // + place name
		    String spotPreStr2 = "</name>\n<Snippet>"; // + time
		    String spotPreStr3 = "</Snippet>\n<description><![CDATA["; // + description(html)
		    String spotPreStr4 = "]]></description>\n<styleUrl>#iconstyle"; // + color
		    String spotPreStr5 = "</styleUrl>\n<Point>\n<coordinates>";
		    String spotPostStr = "</coordinates>\n</Point>\n</Placemark>\n";
		    String linePreStr1 = "<Placemark>\n<name>"; // + name
		    String linePreStr2 = "</name>\n<Snippet>"; // + time
		    String linePreStr3 = "</Snippet>\n<description><![CDATA["; // + description(html)
		    String linePreStr4 = "]]></description>\n<styleUrl>#linestyle"; // + color
		    String linePreStr5 = "</styleUrl>\n<LineString>\n<tessellate>1</tessellate>\n<coordinates>";
		    String linePostStr = "</coordinates>\n</LineString>\n</Placemark>\n";
		    
		    Boolean isMoving = false;
		    Double lng = 0.0;
		    Double lat = 0.0;
		    int color = 0;
		    int lineColor = 0;
		    int order = 0;
		    
		    bw.append(preStr+styleStr);
		    
    
		    for(int i = 0; i < gps.getBasePoints().size(); i++){
		    	if(gps.getDuration().get(i) < 10){
		    		
		    		String start = gps.getRecordDate().get(gps.getBasePoints().get(i)).toString().substring(11, gps.getRecordDate().get(gps.getBasePoints().get(i)).toString().indexOf("PDT"));
		    		String end = "";
		    		
		    		Date startD = gps.getRecordDate().get(gps.getBasePoints().get(i)); 
		            Date endD = gps.getRecordEndDate().get(gps.getBasePoints().get(i)); // temporal initializing
		    		
		    		for(int j = i; j < gps.getBasePoints().size(); j++){
		    			if(gps.getDuration().get(j) >= 10){
		    				endD = gps.getRecordDate().get(gps.getBasePoints().get(j));
		    				end = endD.toString().substring(11, gps.getRecordDate().get(gps.getBasePoints().get(j)).toString().indexOf("PDT"));;
		    				break;
		    			}
		    		}
		    		
		    		long durationMin = Math.round( (double)(endD.getTime() - startD.getTime()) / 60000.0 );
		    				
		    		if(!isMoving){ 
		    			isMoving = true;
		    			
		    			if(order == 0){ // in case gps data start with transitioning 
		    				bw.append(spotPreStr1 + "[P" + order + "] Starting Point (" + start + 
		    						", " + gps.getDuration().get(i) + "min)" + spotPreStr2 + spotPreStr3 + spotPreStr4 + (color%26) + spotPreStr5 + gps.getClusterLongitude().get(gps.getBasePoints().get(i)) + "," + 
		    	    		    			gps.getClusterLatitude().get(gps.getBasePoints().get(i)) + ",0" + spotPostStr);
		    				
		    				color++;		    				
		    			}
		    			
		    		 
			            //crawl google calendar
			            GoogleCalendar calendar = CalendarEventFeedCrawl.start("testcs212@gmail.com", "happy123456", startD, endD);
			            calendar.sort();
			            String calendarStr = "";
			            //GoogleCalendar tmpC = new GoogleCalendar();
			            			            
			            try{
				            for(int j = 0; j < calendar.size();){
				                
				            	if(calendar.getStartTime().get(j).compareTo(startD) < 0
				                        || calendar.getStartTime().get(j).compareTo(endD) > 0){
				                    calendar.remove(j);
				                }
				                else 
				                    j++;
//				                if(calendar.getStartTime().get(j).compareTo(startD) >= 0 
//				                        && calendar.getStartTime().get(j).compareTo(endD) <= 0){
//				                    System.out.print("");
//				                }
//				                    tmpCalendar.setActionLink(calendar.getActionLink());
//				                    tmpCalendar.setDescription(calendar.getDescription());
//				                    tmpCalendar.setEndTime(calendar.getEndTime());
//				                    tmpCalendar.setLocation(calendar.getLocation());
//				                    tmpCalendar.setStartTime(calendar.getStartTime());
//				                    tmpCalendar.setTitle(calendar.getTitle());
//				                }
				            }
//				            System.out.println("Google Calendar: ");
				            calendarStr = calendar.toHTML();
//				            calendarTitles = "\n" + calendar.getTitles();
//				            System.out.println(calendarTitles);
			            }catch(Exception e){
			            	System.out.println(e.getMessage());
			            }
			                        
				    		           
			            //crawl facebook
			            String fbStr = "";
			            
//			            Calendar cal=Calendar.getInstance();
//			            cal.setTime(startD);
//			            cal.add(Calendar.HOUR, 7);
//			            Date t1 = cal.getTime();
//			            cal.setTime(endD);
//			            cal.add(Calendar.HOUR, 7);
//			            Date t2 = cal.getTime();
			            FB fb = FacebookCrawl.start(accessToken, startD, endD);
			            fb.sort();
//			            System.out.println("Facebook: ");

			            fbStr = fb.toHTML();
//			            fbTitles = "\n" + fb.getTitles();
//			            System.out.println(fbTitles);

//			            fbStr = "fbStr";
//			            fbTitles = "fbTitle";

			           
			            //crawl phone
			            String phoneStr = "";
			            File dir = new File("src/Data/photo"); //transfer data from android phone to PC in this directory
			            Photo phone = PhotoCrawl.start(dir, startD, endD);
			            phone.sort();
			            phoneStr = phone.toHTML();
			            //phone.toString();
			            
			            System.out.println("Photo(Image/Video): ");
			            System.out.println(phone.toString());
				    	
			            //crawl message(SMS) from phone
			            String smsStr = "";
			            
			            SMS sms = SMSCrawl.start( "src/Data/SMS.csv", startD, endD);
			            sms.sort();
			            System.out.println("Message: ");
			            smsStr = sms.toHTML();
			            System.out.println(sms.toString());
			            //event.setSms(sms);
			            
			            //crawl call(SMS) from phone
			            String callStr = "";
			            
			            Call call = CallCrawl.start( "src/Data/call.csv", startD, endD);
			            call.sort();
			            callStr = call.toHTML();
			            System.out.println("Call: ");
			            System.out.println(call.toString());
			            
			            
			            String start_end_duraiton = "Start: " + startD.toString().substring(11, startD.toString().indexOf("PDT")) + 
			            		", End: " + endD.toString().substring(11, startD.toString().indexOf("PDT")) + ", Duration: " + durationMin + "min<br>";
			            
			            Boolean isFirst = true;

			            bw.append(linePreStr1 + "[Transitioning: P" + order + " -> P" + (order+1) + "] (" + start + ", " + durationMin + "min)" + linePreStr2);		    					
			            
			            if(calendar.size() > 0){
			            	bw.append("GC: " + calendar.size());
			            	isFirst = false;
			            }
			            
			            if(fb.size() > 0){
			            	if(isFirst){
			            		bw.append("FB: " + fb.size());
			            		isFirst = false;
			            	}
			            	else
			            		bw.append(", FB: " + fb.size());	            		
			            }
			            
			            if(sms.size() > 0){
			            	if(isFirst){
			            		bw.append("SMS: " + sms.size());
			            		isFirst = false;
			            	}
			            	else
			            		bw.append(", SMS: " + sms.size());	            			            	
			            }
			            
			            if(call.size() > 0){
			            	if(isFirst){
			            		bw.append("Call: " + call.size());
			            		isFirst = false;
			            	}
			            	else
			            		bw.append(", Call: " + call.size());	            			            	
			            }
			            
			            if(phone.size() > 0){
			            	if(isFirst){
			            		bw.append("Picture: " + phone.size());
			            		isFirst = false;
			            	}
			            	else
			            		bw.append(", Picture: " + phone.size());	            			            	
			            }
			            
			            bw.append(linePreStr3 + start_end_duraiton + "<br>");
			            if(calendarStr.length() > 0)
			            	bw.append(calendarStr + "<br>");
			            if(fbStr.length() > 0)
			            	bw.append(fbStr + "<br>");
			            if(smsStr.length() > 0)
			            	bw.append(smsStr + "<br>");
			            if(callStr.length() > 0)
			            	bw.append(callStr + "<br>");
			            if(phoneStr.length() > 0)
			            	bw.append(phoneStr + "<br>");
 
			            bw.append(linePreStr4 + (lineColor%20) + linePreStr5);
		    			
		    			lineColor++;
		    			
		    			if(lng != 0.0){
		    				bw.append(lng + "," + lat + ",0 ");
		    			}
		    			bw.append(gps.getClusterLongitude().get(gps.getBasePoints().get(i)) + "," + gps.getClusterLatitude().get(gps.getBasePoints().get(i)) + ",0 ");
		    		}
		    		else{ // is moving. only add a new gps data.
		    			bw.append(gps.getClusterLongitude().get(gps.getBasePoints().get(i)) + "," + gps.getClusterLatitude().get(gps.getBasePoints().get(i)) + ",0 ");
		    		}
		    		
		    		if(i == gps.getBasePoints().size() - 1){ // in case gps data end with transitioning
		    			bw.append(linePostStr);

		    			bw.append(spotPreStr1 + "[P" + (order + 1) + "] Ending Point (" + gps.getRecordDate().get(gps.getBasePoints().get(i)).toString().substring(11, gps.getRecordDate().get(gps.getBasePoints().get(i)).toString().indexOf("PDT")) + 
		    					", " + gps.getDuration().get(i) + "min)" + spotPreStr2 + spotPreStr3 + spotPreStr4 + (color%26) + spotPreStr5 + gps.getClusterLongitude().get(gps.getBasePoints().get(i)) + "," + 
		    					gps.getClusterLatitude().get(gps.getBasePoints().get(i)) + ",0" + spotPostStr);
		    		}
		    		
		    		continue;
		    	}
		    	
		    	if(isMoving){ // finalize lineString
		    		bw.append(gps.getClusterLongitude().get(gps.getBasePoints().get(i)) + "," + gps.getClusterLatitude().get(gps.getBasePoints().get(i)) + ",0 ");
		    		bw.append(linePostStr);
		    		isMoving = false;
		    	}
		    	

	        	String address = geocode(gps.getClusterLongitude().get(gps.getBasePoints().get(i)), gps.getClusterLatitude().get(gps.getBasePoints().get(i)));
	        	address = address.substring(0, address.indexOf(","));
	        	
	            Date startDate = gps.getRecordDate().get(gps.getBasePoints().get(i)); 
	            Date endDate = gps.getRecordEndDate().get(gps.getBasePoints().get(i));
 
	            //crawl google calendar
	            GoogleCalendar calendar = CalendarEventFeedCrawl.start("testcs212@gmail.com", "happy123456", startDate, endDate);
	            calendar.sort();
	            String calendarStr = "";
	            String calendarTitles = "";
	            
	            
	            try{
		            for(int j = 0; j < calendar.size();){
		                if(calendar.getStartTime().get(j).compareTo(startDate) < 0
		                        || calendar.getStartTime().get(j).compareTo(endDate) > 0){
		                    calendar.remove(j);
		                }
		                else
		                    j++;
//                        if(calendar.getStartTime().get(j).compareTo(startDate) > 0
//                                && calendar.getEndTime().get(j).compareTo(endDate) < 0){
//                            System.out.print("");
//                        }
		            }

//		            System.out.println("Google Calendar: ");
		            calendarStr = calendar.toHTML();
//		            calendarTitles = "\n" + calendar.getTitles();
		            System.out.println(calendarTitles);
	            }catch(Exception e){
	            	System.out.println(e.getMessage());
	            }
	                        
		    		           
	            //crawl facebook
	            String fbStr = "";
	            String fbTitles = "";
	            
//	            Calendar cal=Calendar.getInstance();
//	            cal.setTime(startDate);
//	            cal.add(Calendar.HOUR, 7);
//	            Date t1 = cal.getTime();
//	            cal.setTime(endDate);
//	            cal.add(Calendar.HOUR, 7);
//	            Date t2 = cal.getTime();
//	            String accessToken = "CAACEdEose0cBAPrpROF3TelWN5PO75deLNyG5ZB6nt8NqIOekGMXgkyEvZBuZBugZCiPFABrhpIV7GZArD5MSIaEylxLAof0VRFbiJLOLnitZAxgjUl5hxX6gRFnZCZBdGiTRZABGYXwIC3k67EjFQVafAxpCZAvNVrzN6JjYhjR93YwZDZD";
	            FB fb = FacebookCrawl.start(accessToken, startDate, endDate);
	            fb.sort();
//	            System.out.println("Facebook: ");

	            fbStr = fb.toHTML();

	           
	            //crawl phone
	            String phoneStr = "";
	            File dir = new File("src/Data/photo"); //transfer data from android phone to PC in this directory
	            Photo phone = PhotoCrawl.start(dir, startDate, endDate);
	            phone.sort();
	            phoneStr = phone.toHTML();
	            //phone.toString();
	            
	            System.out.println("Photo(Image/Video): ");
	            System.out.println(phone.toString());
		    	
	            //crawl message(SMS) from phone
	            String smsStr = "";
	            String smsTitles = "";
	            
	            SMS sms = SMSCrawl.start( "src/Data/SMS.csv", startDate, endDate);
	            sms.sort();
	            System.out.println("Message: ");
	            smsStr = sms.toHTML();
	            System.out.println(sms.toString());
	            //event.setSms(sms);
	            
	            //crawl call(SMS) from phone
	            String callStr = "";
	            
	            Call call = CallCrawl.start( "src/Data/call.csv", startDate, endDate);
	            call.sort();
	            callStr = call.toHTML();
	            System.out.println("Call: ");
	            System.out.println(call.toString());
	            
	            
	            String start_end_duraiton = "Start: " + startDate.toString().substring(11, startDate.toString().indexOf("PDT")) + 
	            		", End: " + endDate.toString().substring(11, startDate.toString().indexOf("PDT")) + ", Duration: " + gps.getDuration().get(i) + "min<br>";
	            
	            Boolean isFirst = true;

	            order++;
	            bw.append(spotPreStr1 + "[P" + order + "] " + address +" (" + startDate.toString().substring(11, startDate.toString().indexOf("PDT")) + ", " + gps.getDuration().get(i) + "min)" + spotPreStr2); 
	            
	            if(calendar.size() > 0){
	            	bw.append("GC: " + calendar.size());
	            	isFirst = false;
	            }
	            
	            if(fb.size() > 0){
	            	if(isFirst){
	            		bw.append("FB: " + fb.size());
	            		isFirst = false;
	            	}
	            	else
	            		bw.append(", FB: " + fb.size());	            		
	            }
	            
	            if(sms.size() > 0){
	            	if(isFirst){
	            		bw.append("SMS: " + sms.size());
	            		isFirst = false;
	            	}
	            	else
	            		bw.append(", SMS: " + sms.size());	            			            	
	            }
	            
	            if(call.size() > 0){
	            	if(isFirst){
	            		bw.append("Call: " + call.size());
	            		isFirst = false;
	            	}
	            	else
	            		bw.append(", Call: " + call.size());	            			            	
	            }
	            
	            if(phone.size() > 0){
	            	if(isFirst){
	            		bw.append("Picture: " + phone.size());
	            		isFirst = false;
	            	}
	            	else
	            		bw.append(", Picture: " + phone.size());	            			            	
	            }
	            
	            bw.append(spotPreStr3 + start_end_duraiton + "<br>");
	            if(calendarStr.length() > 0)
	            	bw.append(calendarStr + "<br>");
	            if(fbStr.length() > 0)
	            	bw.append(fbStr + "<br>");
	            if(smsStr.length() > 0)
	            	bw.append(smsStr + "<br>");
	            if(callStr.length() > 0)
	            	bw.append(callStr + "<br>");
	            if(phoneStr.length() > 0)
	            	bw.append(phoneStr + "<br>");
	            
	            bw.append(spotPreStr4 + (color%26) + spotPreStr5 + gps.getClusterLongitude().get(gps.getBasePoints().get(i)) + "," + 
	    		    			gps.getClusterLatitude().get(gps.getBasePoints().get(i)) + ",0" + spotPostStr);
		    	lng = gps.getClusterLongitude().get(gps.getBasePoints().get(i));
		    	lat = gps.getClusterLatitude().get(gps.getBasePoints().get(i));
		    	color++;
		    }
		   
		    bw.append(postStr);
		    
		    bw.close();
		    System.out.println("Done!!");
		    
	    } catch(Exception e){
	    	System.out.println("Exception Occurred! Msg: " + e.getMessage() + " ---- " + e.getLocalizedMessage());
	    }          
	}
     
    public static String geocode(Double lng, Double lat){
        String line = "";
        String address = "";
              
        HttpClient client = new DefaultHttpClient();
        
        HttpGet request = new HttpGet("http://maps.googleapis.com/maps/api/geocode/json?latlng="+lat+","+lng+"&sensor=false");
        try{
        	HttpResponse response = client.execute(request);
        	
        	BufferedReader rd = new BufferedReader (new InputStreamReader(response.getEntity().getContent()));
            while ((line = rd.readLine()) != null) {
            	if(line.contains("formatted_address")){
            		address = line.substring(line.indexOf("address\" : \"") + 12, line.length()-2);

            		break;
            	}
            }
        }catch(Exception e){
        	System.out.println("Exception Occurred! Msg: " + e.getMessage());
        }
           
        client.getConnectionManager().shutdown();
        
        return address; 
	}	
}
