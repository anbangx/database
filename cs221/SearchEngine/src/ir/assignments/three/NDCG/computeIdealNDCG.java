package ir.assignments.three.NDCG;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class computeIdealNDCG {

	public static ArrayList<Double> getIdealNDCG(int num, String searchWords) throws IOException{
		ArrayList<Double> DCG = new ArrayList<Double>();
		DCG.add(6.0);
		double gain = 0;
		for(int i = 1; i < num; i++){
			double l = i;
			if(i >= 6)
				gain = 0;
			else
				gain = (double) ((6-l)/(Math.log(l+1)/Math.log(2)));
			DCG.add(DCG.get(i-1)+ gain) ;
		}
		
		FileWriter fw = new FileWriter("DCG/ideal_" + searchWords);
		fw.write(DCG.toString());
		fw.close();
		return DCG;
	}
	
	public static ArrayList<Double> computeActualNDCG(Map<String,Double> score, String searchWords) throws IOException{
		
		ArrayList<String> results = new ArrayList<String>();
		String line = "";
		BufferedReader reader = null;
		try{
			 reader = new BufferedReader(new FileReader("MySearchResult/" + searchWords));
			 while((line = reader.readLine()) != null)
				 results.add(line);
			 reader.close();
		}
		catch(IOException e){e.printStackTrace();}
		finally{
			if(reader != null){
				try{
					reader.close();
				}catch(IOException e1){}
			}
		}	
		
		ArrayList<Double> actualDCG = new ArrayList<Double>();
		String url = results.get(0);
		double DCG = 0.0;
		double gain = 0.0;
		if(score.containsKey(url))
			DCG = score.get(url);
		actualDCG.add(DCG);
		for(int i = 1; i < results.size(); i++){
			url = results.get(i);
			long l = i;
			if(score.containsKey(url)){
				gain = score.get(url)/(Math.log(l+1)/Math.log(2));
				DCG = actualDCG.get(actualDCG.size()-1) + gain;			
			}
				
			actualDCG.add(DCG);
		}
		
		FileWriter fw = new FileWriter("DCG/actual_" + searchWords);
		fw.write(actualDCG.toString());
		fw.close();
		//System.out.println(actualDCG.toString());
		return actualDCG;
	}
	
	public static Map<String,Double> assignScoreToFile(File file){
		Map<String,Double> score = new HashMap<String,Double>();
		BufferedReader reader = null;
		String url = "";
		
		try{
			reader = new BufferedReader(new FileReader(file));
			int count = -1;
			while((url=reader.readLine()) != null){
				count++;
				switch(count){
				case 0:
					score.put(url, 6.0);
					break;
				case 1:
					score.put(url, 5.0);
					break;
				case 2:
					score.put(url, 4.0);
					break;
				case 3:
					score.put(url, 3.0);
					break;
				case 4:
					score.put(url, 2.0);
					break;
				default:
					score.put(url, 1.0);
					break;
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
		
		return score;
	}
	/**
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		
		//File dir = new File("GoogleRearchResult_Html");
		//File[] fs = dir.listFiles(); 
		ArrayList<String> wordList = new ArrayList<String>();
		String words = "";
	    words = "mondego";
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
	
		String searchWords = "";
		for(int i = 0; i < wordList.size(); i++){
			searchWords = wordList.get(i);
			File file = new File("GoogleSearchResult_Html/" + searchWords);
			Map<String,Double> score = assignScoreToFile(file);
			System.out.println(score);
			ArrayList<Double> idealNDCG = getIdealNDCG(score.size(), searchWords);
			ArrayList<Double> actualNDCG = computeActualNDCG(score, searchWords);
			int size = idealNDCG.size() > actualNDCG.size() ? actualNDCG.size() : idealNDCG.size();
			ArrayList<Double> NDCG = new ArrayList<Double>();
			for(int j = 0; j < size; j++)
				NDCG.add(actualNDCG.get(j)/idealNDCG.get(j));
			
			FileWriter fw = new FileWriter("DCG/NDCG" + searchWords);
			fw.write(NDCG.toString());
			fw.close();
			System.out.println(actualNDCG);
			System.out.println();
		}

	}

}
