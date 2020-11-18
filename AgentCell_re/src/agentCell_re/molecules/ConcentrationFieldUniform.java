/*
 * Created on Jan 11, 2006
 *
 * To change the template for this generated file go to
 * Window>Preferences>Java>Code Generation>Code and Comments
 */
package agentCell_re.molecules;

import agentCell_re.math.Vect;

public class ConcentrationFieldUniform extends ConcentrationField {

    private double level=0;
    
    /**
     * @param newMolecularType string conataining the molecular type
     * @param newLevel double containing the value of the concentration
     * 
     * <br>
     * Creates a Concentration Field with uniform concentration.
     */
    public ConcentrationFieldUniform(String newMolecularType,
            double newLevel) {
        this.setMolecularType(newMolecularType);
        this.level=newLevel;
    }

    public double getConcentrationLevelAt(Vect position) {
        return this.level;
    }

}
