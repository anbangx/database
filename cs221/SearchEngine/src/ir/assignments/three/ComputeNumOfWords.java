package ir.assignments.three;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.StringTokenizer;

public class ComputeNumOfWords {

	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub
		File file = new File("-wordList");
		BufferedReader reader = new BufferedReader(new FileReader(file));
		String line = "";
		int numOfWords = 0;
		int numOfUniqueWords = 0;
		StringTokenizer st = null;
		StringTokenizer st2 = null;
		while((line = reader.readLine()) != null){
			numOfUniqueWords++;
			st = new StringTokenizer(line," ");
			st.nextToken();
			String list = st.nextToken();
			st2 = new StringTokenizer(list,",");
			System.out.println(st2.countTokens());
			numOfWords += st2.countTokens();
		}
		
		System.out.println("The number of words is " + numOfWords);
		System.out.println("The number of unique words is " + numOfUniqueWords);
	}

}
