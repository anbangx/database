package crawl.core;

import java.io.File;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import crawl.GPS.GPS;
import crawl.calendar.CalendarEventFeedCrawl;
import crawl.facebook.FacebookCrawl;
import crawl.phone.CallCrawl;
import crawl.phone.PhotoCrawl;
import crawl.phone.SMSCrawl;
import crawl.type.Call;
import crawl.type.GoogleCalendar;
import crawl.type.Event;
import crawl.type.FB;
import crawl.type.GPSStream;
import crawl.type.Photo;
import crawl.type.SMS;

public class drive {
    
    public static void main(String[] args) throws Exception {
        String specificDate = "20130608";
        GPSStream gps = GPS.start(specificDate);
        
        Map<Integer,Event> events = new HashMap<Integer,Event>();
//        String userName = args[0];
//        String userPassword = args[1];
        for(int i = 0; i < gps.getBasePoints().size(); i++){
            if(gps.getDuration().get(i) < 5)
                continue; 
            //basic event
            Event event = new Event();
            Date startDate = gps.getRecordDate().get(gps.getBasePoints().get(i)); 
            Date endDate = gps.getRecordEndDate().get(gps.getBasePoints().get(i));
            
            System.out.println();
            System.out.println("Events from " + startDate.toString() + " to "
                  + endDate.toString() + ":");
            //crawl google calendar
            GoogleCalendar calendar = CalendarEventFeedCrawl.start("testcs212@gmail.com", "happy123456", startDate, endDate);
            for(int j = 0; j < calendar.size();){
                if(calendar.getEndTime().get(j).compareTo(startDate) < 0
                        || calendar.getStartTime().get(j).compareTo(endDate) > 0){
                    calendar.remove(j);
                }
                else
                    j++;
            }
            System.out.println("Google Calendar: ");
            System.out.println(calendar.toString());
            event.setCalendar(calendar);
            

            //crawl facebook
            Calendar cal=Calendar.getInstance();
            cal.setTime(startDate);
            cal.add(Calendar.HOUR, 7);
            Date t1 = cal.getTime();
            cal.setTime(endDate);
            cal.add(Calendar.HOUR, 7);
            Date t2 = cal.getTime();
            String accessToken = args[2];
            FB fb = FacebookCrawl.start(accessToken, t1, t2);
            System.out.println("Facebook: ");
            System.out.println(fb.toString());
            event.setFacebook(fb);
            
            //crawl photo from phone
            File dir = new File("src/Data/photo"); //transfer data from android phone to PC in this directory
            Photo photo = PhotoCrawl.start(dir, startDate, endDate);
            System.out.println("Photo(Image/Video): ");
            System.out.println(photo.toString());
            event.setPhoto(photo);
            
            //crawl message(SMS) from phone
            SMS sms = SMSCrawl.start( "src/Data/SMS.csv", startDate, endDate);
            System.out.println("Message: ");
            System.out.println(sms.toString());
            event.setSms(sms);
            
            //crawl call(SMS) from phone
            Call call = CallCrawl.start( "src/Data/call.csv", startDate, endDate);
            System.out.println("Call: ");
            System.out.println(call.toString());
            event.setCall(call);
            
            events.put(gps.getBasePoints().get(i), event);
        }
        
    }

}
