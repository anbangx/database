package coreservlets;

import ir.assignments.three.lucene.SearchEngine;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.lucene.document.Document;

@WebServlet("/show-search-result")
public class ShowSearchResults extends HttpServlet {
 /**
	 * 
	 */
	private static final long serialVersionUID = 1L;

@Override
  public void doGet(HttpServletRequest request,
                    HttpServletResponse response)
      throws ServletException, IOException {
    response.setContentType("text/html");
    PrintWriter out = response.getWriter();
    String query = request.getParameter("query");
    ArrayList<Document> documents = null;
    System.out.println(query);
    try {
		documents = SearchEngine.generateMySearchResult(query);
		for(int i = 0; i < documents.size(); i++)
			System.out.println(documents.get(i).get("url"));
	} catch (Exception e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
    String url = "";
    String title = "";
    String content = "";
    String docType =
    	      "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 " +
    	      "Transitional//EN\">\n";
    out.println(docType + 
    			"<html>" +  
    			"<head>" + 
    			"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />"+
    			"<title>Show Search Results</title>" +
    			"<link href=\"css/inner.css\" rel=\"stylesheet\" type=\"text/css\" />"+
    			"<link href=\"css/prettyPhoto.css\" rel=\"stylesheet\" type=\"text/css\" />"+
    			"</head>" +
    			"<style type=\"text/css\">" +
    			"#searchBoxOne #search{"+
    			"display: inline;"+
			    "margin-left: 0px;"+
			    "margin-top: 14px;"+
			    "font-size: 24px;"+
			    "height:40px;"+
			    "width:500px;"+
			    "color: #999;"+
			    "padding: 6px;"+
			    "-moz-box-shadow:inset 2px 2px 5px #ccc;"+
			    "box-shadow:inset 2px 2px 5px #ccc;"+
			    "border: 1px solid #b8b8b8;}"+
				"#searchBoxOne #in{"+
			    "height:44px;"+
			    "width:44px;"+
			    "cursor:pointer;"+
			    "position:relative;"+
			    "top:-34px;"+
			    "right:60px;"+
			    "border:0px;}"+
			    "*{"+
				"margin:0px;"+
				"padding:0px}"+
			    ".page_split{"+
				"text-align:center;"+
				"margin:15px 0px;"+
			  	"font-size:12px;"+
				"line-height:23px}"+
			    ".page_split a,.page_split span{"+
				"border:1px solid #CCC;"+
				"padding:2px 5px;"+
				"text-decoration:none;"+
				"color:#787878;}"+
			    ".page_split a:hover{"+
				"background-color:#8cb900;"+
				"color:#FFF;}"+
			    "span.current{"+
				"color:#FFF;"+
				"background-color:#8cb900;"+
				"border:1px solid #436206;}"+
			    "</style>"+
			    "<body>"+
			    "<div id=\"main_wrapper\">"+
			    "<div id=\"top_phone\">"+
			    "Outstanding Search Engine"+
			    "</div>"+			    
			    "<div id=\"searchBoxOne\">"+
				"<form action=\"show-search-result\" method=\"get\">"+
				"<input type=\"text\" NAME=\"query\" value=\"Type in Keywords\" onfocus=\"javascript:this.value=''\" id=\"search\" />"+
				//"<input type=\"image\" src=\"images/search_icon.png\" id=\"in\" onClick=\"document.formname.submit();\" />"+
				"</form>"+
				"</div>"+
			    "<div class=\"full_width\">");
    String resultTitle = "";
    String[] queries = query.split(" ");
    ArrayList<String> q = new ArrayList<String>(); 
    for(int i = 0; i < queries.length; i++)
    	q.add(queries[i]);
    for(int i = 0; i < 10 || i < documents.size(); i++){
		url = documents.get(i).get("url");
		String urls[] = url.split("/");
		String subUrl = "";
		for(int j = 3; j < urls.length - 1; j++)
			subUrl += urls[j] + ".";
		if(urls.length > 0){
			if(urls[urls.length-1].contains(".")){
				int index = urls[urls.length-1].lastIndexOf(".");
				subUrl += urls[urls.length-1].substring(0,index);
			}
			else subUrl += urls[urls.length-1];
		}
		title = documents.get(i).get("title");
		String[] titles = title.split(" ");
		resultTitle = "";
		content = documents.get(i).get("content");
		for(int j = 0; j < titles.length; j++){
			if(q.contains(titles[j].toLowerCase()))
				resultTitle += "<strong>" + titles[j] + "</strong> ";
			else
				resultTitle += titles[j] + " ";
		}
		out.println("<a href=\"" + url + "\">" + resultTitle + "-- " + subUrl + "</a><BR>");
		out.println("<font color=\"green\">" + url +"</font> <BR>");
		out.println("<p>" + SearchEngine.generateSomeWordsFromContent(content,query) + "</p>");
		//out.println("<hr size=2>");
		out.println("<div class=\"hr\"></div>");
		System.out.println(i + " " + url);
		System.out.println(content);
    }
    out.println("<div class=\"page_split\">"+
				"<a href=\"#\">Prev</a>"+
				"<span class=\"current\">1</span>"+
				"<a href=\"#\">2</a>"+
				"<a href=\"#\">3</a>"+
				"<a href=\"#\">4</a>"+
				"<a href=\"#\">5</a>"+
				"<a href=\"#\">Next</a>"+
				"<a href=\"#\">Last</a>"+
				"</div>"+
    			"</div>" +
    			"</div>" +
    			"<div id=\"footer_wrapper\">"+
    			"<div id=\"footer_container\">"+
    			"<div id=\"footer_left\">"+
    			"<div class=\"download_button\">"+
    			"&nbsp;&nbsp;&nbsp;<a href=\"about_our_work.html\"><img src=\"img/learn_more_button.png\" width=\"197\" height=\"54\" alt=\"Learn More\" /></a>"+
    			"</div>" +
    			"</div>" +
    			"<div id=\"footer_middle\">"+
    			"<img src=\"img/google_white.png\" width=\"124\" height=\"48\" alt=\"logo\" />"+
    			"<br/><br/>"+
    			"Inspired by Google"+
    			"</div>"+
    			"<div id=\"footer_right\">"+
    			"<h4>Contact Us Through</h4>"+
    			"<ul class=\"social\">"+
    			"<li><a href=\"https://www.facebook.com/\" title=\"facebook\" class='fade'><img src=\"img/social_facebook.png\" width=\"30\" height=\"30\" alt=\"facebook\" /></a></li>"+
    			"<li><a href=\"http://www.linkedin.com/\" title=\"LinkedIn\" class='fade'><img src=\"img/social_linkedin.png\" width=\"30\" height=\"30\" alt=\"linkedin\" /></a></li>"+
    			"<li><a href=\"https://mail.google.com\" title=\"Email Us\" class='fade'><img src=\"img/social_email.png\" width=\"30\" height=\"30\" alt=\"email us\" /></a></li>"+
    			"<li><a href=\"https://twitter.com/\" title=\"Twitter\" class='fade'><img src=\"img/social_twitter.png\" width=\"30\" height=\"30\" alt=\"Twitter\" /></a></li>"+
    			"</ul>"+
    			"</div>"+
    			"</div>"+
    			"</div>"+
			    "</body>"+
			    "</html>");
 }

 
}
