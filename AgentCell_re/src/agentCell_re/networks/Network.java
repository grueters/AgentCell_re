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

import agentCell_re.molecules.Copynumber;

/**
 * @author emonet
 * 
 * 
 * 
 */
public interface Network{

	/**
	 * @param deltaT
	 *            Move the network model forward in time.
	 */
	public void step(double deltaT);

	/**
	 * @return current StochSim simulation time
	 */
	public double getTime();

	/**
	 * @param copynumber
	 *            (the level will be overwritten) gets a copynumber level from
	 *            the network
	 */
	public void getCopynumber(Copynumber copynumber);

	/**
	 * @param dynamicValue
	 * @param molecularType
	 *            sets a dynamic values in the network
	 */
	public void setDynamicValue(String molecularType, double dynamicValue);

}
