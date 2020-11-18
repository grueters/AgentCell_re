/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*

 AgentCell is an agent-based simulation of bacterial chemotaxis.
 Copyright (c) 2005 Trustees of the University of Chicago

 This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

 Authors: Thierry Emonet (emonet@uchicago.edu) and Michael J. North (north@anl.gov)

 Thierry Emonet
 920 E. 58th Street
 University of Chicago
 Chicago, IL 60637

 Michael J. North
 9700 S. Cass Avenue
 Argonne, IL 60439

 */
/*
 * Created on Jul 6, 2003 by Michael J. North
 *
 * This is a wrapper class for the StochSim model.
 */
//TODO: there is a mismatch between the repast time and the stochsim time. This could be the reason why after about 100 steps the CheYp values in VAR.OUT  differ from those written by repast into CELL.OUT
package agentCell_re.networks;

import agentCell_re.molecules.Copynumber;

/**
 * @author Michael J. North, Modified by Thierry Emonet (June 07 2004).
 * 
 * Declare the main wrapper class for the StochSim model.
 */
public class Stochsim extends StochsimWrapper implements Network {

	// The Stochsim network identifier generator.
	private static int lastID = 0;
    
    private int identifier;

	// Declare two convenience constructors.
    //TODO: remove this unsafe convenience constructor and the corresponding main constructor. From now on we should only create Stochsim giving two strings as argument: the file and the path.
	public Stochsim(String arg0) {
		this(arg0, Stochsim.lastID++);
	}
    public Stochsim(String arg0, String arg1) {
        this(arg0, arg1, Stochsim.lastID++);
    }

	// Declare the main constructor.
	/**
     * Construct Stochsim instance and execute it in the current directory.
	 * @param arg0 string containing initialization file for Stochsim
	 * @param arg1 integer stochsim prcess unique ID
	 */
	private Stochsim(String arg0, int arg1) {
		super(arg0, arg1);
		identifier = arg1;
	}

    // Declare the main constructor.
    /**
     * Construct Stochsim instance and execute it in the directory containing in the string arg1.
     * @param arg0 string: containing initialization file for Stochsim
     * @param arg1 string: containing the path to the execution directory of Stochsim
     * @param arg1 integer: contains stochsim process unique ID
     */
    private Stochsim(String arg0, String arg1, int arg2) {
        super(arg0, arg1, arg2);
        identifier = arg2;
    }

    /*
	 * (non-Javadoc)
	 * 
	 * @see edu.uchicago.agentcell.networks.Network#getCopynumber(edu.uchicago.agentcell.molecules.Copynumber)
	 */
	public void getCopynumber(Copynumber copynumber) {
		copynumber.setLevel(super.getCopynumber(copynumber.getMolecularType()));
	}

	public double getTime() {
		return super.getTime();
	}

	public void setDynamicValue(String molecularType, double dynamicValue) {
		super.setDynamicValue(molecularType,dynamicValue);
	}

	public void step(double deltaT) {
		super.step(deltaT);
	}

    /**
     * @return Returns the identifier.
     */
    public int getIdentifier() {
        return identifier;
    }

}
