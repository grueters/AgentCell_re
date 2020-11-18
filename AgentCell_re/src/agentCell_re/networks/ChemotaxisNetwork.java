/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Feb 24, 2004
 *
 * 
 * 
 */
package agentCell_re.networks;

import java.io.File;
import java.util.Collection;
import java.util.Iterator;

import agentCell_re.cells.Cell;
import agentCell_re.cells.ChemotacticCell;
import agentCell_re.util.general.PathInterface;

/**
 * @author emonet
 * 
 * 
 * 
 */
public class ChemotaxisNetwork extends Stochsim implements Network, PathInterface {
	private ChemotacticCell cell;
	private String path;
	
//	// Leaving constructor here momentarily until the rest of runs are updated to use the new I/O per cell directory structure
//	public ChemotaxisNetwork(ChemotacticCell newCell) {
//		super(ChemotaxisModel.defaultInputDirectory + "STCHSTC.INI");
//		cell = newCell;
//	}

	public ChemotaxisNetwork(ChemotacticCell newCell, String cellPath) {
		super(cellPath);
		cell = newCell;
		path = new File(newCell.getPath(), "network1").toString();
	}
	
	public ChemotaxisNetwork(ChemotacticCell newCell, String stchstcFilePath, String curWorkingDirectory) {
		super(stchstcFilePath, curWorkingDirectory);
		cell = newCell;
		path = new File(newCell.getPath(), "network1").getAbsolutePath();

//		System.out.println("ChemotaxisNetwork; Cell's path, (should be network1 loc):  " + path );
//		File pathCheck = new File(path);
//		System.out.println("ChemotaxisNetwork; Cell's Absolute Path:  " + pathCheck.getAbsolutePath());
//		if (pathCheck.exists()) 
//			System.out.println("*** In ChemotaxisNetwork:  WooHoo, fs operational");
//		else
//			System.out.println("*** In ChemotaxisNetwork:  Problems with fs");
	}

	
	public void step(double dt) {
		if (cell.getReceptors() != null) {
			updateReceptorsDynamicValues();
		}

		// Call Stochsim
		// double currentTime = this.cell.getWorld().getModel().getSchedule()
		// .getCurrentTimeDouble();
		super.step(dt);

//System.out.println(" *** *** In ChemotaxisNetwork.step(dt) for cell " +this.cell.getIdentifier());
		// System.out.println();
		// Update cell value of the response regulator (cheYp)
		this.getCopynumber(cell.getCheYp());
	}

	/**
	 * Read receptors occupation and update stochsim corresponding dynamicValues
	 */
	public void updateReceptorsDynamicValues() {
		Collection c = cell.getReceptors().getDissociationConstants().keySet();
		Iterator i = c.iterator();
		while (i.hasNext()) {
			String s = (String) i.next();
			this.setDynamicValue(s, cell.getReceptors().getOccupancy(s));
		}
	}

	/**
	 * @return
	 */
	public Cell getCell() {
		return cell;
	}

	/**
	 * @param cell
	 */
	public void setCell(ChemotacticCell cell) {
		this.cell = cell;
	}

	public void setPath(String parent, String currentLoc) {
		this.path = new File(parent, currentLoc).toString();
	}
	
	public void setPath(String path) {
		this.path = path;
	}
	
	public String getPath() {
		return path;
	}

	public String getPathDir() {
		return path + PathInterface.fileSeparator;
	}
}
