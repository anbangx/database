package ir.assignments.three.pagerank;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.math.BigDecimal;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.StringTokenizer;

public class generateTopPage {

	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub
		HashMap<Double,ArrayList<Long>> hashMap = null;
		File dir = new File("MySearchResult/PageRank");
		File[] fs = dir.listFiles();
		BufferedReader reader = null;
		StringTokenizer st = null;
		String line = "";
		Long docId = 0L;
		ArrayList<Long> docIds = new ArrayList<Long>();
		Double score = 0.0;
		for(int i = 0; i < fs.length; i++){
			hashMap = new HashMap<Double,ArrayList<Long>>(); 
			reader = new BufferedReader(new FileReader(fs[i]));
			while((line = reader.readLine()) != null){
				st = new StringTokenizer(line,"\t");
				docId = Long.parseLong(st.nextToken());
				score = stringToDouble(st.nextToken());
				if(!hashMap.containsKey(score))
					docIds = new ArrayList<Long>();
				else
					docIds = hashMap.get(score);
				docIds.add(docId);
				hashMap.put(score, docIds);	
			}
			printHashMap(hashMap,fs[i].getName());
		}
	}
	
	public static Double stringToDouble(String s) throws ParseException{
		DecimalFormat format = (DecimalFormat) NumberFormat.getInstance(Locale.US);
		format.setParseBigDecimal(true);
		BigDecimal number = (BigDecimal) format.parse(s);
		return Double.parseDouble(number.toString());
	}

	public static void printHashMap(HashMap<Double,ArrayList<Long>> hashMap, String fileName) throws IOException{
        FileOutputStream stream = new FileOutputStream("MySearchResult/top/" + fileName, true);
        OutputStreamWriter writer = new OutputStreamWriter(stream); 
		List keys = new ArrayList(hashMap.keySet());
		Collections.sort(keys);
		ArrayList<Long> docIds;
		int count = 1;
		for(int i = keys.size()-1; i > 0 ; i--){
			docIds = hashMap.get(keys.get(i));
			for(int j = 0; j < docIds.size(); j++)
				writer.write(keys.get(i).toString()+ " " + docIds.get(j).toString() + " " + count + "\r\n");
			count++;
		}
	    writer.close();
	    stream.close();
	}
}
