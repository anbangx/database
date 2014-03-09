/* Copyright (c) 2008 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


package crawl.calendar;

import com.google.gdata.client.calendar.CalendarQuery;
import com.google.gdata.client.calendar.CalendarService;
import com.google.gdata.data.DateTime;
import com.google.gdata.data.calendar.CalendarEventEntry;
import com.google.gdata.data.calendar.CalendarEventFeed;
import com.google.gdata.util.ServiceException;

import crawl.type.GoogleCalendar;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;

/**
 * Demonstrates basic GoogleCalendar Data API operations on the event feed using the
 * Java client library:
 * 
 * <ul>
 * <li>Retrieving the list of all the user's calendars</li>
 * <li>Retrieving all events on a single calendar</li>
 * <li>Performing a date-range query on a calendar</li>
 * </ul>
 */
public class CalendarEventFeedCrawl {

  // The base URL for a user's calendar metafeed (needs a username appended).
  private static final String METAFEED_URL_BASE = 
      "https://www.google.com/calendar/feeds/";

  // The string to add to the user's metafeedUrl to access the event feed for
  // their primary calendar.
  private static final String EVENT_FEED_URL_SUFFIX = "/private/full";

  // The URL for the metafeed of the specified user.
  // (e.g. http://www.google.com/feeds/calendar/jdoe@gmail.com)
  private static URL metafeedUrl = null;

  // The URL for the event feed of the specified user's primary calendar.
  // (e.g. http://www.googe.com/feeds/calendar/jdoe@gmail.com/private/full)
  private static URL eventFeedUrl = null;
  
  /**
   * Prints the titles of all events in a specified date/time range.
   * 
   * @param service An authenticated CalendarService object.
   * @param startTime Start time (inclusive) of events to print.
   * @param endTime End time (exclusive) of events to print.
   * @throws ServiceException If the service is unable to handle the request.
   * @throws IOException Error communicating with the server.
   */
  private static GoogleCalendar dateRangeQuery(CalendarService service,
      Date startTime, Date endTime) throws ServiceException,
      IOException {
    CalendarQuery myQuery = new CalendarQuery(eventFeedUrl);
    //myQuery.setMinimumStartTime(startTime);
    //myQuery.setMaximumStartTime(endTime); 

    // Send the request and receive the response:
    CalendarEventFeed resultFeed = service.query(myQuery,
        CalendarEventFeed.class);
    
    //store into object
    ArrayList<Date> startList = new ArrayList<Date>();
    ArrayList<Date> endList = new ArrayList<Date>();
    ArrayList<String> titleList = new ArrayList<String>();
    ArrayList<String> locationList = new ArrayList<String>();
    ArrayList<String> descriptionList = new ArrayList<String>();
    ArrayList<String> actionLink = new ArrayList<String>();
    
    //Date cmpStart = new Date(startTime.getValue());
    //Date cmpEnd = new Date(endTime.getValue());
    Date start = new Date();
    Date end = new Date();
    
//    System.out.println("Events from " + start.toString() + " to "
//        + end.toString() + ":");
//    System.out.println();

    for (int i = 0; i < resultFeed.getEntries().size(); i++) {
      CalendarEventEntry entry = resultFeed.getEntries().get(i);
//      System.out.println("\t" + entry.getTitle().getPlainText());
      
      if(entry.getTimes().size() > 0
              && entry.getTimes().size() > 0){
          start = new Date(entry.getTimes().get(0).getStartTime().getValue());
          end = new Date(entry.getTimes().get(0).getEndTime().getValue());
          if(start.compareTo(startTime) >= 0 && start.compareTo(endTime) < 0){
              startList.add(start);
              endList.add(end);
              System.out.println("\t" + start.toString()
                      + "\t" + end.toString());
              titleList.add(entry.getTitle().getPlainText());
              actionLink.add(entry.getLinks().get(0).getHref());
                if(entry.getLocations().size() > 0){
                    locationList.add(entry.getLocations().get(0).getValueString());
    //                System.out.print("\t" + entry.getLocations().get(0).getValueString());
                }
                descriptionList.add(entry.getContent().toString());
    //            System.out.print("\t" + entry.getContent().toString());
    //            System.out.println();
          }
      }
      
    }
    System.out.println();
    
    GoogleCalendar calendar = new GoogleCalendar();
    calendar.setStartTime(startList);
    calendar.setEndTime(endList);
    calendar.setTitle(titleList);
    calendar.setLocation(locationList);
    calendar.setDescription(descriptionList);
    calendar.setActionLink(actionLink);
    
    return calendar;
  }

  /**
   * Instantiates a CalendarService object and uses the command line arguments
   * to authenticate. The CalendarService object is used to demonstrate
   * interactions with the GoogleCalendar data API's event feed.
   * 
   * @param args Must be length 2 and contain a valid username/password
 * @throws Exception 
   */
  public static GoogleCalendar start(String userName, String userPassword, Date startDate, Date endDate) throws Exception {
    CalendarService myService = new CalendarService("Google-GoogleCalendar");

    // Create the necessary URL objects.
    try {
      setMetafeedUrl(new URL(METAFEED_URL_BASE + userName));
      eventFeedUrl = new URL(METAFEED_URL_BASE + userName
          + EVENT_FEED_URL_SUFFIX);
    } catch (MalformedURLException e) {
      // Bad URL
      System.err.println("Uh oh - you've got an invalid URL.");
      e.printStackTrace();
      return null;
    }

    GoogleCalendar calendar = null;
    
    try {
      myService.setUserCredentials(userName, userPassword);
      SimpleDateFormat DATE_FORMAT = new SimpleDateFormat("yyyy-MM-dd");
      String start = DATE_FORMAT.format(startDate);
      String end = DATE_FORMAT.format(endDate);
      //add one day to date
      Calendar c = Calendar.getInstance();
      c.setTime(DATE_FORMAT.parse(end));
      c.add(Calendar.DATE, 1);  // number of days to add
      end = DATE_FORMAT.format(c.getTime());  // dt is now the new date
      startDate = DATE_FORMAT.parse(start);
      endDate = DATE_FORMAT.parse(end);
      //calendar = dateRangeQuery(myService, DateTime.parseDate(start), DateTime
      //    .parseDate(end)); //"2013-05-28" <- Date
      calendar = dateRangeQuery(myService, startDate, endDate); //"2013-05-28" <- Date
      
    } catch (IOException e) {
      // Communications error
      System.err.println("There was a problem communicating with the service.");
      e.printStackTrace();
    } catch (ServiceException e) {
      // Server side error
      System.err.println("The server had a problem handling your request.");
      e.printStackTrace();
    }
    
    return calendar;
  }

    public static URL getMetafeedUrl() {
        return metafeedUrl;
    }
    
    public static void setMetafeedUrl(URL metafeedUrl) {
        CalendarEventFeedCrawl.metafeedUrl = metafeedUrl;
    }
    
    public static void main(String[] args) throws Exception {
        Date startTime = new SimpleDateFormat("yyyy-MM-dd").parse("2013-06-09");
        Date endTime = new SimpleDateFormat("yyyy-MM-dd").parse("2013-06-09");
        GoogleCalendar calendar = CalendarEventFeedCrawl.start("testcs212@gmail.com", "happy123456", startTime, endTime);
    }
}
