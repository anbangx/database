package ir.assignments.three.pagerank;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

public class generateListOfUrl {

	public static void goThroughExistUrl() throws IOException{
		File dir = new File("perfectData/fields");
		HashMap<Long,String> hashMap = new HashMap<Long,String>();
	    File[] ds = dir.listFiles(); 
	    File[] fs;
	    String url = "";
	    int position = 0;
	    long docId = 0L;
	    for(int i = 0; i < ds.length; i++){
			fs = ds[i].listFiles();
			for(int j = 0; j < fs.length; j++){
				//get url
				BufferedReader reader = null;
				try{
					reader = new BufferedReader(new FileReader(fs[j]));
					url = reader.readLine();
					position = fs[j].getName().lastIndexOf(".");
					docId = Long.parseLong(fs[j].getName().substring(0,position));
					hashMap.put(docId, url);
					reader.close();
				}
				catch(IOException e){
					e.printStackTrace();
				}
				finally{
					if(reader != null){
						try{
							reader.close();
						}catch(IOException e1){}
					}
				}			
			}
	    }
	    
	    String urls = "";
	    StringTokenizer st;
		List keys = new ArrayList(hashMap.keySet());
		Collections.sort(keys);
	    long index = (Long) keys.get(keys.size()-1) + 1;
	    for(int i = 0; i < ds.length; i++){
			fs = ds[i].listFiles();
			for(int j = 0; j < fs.length; j++){
				//get url
				BufferedReader reader = null;
				try{
					reader = new BufferedReader(new FileReader(fs[j]));
					reader.readLine();
					reader.readLine();
					reader.readLine();
					reader.readLine();
					urls = reader.readLine();
					st = new StringTokenizer(urls," ");
					while(st.hasMoreTokens()){
						url = st.nextToken();
						if(!hashMap.containsValue(url)){
							hashMap.put(index, url);
							index++;
						}
					}
				}
				catch(IOException e){
					e.printStackTrace();
				}
				finally{
					if(reader != null){
						try{
							reader.close();
						}catch(IOException e1){}
					}
				}			
			}
	    }
	    printHashMap(hashMap);
	}
	
	public static void generateOutgoingLink() throws IOException{
		File dir = new File("perfectData/fields");
		HashMap<Long,String> hashMap = new HashMap<Long,String>();
	    File[] ds = dir.listFiles(); 
	    File[] fs;
	    String url = "";
	    int position = 0;
	    Long docId = 0L;
	    for(int i = 0; i < ds.length; i++){
			fs = ds[i].listFiles();
			for(int j = 0; j < fs.length; j++){
				//get url
				BufferedReader reader = null;
				try{
					reader = new BufferedReader(new FileReader(fs[j]));
					url = reader.readLine();
					position = fs[j].getName().lastIndexOf(".");
					docId = Long.parseLong(fs[j].getName().substring(0,position));
					hashMap.put(docId, url);
					reader.close();
				}
				catch(IOException e){
					e.printStackTrace();
				}
				finally{
					if(reader != null){
						try{
							reader.close();
						}catch(IOException e1){}
					}
				}			
			}
	    }
	    
	    String urls = "";
	    StringTokenizer st;
		List keys = new ArrayList(hashMap.keySet());
		Collections.sort(keys);
	    long index = (Long) keys.get(keys.size()-1) + 1;
	    for(int i = 0; i < ds.length; i++){
			fs = ds[i].listFiles();
			for(int j = 0; j < fs.length; j++){
				//get url
				BufferedReader reader = null;
				try{
					reader = new BufferedReader(new FileReader(fs[j]));
					reader.readLine();
					reader.readLine();
					reader.readLine();
					reader.readLine();
					urls = reader.readLine();
					st = new StringTokenizer(urls," ");
					while(st.hasMoreTokens()){
						url = st.nextToken();
						if(!hashMap.containsValue(url)){
							hashMap.put(index, url);
							index++;
						}
					}
				}
				catch(IOException e){
					e.printStackTrace();
				}
				finally{
					if(reader != null){
						try{
							reader.close();
						}catch(IOException e1){}
					}
				}			
			}
	    }
	    
	    HashMap<String,Long> newHashMap = swapHashMap(hashMap);
        FileOutputStream stream = new FileOutputStream("outgoingUrlList", true);
        OutputStreamWriter writer = new OutputStreamWriter(stream); 
	    for(int i = 0; i < ds.length; i++){
			fs = ds[i].listFiles();
			for(int j = 0; j < fs.length; j++){
				//get url
				BufferedReader reader = null;
				try{
					reader = new BufferedReader(new FileReader(fs[j]));
					position = fs[j].getName().lastIndexOf(".");
					writer.write(fs[j].getName().substring(0,position) + "\t");
					reader.readLine();
					reader.readLine();
					reader.readLine();
					reader.readLine();
					urls = reader.readLine();
					st = new StringTokenizer(urls," ");
					while(st.hasMoreTokens()){
						url = st.nextToken();
						docId = newHashMap.get(url);
						writer.write(docId.toString() + " ");
					}
					writer.write("\r\n");
				}
				catch(IOException e){
					e.printStackTrace();
				}
				finally{
					if(reader != null){
						try{
							reader.close();
						}catch(IOException e1){}
					}
				}			
			}
	    }
	    writer.close();
	    stream.close();
	}
	
	public static HashMap<String,Long> swapHashMap(HashMap<Long,String> oldHashMap){
		HashMap<String,Long> newHashMap = new HashMap<String,Long>();
		Iterator<Map.Entry<Long, String>> entries = oldHashMap.entrySet().iterator();
		Map.Entry<Long, String> entry;
		while(entries.hasNext()){
			 entry = entries.next();
			 newHashMap.put(entry.getValue(), entry.getKey());
		}
		return newHashMap;
	}
	
	public static void printHashMap(HashMap<Long,String> hashMap) throws IOException{
        FileOutputStream stream = new FileOutputStream("urlList", true);
        OutputStreamWriter writer = new OutputStreamWriter(stream); 
		List keys = new ArrayList(hashMap.keySet());
		Collections.sort(keys);
		for(int i = 0; i < keys.size(); i++)
			writer.write(keys.get(i).toString()+ " " + hashMap.get(keys.get(i)) + "\r\n");
	    writer.close();
	    stream.close();
	}
	/**
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		goThroughExistUrl();
		generateOutgoingLink();
	}

}
