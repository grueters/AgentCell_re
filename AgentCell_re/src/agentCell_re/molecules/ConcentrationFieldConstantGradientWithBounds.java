/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Jun 22, 2004
 *
 * 
 * 
 */
package agentCell_re.molecules;

import agentCell_re.math.Vect;


/**
 * @author emonet
 *
 * Adds min and max bounds to a ConcentrationFieldConstantGradient.
 */
public class ConcentrationFieldConstantGradientWithBounds
    extends ConcentrationFieldConstantGradient {
    private double minLevel;
    private double maxLevel;

    /**
     * @param newMolecularType
     * @param newLevelAtOrigin
     * @param newGradient
     */
    public ConcentrationFieldConstantGradientWithBounds(
        String newMolecularType, Vect newOrigin, double newLevelAtOrigin,
        Vect newGradient, double newMinLevel, double newMaxLevel) {
        super(newMolecularType, newOrigin, newLevelAtOrigin, newGradient);
        this.minLevel = newMinLevel;
        this.maxLevel = newMaxLevel;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.molecules.ConcentrationField#getConcentrationLevelAt(edu.uchicago.agentcell.math.Vect)
     * overwrites super method to add min and max bounds.
     */
    public double getConcentrationLevelAt(Vect position) {
        double value = super.getConcentrationLevelAt(position);

        if (value <= minLevel) {
            return minLevel;
        } else if (value >= maxLevel) {
            return maxLevel;
        } else {
            return value;
        }
    }
}
