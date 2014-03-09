package ir.assignments.three.lucene;

import java.util.ArrayList;

import org.apache.lucene.document.Document;

public class testSearchEngine {

	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub
		ArrayList<Document> documents = SearchEngine.generateMySearchResult("machine learning");
		for(int i = 0; i < documents.size(); i++)
			System.out.println(documents.get(i).get("url"));
	}

}
