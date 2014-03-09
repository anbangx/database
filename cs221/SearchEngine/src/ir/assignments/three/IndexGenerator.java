package ir.assignments.three;

import ir.assignments.one.a.Frequency;
import ir.assignments.one.a.Utilities;
import ir.assignments.one.b.WordFrequencyCounter;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

public class IndexGenerator {

	/**
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		listWords();
	}
	
	public static void listWords() throws IOException{
		File dir = new File("test1");
		File[] ds = dir.listFiles(); 
		File[] fs;
		Map<String,List<String>> wordList = new HashMap<String,List<String>>();
		
		
		for(int i = 0; i < ds.length; i++){
			fs = ds[i].listFiles();
			//FileWriter fw = new FileWriter(ds[i].toString() + "-wordList");
			//wordList.clear();
			for(int j = 0; j < fs.length; j++){
				
				//get word
				List<String> words = Utilities.tokenizeFile(fs[j]);
				List<Frequency> frequencies = WordFrequencyCounter.computeWordFrequencies(words);
				
				//store in memory
				for(int n = 0; n < frequencies.size(); n++){
					String word = frequencies.get(n).getText();
					List<String> post = null;
					if(!wordList.containsKey(word)){
						post = new ArrayList<String>();
						post.add(trimExtension(fs[j].getName()));
						wordList.put(word, post);
					}
					else{
						post = wordList.get(word);
						post.add(trimExtension(fs[j].getName()));
						wordList.remove(word);
						wordList.put(word, post);
					}
				}
				/*
				//output to file
				for(int n = 0; n < frequencies.size(); n++)
					fw.write(frequencies.get(n).getText() + " " +  trimExtension(fs[j].getName()) + "\r\n");
					*/
			}
			/*ArrayList<Map.Entry<String, List<String>>> l = sortKey(wordList); 
			Iterator iter=l.iterator();
	        
	        while(iter.hasNext())
	        {
	            Map.Entry m=(Map.Entry)iter.next();

	            String key = (String)m.getKey();
	            List<String> value=(List<String>)m.getValue();
	            
	            fw.write(key + " ");
	            for(int k = 0; k < value.size(); k++){
	            	if(k != value.size() - 1)
	            		fw.write(value.get(k) + ",");
	            	else
	            		fw.write(value.get(k));
	            }
	            
	            fw.write("\r\n");
	        }
	        
			fw.close();*/
		}
		FileWriter fw = new FileWriter("-wordList");
		ArrayList<Map.Entry<String, List<String>>> l = sortKey(wordList); 
		Iterator iter=l.iterator();
        
        while(iter.hasNext())
        {
            Map.Entry m=(Map.Entry)iter.next();

            String key = (String)m.getKey();
            List<String> value=(List<String>)m.getValue();
            
            fw.write(key + " ");
            for(int k = 0; k < value.size(); k++){
            	if(k != value.size() - 1)
            		fw.write(value.get(k) + ",");
            	else
            		fw.write(value.get(k));
            }
            
            fw.write("\r\n");
        }
        fw.close();
	}
	
	public static ArrayList<Map.Entry<String, List<String>>> sortKey(Map<String, List<String>> wordList){
		ArrayList<Map.Entry<String, List<String>>> l = new ArrayList(wordList.entrySet());
		Collections.sort(l,new Comparator<Map.Entry<String, List<String>>>(){

			@Override
			public int compare(Entry<String, List<String>> o1,
					Entry<String, List<String>> o2) {
				// TODO Auto-generated method stub
	
				
				if(o1.getKey().compareTo(o2.getKey()) < 0) 
					return -1;
				else if(o1.getKey().compareTo(o2.getKey()) > 0)
					return 1;
				else
					return 1;	
			}
		});
		return l;
	}
	
	public static String trimExtension(String filename) {   
	    if ((filename != null) && (filename.length() > 0)) {   
	        int i = filename.lastIndexOf('.');   
	        if ((i >-1) && (i < (filename.length()))) {   
	            return filename.substring(0, i);   
	        }   
	    }   
	    return filename;   
	} 
}
