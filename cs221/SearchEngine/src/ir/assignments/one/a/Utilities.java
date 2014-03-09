package ir.assignments.one.a;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;


/**
 * A collection of utility methods for text processing.
 */
public class Utilities {
	/**
	 * Reads the input text file and splits it into alphanumeric tokens.
	 * Returns an ArrayList of these tokens, ordered according to their
	 * occurrence in the original text file.
	 * 
	 * Non-alphanumeric characters delineate tokens, and are discarded.
	 *
	 * Words are also normalized to lower case. 
	 * 
	 * Example:
	 * 
	 * Given this input string
	 * "An input string, this is! (or is it?)"
	 * 
	 * The output list of strings should be
	 * ["an", "input", "string", "this", "is", "or", "is", "it"]
	 * 
	 * @param input The file to read in and tokenize.
	 * @return The list of tokens (words) from the input file, ordered by occurrence.
	 * @throws IOException 
	 */
	public static ArrayList<String> tokenizeFile(File input) throws IOException {
		// TODO Write body!
		File inputStop = new File("stopword.txt");
		ArrayList<String> stopWords = new ArrayList<String>();
		BufferedReader reader = null;
		try{
			reader = new BufferedReader(new FileReader(inputStop));
			String line = null;
			
			while((line = reader.readLine()) != null)
				stopWords.add(line);
		}
		catch(IOException e){
			
		}
		reader.close();
		
		ArrayList<String> arrayList = new ArrayList<String>();
		
		if(!input.exists()){
			System.out.println("This file doesn't exist!");
			return arrayList;
		}
		
		reader = null;
		try{
			reader = new BufferedReader(new FileReader(input));
			String line = null;
			
			while((line = reader.readLine()) != null){
				StringTokenizer st = new StringTokenizer(line, " ");
				String word;
				while(st.hasMoreTokens()){
					word = st.nextToken();
					char[] ch = word.toCharArray();
					if(ch.length == 1)
						break;
					ArrayList<Character> last = new ArrayList<Character>();
					int j = 0;
					for(int i = 0; i < ch.length; i++){
						if((ch[i]>='A'&&ch[i]<='Z') || 
								(ch[i]>='a'&&ch[i]<='z')||
								(ch[i]>='0'&&ch[i]<='9')){
							if(ch[i]>='A'&&ch[i]<='Z')
								ch[i] = (char)(ch[i]+32);
							last.add(ch[i]);
							j++;
						}
					}

					StringBuilder builder = new StringBuilder(last.size());
				    for(Character c: last)
				    {
				        builder.append(c);
				    }
					word = builder.toString();
					if(!word.equals("")&&!stopWords.contains(word)
							&&!word.contains("1")&&!word.contains("2")
							&&!word.contains("3")&&!word.contains("4")
							&&!word.contains("5")&&!word.contains("6")
							&&!word.contains("7")&&!word.contains("8")
							&&!word.contains("9")&&!word.contains("0")
							){
						arrayList.add(word);
					}
						
				}
			}
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
		return arrayList;
	}
	/*
	public static ArrayList<String> tokenizeString(String input) {
		// TODO Write body!
		
		
		ArrayList<String> arrayList = new ArrayList<String>();

		StringTokenizer st = new StringTokenizer(input, " ");
		String word;
		while(st.hasMoreTokens()){
			word = st.nextToken();
			char[] ch = word.toCharArray();
			ArrayList<Character> last = new ArrayList<Character>();
			int j = 0;
			for(int i = 0; i < ch.length; i++){
				if((ch[i]>='A'&&ch[i]<='z') || 
						(ch[i]>='0'&&ch[i]<='9')){
					if(ch[i]>='A'&&ch[i]<='Z')
						ch[i] = (char)(ch[i]+32);
					last.add(ch[i]);
					j++;
				}
			}

			StringBuilder builder = new StringBuilder(last.size());
		    for(Character c: last)
		    {
		        builder.append(c);
		    }
			word = builder.toString();
			if(!word.equals("")&&checkIfStopWord(word)==1)
				arrayList.add(word);
		}
			
		return arrayList;
	}
	
	*/
	/**
	 * Takes a list of {@link Frequency}s and prints it to standard out. It also
	 * prints out the total number of items, and the total number of unique items.
	 * 
	 * Example one:
	 * 
	 * Given the input list of word frequencies
	 * ["sentence:2", "the:1", "this:1", "repeats:1",  "word:1"]
	 * 
	 * The following should be printed to standard out
	 * 
	 * Total item count: 6
	 * Unique item count: 5
	 * 
	 * sentence	2
	 * the		1
	 * this		1
	 * repeats	1
	 * word		1
	 * 
	 * 
	 * Example two:
	 * 
	 * Given the input list of 2-gram frequencies
	 * ["you think:2", "how you:1", "know how:1", "think you:1", "you know:1"]
	 * 
	 * The following should be printed to standard out
	 * 
	 * Total 2-gram count: 6
	 * Unique 2-gram count: 5
	 * 
	 * you think	2
	 * how you		1
	 * know how		1
	 * think you	1
	 * you know		1
	 * 
	 * @param frequencies A list of frequencies.
	 */
	public static void printFrequencies(List<Frequency> frequencies) {
		// TODO Write body!
		if(frequencies.isEmpty()){
			System.out.println("There is no token!"); 
			return;
		}

		//Count the total count of phrase
		int numOfGrams = countNumOfGrams(frequencies,0);
		int totalCount = frequencies.get(0).getFrequency();
		int i;
		int flag = 0;
		int preI = -1;
		
		for(i = 0; i < frequencies.size()-1; i++){
			if(numOfGrams == countNumOfGrams(frequencies,i+1)){
				totalCount += frequencies.get(i+1).getFrequency();
				flag = 0;
			}
			else{
				System.out.println("Total " + numOfGrams + "-gram count: " + totalCount);
				int k = i - preI;
				System.out.println("Unique " + numOfGrams + "-gram count: " + k);
				System.out.println();
				numOfGrams = countNumOfGrams(frequencies,i+1);
				flag = 1;
				preI = i;
				totalCount = 1;
			}	
		}
		int p;
		if(flag == 0){
			//totalCount++;
			p = frequencies.size()-1-preI;
		}
		else
			p = frequencies.size()-i;
		
		System.out.println("Total " + numOfGrams + "-gram count: " + totalCount);
		System.out.println("Unique " + numOfGrams + "-gram count: " + p);
		System.out.println();
		
		//Standard out the frequencies and print out 
		//maximum number of output
		int max = 20;
		for(i = 0; i < frequencies.size(); i++){
			if(i == max)
				break;
			System.out.println(frequencies.get(i).toString());
		}
		
	}
	
	public static int countNumOfGrams(List<Frequency> frequencies, int index){
		String line = frequencies.get(index).getText();
		StringTokenizer st = new StringTokenizer(line," ");
		int n = 0;
		while(st.hasMoreTokens()){
			n++;
			st.nextToken();
		}
		return n;
	}
	
	
	
}
