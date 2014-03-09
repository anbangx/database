/*
 * Copyright (c) 2010-2013 Mark Allen.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

package crawl.facebook;

import static java.lang.System.out;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import com.restfb.Connection;
import com.restfb.DefaultFacebookClient;
import com.restfb.FacebookClient;
import com.restfb.Parameter;
import com.restfb.types.Photo;
import com.restfb.types.Post;
import com.restfb.types.User;

import crawl.calendar.CalendarEventFeedCrawl;
import crawl.type.FB;
import crawl.type.GoogleCalendar;

/**
 * Examples of RestFB's Graph API functionality.
 * 
 * @author <a href="http://restfb.com">Mark Allen</a>
 */
public class FacebookCrawl extends Example {
  /**
   * RestFB Graph API client.
   */
  private final FacebookClient facebookClient;

  /**
   * Entry point. You must provide a single argument on the command line: a valid Graph API access token.
   * 
   * @param args
   *          Command-line arguments.
   * @throws IllegalArgumentException
   *           If no command-line arguments are provided.
   */
  public static FB start(String accessToken, Date startDate, Date endDate) {
      FB fb = new FB();
      
      ArrayList<Date> postTime = new ArrayList<Date>();
      ArrayList<String> type = new ArrayList<String>();
      ArrayList<String> title = new ArrayList<String>();
      ArrayList<String> link = new ArrayList<String>();
      ArrayList<String> description = new ArrayList<String>();
      ArrayList<String> actionLink = new ArrayList<String>();
      
      FacebookCrawl facebookCrawl = new FacebookCrawl(accessToken);
      
      Connection<Post> filteredFeed = facebookCrawl.filterConnectionsPosts(startDate, endDate);
      Post post = null;
      for(int i = 0; i < filteredFeed.getData().size(); i++){
          post = filteredFeed.getData().get(i);
          postTime.add(post.getCreatedTime()); 
          System.out.println(post.getCreatedTime().toString());
          actionLink.add(post.getActions().get(0).getLink());
          type.add("Post");
          if(post.getMessage().length() > 50)
        	  title.add(post.getMessage().substring(0, 50));
          else
        	  title.add(post.getMessage());
          
          link.add(post.getLink());
          description.add(post.getDescription());
      }
      
      Connection<Photo> filteredPhoto = facebookCrawl.filterConnectionsPhotos(startDate, endDate);
      Photo photo = null;
      for(int i = 0; i < filteredPhoto.getData().size(); i++){
          photo = filteredPhoto.getData().get(i);
          postTime.add(photo.getCreatedTime());
          actionLink.add(post.getActions().get(0).getLink());
          type.add("Photo");
          title.add(photo.getName());
          link.add(photo.getLink());
          description.add("");
      }
      
      fb.setPostTime(postTime);
      fb.setType(type);
      fb.setTitle(title);
      fb.setLink(link);
      fb.setdescription(description);
      fb.setActionLink(actionLink);
      
      return fb;
  }

  public FacebookCrawl(String accessToken) {
    facebookClient = new DefaultFacebookClient(accessToken);
  }

  public User fetchBio() {
    out.println("* Fetching bio object *");
    return facebookClient.fetchObject("me", User.class);
  }
  
  public Connection<User> fetchConnectionsFriends() {
      out.println("* Fetching friends connections *");
      return facebookClient.fetchConnection("me/friends", User.class);
   }
   
   public Connection<Post> fetchConnectionsPost() {
       out.println("* Fetching posts connections *");
       return facebookClient.fetchConnection("me/feed", Post.class);
   }
   
   public Connection<Post> filterConnectionsPosts(Date startTime, Date endTime) {
   
       Connection<Post> filteredFeed =
           facebookClient.fetchConnection("me/feed", Post.class, Parameter.with("limit", 100),
             Parameter.with("until", endTime), Parameter.with("since", startTime));
       
       return filteredFeed;
   }
   
   public Connection<Photo> filterConnectionsPhotos(Date startTime, Date endTime) {
       
       Connection<Photo> filteredPhoto =
               facebookClient.fetchConnection("me/photo", Photo.class, Parameter.with("limit", 100),
                 Parameter.with("until", endTime), Parameter.with("since", startTime));
       
       return filteredPhoto;
   }

  void rawJsonResponse() {
    out.println("* Raw JSON *");
    out.println("User object JSON: " + facebookClient.fetchObject("me", String.class));
  }
  
  public static void main(String[] args) throws Exception {
      Date startTime = new SimpleDateFormat("HH:mm yyyy-MM-dd").parse("00:01 2013-06-07");
      Date endTime = new SimpleDateFormat("HH:mm yyyy-MM-dd").parse("12:00 2013-06-07");
      FB facebok = FacebookCrawl.start("CAACEdEose0cBAOmP6IJS4QtZCnvAgftF6sj1Q24xfZCvIYJ0XQjZB2JvvfUxabt0QCk8PUBW5rkLgAq7qIntIpeLcxXTSmMZCqXwIIIYfYZBzWaLmPPya6MNUDe1zW7CbvyOb6hpptloN3WM1othTOsYyvhlOzTEZD", 
              startTime, endTime);
  }
}