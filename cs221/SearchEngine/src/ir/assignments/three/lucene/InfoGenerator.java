package ir.assignments.three.lucene;

import java.io.File;
import java.io.FileWriter;
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

public class InfoGenerator {
	
	public static void generateMySearchResult(String words) throws ParseException, CorruptIndexException, IOException{
		
		StandardAnalyzer analyzer = new StandardAnalyzer(Version.LUCENE_40);
		File file = new File("index");
		
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
	    //searcher.setSimilarity( new IsolationSimilarity() );
	    TopScoreDocCollector collector = TopScoreDocCollector.create(hitsPerPage, true);
	    searcher.search(mergedQuery, collector); //mergedQuery
	    ScoreDoc[] hits = collector.topDocs().scoreDocs;
	    
	    // 4. display results
	    FileWriter fw = new FileWriter("MySearchResult/" + words);
	    FileWriter fw2 = new FileWriter("MySearchResult/docId/" + words);
	    System.out.println("Found " + hits.length + " hits.");
	    for(int i=0;i<hits.length;++i) {
	      int docId = hits[i].doc;
	      Document d = searcher.doc(docId);
	      System.out.println((i + 1) + ". " + d.get("url") + "\t" + words);
	      fw.write(d.get("url") + "\r\n");
	      fw2.write(d.get("docId") + "\r\n");
	      System.out.println(docId + ". ");
	    }
	    fw.close();
	    fw2.close();
	    // reader can only be closed when there
	    // is no need to access the documents any more.
	    reader.close();
	}
	
	public static void main(String[] args) throws IOException, ParseException{
		ArrayList<String> wordList = new ArrayList<String>();
		String words = "mondego";
		wordList.add(words);
		words = "machine learning";
		words = words.replaceAll(" ", "+");
		wordList.add(words);
		words = "software engineering";
		words = words.replaceAll(" ", "+");
		wordList.add(words);
		words = "security";
		wordList.add(words);
		words = "student affairs";
		words = words.replaceAll(" ", "+");
		wordList.add(words);
		words = "graduate courses";
		words = words.replaceAll(" ", "+");
		wordList.add(words);
		words = "Crista Lopes";
		words = words.replaceAll(" ", "+");
		wordList.add(words);
		words = "REST";
		wordList.add(words);
		words = "computer games";
		words = words.replaceAll(" ", "+");
		wordList.add(words);
		words = "information retrieval";
		words = words.replaceAll(" ", "+");
		wordList.add(words);
		
		for(int i = 0; i < wordList.size(); i++)
			generateMySearchResult(wordList.get(i));
	}

}
