/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Jan 29, 2004
 *
 * 
 * 
 */
package agentCell_re.motor;

import agentCell_re.cells.ChemotacticCell;
import repast.simphony.random.RandomHelper;

/**
 * @author emonet
 *
 * Base class for motors. Defines the allowed states
 * (CW clockwise and CCW counterclockwise) and stores
 * the current state of the motor. Provide a method to
 * switch states and to randomize the state
 */
public abstract class Motor {
    // two possible states of the motor
    public static final int CW = 1;
    public static final int CCW = 0;
    public static final int INVALIDSTATE = -1;
    private ChemotacticCell cell;
    private int state;

    /**
     * Create a new motor with a given state
     */
    public Motor(ChemotacticCell newCell, int newState) {
        this.cell = newCell;
        state = newState;
    }

    /**
     * @param dt
     *  Advances motor state from t to t+dt
     *  The state of a motor is assumed to be the same in the intervall:  t &le; time &lt; t+dt.
     */
    public abstract void step(double dt);

    /**
     * Switches the states
     */
    public void switchState() {
        if (state == CCW) {
            state = CW;
        } else {
            state = CCW;
        }
    }

    //Getters and Setters

    /**
     * @return
     */
    public int getState() {
        return state;
    }

    /**
     * @param newState
     */
    public void setState(int newState) {
        state = newState;
    }

    public void randomizeState() {
        if (RandomHelper.nextIntFromTo(0, 1) == 0) {
            state = CCW;
        } else {
            state = CW;
        }
    }

    /**
     * @return
     */
    public ChemotacticCell getCell() {
        return cell;
    }

    /**
     * @param cell
     */
    public void setCell(ChemotacticCell cell) {
        this.cell = cell;
    }
}
