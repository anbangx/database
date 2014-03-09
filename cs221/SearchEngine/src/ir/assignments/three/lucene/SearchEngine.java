package ir.assignments.three.lucene;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.DirectoryReader;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.queryparser.classic.ParseException;
import org.apache.lucene.queryparser.classic.QueryParser;
import org.apache.lucene.search.BooleanClause;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.TopScoreDocCollector;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.util.Version;

public class SearchEngine {
	public static ArrayList<Document> generateMySearchResult(String words) throws Exception{
		
		StandardAnalyzer analyzer = new StandardAnalyzer(Version.LUCENE_40);
		File file = new File("/home/anbangx/EE/workspace/search_engine/src/index");
		
		// 2. query
	    String querystr = words;

	    // the "title" arg specifies the default field to use
	    // when no field is explicitly specified in the query.
	    Query anchorQuery = new QueryParser(Version.LUCENE_40, "anchor", analyzer).parse(querystr);
	    Query titleQuery = new QueryParser(Version.LUCENE_40, "title", analyzer).parse(querystr);
	    Query contentQuery = new QueryParser(Version.LUCENE_40, "content", analyzer).parse(querystr);
	    BooleanQuery mergedQuery = new BooleanQuery();
	    mergedQuery.add(anchorQuery, BooleanClause.Occur.SHOULD);
	    mergedQuery.add(titleQuery, BooleanClause.Occur.SHOULD);
	    mergedQuery.add(contentQuery, BooleanClause.Occur.SHOULD);
	    
	    // 3. search
	    int hitsPerPage = 500;
	    IndexReader reader = DirectoryReader.open(FSDirectory.open(file));
	    IndexSearcher searcher = new IndexSearcher(reader);
	    TopScoreDocCollector collector = TopScoreDocCollector.create(hitsPerPage, true);
	    searcher.search(mergedQuery, collector); //mergedQuery
	    ScoreDoc[] hits = collector.topDocs().scoreDocs;
	    
	    // 4. display results
	    ArrayList<Document> documents = new ArrayList<Document>();
	    System.out.println("Found " + hits.length + " hits.");
	    for(int i=0;i<hits.length;++i) {
	      int docId = hits[i].doc;
	      Document d = searcher.doc(docId);
	      documents.add(d);
	      System.out.println((i + 1) + ". " + d.get("url") + "\t" + words);
	      System.out.println(docId + ". ");
	    }
	    // reader can only be closed when there
	    // is no need to access the documents any more.
	    reader.close();
	    
	    return documents;
	}
	
	public static String generateSomeWordsFromContent(String content, String query){
		/*int contentLength = content.length() < 500 ? content.length() : 500;
		String substring = content.substring(0, contentLength);
		String[] words = substring.split(" ");
		int length = words.length;
		String text = "";
		for(int i = 0; i < length-1; i++){
			String term = words[i] + " " + words[i+1];
			if(query.equals(term.toLowerCase())){
				for(int j = 0; j < i; j++)
					text += words[j] + " ";
				text += "<strong>" + term + "</strong>";
				for(int j = i+2; j < i+10; j++)
					text += " " + words[j];
				break;
			}
		}
		
		return text + "...";*/
		
		/*int contentLength = content.length() < 1000 ? content.length() : 1000;
		String substring = content.substring(0, contentLength);
		String[] sentences = substring.split(".");
		String text = sentences[0];
		for(int n = 0; n < sentences.length; n++){
			String[] words = sentences[n].split(" ");
			int length = words.length;
			for(int i = 0; i < length; i++){
				String term = words[i] + " " + words[i+1];
				if(query.equals(term.toLowerCase())){
					for(int j = 0; j < i; j++)
						text += words[j] + " ";
					text += "<strong>" + term + "</strong>";
					for(int j = i+2; j < length; j++)
						text += " " + words[j];
				}
			}
		}
		return text + "...";*/
		
		
		/*int contentLength = content.length() < 500 ? content.length() : 500;
		String substring = content.substring(0, contentLength);
		String[] words = substring.split(" ");
		int length = words.length;
		String text = "";
		// 20 - 50
		if(length < 50){
			int size = length < 30 ? length : 30;
			for(int i = 0; i < size-1; i++){
				if(words[i].compareTo(" ") != 0 
					&& words[i].compareTo("\t") != 0
					&& words[i].compareTo("\n") != 0){
					String term = words[i] + " " + words[i+1]; 
					if(query.equals(term.toLowerCase())){
						text += "<strong>" + term + "</strong> ";
						i++;
					}
					else
						text += words[i] + " ";
				}
					
			}
		}
		else{
			for(int i = 20; i < 50-1; i++){
				if(words[i].compareTo(" ") != 0 
					&& words[i].compareTo("\t") != 0
					&& words[i].compareTo("\n") != 0){
					String term = words[i] + " " + words[i+1];
					if(query.equals(term.toLowerCase())){
						text += "<strong>" + term + "</strong> ";
						i++;
					}
					else
						text += words[i] + " ";
				}
					
			}
		}
		return text + "...";*/
		int contentLength = content.length() < 1000 ? content.length() : 1000;
		String substring = content.substring(0, contentLength);
		String[] words = substring.split(" ");
		int length = words.length;
		String text = "";
		// 20 - 50
		for(int i = 0; i < length-1 && i < 30; i++){
			String term = words[i] + " " + words[i+1]; 
			if(!words[i].equals(" ")){
				if(query.equals(term.toLowerCase())){
					text += "<strong>" + term + "</strong> ";
					i++;
				}
				else
					text += words[i] + " ";
			}
		}
		
		return text + "...";
		/*int contentLength = content.length() < 500 ? content.length() : 500;
		String substring = content.substring(0, contentLength);
		String[] words = substring.split(" ");
		int length = words.length;
		String text = "";
		// 20 - 50
		if(length < 50){
			int size = length < 30 ? length : 30;
			for(int i = 0; i < size-1; i++){
				if(words[i].compareTo(" ") != 0 
					&& words[i].compareTo("\t") != 0
					&& words[i].compareTo("\n") != 0)
					text += words[i] + " ";		
			}
		}
		else{
			for(int i = 20; i < 50; i++){
				if(words[i].compareTo(" ") != 0 
					&& words[i].compareTo("\t") != 0
					&& words[i].compareTo("\n") != 0){
					text += words[i] + " ";
				}
					
			}
		}
		return text + "...";*/
	}
}
