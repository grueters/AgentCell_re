/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Jul 30, 2004
 *
 * 
 * 
 */
package agentCell_re.flagella;

import agentCell_re.cells.ChemotacticCell;
import agentCell_re.motor.Motor;


/**
 * @author emonet
 *
 * 
 * 
 */
public class TetheredFlagellum extends Flagella {
    /**
     * Flagella that exactly reflects the states of Motor.
     * @param newCell
     * @param newState
     */
    public TetheredFlagellum(ChemotacticCell newCell, int newState) {
        super(newCell, newState);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.flagella.Flagella#step(double)
     * Updates the state of the flagellum to INSTANTANEOUSLY reflect the state of the Motor
     */
    public void step(double dt) {
        if (this.getCell().getMotor().getState() == Motor.CCW) {
            this.setState(Flagella.BUNDLED);
        } else {
            this.setState(Flagella.APART);
        }
    }
}
