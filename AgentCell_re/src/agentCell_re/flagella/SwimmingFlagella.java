/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Jul 31, 2004
 *
 * 
 * 
 */
package agentCell_re.flagella;

import agentCell_re.cells.ChemotacticCell;
import agentCell_re.motor.Motor;
import repast.simphony.random.RandomHelper;

/**
 * @author emonet
 *
 * 
 * 
 */
public class SwimmingFlagella extends Flagella {
    private double dropTumbleProbability;
    private double apartDuration;
    private boolean dropTumble;

    /**
     * Flagella that reflect the states of the motor only approximately.
     * According to Philippe Cluzel's measurements of swimming cells in 20%
     * of the cases the tumble does not occur (the CheYp-CWbias curve max
     * at 80% instead of 100% when CheYp is high). Here we just drop a tumble
     * in 20% of the cases. The tumbles to be dropped are chosen randomly.
     * @param newCell
     * @param newState
     */
    public SwimmingFlagella(ChemotacticCell newCell, int newState) {
        super(newCell, newState);
        dropTumbleProbability = 0.2;
        apartDuration = 0.0;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.flagella.Flagella#step(double)
     */
    public void step(double dt) {
        //set state equal to motor state
        if (this.getCell().getMotor().getState() == Motor.CCW) {
            this.setState(Flagella.BUNDLED);
        } else {
            this.setState(Flagella.APART);
        }

        //drop some of the APART states
        if (this.getState() == Flagella.APART) {
            if (apartDuration == 0) {
                dropTumble = RandomHelper.nextDoubleFromTo(0, 1) < dropTumbleProbability;
            }

            if (dropTumble) {
                this.setState(Flagella.BUNDLED);
            }

            apartDuration = apartDuration + dt;
        } else {
            apartDuration = 0;
        }
    }

	public double getApartDuration() {
		return apartDuration;
	}
}
