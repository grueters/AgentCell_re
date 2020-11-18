package agentCell_re.util.general;

import java.io.File;
import java.io.FilenameFilter;
import java.util.regex.*;

/**
 * @author sneedham
 * 
 * A filter used primarily for collecting information on directories matching
 * the filter. Will be used for cells within a run named runs/runz and for
 * networks within the cells. 
 * 
 * Left some flexibility in place in case there is a need to match against a file
 * in addition to the directory matching it currently performs.
 */

public class RegexFilter implements FilenameFilter{
	public static final int DIR = 1;
	public static final int FILE = 2;
	private Pattern matchingPattern;
	private int fileType;
	
	public RegexFilter(String matchingPattern){
		this(matchingPattern, DIR);
	}
	
	public RegexFilter(String matchingPattern, int fileType){
		if (matchingPattern != null){
			this.matchingPattern = Pattern.compile(matchingPattern);
		}		
		this.fileType = fileType;
	}

	public boolean accept (File dir, String name){
		File file = new File(dir, name);

		// Putting this in place in case there's a need for mathching for files at some point
		// currently the constructor is setting it to a Dir, can relax this in future
		switch (fileType){
		case DIR:
			if (!file.isDirectory()){
					return false;				
			}
			break;
		case FILE:
			if (!file.isFile()){
				return false;
			}
			break;
		}
		// starts at the beginning and attempts to match sbuset of input against pattern provided
		if (this.matchingPattern != null && !(this.matchingPattern.matcher(name).lookingAt() )){
			return false;
		}
		return true;
	}
	
}
