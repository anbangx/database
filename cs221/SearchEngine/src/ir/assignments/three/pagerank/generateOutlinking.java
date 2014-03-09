package ir.assignments.three.pagerank;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;

public class generateOutlinking {

	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub
		HashMap<Long,ArrayList<Long>> hashMap = new HashMap<Long,ArrayList<Long>>();
		File file = new File("outgoingUrlList");
		BufferedReader reader = new BufferedReader(new FileReader(file));
		StringTokenizer st;
		String line = "";
		Long docId;
		String outgoingsString = "";
		ArrayList<Long> outgoings = null;
		while((line = reader.readLine()) != null){
			outgoings = new ArrayList<Long>();
			st = new StringTokenizer(line,"\t");
			docId = Long.parseLong(st.nextToken()); 
			if(st.hasMoreTokens()){
				outgoingsString = st.nextToken();
				st = new StringTokenizer(outgoingsString," ");
				while(st.hasMoreTokens())
					outgoings.add(Long.parseLong(st.nextToken())); 
			}
			else
				outgoings = new ArrayList<Long>();
			hashMap.put(docId, outgoings);
		}
		
		File dir = new File("MySearchResult/docId");
		File[] fs = dir.listFiles(); 
		File output;
		reader = null;
		FileWriter fw = null;
		for(int i = 0; i < fs.length; i++){
			output = new File("MySearchResult/outgoing/" + fs[i].getName());
			fw = new FileWriter(output);
			reader = new BufferedReader(new FileReader(fs[i]));
			while((line = reader.readLine()) != null){
				docId = Long.parseLong(line);
				fw.write(line + "\t");
				outgoings = hashMap.get(docId);
				if(!outgoings.isEmpty()){
					for(int j = 0; j < outgoings.size()-1; j++)
						fw.write(outgoings.get(j) + " ");
					fw.write(outgoings.get(outgoings.size()-1) + "\r\n");
				}
				else 
					fw.write("\r\n");
			}
			fw.close();
		}
		reader.close();
		
		filter();
	}
	
	public static void filter() throws Exception{
		HashMap<Long,String> hashMap = null;
		File dir = new File("MySearchResult/outgoing");
		File[] fs = dir.listFiles(); 
		String line = "";
		Long docId;
		BufferedReader reader = null;
		StringTokenizer st;
		String outgoingsString = "";
		for(int i = 0; i < fs.length; i++){
			hashMap = new HashMap<Long,String>();
			reader = new BufferedReader(new FileReader(fs[i]));
			while((line = reader.readLine()) != null){
				st = new StringTokenizer(line,"\t");
				docId = Long.parseLong(st.nextToken());
				if(st.hasMoreTokens()){
					outgoingsString = st.nextToken();
					if(!hashMap.containsKey(docId))
						hashMap.put(docId, outgoingsString);
				}
			}
			printHashMap(hashMap, fs[i].getName()); 
		}
	}
	
	public static void printHashMap(HashMap<Long,String> hashMap, String fileName) throws IOException{
        FileOutputStream stream = new FileOutputStream("MySearchResult/last/" + fileName, true);
        OutputStreamWriter writer = new OutputStreamWriter(stream); 
		List keys = new ArrayList(hashMap.keySet());
		Collections.sort(keys);
		for(int i = 0; i < keys.size(); i++)
			writer.write(keys.get(i).toString()+ " " + hashMap.get(keys.get(i)) + "\r\n");
	    writer.close();
	    stream.close();
	}

}
