package ir.assignments.three;


/**
 * Basic class for pairing a word/2-gram/palindrome with its frequency.
 * 
 * DO NOT MODIFY THIS CLASS
 */
public final class Occurence {
	private final String word;
	private int frequency;
	
	public Occurence(String word) {
		this.word = word;
		this.frequency = 0;
	}
	
	public Occurence(String word, int frequency) {
		this.word = word;
		this.frequency = frequency;
	}
	
	public String getText() {
		return word;
	}
	
	public int getFrequency() {
		return frequency;
	}
	
	public void incrementFrequency() {
		frequency++;
	}
	
	@Override
	public String toString() {
		return word + ":" + frequency;
	}
}

