/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Jan 30, 2004
 *
 * 
 * 
 */
package agentCell_re.motor;

import java.util.ArrayList;
import java.util.Iterator;

import agentCell_re.cells.ChemotacticCell;


/**
 * @author emonet
 *
 *
 */
public class SeriesThresholdMotor extends ThresholdMotor {
    private ArrayList states;
    private Iterator stateIterator;

    /**
     * Extends ThresholdMotor by defining isAboveThreshold using
     * an ArrayList of states.
     * Useful to use time series of switching events measured in real cells.
     *
     * @param newCell
     * @param newState
     * @param newMinCWDuration
     * @param newMinCCWDuration
     * @param newThreshold
     */
    public SeriesThresholdMotor(ChemotacticCell newCell, ArrayList newStates,
        double newMinCWDuration, double newMinCCWDuration) {
        super(newCell, ((Integer) newStates.get(0)).intValue(),
            newMinCWDuration, newMinCCWDuration);
        this.states = newStates;
        stateIterator = states.iterator();
        stateIterator.next();
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.motor.ThresholdMotor#isAboveThreshold()
     * This method FIRST gets the next state in the series, THEN tests if this state is CW.
     * The methods returns the result of the test. I.e. we are above threshold if the state that
     * we got from the series was CW. Note that the state of the motor is NOT set here. The
     * superclass takes care of that.
     */
    public boolean isAboveThreshold() {
        // if we don't have next we restart from the beginning 
        if (!stateIterator.hasNext()) {
            stateIterator = states.iterator();
        }

        // get the current state 
        int state = ((Integer) stateIterator.next()).intValue();

        //return comparison
        return (state == Motor.CW);
    }
}
