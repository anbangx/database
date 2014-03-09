package ir.assignments.three.GJson;

import ir.assignments.three.GJson.GoogleResults.Result;

import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.Reader;
import java.net.URL;
import java.net.URLEncoder;
import java.util.List;

import com.google.gson.Gson;

public class issueQueryToGoogle {

	/**
	 * @param args
	 */
	public static void main(String[] args) throws Exception {
	    String google = "https://www.googleapis.com/customsearch/v1?q="; //"http://ajax.googleapis.com/ajax/services/search/web?v=1.0&q=";
	    String search = "machine";
	    String charset = "UTF-8";

	    URL url = new URL(google + URLEncoder.encode(search, charset));
	    Reader reader = new InputStreamReader(url.openStream(), charset);
	    GoogleResults results = new Gson().fromJson(reader, GoogleResults.class);
	    
	    //print out research result(URL) to file
	    List<Result> resultList = results.getResponseData().getResults();
	    FileWriter fw = new FileWriter("GoogleSearchResult/" + search);
	    for(int i = 0; i < 10 && i < resultList.size(); i++)
	    	fw.write(resultList.get(i).getUrl() + "\r\n");
	    fw.close();
	}

}
