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


/**
 * @author emonet
 *
 * Concentrations store the molecularType from Molecular and the level
 */
public class Concentration extends Molecule {
    private double level = 0;

    /**
     *
     */
    public Concentration(String newMolecularType) {
        this(newMolecularType, 0);
    }

    public Concentration(String newMolecularType, double newLevel) {
        this.setMolecularType(newMolecularType);
        level = newLevel;
    }

    /**
     * @return
     */
    public double getLevel() {
        return level;
    }

    /**
     * @param d
     */
    public void setLevel(double d) {
        level = d;
    }

    /**
     * Given a volume, returns the copynumber (the concentration level is assumed to be in Mol).
     * @param volume (in liters)
     * @return copynumber = [C] * vol * avogadro
     */
    public Copynumber getCopynumber(double volume) {
        return new Copynumber(this.getMolecularType(),
            Math.round(level * volume * Molecule.AVOGADRO));
    }
}
