package ir.assignments.three.lucene;

import org.apache.lucene.search.similarities.DefaultSimilarity;

class IsolationSimilarity
	extends DefaultSimilarity {
  public IsolationSimilarity(){}
  public float idf(int docFreq, int numDocs) {
    return (float)20.0;
  }
  public float coord(int overlap, int maxOverlap) {
    return 1.0f;
  }
  public float lengthNorm(String fieldName, int numTerms) {
    return 100.0f;
  }
}