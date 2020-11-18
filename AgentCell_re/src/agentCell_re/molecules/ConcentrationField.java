/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Jun 8, 2004
 *
 *
 * 
 */
package agentCell_re.molecules;

import agentCell_re.math.Vect;


/**
 * @author emonet
 *
 * Adds a scalar concentration field to Molecular
 */
public abstract class ConcentrationField extends Molecule {
    /**
     * @param position
     * @return the concentration object at given position
     */
    public Concentration getConcentrationAt(Vect position) {
        return new Concentration(this.getMolecularType(),
            this.getConcentrationLevelAt(position));
    }

    /**
     * @param position
     * @return the concentration (double) at given position
     */
    public abstract double getConcentrationLevelAt(Vect position);
}
