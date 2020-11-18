package agentCell_re.util.general;

/*
 * Some methods that the Cell and Network must maintain.
 * Classes that implement should have a private String path iv that they use
 * 
 * For convenience this could easily be a concrete or abstract subclass if neither of 
 * those classes have a need for a different supertype.
 */


public interface PathInterface {
	String fileSeparator = System.getProperty("file.separator", "//");
	
	String getPath();
	String getPathDir();
	void setPath(String parent, String currentLoc);
	void setPath(String currentLoc);
}
