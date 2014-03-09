package ir.assignments.one.a;

/**
 * Basic class for pairing a word/2-gram/palindrome with its frequency.
 * 
 * DO NOT MODIFY THIS CLASS
 */
public final class Frequency {
	private final String word;
	private int frequency;
	
	public Frequency(String word) {
		this.word = word;
		this.frequency = 0;
	}
	
	public Frequency(String word, int frequency) {
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
