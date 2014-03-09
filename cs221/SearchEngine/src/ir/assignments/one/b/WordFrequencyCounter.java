package ir.assignments.one.b;

import ir.assignments.one.a.Frequency;
import ir.assignments.one.a.Utilities;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

/**
 * Counts the total number of words and their frequencies in a text file.
 */
public final class WordFrequencyCounter {
	/**
	 * This class should not be instantiated.
	 */
	private WordFrequencyCounter() {}
	
	/**
	 * Takes the input list of words and processes it, returning a list
	 * of {@link Frequency}s.
	 * 
	 * This method expects a list of lowercase alphanumeric strings.
	 * If the input list is null, an empty list is returned.
	 * 
	 * There is one frequency in the output list for every 
	 * unique word in the original list. The frequency of each word
	 * is equal to the number of times that word occurs in the original list. 
	 * 
	 * The returned list is ordered by decreasing frequency, with tied words sorted
	 * alphabetically.
	 * 
	 * The original list is not modified.
	 * 
	 * Example:
	 * 
	 * Given the input list of strings 
	 * ["this", "sentence", "repeats", "the", "word", "sentence"]
	 * 
	 * The output list of frequencies should be 
	 * ["sentence:2", "the:1", "this:1", "repeats:1",  "word:1"]
	 *  
	 * @param words A list of words.
	 * @return A list of word frequencies, ordered by decreasing frequency.
	 */
	public static List<Frequency> computeWordFrequencies(List<String> words) {
		// TODO Write body!
		List<Frequency> frequencies = new ArrayList<Frequency>(); 

		if(words.size() == 0)
			return frequencies;
		
		Hashtable<String,Integer> hashtable = new Hashtable<String,Integer>();
		String word = "";
		for(int i = 0; i < words.size(); i++){
			word = words.get(i);
			if(hashtable.containsKey(word))  
				hashtable.put(word, hashtable.get(word)+1);
			else
				hashtable.put(word, 1);
		}
		ArrayList<Map.Entry<String, Integer>> l = sortKey(hashtable); 
		hashtable.clear();
		
        Iterator i=l.iterator();
        
        int n = 0;
        
        while(i.hasNext())
        {
        	if(n == 500)
        		break;
            Map.Entry m=(Map.Entry)i.next();

            String key = (String)m.getKey();
            Integer value=(Integer)m.getValue();

            Frequency f = new Frequency(key,value);
            
            frequencies.add(f);
            
            n++;
        }

		return frequencies;
	}
	
	public static ArrayList<Map.Entry<String, Integer>> sortKey(Hashtable<String,Integer> hashtable){
		ArrayList<Map.Entry<String, Integer>> l = new ArrayList(hashtable.entrySet());
		Collections.sort(l,new Comparator<Map.Entry<String, Integer>>(){

			@Override
			public int compare(Entry<String, Integer> o1,
					Entry<String, Integer> o2) {
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
	
	/**
	 * Runs the word frequency counter. The input should be the path to a text file.
	 * 
	 * @param args The first element should contain the path to a text file.
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException {
		File file = new File("hello");
		List<String> words = Utilities.tokenizeFile(file);
		List<Frequency> frequencies = computeWordFrequencies(words);
		Utilities.printFrequencies(frequencies);
	}
}
