package ir.assignments.three.GoogleCustomerSearch;

import java.io.BufferedReader;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class GoogleSearch {
	public static void main(String[] args) throws Exception {
		/**
		 * useful one
		 */
	    String key="AIzaSyDLyJEPqvFYX7ZtXSWLeRm4cwGqeKPP5p4";
	    String cx = "000871406312733320210:n0ht0gelmoc";
	    String qry="information retrieval"; //computer games site:ics.uci.edu
	    qry = qry.replaceAll(" ", "+");
        URL url = new URL(
        		"https://www.googleapis.com/customsearch/v1?key="+key+ "&cx=" + cx + "&q="+ qry + "&hq=site:ics.uci.edu" + "&alt=json");
	    HttpURLConnection conn = (HttpURLConnection) url.openConnection();
	    conn.setRequestMethod("GET");
	    conn.setRequestProperty("Accept", "application/json");
	    BufferedReader br = new BufferedReader(new InputStreamReader(
	            (conn.getInputStream())));

	    String output;
	    System.out.println("Output from Server .... \n");
	    FileWriter fw = new FileWriter("GoogleSearchResult_Html/" + qry);
	    while ((output = br.readLine()) != null) {

	        if(output.contains("\"link\": \"") && !output.contains("pdf")){                
	            String link=output.substring(output.indexOf("\"link\": \"")+("\"link\": \"").length(), output.indexOf("\","));
	            System.out.println(link);       //Will print the google search links
	            fw.write(link + "\r\n");
	        }     
	    }
	    fw.close();
	    conn.disconnect();                              
	}
}
