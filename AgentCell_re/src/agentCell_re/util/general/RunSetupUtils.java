/**
 * 
 */
package agentCell_re.util.general;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.List;

import agentCell_re.models.ChemotaxisModel;

/**
 * @author sneedham
 *	
 *	A collection of static methods to ease/cleanup the setup of runs
 *	
 *  Given a File Location, and a RegEx String, method returns a list with the MatchingDirectories
 *  public static List findValidDirectories(File pathLocation, String matchingPattern)
 *
 */
public class RunSetupUtils {

	/*
	 * @return List containing cells as File objects
	 * @param pathLocation
	 * @matchingPattern
	 * 		returns list based on a input that matches the pattern up to the matchingPattern provided
	 */
	public static List findMatchingDirectories(File pathLocation, String matchingPattern){
		String objectArray[] = pathLocation.list(new RegexFilter(matchingPattern));
		List objectList = new ArrayList(objectArray.length);
		
		for (int i = 0; i < objectArray.length; i++){
			objectList.add(new File(pathLocation, 
									File.separator + objectArray[i] + File.separator));
		}
		return objectList;
	}


	/*
	 * @param seed
	 * @param seedInputFileName
	 * @param seedOutputFileName
	 * 		Creates the Agentcell seed if it's not found and saves it in the Output directory
	 */
	public static void readWriteSeed(long seed, String seedInputFileName, String seedOutputFileName) {
		try {
        	BufferedReader randomSeedInputFile = new BufferedReader(new FileReader(
                        seedInputFileName));
            String seedText = randomSeedInputFile.readLine();
            seed = Long.parseLong(seedText);
        } catch (Exception e) {
            System.out.println("No Agentcell Seed Input File (" +
                seedInputFileName + "), generated seed (seed = " + seed + ").");
        } finally {
            try {
                BufferedWriter randomSeedOutputFile = new BufferedWriter(new FileWriter(
                            seedOutputFileName));
                String seedText = "" + seed;
                randomSeedOutputFile.write(seedText);
                randomSeedOutputFile.write(ChemotaxisModel.lineSeparator);
                randomSeedOutputFile.close();
            } catch (Exception e) {
                System.out.println("Error: Could not open " +
                    seedOutputFileName);
            }
        }
	}	

}
