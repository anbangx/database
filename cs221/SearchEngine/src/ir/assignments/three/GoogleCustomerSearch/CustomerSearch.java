package ir.assignments.three.GoogleCustomerSearch;

import java.io.FileWriter;
import java.io.IOException;
import java.util.List;

import com.google.api.client.http.javanet.NetHttpTransport;
import com.google.api.client.json.jackson.JacksonFactory;
import com.google.api.services.customsearch.Customsearch;
import com.google.api.services.customsearch.model.Result;
import com.google.api.services.customsearch.model.Search;

public class CustomerSearch {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		Customsearch customsearch = new Customsearch(new NetHttpTransport(), new JacksonFactory(), null);
		String YOUR_SEARCH_STRING_GOES_HERE = "helloworld";
	    try {
	        com.google.api.services.customsearch.Customsearch.Cse.List list = customsearch.cse().list(YOUR_SEARCH_STRING_GOES_HERE);
	        list.setKey("AIzaSyDLyJEPqvFYX7ZtXSWLeRm4cwGqeKPP5p4"); 
	        list.setCx("000871406312733320210:n0ht0gelmoc");
	        Search results = list.execute();
	        List<Result> items = results.getItems();
	       // System.out.println("Size = " + list.ge);

	        FileWriter fw = new FileWriter("GoogleSearchResult_Cusomer/" + YOUR_SEARCH_STRING_GOES_HERE);
	        for(Result result:items)
	        {;
	            fw.write(result.getLink() + "\r\n");
	        }
	        fw.close();

	    } catch (IOException e) {
	        // TODO Auto-generated catch block
	        e.printStackTrace();
	    }
	}

}
